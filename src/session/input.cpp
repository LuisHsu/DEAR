#include "input.hpp"

Input::Input(uv_loop_t *loop):
	loop(loop)
{
	// Create new udev context
	struct udev *udev_ = udev_new();
	// libinput interface
	struct libinput_interface interface_ = {
		.open_restricted = open_restricted,
		.close_restricted = close_restricted
	};
	// Create libinput context
	inputCtx = libinput_udev_create_context(&interface_, nullptr, udev_);
	// Get systemd seats
	char **seats;
	sd_get_seats(&seats);
	// Assign seat
	libinput_udev_assign_seat(inputCtx, seats[0]);
	free(seats);
	// Get fd
	int inputfd = libinput_get_fd(inputCtx);
	// Uv poll
	uvPoll.data = this;
	uv_poll_init(loop, &uvPoll, inputfd);
	// Start poll
	uv_poll_start(&uvPoll, UV_READABLE, [](uv_poll_t* handle, int status, int events){
		Input *input = (Input *) handle->data;
		libinput_dispatch(input->inputCtx);
		struct libinput_event *event = libinput_get_event(input->inputCtx);
		switch(libinput_event_get_type(event)){
			case LIBINPUT_EVENT_KEYBOARD_KEY:
				input->keyboardKey(event);
			break;
			case LIBINPUT_EVENT_POINTER_MOTION:
				input->pointerMotion(event);
			break;
			case LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE:
				input->pointerMotionAbsolute(event);
			break;
			case LIBINPUT_EVENT_POINTER_BUTTON:
				input->pointerButton(event);
			break;
			case LIBINPUT_EVENT_POINTER_AXIS:
				input->pointerAxis(event);
			break;
			default: 
			break;
		}
		if(event){
			libinput_event_destroy(event);
		}
	});
}

int Input::open_restricted(const char *path, int flags, void *user_data){
	int fd = open(path, flags);
	if(fd == -1){
		std::cout << path << " open error :";
		perror("");
	}
	return fd;
}

void Input::close_restricted(int fd, void *user_data){
	close(fd);
}

void Input::keyboardKey(struct libinput_event *event){

}
void Input::pointerMotion(struct libinput_event *event){

}
void Input::pointerMotionAbsolute(struct libinput_event *event){

}
void Input::pointerButton(struct libinput_event *event){

}
void Input::pointerAxis(struct libinput_event *event){
	
}