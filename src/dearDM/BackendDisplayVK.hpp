#ifndef BACKENDISPLAYVK_DEF
#define BACKENDISPLAYVK_DEF

#include <iostream>
#include <cstring>
#include <vulkan/vulkan.hpp>
#include <unistd.h>

#include <BackendBase.hpp>

class BackendDisplayVK : public BackendBase{
public:
	BackendDisplayVK();
	~BackendDisplayVK();
	void paint();
	void initTexture(int fd);
private:
};

#endif