#include <AreaNode.hpp>

AreaNode::AreaNode(const char *path){
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
		throw "[Backend area node] Can't bind socket.";
	}
}

AreaNode::~AreaNode(){
	close(socketFd);
	unlink(clientAddr.sun_path);
}

void AreaNode::run(){
	// Connect
	if(connect(socketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
		std::cerr << strerror(errno) << std::endl;
		close(socketFd);
		throw "[Backend area node] Can't connect to server.";
	}
	IPCMessage connectMessage;
	connectMessage.type = IPC_Connect;
	connectMessage.length = sizeof(connectMessage);
	if(sendMsg(*connectMessage) < 0){
		close(socketFd);
		throw "[Backend area node] Can't send connect message.";
	}
}

ssize_t AreaNode::sendMsg(IPCMessage *message){
	return send(socketFd, message, message->length, 0);
}