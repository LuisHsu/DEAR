#ifndef BACKEND_BASE_DEF
#define BACKEND_BASE_DEF

class BackendBase{
public:
	enum LibType {AUTO, VULKAN, OPENGL};
	LibType lib;

	BackendBase(LibType lib);
};

#endif