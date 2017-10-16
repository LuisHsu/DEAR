#include "area.hpp"

#include <iostream>

void Area::handleMessage(Message *message, void *server){
	switch(message->type){
		case KeyUp_request:
			std::cout << ((KeyboardRequest *) message)->key << std::endl;
		break;
		default:
		break;
	}
}