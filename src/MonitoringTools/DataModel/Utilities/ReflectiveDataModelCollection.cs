using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

namespace DataModel
{
    public class ReflectiveDataModelCollection
    {
        private static readonly List<Type>               SERIALIZABLE_TYPES;
        private static readonly Dictionary<string, Type> SERIALIZABLE_TYPES_BY_NAME;

        static ReflectiveDataModelCollection()
        {
            SERIALIZABLE_TYPES = Assembly.GetExecutingAssembly().GetTypes()
                                                                .Where(t => t.Namespace.StartsWith("DataModel"))
                                                                .Where(t => !t.Name.EndsWith("Reflection"))
                                                                .Where(t => !t.Name.Any(ch => !Char.IsLetterOrDigit(ch)))
                                                                .Where(t => !t.FullName.Any(ch => ch == '+'))
                                                                .Where(t => !t.Name.Equals("Types"))
                                                                .Where(t => !t.Name.Equals("ReflectiveDataModelCollection"))
                                                                .ToList();

            SERIALIZABLE_TYPES_BY_NAME = new Dictionary<string, Type>();
            foreach (var dataType in SERIALIZABLE_TYPES)
            {
                SERIALIZABLE_TYPES_BY_NAME.Add(dataType.FullName, dataType);
            }
        }

        public static List<Type> GetSerializableTypes()
        {
            return SERIALIZABLE_TYPES;
        }

        public static Type GetSerializableType( string fullName )
        {
            if ( !SERIALIZABLE_TYPES_BY_NAME.ContainsKey(fullName) )
            {
                return null;
            }
            return SERIALIZABLE_TYPES_BY_NAME[fullName];
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
