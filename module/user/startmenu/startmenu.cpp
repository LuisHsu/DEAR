#include "startmenu.hpp"

StartMenuModule::StartMenuModule(User *user):user(user){
	Display *display = user->display;
/*** Image ***/
	// Create image
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.extent.width = display->displayExtentVk.width;
	imageCreateInfo.extent.height = display->displayExtentVk.height;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.format = display->surfaceFormatVk.format;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	if (vkCreateImage(display->deviceVk, &imageCreateInfo, nullptr, &imageVk) != VK_SUCCESS) {
		throw "[Startmenu image] Error create image";
	}
	// Image memory
	VkMemoryRequirements imageMemRequirements;
	vkGetImageMemoryRequirements(display->deviceVk, imageVk, &imageMemRequirements);
	VkMemoryAllocateInfo imageMemAllocInfo = {};
	imageMemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	imageMemAllocInfo.allocationSize = imageMemRequirements.size;
	imageMemAllocInfo.memoryTypeIndex = display->findMemoryType(imageMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (vkAllocateMemory(display->deviceVk, &imageMemAllocInfo, nullptr, &imageMemoryVk) != VK_SUCCESS) {
		throw "[Startmenu image] Error create image memory";
	}
	vkBindImageMemory(display->deviceVk, imageVk, imageMemoryVk, 0);
/*=== Skia ===*/
	// Context
	GrVkBackendContext* vkBackendCtxSkia = display->createSkiaContext();
	sk_sp<GrContext> contextSkia = GrContext::MakeVulkan(vkBackendCtxSkia);
	// Vulkan image info
	GrVkImageInfo imageInfoSkia = {};
	imageInfoSkia.fImage = imageVk;
	imageInfoSkia.fAlloc.fMemory = imageMemoryVk;
	imageInfoSkia.fAlloc.fOffset = 0;
	imageInfoSkia.fAlloc.fSize = imageMemAllocInfo.allocationSize;
	imageInfoSkia.fImageTiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfoSkia.fImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfoSkia.fFormat = display->surfaceFormatVk.format;
	imageInfoSkia.fLevelCount = 1;
	// Surface
	backendTextureSkia = GrBackendTexture(display->displayExtentVk.width, display->displayExtentVk.height, imageInfoSkia);
	surfaceSkia = SkSurface::MakeFromBackendTexture(
		contextSkia.get(), 
		backendTextureSkia, 
		kTopLeft_GrSurfaceOrigin, 
		1, 
		SkColorSpace::MakeSRGB(), 
		new SkSurfaceProps(SkSurfaceProps::InitType::kLegacyFontHost_InitType)
	);
	if (!surfaceSkia) {
        SkDebugf("SkSurface::MakeRenderTarget returned null\n");
    }
}

void StartMenuModule::removeModule(){
	delete this;
}

UserModule *createUserModule(void *user){
	return new StartMenuModule((User *)user);
}