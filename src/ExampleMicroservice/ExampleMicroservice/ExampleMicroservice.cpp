
#include <MicroserviceLib/Microservice.h>

#include "ReactiveAgent.h"

int main(int argc, char* argv[])
{
	// Instantiate "Agents" here
	ReactiveAgent agent;

	// Start Communications - This should always be the last call on the main thread
	return Microservice::start(argc, argv);
}