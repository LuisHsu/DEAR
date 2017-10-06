#ifndef GREETER_DEF
#define GREETER_DEF

#include <vulkan/vulkan.hpp>
#include <IPCMessage.hpp>
#include <AreaSocket.hpp>

class Greeter;

class GreeterClient{
public:
};

class Greeter : public AreaServerHandler{
public:
	Greeter();
	~Greeter();
private:
	void handleMessage(IPCMessage *message, AreaClient *client);
	void paint(AreaClient *client);
};

#endif
