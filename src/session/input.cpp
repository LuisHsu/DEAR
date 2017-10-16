#include "input.hpp"

Input::Input(uv_loop_t *loop, IPCClient *client):
	loop(loop), client(client)
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
		if(event){
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
	struct libinput_event_keyboard *kbevent = libinput_event_get_keyboard_event(event);
	if(kbevent){
		KeyboardRequest *request = new KeyboardRequest;
		request->type = (libinput_event_keyboard_get_key_state(kbevent) == LIBINPUT_KEY_STATE_PRESSED) ? KeyDown_request : KeyUp_request; 
		request->length = sizeof(*request);
		request->time = libinput_event_keyboard_get_time(kbevent);
		request->utime = libinput_event_keyboard_get_time_usec(kbevent);
		request->key = codeMap[libinput_event_keyboard_get_key(kbevent)];
		request->count = libinput_event_keyboard_get_seat_key_count(kbevent);
		client->sendMessage(request, [](uv_write_t* req, int status){
			delete (KeyboardRequest *)req->data;
		}, request);
	}
}
void Input::pointerMotion(struct libinput_event *event){

}
void Input::pointerMotionAbsolute(struct libinput_event *event){

}
void Input::pointerButton(struct libinput_event *event){

}
void Input::pointerAxis(struct libinput_event *event){
	
}