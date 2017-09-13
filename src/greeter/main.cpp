#include <iostream>

extern "C"{
	#include <wayland-client.h>
}

int main(int argc, char *argv[]){
	// Get display
	struct wl_display *display = wl_display_connect(nullptr);
	if(!display){
		std::cerr << "[DEAR Greeter] Can't get wayland display." << std::endl;
		return -1;
	}
	// 
	// Clean
	wl_display_disconnect(display);
	return 0;
}