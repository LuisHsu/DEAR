#include <iostream>
#include <cstdlib>

extern "C"{
	#include <libudev.h>
	#include <libinput.h>
	#include <systemd/sd-login.h>
	#include <uv.h>
}

int open_restricted_(const char *path, int flags, void *user_data){
	std::cout << path << std::endl;
	return -1;
}

void close_restricted_(int fd, void *user_data){
	std::cout << "close" << std::endl;
}

int main(void){
	// Create new udev context
	struct udev *udev_ = udev_new();
	// libinput interface
	struct libinput_interface interface_ = {
		.open_restricted = open_restricted_,
		.close_restricted = close_restricted_
	};
	// Create libinput context
	struct libinput *libinput_ = libinput_udev_create_context(&interface_, nullptr, udev_);
	// Get systemd seats
	char **seats;
	int seatCount = sd_get_seats(&seats);
	// Assign seat
	libinput_udev_assign_seat(libinput_, seats[0]);
	free(seats);
	return 0;
}
