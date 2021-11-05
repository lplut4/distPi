using System;
using StackExchange.Redis;

namespace CommandLineLogViewer
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Subscribing...");

            var redis = ConnectionMultiplexer.Connect("localhost");
            var sub = redis.GetSubscriber();

            sub.Subscribe("DataModel.Utility.LogMessage").OnMessage(channelMessage => {

                var inputStream = new Google.Protobuf.CodedInputStream(channelMessage.Message);
                
                var message = new DataModel.Utility.LogMessage();
                message.MergeFrom(inputStream);

                var consoleMessage = message.File + "(line " + message.Line + ") - " + message.Category.ToString() + " - " + message.Message;  
                
                Console.WriteLine(consoleMessage);
            });

            while (true) ;
        }
    }
}
 