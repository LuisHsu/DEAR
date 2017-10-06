#include <iostream>
#include <AreaSocket.hpp>
#include <Greeter.hpp>

int main(int argc, char *argv[]){
	// Create server 
	AreaServer *greeterServer = new AreaServer("/tmp/dear_greeter");
	// Greeter
	Greeter greeter;
	// Listen
	greeterServer->start(&greeter);
	// Clean
	delete greeterServer;
	return 0;
}