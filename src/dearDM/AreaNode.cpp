#include <AreaNode.hpp>

AreaNode::AreaNode(const char *path, BackendBase *backend):
	backend(backend),
	messageQueue(new IPCMessageQueue(10)),
	isPainting(false)
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
	evutil_make_socket_nonblocking(socketFd);
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
	isPainting = false;
	paintThread->join();
	delete paintThread;
	unlink(clientAddr.sun_path);
	unlink(displayAddr.sun_path);
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
		case IPC_Connect:
			{
				// Create display socket
				displayFd = socket(AF_UNIX, SOCK_STREAM, 0);
				// Fill display address
				displayAddr.sun_family = AF_UNIX;
				sprintf(displayAddr.sun_path, "/tmp/dearDM-display-%d", getpid());
				// Unlink previous display file
				unlink(displayAddr.sun_path);
				// Bind display
				if(bind(displayFd, (struct sockaddr *)&displayAddr, sizeof(displayAddr)) < 0){
					close(displayFd);
					throw "[DearDM connect] Can't bind display socket.";
				}
				// Listen display
				if(listen(displayFd, 10) < 0){
					close(displayFd);
					throw "[DearDM connect] Can't listen display socket.";
				}
				// Display connect event
				evutil_make_socket_nonblocking(displayFd);
				AreaSocketReceiveArg *connectArg = new AreaSocketReceiveArg;
				connectArg->areaNode = this;
				connectArg->recvEvent = event_new(eventBase, displayFd, EV_READ|EV_PERSIST, displayConnect, connectArg);
				event_add(connectArg->recvEvent, nullptr);
				// Send connect message
				IPCConnectMessage connectMessage;
				connectMessage.type = IPC_Connect;
				connectMessage.length = sizeof(connectMessage);
				connectMessage.extent = backend->vkDisplayExtent;
				connectMessage.format = backend->vkSurfaceFormat.format;
				strcpy(connectMessage.displayFile, displayAddr.sun_path);
				if(send(socketFd, &connectMessage, connectMessage.length, 0) < 0){
					close(socketFd);
					throw "[DearDM area node] Can't send connect message.";
				}
			}
		break;
		default:
		break;
	}
}
void AreaNode::displayConnect(evutil_socket_t listenFd, short event, void *arg){
	AreaSocketReceiveArg *areaArg = (AreaSocketReceiveArg *)arg;
	// Accept
	int areaFd = accept(listenFd, nullptr, nullptr);
	if(areaFd >= 0){
		// Delete event and close listen socket
		event_del(areaArg->recvEvent);
		event_free(areaArg->recvEvent);
		close(listenFd);
		// Get fd
		areaArg->areaNode->displayFd = recvDisplayFd(areaFd);
		// Init texture
		areaArg->areaNode->backend->initTexture(areaArg->areaNode->displayFd);
		// Start paint thread
		areaArg->areaNode->isPainting = true;
		areaArg->areaNode->paintThread = new std::thread(runBackendPaint, areaArg->areaNode);
		delete areaArg;
	}else{
		std::cerr << strerror(errno) << std::endl;
	}
}
void AreaNode::runBackendPaint(AreaNode *areaNode){
	while(areaNode->isPainting){
		areaNode->backend->paint();
	}
}
int AreaNode::recvDisplayFd(int sockFd){
	int newfd, recvNum;
	char *ptr;
	char buf[1024];
	struct iovec iov;
	struct msghdr msg;
	struct cmsghdr *cmptr;
	iov.iov_base = buf;
	iov.iov_len = sizeof(buf);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_name = nullptr;
	msg.msg_namelen = 0;
	if((cmptr = (struct cmsghdr *)malloc(CMSG_LEN(sizeof(int)))) == nullptr){
		throw "[DearDM receive display fd] Can't alloc cmptr.";
	}
	msg.msg_control = cmptr;
	msg.msg_controllen = CMSG_LEN(sizeof(int));
	if((recvNum = recvmsg(sockFd, &msg, 0)) <= 0) {
		throw "[DearDM receive display fd] Error receive message.";
	}
	for(ptr = buf; ptr < &buf[recvNum]; ){
		if(*ptr++ == 0) {
			if(ptr != &buf[recvNum-1]){
				throw "[DearDM receive display fd] Message format error.";
			}
			if((*ptr & 0xFF) == 0) {
				if (msg.msg_controllen != CMSG_LEN(sizeof(int)))
					throw "[DearDM receive display fd] No file descriptor.";
				newfd = *(int *)CMSG_DATA(cmptr);
			}else{
				newfd = -1;
			}
			recvNum -= 2;
		}
	}
	return newfd;
}