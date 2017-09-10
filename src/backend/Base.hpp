#ifndef BACKEND_BASE_DEF
#define BACKEND_BASE_DEF

class BackendBase{
public:
	enum LibType {AUTO, VULKAN, OPENGL};
	LibType lib;

	BackendBase(LibType lib);
	virtual ~BackendBase();
	virtual void run() = 0;
};

#endif