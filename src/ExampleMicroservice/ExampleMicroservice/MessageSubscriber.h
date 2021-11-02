#pragma once

#include <google/protobuf/message.h>

#include "Logger.h"
#include "SafeQueue.h"

using DeserializedMessage = std::shared_ptr<google::protobuf::Message>;

class IMessageSubscriber
{
public:

	// Deserialize the message, and pass it on to the subscriber
	virtual DeserializedMessage processSubscription(std::string encodedMsg) = 0;

	// The Message has already been deserialized, but we want to pass it to a subscriber
	virtual void processSubscription(DeserializedMessage msg) = 0;
};

template <class T>
class MessageSubscriber : public IMessageSubscriber
{
public:

	MessageSubscriber() : m_consumeThread{} 
	{
		m_consumeThread = std::thread{ &MessageSubscriber::ConsumeThreadMain, this };
	};
	~MessageSubscriber() 
	{
	};

	using SubscribeCallback = std::function<void(std::shared_ptr<T> msg)>;

	void onMessage(SubscribeCallback subCallback)
	{
		m_subscribeCallback = subCallback;
		DeserializedMessage message = std::make_shared<T>();
		ExampleMicroservice::registerSubscriber(message->GetTypeName(), this);
	}

	// Deserialize the message, and pass it on to the subscriber
	DeserializedMessage processSubscription(std::string encodedMsg)
	{
		DeserializedMessage msg = std::make_shared<T>();
		auto success = msg->ParseFromString(encodedMsg);
		if (!success)
		{
			auto error = "Could not deserialize: " + msg->GetTypeName();
			Logger::logError(error);
		}
		m_messageQueue.enqueue(std::dynamic_pointer_cast<T>(msg));
		return msg;
	}

	// The Message has already been deserialized, but we want to pass it to a subscriber
	void processSubscription(DeserializedMessage msg)
	{
		m_messageQueue.enqueue(std::dynamic_pointer_cast<T>(msg));
	}

private:

	void ConsumeThreadMain()
	{
		while (true)
		{
			auto msg = m_messageQueue.dequeue();
			m_subscribeCallback(msg);
		}
	}

	SubscribeCallback              m_subscribeCallback;
	SafeQueue<std::shared_ptr<T>>  m_messageQueue;
	std::thread                    m_consumeThread;
};