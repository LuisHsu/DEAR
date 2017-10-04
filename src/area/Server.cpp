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

void AreaServer::start(AreaServerHandler *handler){
	this->handler = handler;
	// Set event base
	evutil_make_socket_nonblocking(listenFd);
	eventBase = event_base_new();
	if(eventBase == nullptr){
		std::cerr << strerror(errno) << std::endl;
		close(listenFd);
		throw "[Area server] Can't create event base.";
	}
	acceptEvent = event_new(eventBase, listenFd, EV_READ|EV_PERSIST, socketAccept, this);
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
	struct sockaddr_un clientAddr;
	uint32_t clientAddrLen;
	int clientFd = accept(listenFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if(clientFd >= 0){
		AreaClient *newClient = new AreaClient(
			server->clients.size(),
			clientFd,
			clientAddr,
			clientAddrLen,
			server,
			server->eventBase,
			server->handler
		);
		server->clients.push_back(newClient);
	}
}

AreaClient::AreaClient(
	uint32_t index,
	int fd,
	struct sockaddr_un addr,
	uint32_t addrlen,
	AreaServer *server,
	struct event_base *eventBase,
	AreaServerHandler *handler
):
	index(index), clientFd(fd), addr(addr), addrlen(addrlen), server(server), messageHandler(handler)
{
	// Add new event
	receiveEvent = event_new(eventBase, clientFd, EV_READ|EV_PERSIST, socketReceive, this);
	event_add(receiveEvent, nullptr);
	// Send connect message
	IPCMessage connectMessage;
	connectMessage.type = IPC_Connect;
	connectMessage.length = sizeof(connectMessage);
	if(send(clientFd, &connectMessage, connectMessage.length, 0) < 0){
		close(clientFd);
		throw "[Area client] Can't send connect message.";
	}
}

void AreaClient::socketReceive(evutil_socket_t fd, short event, void *arg){
	AreaClient *client = (AreaClient *)arg;
	IPCMessage header;
	std::vector<char> buffer;
	if(recv(fd, &header, sizeof(header), 0) > 0){
		buffer.resize(header.length);
		IPCMessage *message = (IPCMessage *)buffer.data();
		*message = header;
		int32_t received = sizeof(header);
		while(received < header.length){
			int curRecv = recv(fd, buffer.data() + received, header.length - received, 0);
			if(curRecv <= 0){
				std::cerr << "[Area client] Abort receiving from backend." << std::endl;
				break;
			}
			received += curRecv;
		}
		if(received == header.length){
			client->messageHandler->handleMessage(message, client);
		}
	}else{
		event_del(client->receiveEvent);
		event_free(client->receiveEvent);
		close(client->clientFd);
		client->server->eraseClient(client->index);
	}
}
void AreaServer::eraseClient(uint32_t index){
	std::list<AreaClient *>::iterator it = clients.begin();
	advance(it, index);
	delete *it;
	clients.erase(it);
}

ssize_t AreaClient::sendMessage(IPCMessage *message){
	return send(clientFd, message, message->length, 0);
}