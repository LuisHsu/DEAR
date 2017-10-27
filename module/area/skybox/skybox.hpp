#ifndef DEAR_AREA_MODULE_SKYBOX_DEF
#define DEAR_AREA_MODULE_SKYBOX_DEF

#include <vulkan/vulkan.hpp>
#include <map>
#include <fstream>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
	void pointerMotion(Message *message, void *deliver, MessageHandler::DeliverType type, void *data);
private: 
	Area *area;
	std::map<User*, VkCommandBuffer *> commandBuffers;
	VkShaderModule vertexModuleVk;
	VkShaderModule fragmentModuleVk;
	VkPipeline graphicsPipelineVk;
	VkBuffer uniformBufferVk;
	VkDeviceMemory uniformBufferMemoryVk;
	VkDescriptorPool descriptorPoolVk;
	VkDescriptorSet descriptorSetVk;
	VkPipelineLayout pipelineLayoutVk;
	UniformBufferObject uniformBufferObject;
	VkShaderModule createShaderModule(const char *fileName, VkDevice device);
	uint32_t findMemoryType(VkPhysicalDevice phyDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
};

#endif