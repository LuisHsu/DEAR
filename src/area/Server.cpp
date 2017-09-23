#include <Server.hpp>

GreeterServer::GreeterServer(){
	// Create socket
	listenFd = socket(AF_UNIX, SOCK_STREAM, 0);
	// Fill address
	listenAddr.sun_family = AF_UNIX;
	strcpy(listenAddr.sun_path, "/tmp/dear_greeter");
	// Unlink
	unlink(listenAddr.sun_path);
	// Bind
	if(bind(listenFd, (struct sockaddr *)&listenAddr, sizeof(listenAddr)) < 0){
		close(listenFd);
		throw "[Greeter server] Can't bind socket.";
	}
}

GreeterServer::~GreeterServer(){
	if(clientFd >= 0){
		close(clientFd);
	}
}

void GreeterServer::startGreeter(){
	// Listen
	if(listen(listenFd, 5) < 0){
		close(listenFd);
		throw "[Greeter server] Can't listen server.";
	}
	// Accept
	clientFd = accept(listenFd, (struct sockaddr *)&clientAddr, &clientAddrlen);
	if(clientFd < 0){
		std::cerr << strerror(errno) << std::endl;
		close(listenFd);
		throw "[Greeter server] Error accept incoming client.";
	}
	// Close listen file descripter
	close(listenFd);
	// Receive message
	IPCMessage header;
	std::vector<char> buffer;
	while(recv(clientFd, &header, sizeof(header), 0) > 0){
		buffer.resize(header.length);
		IPCMessage *message = (IPCMessage *)buffer.data();
		*message = header;
		int32_t received = sizeof(header);
		while(received < header.length){
			int curRecv = recv(clientFd, buffer.data() + received, header.length - received, 0);
			if(curRecv <= 0){
				std::cerr << "[Greeter server] Abort receiving from backend." << std::endl;
				break;
			}
			received += curRecv;
		}
		if(received == header.length){
			(*messageHandler)(message);
		}
	}
	// Clean
	close(clientFd);
}