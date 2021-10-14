#include <iostream>
#include <sw/redis++/redis++.h>

int main()
{
    std::cout << "Hello World!\n";

    while (true) {

        try
        {
            auto redis = sw::redis::Redis::Redis("tcp://raspberrypi:6379");

            // Create a Subscriber.
            auto sub = redis.subscriber();

            // Set callback functions.
            sub.on_message([](std::string channel, std::string msg) 
                {
                std::cout << msg << std::endl;
                // Process message of MESSAGE type.
                });

            sub.on_pmessage([](std::string pattern, std::string channel, std::string msg) 
                {
                // Process message of PMESSAGE type.
                });

            sub.on_meta([](sw::redis::Subscriber::MsgType type, sw::redis::OptionalString channel, long long num) 
                {
                // Process message of META type.
                });

            // Subscribe to channels and patterns.
            sub.subscribe("channel-1");
            sub.subscribe({ "channel-2", "channel-3" });

            sub.psubscribe("pattern1*");

            // Consume messages in a loop.
            while (true) 
            {
                try 
                {
                    sub.consume();
                }
                catch (const sw::redis::Error& err) 
                {
                    std::cout << "Redis Exception Occurred!" << std::endl;// Handle exceptions.
                }
            }
        }
        catch (...) 
        {
            std::cout << "Exception Occurred!" << std::endl;
        }
    }
}