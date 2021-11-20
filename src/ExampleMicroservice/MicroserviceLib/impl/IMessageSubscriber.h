#pragma once

using DeserializedMessage = std::shared_ptr<google::protobuf::Message>;

class IMessageSubscriber
{
public:

    // Deserialize the message, and pass it on to the subscriber
    virtual DeserializedMessage processSubscription(const std::string & encodedMsg) = 0;

    // The Message has already been deserialized, but we want to pass it to a subscriber
    virtual void processSubscription(DeserializedMessage msg) = 0;
};