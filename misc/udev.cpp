#include <iostream>
#include <cstdlib>
#include <cstdio>

extern "C"{
	#include <libudev.h>
	#include <libinput.h>
	#include <systemd/sd-login.h>
	#include <uv.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>
}

int open_restricted_(const char *path, int flags, void *user_data){
	int fd = open(path, flags);
	if(fd == -1){
		std::cout << path << " open error :";
		perror("");
	}
	return fd;
}

void close_restricted_(int fd, void *user_data){
	close(fd);
}

void addDevice(struct libinput_event *event){
	struct libinput_device *device = libinput_event_get_device(event);
	std::cout << libinput_device_get_sysname(device) << " Support: ";
	// Keyboard
	if(libinput_device_has_capability(device, LIBINPUT_DEVICE_CAP_KEYBOARD)){
		std::cout << " Keyboard ";
	}
	// Pointer
	if(libinput_device_has_capability(device, LIBINPUT_DEVICE_CAP_POINTER)){
		std::cout << " Pointer ";
	}
	// Gesture
	if(libinput_device_has_capability(device, LIBINPUT_DEVICE_CAP_GESTURE)){
		std::cout << " Gesture ";
	}
	std::cout << std::endl;
}

void pointerMotion(struct libinput_event *event){
	struct libinput_event_pointer *pointerEvent = libinput_event_get_pointer_event(event);
	std::cout << "Pointer (" << libinput_event_pointer_get_dx(pointerEvent);
	std::cout << "," << libinput_event_pointer_get_dy(pointerEvent) << ")" << std::endl;
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
	// Get fd
	int inputfd = libinput_get_fd(libinput_);
	// Create libuv loop
	uv_loop_t loop;
	uv_loop_init(&loop);
	// libuv poll event
	uv_poll_t uvpoll;
	uvpoll.data = libinput_;
	uv_poll_init(&loop, &uvpoll, inputfd);
	uv_poll_start(&uvpoll, UV_READABLE, [](uv_poll_t* handle, int status, int events){
		struct libinput *input = (struct libinput *) handle->data;
		libinput_dispatch(input);
		if(libinput_next_event_type(input) != LIBINPUT_EVENT_NONE){
			struct libinput_event *event = libinput_get_event(input);
			switch(libinput_event_get_type(event)){
				case LIBINPUT_EVENT_DEVICE_ADDED:
					addDevice(event);
				break;
				case LIBINPUT_EVENT_DEVICE_REMOVED:
					std::cout << "LIBINPUT_EVENT_DEVICE_REMOVED" << std::endl;
				break;	
				case LIBINPUT_EVENT_KEYBOARD_KEY:
					std::cout << "LIBINPUT_EVENT_KEYBOARD_KEY" << std::endl;
				break;	
				case LIBINPUT_EVENT_POINTER_MOTION:
					pointerMotion(event);
				break;		
				case LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE:
					std::cout << "LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE" << std::endl;
				break;	
				case LIBINPUT_EVENT_POINTER_BUTTON:
					std::cout << "LIBINPUT_EVENT_POINTER_BUTTON" << std::endl;
				break;	
				case LIBINPUT_EVENT_POINTER_AXIS:
					std::cout << "LIBINPUT_EVENT_POINTER_AXIS" << std::endl;
				break;	
				case LIBINPUT_EVENT_TOUCH_DOWN:
					std::cout << "LIBINPUT_EVENT_TOUCH_DOWN" << std::endl;
				break;	
				case LIBINPUT_EVENT_TOUCH_UP:
					std::cout << "LIBINPUT_EVENT_TOUCH_UP" << std::endl;
				break;		
				case LIBINPUT_EVENT_TOUCH_MOTION:
					std::cout << "LIBINPUT_EVENT_TOUCH_MOTION" << std::endl;
				break;
				case LIBINPUT_EVENT_TOUCH_CANCEL:
					std::cout << "LIBINPUT_EVENT_TOUCH_CANCEL" << std::endl;
				break;	 	
				case LIBINPUT_EVENT_TOUCH_FRAME:
					std::cout << "LIBINPUT_EVENT_TOUCH_FRAME" << std::endl;
				break;	 	
				case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN:
					std::cout << "LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN" << std::endl;
				break;	 	
				case LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE:
					std::cout << "LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE" << std::endl;
				break;	 	
				case LIBINPUT_EVENT_GESTURE_SWIPE_END:
					std::cout << "LIBINPUT_EVENT_GESTURE_SWIPE_END" << std::endl;
				break;	 	
				case LIBINPUT_EVENT_GESTURE_PINCH_BEGIN:
					std::cout << "LIBINPUT_EVENT_GESTURE_PINCH_BEGIN" << std::endl;
				break;	 	
				case LIBINPUT_EVENT_GESTURE_PINCH_UPDATE:
					std::cout << "LIBINPUT_EVENT_GESTURE_PINCH_UPDATE" << std::endl;
				break;	 	
				case LIBINPUT_EVENT_GESTURE_PINCH_END:
					std::cout << "LIBINPUT_EVENT_GESTURE_PINCH_END" << std::endl;
				break;	 	
				default:
				break;
			}
			libinput_event_destroy(event);
		}
	});
	// Run loop
	uv_run(&loop, UV_RUN_DEFAULT);
	uv_loop_close(&loop);
	return 0;
}
