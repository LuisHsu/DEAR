#include "session.hpp"

Session::Session(){

}

void Session::handleMessage(Message *message, void *deliver, DeliverType type, void *data){
	IPCClient *ipcClient = (IPCClient *)deliver;
	switch(message->type){
		case DEAR_IPC_Connect_notice:
			std::cout << "Area noticed" << std::endl;
		break;
		case DEAR_Close_notice:
			ipcClient->stop();
		break;
		default:
		break;
	}
}

void Session::messageReady(void *deliver, DeliverType type){
	switch(type){
		case DEAR_MESSAGE_IPCClient:{
			IPCClient *client = (IPCClient *)deliver;
			ipcClient = client;
			deliverType = type;
			// Send connect request
			Message *msg = new Message;
			msg->type = DEAR_IPC_Connect_request;
			msg->length = sizeof(*msg);
			client->sendMessage(msg, [](uv_write_t* req, int status){
				delete (Message *)req->data;
				delete req;
			}, msg);
		}
		break;
		case DEAR_MESSAGE_TCPClient:{
			deliverType = type;
		}
		break;
		default: 
		break;
	}
}
