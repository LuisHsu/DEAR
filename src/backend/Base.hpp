#ifndef BACKEND_BASE_DEF
#define BACKEND_BASE_DEF

class BackendBase{
public:
	enum LibType {AUTO_LIB, VULKAN, OPENGL};
	enum SurfType {XCB, DISPLAY};
	LibType lib;
	SurfType surf;

	BackendBase(LibType lib, SurfType surf);
	virtual ~BackendBase();
	virtual void run() = 0;
};

#endif