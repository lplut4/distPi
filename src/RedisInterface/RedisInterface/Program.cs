using System;
using System.Threading.Tasks;
using StackExchange.Redis;

namespace RedisInterface
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Subscribing...");

            var redis = ConnectionMultiplexer.Connect("localhost");
            var sub = redis.GetSubscriber();

            // Synchronous handler
            sub.Subscribe("channel-1").OnMessage(channelMessage => {
                Console.WriteLine((string)channelMessage.Message);
            });

            // Asynchronous handler
            sub.Subscribe("channel-1").OnMessage(async channelMessage => {
                await Task.Delay(1000);
                Console.WriteLine((string)channelMessage.Message);
            });

            sub.Publish("channel-1", "RedisInterface Publishing Message");
            while (true) ;
        }
    }
}
 