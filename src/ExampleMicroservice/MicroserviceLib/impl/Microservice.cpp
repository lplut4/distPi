
#define _CRT_SECURE_NO_WARNINGS

#include "Logger.h"
#include "Microservice.h"
#include "Publisher.h"
#include "Subscriber.h"

#include <atomic>
#include <iostream>
#include <mutex>
#include <string>
#include <sw/redis++/redis++.h>
#include <thread>
#include <time.h>

#include "IMessageSubscriber.h"
#include "LogMessage.pb.h"
#include "PubSubMessage.h"
#include "SafeQueue.h"
#include "uuid.h"

namespace // private anonymous namespace
{
    std::string              g_redisHost("localhost");
    int                      g_redisPort(6379);
    SafeQueue<PubSubMessage> g_publishQueue(1000);
    SafeQueue<PubSubMessage> g_subscribeQueue(1000);
    std::atomic<bool>        g_startProcessing(false);
    std::atomic<bool>        g_continueProcessing(true);
    std::mutex               g_registrationMutex;
    std::map<std::string, std::vector<IMessageSubscriber*>> g_subscribersPerChannel;
    
    std::string msgTypeToString(const sw::redis::Subscriber::MsgType& type)
    {
        if (type == sw::redis::Subscriber::MsgType::SUBSCRIBE) return "SUBSCRIBE";
        if (type == sw::redis::Subscriber::MsgType::UNSUBSCRIBE) return "UNSUBSCRIBE";
        if (type == sw::redis::Subscriber::MsgType::PSUBSCRIBE) return "PSUBSCRIBE";
        if (type == sw::redis::Subscriber::MsgType::PUNSUBSCRIBE) return "PUNSUBSCRIBE";
        if (type == sw::redis::Subscriber::MsgType::MESSAGE) return "MESSAGE";
        if (type == sw::redis::Subscriber::MsgType::PMESSAGE) return "PMESSAGE";
        return "UNKNOWN";
    }
    
    void ProcessPublishQueueTask()
    {
        while (!g_startProcessing);

        sw::redis::ConnectionOptions connection_options;
        connection_options.host = g_redisHost;
        connection_options.port = g_redisPort;
        connection_options.socket_timeout = std::chrono::milliseconds(0);

        sw::redis::ConnectionPoolOptions pool_options;
        pool_options.size = 1;
        pool_options.wait_timeout = std::chrono::milliseconds(0);
        pool_options.connection_lifetime = std::chrono::minutes(0);

        sw::redis::Redis redis(connection_options, pool_options);

        while (g_continueProcessing)
        {
            auto pubMessage = g_publishQueue.dequeue();
            try
            {
                redis.publish(pubMessage.fullName, pubMessage.serializedData);
            }
            catch (const sw::redis::Error&)
            {
                std::cout << "Dropping message: " << pubMessage.fullName << std::endl;
            }
        }
    }
    
    void queueUpSubscribe(const std::string& channel, const std::string& msg)
    {
        g_subscribeQueue.enqueue(PubSubMessage(channel, msg));
    }
    
    void ProcessSubscribeQueueTask()
    {
        while (!g_startProcessing);

        while (g_continueProcessing)
        {
            const auto subMessage = g_subscribeQueue.dequeue();
            const auto data       = subMessage.serializedData;
            const auto name       = subMessage.fullName;

            std::lock_guard<std::mutex> guard(g_registrationMutex);

            if (g_subscribersPerChannel.find(name) != g_subscribersPerChannel.end())
            {
                DeserializedMessage msg = nullptr;
                bool deserialized = false;
                for (auto subscriber : g_subscribersPerChannel[name])
                {
                    if (!deserialized)
                    {
                        msg = subscriber->processSubscription(data);
                        deserialized = true;
                        continue;
                    }
                    subscriber->processSubscription(msg);
                }
            }
        }
    }

    void runSubscriber(const std::map<std::string, std::vector<IMessageSubscriber*>>& channelSubscriptions)
    {
        sw::redis::ConnectionOptions connection_options;
        connection_options.host = g_redisHost;
        connection_options.port = g_redisPort;
        connection_options.socket_timeout = std::chrono::milliseconds(0);

        sw::redis::ConnectionPoolOptions pool_options;
        pool_options.size = 1;
        pool_options.wait_timeout = std::chrono::milliseconds(0);
        pool_options.connection_lifetime = std::chrono::minutes(0);

        // Connect to Redis server with a connection pool.
        sw::redis::Redis redis(connection_options, pool_options);

        auto sub = redis.subscriber();

        std::cout << "Connected!" << std::endl;

        sub.on_message([](std::string channel, std::string msg)
            {
                queueUpSubscribe(channel, msg);
            });

        std::cout << "##################################################" << std::endl;
        std::cout << "Number\tType\t\tChannel" << std::endl;
        std::cout << "##################################################" << std::endl;

        sub.on_meta([](sw::redis::Subscriber::MsgType type, sw::redis::OptionalString channel, long long num)
            {
                std::cout << num << "\t" << msgTypeToString(type) << "\t" << channel.value() << std::endl;
            });

        std::vector<std::string> channels;
        for (auto keyValuePair : channelSubscriptions)
        {
            channels.push_back(keyValuePair.first);
        }
        sub.subscribe(channels.begin(), channels.end());

        g_startProcessing = true;

        auto noException = true;
        while (noException)
        {
            try
            {
                sub.consume();
            }
            catch (const sw::redis::ReplyError& err)
            {
                std::cout << err.what() << std::endl;
            }
            catch (const sw::redis::TimeoutError& err)
            {
                std::cout << err.what() << std::endl;
            }
            catch (const sw::redis::Error& err)
            {
                std::cout << err.what() << std::endl;
                noException = false;
            }
        }
    }

