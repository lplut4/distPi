using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ServiceStack.Redis;

namespace RedisInterface
{
    class Program
    {
        static void Main(string[] args)
        {
            //RedisClient client = new RedisClient("raspberrypi");
            //var ping = client.Ping();

            //var added = client.Add("key123", 123);

            //var value = client.Get<int>("key123");

            var clientsManager = new PooledRedisClientManager("raspberrypi");
            var redisPubSub = new RedisPubSubServer(clientsManager, "channel-1")
            {
                OnMessage = (channel, msg) => Console.WriteLine("Received " + msg + " on channel " + channel)
            }
            .Start();

            Console.WriteLine("Subscribing...");

            while (true) ;
        }
    }
}
