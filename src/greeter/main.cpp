#include <iostream>
#include <area/Server.hpp>

int main(int argc, char *argv[]){
	// Create 
	AreaServer *greeterServer = new AreaServer();
	// Listen
	greeterServer->start();
	// Clean
	delete greeterServer;
	return 0;
}