#ifndef BACKENDBASE_DEF
#define BACKENDBASE_DEF

#include <iostream>

class BackendBase{
public:
	enum SurfType {XCB, DISPLAY};
	SurfType surf;

	BackendBase(SurfType surf);
	virtual ~BackendBase();
	virtual void paint() = 0;
};

#endif