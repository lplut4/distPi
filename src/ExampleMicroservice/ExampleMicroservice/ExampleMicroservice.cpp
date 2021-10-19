#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <sw/redis++/redis++.h>

#include "Libs.h"
#include "uuid.h"

#include <time.h>

#include "../../DataModel/out/cpp/LogMessage.pb.h"
#include "../../DataModel/out/cpp/TimeSpec.pb.h"

std::string msgTypeToString( sw::redis::Subscriber::MsgType type )
{
    if ( type == sw::redis::Subscriber::MsgType::SUBSCRIBE    ) return "SUBSCRIBE";
    if ( type == sw::redis::Subscriber::MsgType::UNSUBSCRIBE  ) return "UNSUBSCRIBE";
    if ( type == sw::redis::Subscriber::MsgType::PSUBSCRIBE   ) return "PSUBSCRIBE";
    if ( type == sw::redis::Subscriber::MsgType::PUNSUBSCRIBE ) return "PUNSUBSCRIBE";
    if ( type == sw::redis::Subscriber::MsgType::MESSAGE      ) return "MESSAGE";
    if ( type == sw::redis::Subscriber::MsgType::PMESSAGE     ) return "PMESSAGE";
    return "UNKNOWN";
}

void publish(sw::redis::Redis& redis, const google::protobuf::Message& message)
{
    auto fullName = message.GetDescriptor()->full_name();
    auto serializedData = message.SerializeAsString();

    redis.publish( fullName, serializedData );
}

int runService( char* redisHost, std::vector<std::string> channelSubscriptions )
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::string uuid = newUUID();
    std::cout << "Running Example Microservice Instance: " << uuid << std::endl;
    std::cout << "Attempting to connect to " << redisHost << std::endl;

    auto connectedOnce = false;

    while (true)
    {
        try
        {
            auto redis = sw::redis::Redis::Redis(redisHost);
            auto sub = redis.subscriber();

            std::cout << std::endl << (connectedOnce ? "Reconnected!" : "Connected!") << std::endl;

            sub.on_message([](std::string channel, std::string msg)
                {
                    std::cout << "Received message: " << msg << " on " << channel << std::endl;
                });

            sub.on_meta([](sw::redis::Subscriber::MsgType type, sw::redis::OptionalString channel, long long num)
                {
                    std::cout << "Received " << msgTypeToString(type) << " on " << channel.value() << " number " << num << std::endl;
                });

            sub.subscribe(channelSubscriptions.begin(), channelSubscriptions.end());

            connectedOnce = true;
         

            DataModel::LogMessage logMessage;

            struct timespec ts;
            timespec_get(&ts, TIME_UTC);

            DataModel::TimeSpec spec;
            spec.set_tv_sec(ts.tv_sec);
            spec.set_tv_nsec(ts.tv_nsec);

            std::string message( "New ExampleMicroservice is up: " + uuid );

            logMessage.set_allocated_timeoflog(&spec);
            logMessage.set_category(DataModel::LogMessage_Category_INFO);
            logMessage.set_allocated_message(&message);

            publish(redis, logMessage);
            
            while (true)
            {
                try
                {
                    sub.consume();
                }
                catch (const sw::redis::Error& err)
                {
                    std::cout << "Consumer exception occurred! " << std::endl;
                }
            }
        }
        catch (...)
        {
            std::cout << ".";
        }
    }

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}

int main(int argc, char* argv[])
{
    char redisHost[128];
    memset(redisHost, '\0', 128);
    strcat(redisHost, "tcp://");

    std::vector<std::string> channelSubscriptions;
    channelSubscriptions.push_back("Node Info");
    channelSubscriptions.push_back("channel-1");
    channelSubscriptions.push_back("channel-2");

    if (argc == 1)
    {
        strcat(redisHost, "localhost");
    }

    for (int i = 0; i < argc; ++i) 
    {
        if (i == 1 && strlen(argv[i]) < 128)
        {
            strcat(redisHost, argv[i]);
        }
        else if (i > 1)
        {
            channelSubscriptions.push_back(argv[i]);
        }
    }

    strcat(redisHost, ":6379");

    return runService( redisHost, channelSubscriptions );
}