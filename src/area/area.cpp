#include "area.hpp"

#include <iostream>

Area::Area(){
	
}

void Area::handleMessage(Message *message, void *deliver, DeliverType type, void *data){
	switch(message->type){
		case DEAR_IPC_Connect_request:
			ipcConnect(message, deliver, type, data);
		break;
		case DEAR_KeyUp_request:
			std::cout << "KeyUp: " << ((KeyboardRequest *) message)->key << std::endl;
		break;
		case DEAR_KeyDown_request:
			std::cout << "KeyDown: " << ((KeyboardRequest *) message)->key << std::endl;
			if(((KeyboardRequest *) message)->key == DEAR_KEY_Q){
				((IPCServer *)deliver)->stop();
			}
		break;
		case DEAR_PointerMotion_request:
			std::cout << "Motion: (" << ((PointerMotionRequest *) message)->dx << ", " << ((PointerMotionRequest *) message)->dy << ")" << std::endl;
		break;
		case DEAR_PointerUp_request:
			std::cout << "PointerUp: " << ((PointerButtonRequest *) message)->button << std::endl;
		break;
		case DEAR_PointerDown_request:
			std::cout << "PointerDown: " << ((PointerButtonRequest *) message)->button << std::endl;
		break;
		case DEAR_PointerAxis_request:
			std::cout << "PointerAxis: ";
			switch(((PointerAxisRequest *) message)->source){
				case PointerAxisRequest::AxisSource::DEAR_POINTER_AXIS_SRC_Wheel:
					std::cout << "Wheel ";
				break;
				case PointerAxisRequest::AxisSource::DEAR_POINTER_AXIS_SRC_Finger:
					std::cout << "Finger ";
				break;
				case PointerAxisRequest::AxisSource::DEAR_POINTER_AXIS_SRC_Continuous:
					std::cout << "Continuous ";
				break;
			}
			if(((PointerAxisRequest *) message)->axis & PointerAxisRequest::Axis::DEAR_POINTER_AXIS_Horizontal){
				std::cout << "Horizontal: " << ((PointerAxisRequest *) message)->valueH;
			}
			if(((PointerAxisRequest *) message)->axis & PointerAxisRequest::Axis::DEAR_POINTER_AXIS_Vertical){
				std::cout << "Vertical: " << ((PointerAxisRequest *) message)->valueV;
			}
			std::cout << std::endl;
		break;
		default:
		break;
	}
}
void Area::ipcConnect(Message *message, void *deliver, DeliverType type, void *data){
	// Get display
	display = getDisplay();
	display->init();
	// Send connect notice
	Message *msg = new Message;
	msg->type = DEAR_IPC_Connect_notice;
	msg->length = sizeof(*msg);
	sendMessage(msg, deliver, type, data);
}
void Area::sendMessage(Message *message, void *deliver, DeliverType type, void *data, uv_write_cb callback, void *callbackData){
	switch(type){
		case MessageHandler::DeliverType::DEAR_MESSAGE_IPCserver:
			((IPCServer *)deliver)->sendMessage(message, callback, callbackData);
		break;
		case MessageHandler::DeliverType::DEAR_MESSAGE_TCPserver:
			((TCPServer *)deliver)->sendMessage(message, (uv_stream_t *)data, callback, callbackData);
		break;
		default:
		break;
	}
}
void Area::messageReady(void *deliver, DeliverType type){
	switch(type){
		case DEAR_MESSAGE_IPCserver:
			std::cout << "IPC listening." << std::endl;
		break;
		case DEAR_MESSAGE_TCPserver:
			std::cout << "TCP listening." << std::endl;
		break;
		default:
		break;
	}
}

Display *Area::getDisplay(){
	Display *ret = nullptr;
	try{
		ret = new DisplayXcb();
	}catch(std::exception e){
		std::cerr << "XCB display init error: " << e.what() << ". Try direct display." << std::endl;
		ret = nullptr;
	}catch(const char *e){
		std::cerr << "XCB display init error: " << e << ". Try direct display." << std::endl;
		ret = nullptr;
	}
	return ret;
}