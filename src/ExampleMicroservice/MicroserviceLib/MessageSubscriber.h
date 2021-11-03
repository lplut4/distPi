#pragma once

#include <google/protobuf/message.h>
#include <type_traits>
#include <functional>
#include <memory>

#include "Logger.h"
#include "Subscriber.h"
#include "IMessageSubscriber.h"
#include "SafeQueue.h"

template <class T>
class MessageSubscriber : public IMessageSubscriber
{
	static_assert(std::is_base_of<google::protobuf::Message, T>::value, "T must be a subclass of google::protobuf::Message (in other words, it must be a class generated by protoc)");

public:

	MessageSubscriber()
	{
		m_consumeThread = std::thread{ &MessageSubscriber::ConsumeThreadMain, this };
	};

	~MessageSubscriber() {};

	using SubscribeCallback = std::function<void(std::shared_ptr<T> msg)>;

	void onMessage(SubscribeCallback subCallback)
	{
		m_subscribeCallback = subCallback;
		DeserializedMessage message = std::make_shared<T>();
		Subscriber::registerSubscriber(message->GetTypeName(), this);
	}

	// Deserialize the message, and pass it on to the subscriber
	virtual DeserializedMessage processSubscription(std::string encodedMsg)
	{
		DeserializedMessage msg = std::make_shared<T>();
		auto success = msg->ParseFromString(encodedMsg);
		if (!success)
		{
			Logger::error("Could not deserialize: " + msg->GetTypeName());
		}
		m_messageQueue.enqueue(std::dynamic_pointer_cast<T>(msg));
		return msg;
	}

	// The Message has already been deserialized, but we want to pass it to a subscriber
	virtual void processSubscription(DeserializedMessage msg)
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