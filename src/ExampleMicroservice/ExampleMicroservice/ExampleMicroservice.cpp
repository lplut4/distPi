#include <MicroserviceLib/Microservice.h>
#include "ReactiveAgent.h"

int main(int argc, char* argv[])
{
	// Instantiate "Agents" here
	ReactiveAgent agent1;
	ReactiveAgent agent2;
	ReactiveAgent agent3;
	ReactiveAgent agent4;
	ReactiveAgent agent5;
	ReactiveAgent agent6;

	// Start Communications
	return Microservice::start(argc, argv);
}