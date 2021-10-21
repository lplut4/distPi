#pragma once

#include "google/protobuf/message.h"
#include <string>


class PubSubMessage
{
public:
    const std::string fullName;
    const std::string serializedData;

    PubSubMessage(const google::protobuf::Message& message) :
        fullName( message.GetDescriptor()->full_name() ),
        serializedData( message.SerializeAsString() )
    {}    

    PubSubMessage(const std::string & fullName, const std::string & serializedData) :
        fullName(fullName),
        serializedData(serializedData)
    {}
};