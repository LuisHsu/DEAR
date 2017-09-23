#ifndef IPCMESSAGE_DEF
#define IPCMESSAGE_DEF

enum IPCType{
	IPC_Connect
};

class IPCMessage{
public:
	IPCType type;
	int32_t length;
};

#endif