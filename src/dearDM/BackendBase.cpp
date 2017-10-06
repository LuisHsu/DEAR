#include <BackendBase.hpp>

BackendBase::BackendBase(SurfType surf):surf(surf){
}

BackendBase::~BackendBase(){
}

int32_t BackendBase::findMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags properties){
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(vkPhyDevice, &memProperties);
	for(int32_t i = 0; i < 32; ++i){
		if ((memoryTypeBits & (1 << i)) && ((memProperties.memoryTypes[i].propertyFlags & properties) == properties)){
			return i;
		}
	}
	return -1;
}