#include "session.hpp"

void Session::handleMessage(Message *message, void *deliver, DeliverType type, void *data){
	IPCClient *client = (IPCClient *)deliver;
	switch(message->type){
		case DEAR_Close_notice:
			client->stop();
		break;
		default:
		break;
	}
}