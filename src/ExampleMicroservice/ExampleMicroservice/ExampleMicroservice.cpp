
#include "Libs.h"

#include <iostream>
#include <sw/redis++/redis++.h>

#include "uuid.h"
//#include "TimeSpec.ph.h"
//#include "LogMessage.ph.h"
//#include "../../DataModel/out/cpp/TimeSpec.pb.h"


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

int main()
{
    std::string uuid = newUUID();

    std::cout << "Running Example Microservice Instance: " << uuid << std::endl;

    auto redisHost = "";
    std::vector<std::string> channelSubscriptions;

    redisHost = "tcp://localhost:6379";
    channelSubscriptions.push_back( "Node Info" );
    channelSubscriptions.push_back( "channel-1" );
    channelSubscriptions.push_back( "channel-2" );
    channelSubscriptions.push_back( "channel-3" );

    std::cout << "Attempting to connect to " << redisHost << std::endl;

    auto connectedOnce = false;

    while ( true ) 
    {
        try
        {
            auto redis = sw::redis::Redis::Redis( redisHost );
            auto sub   = redis.subscriber();

            std::cout << std::endl << ( connectedOnce ? "Reconnected!" : "Connected!" ) << std::endl;

            sub.on_message([]( std::string channel, std::string msg ) 
                {
                    std::cout << "Received message: " << msg << " on " << channel << std::endl;
                });

            sub.on_meta([]( sw::redis::Subscriber::MsgType type, sw::redis::OptionalString channel, long long num ) 
                {
                    std::cout << "Received " << msgTypeToString( type ) << " on " << channel.value() << " number " << num << std::endl;
                });

            sub.subscribe( channelSubscriptions.begin(), channelSubscriptions.end() );

            connectedOnce = true;

            redis.publish("Node Info", "New ExampleMicroservice is up: " + uuid);

            while ( true ) 
            {
                try 
                {
                    sub.consume();
                }
                catch ( const sw::redis::Error& err ) 
                {  
                    std::cout << "Consumer exception occurred! " << std::endl;
                }
            }
        }
        catch ( ... ) 
        {
            std::cout << ".";
        }
    }

    return 0;
}