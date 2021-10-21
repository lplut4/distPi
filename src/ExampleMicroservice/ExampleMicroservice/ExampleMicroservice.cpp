#include <iostream>
#include <sw/redis++/redis++.h>
#include <time.h>
#include <thread>

#include "Libs.h"
#include "uuid.h"
#include "SafeQueue.h"
#include "PubSubMessage.h"
#include "ExampleMicroservice.h"

#include "../../DataModel/out/cpp/LogMessage.pb.h"
#include "../../DataModel/out/cpp/TimeSpec.pb.h"

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

std::mutex redisMutex;
sw::redis::Redis* redis = nullptr;
bool continueProcessing = true;;

void ExampleMicroservice::queueUpPublish(const google::protobuf::Message& message)
{
	publishQueue.enqueue(PubSubMessage(message));
}

void ProcessPublishQueueTask()
{
	while (continueProcessing)
	{
		auto pubMessage = publishQueue.dequeue();

		redisMutex.lock();
		if (redis != nullptr)
		{
			redis->publish(pubMessage.fullName, pubMessage.serializedData);
		}
		redisMutex.unlock();
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

		// TODO - Figure out how to distribute these messages to the various internal "subscribers"

		std::cout << "Received message: " << subMessage.serializedData << " on " << subMessage.fullName << std::endl;
	}
}

void runSubscriber(sw::redis::Redis* redis, const std::vector<std::string>& channelSubscriptions, const std::string& uuid)
{
	auto redisExists = false;

	redisMutex.lock();
	if (redis != nullptr)
	{
		redisExists = true;
	}
	redisMutex.unlock();

	if (!redisExists)
	{
		return;
	}

	auto sub = redis->subscriber();

	std::cout << std::endl << "Connected!" << std::endl;

	sub.on_message([](std::string channel, std::string msg)
		{
			queueUpSubscribe(channel, msg);
		});

	sub.on_meta([](sw::redis::Subscriber::MsgType type, sw::redis::OptionalString channel, long long num)
		{
			std::cout << "Received " << msgTypeToString(type) << " on " << channel.value() << " number " << num << std::endl;
		});

	sub.subscribe(channelSubscriptions.begin(), channelSubscriptions.end());

	DataModel::LogMessage logMessage;

	struct timespec ts;
	timespec_get(&ts, TIME_UTC);

	auto spec = new DataModel::TimeSpec();
	spec->set_tv_sec(ts.tv_sec);
	spec->set_tv_nsec(ts.tv_nsec);

	auto message = new std::string("New ExampleMicroservice is up: " + uuid);

	logMessage.set_allocated_timeoflog(spec);
	logMessage.set_category(DataModel::LogMessage_Category_INFO);
	logMessage.set_allocated_message(message);

	ExampleMicroservice::queueUpPublish(logMessage);

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

void runService(char* redisHost, const std::vector<std::string>& channelSubscriptions)
{
	std::string uuid = newUUID();
	std::cout << "Running Example Microservice Instance: " << uuid << std::endl;
	std::cout << "Attempting to connect to " << redisHost << std::endl;

	while (true)
	{
		redisMutex.lock();
		try
		{
			redis = new sw::redis::Redis(redisHost);
		}
		catch (...)
		{
			if (redis != nullptr)
			{
				delete redis;
				redis = nullptr;
			}
			std::cout << ".";
		}
		redisMutex.unlock();

		try
		{
			if (redis != nullptr)
			{
				runSubscriber(redis, channelSubscriptions, uuid);
			}
		}
		catch (...)
		{
			if (redis != nullptr)
			{
				delete redis;
				redis = nullptr;
			}
			std::cout << ".";
		}

		redisMutex.lock();
		if (redis != nullptr)
		{
			delete redis;
			redis = nullptr;
		}
		redisMutex.unlock();
	}
}

int main(int argc, char* argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	char redisHost[128];
	memset(redisHost, '\0', 128);
	strcat_s(redisHost, "tcp://");

	std::vector<std::string> channelSubscriptions;
	channelSubscriptions.push_back("Node Info");
	channelSubscriptions.push_back("channel-1");
	channelSubscriptions.push_back("channel-2");

	if (argc == 1)
	{
		strcat_s(redisHost, "localhost");
	}

	for (int i = 0; i < argc; ++i)
	{
		if (i == 1 && strlen(argv[i]) < 128)
		{
			strcat_s(redisHost, argv[i]);
		}
		else if (i > 1)
		{
			// TODO - Figure out how to introspectively obtain the current data model objects   
			channelSubscriptions.push_back(argv[i]);
		}
	}

	strcat_s(redisHost, ":6379");

	std::thread subscribeThread(ProcessSubscribeQueueTask);
	std::thread publishThread(ProcessPublishQueueTask);

	try
	{
		runService(redisHost, channelSubscriptions);
	}
	catch (const std::system_error& exception)
	{
		std::cout << exception.what() << std::endl;
	}

	continueProcessing = false;

	publishThread.join();
	subscribeThread.join();

	google::protobuf::ShutdownProtobufLibrary();
}