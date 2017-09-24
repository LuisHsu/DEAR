#include <iostream>
#include <Server.hpp>
#include <Greeter.hpp>

int main(int argc, char *argv[]){
	// Create server 
	AreaServer *greeterServer = new AreaServer("/tmp/dear_greeter");
	// Greeter
	Greeter greeter(greeterServer);
	// Listen
	greeterServer->start();
	// Clean
	delete greeterServer;
	return 0;
}