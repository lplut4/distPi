#include <iostream>
#include <sw/redis++/redis++.h>
#include <time.h>
#include <thread>
#include <chrono>

#include "Libs.h"
#include "uuid.h"
#include "SafeQueue.h"
#include "PubSubMessage.h"
#include "ExampleMicroservice.h"

#include "LogMessage.pb.h"
#include "TimeSpec.pb.h"

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

bool continueProcessing = true;;

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

void PublishDummyMessageTask()
{
	while (continueProcessing)
	{
		DataModel::LogMessage logMessage;

		struct timespec ts;
		auto retVal = timespec_get(&ts, TIME_UTC);

		auto spec = new DataModel::TimeSpec();
		spec->set_tv_sec(ts.tv_sec);
		spec->set_tv_nsec(ts.tv_nsec);

		auto message = new std::string("Spam" + newUUID());

		logMessage.set_allocated_timeoflog(spec);
		logMessage.set_category(DataModel::LogMessage_Category_INFO);
		logMessage.set_allocated_message(message);

		ExampleMicroservice::queueUpPublish(logMessage);

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

void runSubscriber(const std::vector<std::string>& channelSubscriptions, const std::string& uuid)
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

	sub.subscribe(channelSubscriptions.begin(), channelSubscriptions.end());

	DataModel::LogMessage logMessage;

	struct timespec ts;
	auto retVal = timespec_get(&ts, TIME_UTC);

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

void runService(const std::vector<std::string>& channelSubscriptions)
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

	g_redisHost = redisHost;

	std::thread subscribeThread(ProcessSubscribeQueueTask);
	std::thread publishThread(ProcessPublishQueueTask);
	std::thread dummyThread(PublishDummyMessageTask);

	try
	{
		runService(channelSubscriptions);
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
}