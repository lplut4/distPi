
#define _CRT_SECURE_NO_WARNINGS

#include "Logger.h"
#include "Microservice.h"
#include "Publisher.h"
#include "Subscriber.h"

#include <string>
#include <time.h>
#include <iostream>
#include <sw/redis++/redis++.h>
#include <thread>

#include "IMessageSubscriber.h"
#include "LogMessage.pb.h"
#include "PubSubMessage.h"
#include "SafeQueue.h"
#include "uuid.h"

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
			catch (const sw::redis::Error&)
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
			catch (const sw::redis::Error&)
			{
				std::cout << ".";
			}
		}
	}
	
	void logMessage(const std::string& prefix, const char* file, const int line, const DataModel::LogMessage_Category& category, const char* msg)
	{
		DataModel::LogMessage logMessage;

		struct timespec ts;

		const auto retVal = timespec_get(&ts, TIME_UTC);

		auto spec = new DataModel::TimeSpec();
		spec->set_tv_sec(ts.tv_sec);
		spec->set_tv_nsec(ts.tv_nsec);

		auto fileName = new std::string(file);
		auto message = new std::string(msg);
		
		logMessage.set_allocated_timeoflog(spec);
		logMessage.set_category(category);
		logMessage.set_allocated_message(message);
		logMessage.set_allocated_file(fileName);
		logMessage.set_line(line);

		std::cout << prefix << msg << std::endl;

		Publisher::addToQueue(logMessage);
	}

}  // end private

void Logger::error(const char* file, const int line, const char* message)
{
	logMessage("ERROR: ", file, line, DataModel::LogMessage_Category_ERROR, message);
}

void Logger::warning(const char* file, const int line, const char* message)
{
	logMessage("WARNING: ", file, line, DataModel::LogMessage_Category_WARNING, message);
}

void Logger::info(const char* file, const int line, const char* message)
{
	logMessage("INFO: ", file, line, DataModel::LogMessage_Category_INFO, message);
}

void Logger::error(const char* file, const int line, const std::string& message)
{
	logMessage("ERROR: ", file, line, DataModel::LogMessage_Category_ERROR, message.c_str());
}

void Logger::warning(const char* file, const int line, const std::string& message)
{
	logMessage("WARNING: ", file, line, DataModel::LogMessage_Category_WARNING, message.c_str());
}

void Logger::info(const char* file, const int line, const std::string& message)
{
	logMessage("INFO: ", file, line, DataModel::LogMessage_Category_INFO, message.c_str());
}

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

	g_redisHost = std::string( redisHost );

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