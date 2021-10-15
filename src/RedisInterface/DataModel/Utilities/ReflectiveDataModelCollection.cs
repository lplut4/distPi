using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

namespace DataModel
{
    public class ReflectiveDataModelCollection
    {
        public static List<Type> getSerializableTypes()
        {
            return Assembly.GetExecutingAssembly().GetTypes()
                           .Where(t => t.Namespace.StartsWith("DataModel"))
                           .Where(t => !t.Name.EndsWith("Reflection"))
                           .Where(t => !t.Name.Any(ch => !Char.IsLetterOrDigit(ch)))
                           .Where(t => !t.FullName.Any(ch => ch == '+'))
                           .Where(t => !t.Name.Equals("Types"))
                           .Where(t => !t.Name.Equals("ReflectiveDataModelCollection"))
                           .ToList();
        }

        public static void PrintMessage(Google.Protobuf.IMessage message)
        {
            var descriptor = message.Descriptor;
            foreach (var field in descriptor.Fields.InDeclarationOrder())
            {
                Console.WriteLine(
                    "Field {0} ({1}): {2}",
                    field.FieldNumber,
                    field.Name,
                    field.Accessor.GetValue(message));
            }
        }

        public static byte[] serialize(Google.Protobuf.IMessage message)
        {
            byte[] buffer = new byte[message.CalculateSize()];
            message.WriteTo(new Google.Protobuf.CodedOutputStream(buffer));
            return buffer;
        }

        public static Google.Protobuf.IMessage deserialize( Type deserializableType, byte[] buffer )
        {
            Google.Protobuf.IMessage obj = (Google.Protobuf.IMessage) deserializableType.GetConstructor( Type.EmptyTypes )
                                                                                        .Invoke( new object[] {} );
            obj.MergeFrom(new Google.Protobuf.CodedInputStream(buffer));
            return obj;
        }
    }
}
