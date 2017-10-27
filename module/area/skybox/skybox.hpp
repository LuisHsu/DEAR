#ifndef DEAR_AREA_MODULE_SKYBOX_DEF
#define DEAR_AREA_MODULE_SKYBOX_DEF

#include <vulkan/vulkan.hpp>
#include <map>
#include <fstream>
#include <glm/glm.hpp>

#include <module/areaModule.hpp>
#include <area/area.hpp>
#include <area/user.hpp>

class UniformBufferObject {
public:
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class SkyBoxModule : public AreaModule {
public: 
	SkyBoxModule(Area *area);
	void userInit(void *userPtr);
	void removeModule();
	void recordCommand(VkCommandBuffer cmdBuffer, Display *display);
	VkShaderModule createShaderModule(const char *fileName, VkDevice device);
private: 
	Area *area;
	std::map<User*, VkCommandBuffer *> commandBuffers;
	VkShaderModule vertexModuleVk;
	VkShaderModule fragmentModuleVk;
	VkPipeline graphicsPipelineVk;
	VkBuffer uniformBufferVk;
	VkDeviceMemory uniformBufferMemoryVk;
	
};

#endif