#include "session.hpp"

void Session::handleMessage(Message *message, void *server){
	switch(message->type){
		case IPC_Connect_rep:
		break;
		default:
		break;
	}
}