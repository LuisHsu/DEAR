#include <Server.hpp>

AreaServer::AreaServer(const char *path){
	// Create socket
	listenFd = socket(AF_UNIX, SOCK_STREAM, 0);
	// Fill address
	listenAddr.sun_family = AF_UNIX;
	strcpy(listenAddr.sun_path, path);
	// Unlink
	unlink(listenAddr.sun_path);
	// Bind
	if(bind(listenFd, (struct sockaddr *)&listenAddr, sizeof(listenAddr)) < 0){
		close(listenFd);
		throw "[Area server] Can't bind socket.";
	}
}

AreaServer::~AreaServer(){
	if(listenFd >= 0){
		close(listenFd);
	}
}

void AreaServer::start(){
	// Set event base
	evutil_make_socket_nonblocking(listenFd);
	eventBase = event_base_new();
	if(eventBase == nullptr){
		std::cerr << strerror(errno) << std::endl;
		close(listenFd);
		throw "[Area server] Can't create event base.";
	}
	struct event *acceptEvent = event_new(eventBase, listenFd, EV_READ|EV_PERSIST, socketAccept, this);
	event_add(acceptEvent, nullptr);
	// Listen
	if(listen(listenFd, 10) < 0){
		close(listenFd);
		throw "[Area server] Can't listen server.";
	}
	event_base_dispatch(eventBase);
}

void AreaServer::socketAccept(evutil_socket_t listenFd, short event, void *arg){
	AreaServer *server = (AreaServer *)arg;
	// Accept
	server->clientFd = accept(listenFd, (struct sockaddr *)&(server->clientAddr), &(server->clientAddrlen));
	if(server->clientFd >= 0){
		// Add new event
		struct event *receiveEvent = event_new(server->eventBase, server->clientFd, EV_READ|EV_PERSIST, socketReceive, nullptr);
		AreaSocketReceiveArg *receiveArg = new AreaSocketReceiveArg;
		receiveArg->areaServer = server;
		receiveArg->receiveEvent = receiveEvent;
		event_assign(receiveEvent, server->eventBase, server->clientFd, EV_READ|EV_PERSIST, socketReceive, receiveArg);
		event_add(receiveEvent, nullptr);
		// Send connect message
		IPCMessage connectMessage;
		connectMessage.type = IPC_Connect;
		connectMessage.length = sizeof(connectMessage);
		if(send(server->clientFd, &connectMessage, connectMessage.length, 0) < 0){
			close(server->clientFd);
			throw "[Area server] Can't send connect message.";
		}
	}
}

void AreaServer::socketReceive(evutil_socket_t clientFd, short event, void *arg){
	AreaSocketReceiveArg *receiveArg = (AreaSocketReceiveArg *)arg;
	IPCMessage header;
	std::vector<char> buffer;
	if(recv(clientFd, &header, sizeof(header), 0) > 0){
		buffer.resize(header.length);
		IPCMessage *message = (IPCMessage *)buffer.data();
		*message = header;
		int32_t received = sizeof(header);
		while(received < header.length){
			int curRecv = recv(clientFd, buffer.data() + received, header.length - received, 0);
			if(curRecv <= 0){
				std::cerr << "[Area server] Abort receiving from backend." << std::endl;
				break;
			}
			received += curRecv;
		}
		if(received == header.length){
			receiveArg->areaServer->messageHandler->handleMessage(message);
		}
	}else{
		event_del(receiveArg->receiveEvent);
		event_free(receiveArg->receiveEvent);
		delete receiveArg;
		close(clientFd);
	}
}

void AreaServer::setHandler(AreaServerHandler *handler){
	messageHandler = handler;
}