#include <iostream>
#include <sw/redis++/redis++.h>

std::string msgTypeToString( sw::redis::Subscriber::MsgType type )
{
    if (type == sw::redis::Subscriber::MsgType::SUBSCRIBE)    return "SUBSCRIBE";
    if (type == sw::redis::Subscriber::MsgType::UNSUBSCRIBE)  return "UNSUBSCRIBE";
    if (type == sw::redis::Subscriber::MsgType::PSUBSCRIBE)   return "PSUBSCRIBE";
    if (type == sw::redis::Subscriber::MsgType::PUNSUBSCRIBE) return "PUNSUBSCRIBE";
    if (type == sw::redis::Subscriber::MsgType::MESSAGE)      return "MESSAGE";
    if (type == sw::redis::Subscriber::MsgType::PMESSAGE)     return "PMESSAGE";
    return "UNKNOWN";
}

int main()
{
    std::cout << "Running Example Microservice" << std::endl;

    auto redisHost = "tcp://raspberrypi:6379";
    std::cout << "Attempting to connect to " << redisHost << std::endl;

    while (true) 
    {
        try
        {
            auto redis = sw::redis::Redis::Redis("tcp://raspberrypi:6379");
            auto sub = redis.subscriber();

            std::cout << "Connected!" << std::endl;

            sub.on_message([](std::string channel, std::string msg ) 
                {
                    std::cout << "Received message: " << msg << " on " << channel << std::endl;
                });

            sub.on_meta([](sw::redis::Subscriber::MsgType type, sw::redis::OptionalString channel, long long num) 
                {
                    auto channelStr = (channel.has_value() ? channel.value() : "<NO CHANNEL>");
                    std::cout << "Received " << msgTypeToString( type ) << " on " << channelStr << " number " << num << std::endl;
                });

            sub.subscribe({ "channel-1", "channel-2", "channel-3" });
            sub.psubscribe("pattern1*");

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
            std::cout << "No response..." << std::endl;
        }
    }
}