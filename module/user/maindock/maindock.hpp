#ifndef DEAR_AREA_MODULE_MAINDOCK_DEF
#define DEAR_AREA_MODULE_MAINDOCK_DEF

#include <area/user.hpp>
#include <module/userModule.hpp>

class MainDockModule : public UserModule {
public: 
	MainDockModule(User *user);
	void removeModule();
	void paint();
	void recordCommand(Display *display);
private: 
	User *user;
	sk_sp<GrContext> skiaCtx;
	VkImage imageVk;
	VkDeviceMemory imageMemoryVk;
	sk_sp<SkSurface> surfaceSkia;
	GrBackendTexture backendTextureSkia;
	VkShaderModule vertexModuleVk;
	VkShaderModule fragmentModuleVk;
	VkPipelineLayout pipelineLayoutVk;
	VkPipeline graphicsPipelineVk;
	VkCommandBuffer *commandBufferVk;
	VkImageView imageViewVk;
	VkSampler samplerVk;
	VkDescriptorPool descriptorPoolVk;
	VkDescriptorSet descriptorSetVk;
};

#endif