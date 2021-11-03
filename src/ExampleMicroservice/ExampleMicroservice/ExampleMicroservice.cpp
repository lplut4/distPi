
#include "ExampleAgent.h"
#include "Microservice.h"

int main(int argc, char* argv[])
{
	// Instantiate "Agents" here
	ExampleAgent agent1;
	ExampleAgent agent2;
	ExampleAgent agent3;
	ExampleAgent agent4;
	ExampleAgent agent5;
	ExampleAgent agent6;

	// Start Communications
	return Microservice::start(argc, argv);
}