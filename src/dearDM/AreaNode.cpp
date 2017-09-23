#include <AreaNode.hpp>

AreaNode::AreaNode(const char *path):
	messageQueue(new IPCMessageQueue(10))
{
/*** IPC socket ***/
	// Create socket
	socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
	// Fill address
	serverAddr.sun_family = AF_UNIX;
	strcpy(serverAddr.sun_path, path);
	clientAddr.sun_family = AF_UNIX;
	sprintf(clientAddr.sun_path, "dearDM-%d", getpid());
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
	evutil_make_socket_nonblocking(socketFd);
	eventBase = event_base_new();
	if(eventBase == nullptr){
		std::cerr << strerror(errno) << std::endl;
		close(socketFd);
		throw "[DearDM area node] Can't create event base.";
	}
	struct event *recvEvent = event_new(eventBase, socketFd, EV_READ|EV_PERSIST, socketReceive, nullptr);
	event_add(recvEvent, nullptr);
	event_base_dispatch(eventBase);
}

AreaNode::~AreaNode(){
	close(socketFd);
	unlink(clientAddr.sun_path);
	delete messageQueue;
}
void AreaNode::socketReceive(evutil_socket_t fd, short event, void *arg){
	IPCMessage header;
	std::vector<char> buffer;
	while(recv(fd, &header, sizeof(header), 0) > 0){
		buffer.resize(header.length);
		IPCMessage *message = (IPCMessage *)buffer.data();
		*message = header;
		int32_t received = sizeof(header);
		while(received < header.length){
			int curRecv = recv(fd, buffer.data() + received, header.length - received, 0);
			if(curRecv <= 0){
				std::cerr << "[DearDM area node] Abort receiving." << std::endl;
				break;
			}
			received += curRecv;
		}
		if(received == header.length){
			//(*messageHandler)(message);
			std::cout << received << std::endl;
		}
	}
}

void AreaNode::socketError(struct bufferevent *bev, short event, void *arg){

}