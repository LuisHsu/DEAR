#include "session.hpp"

void Session::handleMessage(Message *message, void *server){
	switch(message->type){
		case IPC_Connect_notice:
		break;
		default:
		break;
	}
}