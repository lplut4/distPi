
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <sw/redis++/redis++.h>
#include <memory>
#include <time.h>
#include <thread>
#include <chrono>

#include "Libs.h"
#include "uuid.h"
#include "SafeQueue.h"
#include "PubSubMessage.h"
#include "Microservice.h"
#include "IMessageSubscriber.h"

#include <string>
#include <time.h>
#include "LogMessage.pb.h"

#include "Publisher.h"
#include "Subscriber.h"
#include "Logger.h"

namespace // private
{
	SafeQueue<PubSubMessage> publishQueue;
	SafeQueue<PubSubMessage> subscribeQueue;
	std::string g_redisHost = "";
	bool continueProcessing = true;
	std::map<std::string, std::vector<IMessageSubscriber*>> subscribersPerChannel;
	
	std::string msgTypeToString(const sw::redis::Subscriber::MsgType& type)
	{
		if (type == sw::redis::Subscriber::MsgType::SUBSCRIBE) return "SUBSCRIBE";
		if (type == sw::redis::Subscriber::MsgType::UNSUBSCRIBE) return "UNSUBSCRIBE";
		if (type == sw::redis::Subscriber::MsgType::PSUBSCRIBE) return "PSUBSCRIBE";
		if (type == sw::redis::Subscriber::MsgType::PUNSUBSCRIBE) return "PUNSUBSCRIBE";
		if (type == sw::redis::Subscriber::MsgType::MESSAGE) return "MESSAGE";
		if (type == sw::redis::Subscriber::MsgType::PMESSAGE) return "PMESSAGE";
		return "UNKNOWN";
	}
	
	void ProcessPublishQueueTask()
	{
		auto redis = sw::redis::Redis(g_redisHost);

		while (continueProcessing)
		{
			auto pubMessage = publishQueue.dequeue();
			try
			{
				redis.publish(pubMessage.fullName, pubMessage.serializedData);
			}
			catch (...)
			{
				std::cout << "Dropping message: " << pubMessage.fullName << std::endl;
			}
		}
	}
	
	void queueUpSubscribe(const std::string& channel, const std::string& msg)
	{
		subscribeQueue.enqueue(PubSubMessage(channel, msg));
	}
	
	void ProcessSubscribeQueueTask()
	{
		while (continueProcessing)
		{
			auto subMessage = subscribeQueue.dequeue();
			auto data       = subMessage.serializedData;
			auto name       = subMessage.fullName;

			if (subscribersPerChannel.find(name) != subscribersPerChannel.end())
			{
				DeserializedMessage msg = nullptr;
				bool deserialized = false;
				for (auto subscriber : subscribersPerChannel[name])
				{
					if (!deserialized)
					{
						msg = subscriber->processSubscription(data);
						deserialized = true;
						continue;
					}
					subscriber->processSubscription(msg);
				}
			}
		}
	}

	void runSubscriber(const std::map<std::string, std::vector<IMessageSubscriber*>>& channelSubscriptions, const std::string& uuid)
	{
		auto redis = sw::redis::Redis(g_redisHost);
		auto sub = redis.subscriber();

		std::cout << std::endl << "Connected!" << std::endl;

		sub.on_message([](std::string channel, std::string msg)
			{
				queueUpSubscribe(channel, msg);
			});

		sub.on_meta([](sw::redis::Subscriber::MsgType type, sw::redis::OptionalString channel, long long num)
			{
				std::cout << "Received " << msgTypeToString(type) << " on " << channel.value() << " number " << num << std::endl;
			});

		std::vector<std::string> channels;
		for (auto keyValuePair : channelSubscriptions)
		{
			channels.push_back(keyValuePair.first);
		}
		sub.subscribe(channels.begin(), channels.end());

		std::cout << "New Microservice is up: " << uuid << std::endl;;

		auto noException = true;
		while (noException)
		{
			try
			{
				sub.consume();
			}
			catch (const sw::redis::ReplyError& err)
			{
				std::cout << err.what() << std::endl;
			}
			catch (const sw::redis::TimeoutError& err)
			{
				std::cout << err.what() << std::endl;
			}
			catch (const sw::redis::Error& err)
			{
				std::cout << err.what() << std::endl;
				noException = false;
			}
		}
	}

	void runService(const std::map<std::string, std::vector<IMessageSubscriber*>>& channelSubscriptions)
	{
		std::string uuid = newUUID();
		std::cout << "Running Example Microservice Instance: " << uuid << std::endl;
		std::cout << "Attempting to connect to " << g_redisHost << std::endl;

		while (true)
		{
			try
			{
				runSubscriber(channelSubscriptions, uuid);
			}
			catch (...)
			{
				std::cout << ".";
			}
		}
	}
	
	enum Category
	{
		INFO_MSG,
		WARNING_MSG,
		ERROR_MSG
	};

	void logMessage(const std::string& prefix, const std::string& msg, const Category& category)
	{
		DataModel::LogMessage logMessage;

		struct timespec ts;

		auto retVal = timespec_get(&ts, TIME_UTC);

		auto spec = new DataModel::TimeSpec();
		spec->set_tv_sec(ts.tv_sec);
		spec->set_tv_nsec(ts.tv_nsec);

		auto message = new std::string(msg);

		logMessage.set_allocated_timeoflog(spec);
		logMessage.set_category(DataModel::LogMessage_Category_INFO);
		logMessage.set_allocated_message(message);

		std::cout << "INFO: " << msg << std::endl;

		Publisher::addToQueue(logMessage);
	}

}  // end private

void Logger::logError(const std::string& message) 
{ 
	logMessage("ERROR", message, Category::ERROR_MSG); 
};

void Logger::logWarning(const std::string& message) 
{ 
	logMessage("WARNING", message, Category::WARNING_MSG); 
};

void Logger::logInfo(const std::string& message) 
{ 
	logMessage("INFO", message, Category::INFO_MSG); 
};

void Publisher::addToQueue(const google::protobuf::Message& message)
{
	publishQueue.enqueue(PubSubMessage(message));
}

void Subscriber::registerSubscriber(const std::string& channel, IMessageSubscriber* sub)
{
	if (subscribersPerChannel.find(channel) == subscribersPerChannel.end())
	{
		std::vector<IMessageSubscriber*> subscribers;
		subscribersPerChannel.emplace(channel, subscribers);
	}

	subscribersPerChannel[channel].push_back(sub);
}

int Microservice::start(int argc, char* argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	char redisHost[128];
	memset(redisHost, '\0', 128);
	strcat(redisHost, "tcp://");

	if (argc == 1)
	{
		strcat(redisHost, "localhost");
	}

	for (int i = 0; i < argc; ++i)
	{
		if (i == 1 && strlen(argv[i]) < 128)
		{
			strcat(redisHost, argv[i]);
		}
	}

	strcat(redisHost, ":6379");

	g_redisHost = redisHost;

	std::thread subscribeThread(ProcessSubscribeQueueTask);
	std::thread publishThread(ProcessPublishQueueTask);

	try
	{
		runService(subscribersPerChannel);
	}
	catch (const std::system_error& exception)
	{
		std::cout << exception.what() << std::endl;
	}

	continueProcessing = false;

	publishThread.join();
	subscribeThread.join();

	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}