
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
#include "ExampleMicroservice.h"
#include "MessageSubscriber.h"
#include "Logger.h"

#include "ExampleAgent.h"

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

SafeQueue<PubSubMessage> publishQueue;
SafeQueue<PubSubMessage> subscribeQueue;

std::string g_redisHost = "";

bool continueProcessing = true;

void ExampleMicroservice::queueUpPublish(const google::protobuf::Message& message)
{
	publishQueue.enqueue(PubSubMessage(message));
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

std::map<std::string, std::vector<IMessageSubscriber*>> subscribersPerChannel;

void ExampleMicroservice::registerSubscriber(std::string channel, IMessageSubscriber* sub)
{
	if (subscribersPerChannel.find(channel) == subscribersPerChannel.end())
	{
		std::vector<IMessageSubscriber*> subscribers;
		subscribersPerChannel.emplace(channel, subscribers);
	}

	subscribersPerChannel[channel].push_back(sub);
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

void PublishDummyMessageTask()
{
	while (continueProcessing)
	{
		Logger::logInfo("Spam: " + newUUID());

		DataModel::LogMessage logMessage;

		struct timespec ts;

		auto retVal = timespec_get(&ts, TIME_UTC);

		auto spec = new DataModel::TimeSpec();
		spec->set_tv_sec(ts.tv_sec);
		spec->set_tv_nsec(ts.tv_nsec);

		auto message = new std::string("Spam: " + newUUID());

		logMessage.set_allocated_timeoflog(spec);
		logMessage.set_category(DataModel::LogMessage_Category_INFO);
		logMessage.set_allocated_message(message);

		ExampleMicroservice::queueUpPublish(logMessage);

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
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

	Logger::logInfo("New ExampleMicroservice is up: " + uuid);

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

int ExampleMicroservice::startService(int argc, char* argv[])
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
	std::thread dummyThread(PublishDummyMessageTask);

	try
	{
		runService(subscribersPerChannel);
	}
	catch (const std::system_error& exception)
	{
		std::cout << exception.what() << std::endl;
	}

	continueProcessing = false;

	dummyThread.join();
	publishThread.join();
	subscribeThread.join();

	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}

int main(int argc, char* argv[])
{
	// Instantiate "Agents" here
	ExampleAgent agent1;
	ExampleAgent agent2;
	ExampleAgent agent3;
	ExampleAgent agent4;
	ExampleAgent agent5;
	ExampleAgent agent6;

	ExampleMicroservice::startService(argc, argv);
}