#include <Greeter.hpp>

Greeter::Greeter(){
}
Greeter::~Greeter(){
	vkDestroyDevice(vkDevice, nullptr);
	vkDestroyInstance(vkInstance, nullptr);
}
void Greeter::handleMessage(IPCMessage *message, AreaClient *client){
	switch(message->type){
		case IPC_Connect:{
			GreeterClient *greeterClient = new GreeterClient;
			client->data = greeterClient;
			client->initClient(message);
			paint(client);
		}break;
		default:
		break;
	}
}

void Greeter::paint(AreaClient *client){
	// Wait for finished
	sem_wait(&(client->transferFinishedSem));
	// Submit command buffer
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_TRANSFER_BIT};
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &(client->vkCommandBuffer);
	submitInfo.signalSemaphoreCount = 0;
	switch(vkQueueSubmit(vkGraphicsQueue, 1, &submitInfo, client->vkStartTransferFence)){
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			throw "[Vulkan queue submit] VK_ERROR_OUT_OF_HOST_MEMORY";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			throw "[Vulkan queue submit] VK_ERROR_OUT_OF_DEVICE_MEMORY";
		break;
		case VK_ERROR_DEVICE_LOST:
			throw "[Vulkan queue submit] VK_ERROR_DEVICE_LOST";
		break;
		default:
		break;
	}
}
