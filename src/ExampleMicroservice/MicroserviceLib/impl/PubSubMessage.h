#pragma once

#include "google/protobuf/message.h"
#include <string>

class PubMessage
{
public:
    google::protobuf::Message* message;

    PubMessage(const google::protobuf::Message& m, google::protobuf::Arena * arena)
    {
        message = m.New(arena);
        message->CopyFrom(m);
    }    

    ~PubMessage()
    {
    }
};

class SubMessage
{
public:
    const std::string fullName;
    const std::string serializedData;
 
    SubMessage(const std::string & fullName, const std::string & serializedData) :
        fullName(fullName),
        serializedData(serializedData)
    {}

    ~SubMessage()
    {}
};