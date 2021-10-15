using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;

namespace DataModel
{
    public class ReflectiveDataModelCollection
    {
        private static List<Type> serializableTypes = Assembly.GetExecutingAssembly().GetTypes()
                                                              .Where(t => t.Namespace.StartsWith("DataModel"))
                                                              .Where(t => !t.Name.EndsWith("Reflection"))
                                                              .Where(t => !t.Name.Any(ch => !Char.IsLetterOrDigit(ch)))
                                                              .Where(t => !t.FullName.Any(ch => ch == '+'))
                                                              .Where(t => !t.Name.Equals("Types"))
                                                              .Where(t => !t.Name.Equals("ReflectiveDataModelCollection"))
                                                              .ToList();

        private static bool dictInit = false;

        private static Dictionary<string, Type> serializableTypesByFullName = new Dictionary<string, Type>();

        public static List<Type> GetSerializableTypes()
        {
            if (!dictInit)
            {
                foreach (var dataType in serializableTypes)
                {
                    serializableTypesByFullName.Add(dataType.FullName, dataType);
                }
                dictInit = true;
            }

            return serializableTypes;
        }

        public static Type GetSerializableType( string fullName )
        {
            if (!dictInit)
            {
                foreach ( var dataType in serializableTypes )
                {
                    serializableTypesByFullName.Add(dataType.FullName, dataType);
                }
                dictInit = true;
            }

            if ( !serializableTypesByFullName.ContainsKey(fullName) )
            {
                return null;
            }

            return serializableTypesByFullName[fullName];
        }

        public static string DecodeMessage(Google.Protobuf.IMessage message)
        {
            var builder = new StringBuilder();
            foreach (var field in message.Descriptor.Fields.InDeclarationOrder())
            {
                builder.Append(field.Name).Append(": ").Append(field.Accessor.GetValue(message)).Append( "  " );
            }
            return builder.ToString();
        }

        public static byte[] Serialize(Google.Protobuf.IMessage message)
        {
            byte[] buffer = new byte[message.CalculateSize()];
            message.WriteTo(new Google.Protobuf.CodedOutputStream(buffer));
            return buffer;
        }

        public static Google.Protobuf.IMessage Deserialize( Type deserializableType, byte[] buffer )
        {
            Google.Protobuf.IMessage obj = (Google.Protobuf.IMessage) deserializableType.GetConstructor( Type.EmptyTypes )
                                                                                        .Invoke( new object[] {} );
            obj.MergeFrom(new Google.Protobuf.CodedInputStream(buffer));
            return obj;
        }
    }
}
