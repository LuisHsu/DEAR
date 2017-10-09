#ifndef REQUEST_DEF
#define REQUEST_DEF

enum MsgType{
	IPC_Connect_req
};

class Message{
public:
	MsgType type;
	int32_t length;
};

#endif