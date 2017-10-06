#include <AreaNode.hpp>

AreaNode::AreaNode(const char *path, BackendBase *backend):
	backend(backend),
	messageQueue(new IPCMessageQueue(10))
{
/*** IPC socket ***/
	// Create socket
	socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
	// Fill address
	serverAddr.sun_family = AF_UNIX;
	strcpy(serverAddr.sun_path, path);
	clientAddr.sun_family = AF_UNIX;
	sprintf(clientAddr.sun_path, "/tmp/dearDM-%d", getpid());
	// Bind
	if(bind(socketFd, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) < 0){
		close(socketFd);
		throw "[DearDM area node] Can't bind socket.";
	}
	// Connect
	if(connect(socketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
		std::cerr << strerror(errno) << std::endl;
		close(socketFd);
		throw "[DearDM area node] Can't connect to server.";
	}
/*** Event ***/
	eventBase = event_base_new();
	if(eventBase == nullptr){
		std::cerr << strerror(errno) << std::endl;
		close(socketFd);
		throw "[DearDM area node] Can't create event base.";
	}
	AreaSocketReceiveArg *recvArg = new AreaSocketReceiveArg;
	recvArg->areaNode = this;
	recvArg->recvEvent = event_new(eventBase, socketFd, EV_READ|EV_PERSIST, socketReceive, recvArg);
	event_add(recvArg->recvEvent, nullptr);
	event_base_dispatch(eventBase);
}
AreaNode::~AreaNode(){
	unlink(clientAddr.sun_path);
	delete messageQueue;
}
void AreaNode::socketReceive(evutil_socket_t socketFd, short event, void *arg){
	AreaSocketReceiveArg *recvArg = (AreaSocketReceiveArg *)arg;
	IPCMessage header;
	std::vector<char> buffer;
	if(recv(socketFd, &header, sizeof(header), 0) > 0){
		buffer.resize(header.length);
		IPCMessage *message = (IPCMessage *)buffer.data();
		*message = header;
		int32_t received = sizeof(header);
		while(received < header.length){
			int curRecv = recv(socketFd, buffer.data() + received, header.length - received, 0);
			if(curRecv <= 0){
				std::cerr << "[DearDM area node] Abort receiving." << std::endl;
				break;
			}
			received += curRecv;
		}
		if(received == header.length){
			recvArg->areaNode->messageHandler(message);
		}
	}else{
		event_del(recvArg->recvEvent);
		event_free(recvArg->recvEvent);
		delete recvArg;
		close(socketFd);
	}
}
void AreaNode::messageHandler(IPCMessage *message){
	switch(message->type){
		case IPC_Connect:{
			// Init texture
			backend->initTexture();
			// Send connect message
			IPCConnectMessage connectMessage;
			connectMessage.type = IPC_Connect;
			connectMessage.length = sizeof(connectMessage);
			connectMessage.extent = backend->vkDisplayExtent;
			connectMessage.format = backend->vkSurfaceFormat.format;
			if(send(socketFd, &connectMessage, connectMessage.length, 0) < 0){
				close(socketFd);
				throw "[DearDM area node] Can't send connect message.";
			}
		}break;
		case IPC_Frame:{
			IPCFrameMessage *frameMessage = (IPCFrameMessage *)message;
			frameMessage->dataBytes = (char *)&(frameMessage->dataBytes);
			backend->paint(frameMessage);
		}break;
		default:
		break;
	}
}