    void runService(const std::map<std::string, std::vector<IMessageSubscriber*>>& channelSubscriptions)
    {
        std::string uuid = newUUID();
        std::cout << "Running Example Microservice Instance: " << uuid << std::endl;
        std::cout << "Attempting to connect to " << g_redisHost << std::endl;
        while (true)
        {
            try
            {
                runSubscriber(channelSubscriptions);
            }
            catch (const sw::redis::Error&)
            {
                std::cout << ".";
            }
        }
    }
    
    void logMessage(const std::string& prefix, const char* file, const int line, const DataModel::Utility::LogMessage_Category& category, const char* msg)
    {
        DataModel::Utility::LogMessage logMessage;

        struct timespec ts;

        const auto retVal = timespec_get(&ts, TIME_UTC);

        auto spec = new DataModel::Utility::TimeSpec();
        spec->set_tv_sec(ts.tv_sec);
        spec->set_tv_nsec(ts.tv_nsec);

        auto fileName = new std::string(file);
        auto message = new std::string(msg);
        
        logMessage.set_allocated_timeoflog(spec);
        logMessage.set_category(category);
        logMessage.set_allocated_message(message);
        logMessage.set_allocated_file(fileName);
        logMessage.set_line(line);

        std::cout << prefix << msg << std::endl;

        Publisher::addToQueue(logMessage);
    }

}  // end private namespace

void Logger::error(const char* file, const int line, const char* message)
{
    logMessage("ERROR: ", file, line, DataModel::Utility::LogMessage_Category_ERROR, message);
}

void Logger::warning(const char* file, const int line, const char* message)
{
    logMessage("WARNING: ", file, line, DataModel::Utility::LogMessage_Category_WARNING, message);
}

void Logger::info(const char* file, const int line, const char* message)
{
    logMessage("INFO: ", file, line, DataModel::Utility::LogMessage_Category_INFO, message);
}

void Logger::error(const char* file, const int line, const std::string& message)
{
    logMessage("ERROR: ", file, line, DataModel::Utility::LogMessage_Category_ERROR, message.c_str());
}

void Logger::warning(const char* file, const int line, const std::string& message)
{
    logMessage("WARNING: ", file, line, DataModel::Utility::LogMessage_Category_WARNING, message.c_str());
}

void Logger::info(const char* file, const int line, const std::string& message)
{
    logMessage("INFO: ", file, line, DataModel::Utility::LogMessage_Category_INFO, message.c_str());
}

void Publisher::addToQueue(const google::protobuf::Message& message)
{
    g_publishQueue.enqueue(PubSubMessage(message));
}

bool Subscriber::registerSubscriber(const std::string& channel, IMessageSubscriber* sub)
{
    std::lock_guard<std::mutex> guard(g_registrationMutex);

    if (g_startProcessing)
    {
        return false;
    }

    if (g_subscribersPerChannel.find(channel) == g_subscribersPerChannel.end())
    {
        std::vector<IMessageSubscriber*> subscribers;
        g_subscribersPerChannel.emplace(channel, subscribers);
    }

    g_subscribersPerChannel[channel].push_back(sub);

    return true;
}

void Subscriber::unregisterSubscriber(const std::string& channel, IMessageSubscriber* sub)
{
    std::lock_guard<std::mutex> guard(g_registrationMutex);
    
    if (g_subscribersPerChannel.find(channel) == g_subscribersPerChannel.end())
    {
        return;
    }
    
    auto & channelSubscribers = g_subscribersPerChannel[channel];
    for ( auto iter = channelSubscribers.begin(); iter != channelSubscribers.end(); iter++ )
    {
        // TODO - Implement cleanup routine
        //if ( iter == sub )
        //{
        //  channelSubscribers.erase( iter );
        //}
    }
}

int Microservice::start(int argc, char* argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    char redisHost[128];
    memset(redisHost, '\0', 128);

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
    }

    g_redisHost = std::string( redisHost );

    std::thread subscribeThread(ProcessSubscribeQueueTask);
    std::thread publishThread(ProcessPublishQueueTask);

    try
    {
        runService(g_subscribersPerChannel);
    }
    catch (const std::system_error& exception)
    {
        std::cout << exception.what() << std::endl;
    }

    g_continueProcessing = false;

    publishThread.join();
    subscribeThread.join();

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}