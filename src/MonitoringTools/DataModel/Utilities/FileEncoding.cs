using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;

namespace DataModel.Utilities
{
    public class FileEncoding
    {
        public static string ToJson(Google.Protobuf.IMessage message)
        {
            var json = JsonConvert.SerializeObject(message, Formatting.None);
            return json;
        }

        public static Google.Protobuf.IMessage FromJson(string messageType, string message)
        {
            var type = ReflectiveDataModelCollection.GetSerializableType(messageType);
            var obj = JsonConvert.DeserializeObject(message, type);
            return (Google.Protobuf.IMessage)obj;
        }

        public static void ToFile(StreamWriter stream, FileMessage message)
        {
            stream.WriteLine(message.timestamp.ToBinary() + "|~|" + message.message.GetType().ToString() + "|~|" + ToJson(message.message));
        }
        public struct FileMessage
        {
            public readonly DateTime timestamp;
            public readonly Google.Protobuf.IMessage message;

            public FileMessage( DateTime t, Google.Protobuf.IMessage m )
            {
                timestamp = t;
                message = m;
            }
        }

        public static List<FileMessage> FromFile(string path)
        {
            var lines = System.IO.File.ReadAllLines(path);

            var messages = new List<FileMessage>(lines.Length);
            foreach ( var line in lines )
            {
                var lineSplit = line.Split(new string[] { "|~|" }, StringSplitOptions.RemoveEmptyEntries);
                var timeStamp = DateTime.FromBinary(long.Parse(lineSplit[0]));
                var message = FromJson(lineSplit[1], lineSplit[2]);
                messages.Add(new FileMessage(timeStamp, message));
            }

            return messages;
        }


    }
}
