#include "input.hpp"

Input::Input(uv_loop_t *loop, IPCClient *client):
	loop(loop), client(client)
{
	// Create new udev context
	udevCtx = udev_new();
	// libinput interface
	struct libinput_interface interface_ = {
		.open_restricted = open_restricted,
		.close_restricted = close_restricted
	};
	// Create libinput context
	inputCtx = libinput_udev_create_context(&interface_, nullptr, udevCtx);
	// Get fd
	int inputfd = libinput_get_fd(inputCtx);
	// Uv poll
	uvPoll.data = this;
	uv_poll_init(loop, &uvPoll, inputfd);
	// Start poll
	uv_poll_start(&uvPoll, UV_READABLE, [](uv_poll_t* handle, int status, int events){
		Input *input = (Input *) handle->data;
		libinput_dispatch(input->inputCtx);
		struct libinput_event *event = nullptr;
		while((event = libinput_get_event(input->inputCtx)) != nullptr){
			switch(libinput_event_get_type(event)){
				case LIBINPUT_EVENT_DEVICE_ADDED:
					input->deviceAdd(event);
				break;
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
	// Get systemd seats
	char **seats;
	int seatCount = sd_get_seats(&seats);
	// Assign seat
	libinput_udev_assign_seat(inputCtx, seats[0]);
	// Clean
	for(int i = 0; i < seatCount; ++i){
		free(seats[i]);
	}
	free(seats);
}

Input::~Input(){
	// Unref devices
	for(struct libinput_device *device : devices){
		libinput_device_unref(device);
	}
	// Undef context
	libinput_unref(inputCtx);
	udev_unref(udevCtx);
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

void Input::deviceAdd(struct libinput_event *event){
	devices.push_back(libinput_event_get_device(event));
}
void Input::keyboardKey(struct libinput_event *event){
	struct libinput_event_keyboard *kbevent = libinput_event_get_keyboard_event(event);
	if(kbevent){
		KeyboardRequest *request = new KeyboardRequest;
		request->type = (libinput_event_keyboard_get_key_state(kbevent) == LIBINPUT_KEY_STATE_PRESSED) ? DEAR_KeyDown_request : DEAR_KeyUp_request; 
		request->length = sizeof(*request);
		request->time = libinput_event_keyboard_get_time(kbevent);
		request->utime = libinput_event_keyboard_get_time_usec(kbevent);
		request->key = codeMap[libinput_event_keyboard_get_key(kbevent)];
		request->count = libinput_event_keyboard_get_seat_key_count(kbevent);
		client->sendMessage(request, [](uv_write_t* req, int status){
			delete (KeyboardRequest *)req->data;
			delete req;
		}, request);
	}
}
void Input::pointerMotion(struct libinput_event *event){
	struct libinput_event_pointer *ptrevent = libinput_event_get_pointer_event(event);
	if(ptrevent){
		PointerMotionRequest *request = new PointerMotionRequest;
		request->type = DEAR_PointerMotion_request;
		request->length = sizeof(*request);
		request->time = libinput_event_pointer_get_time(ptrevent);
		request->utime = libinput_event_pointer_get_time_usec(ptrevent);
		request->dx = libinput_event_pointer_get_dx(ptrevent);
		request->dy = libinput_event_pointer_get_dy(ptrevent);
		client->sendMessage(request, [](uv_write_t* req, int status){
			delete (PointerMotionRequest *)req->data;
			delete req;
		}, request);
	}
}
void Input::pointerMotionAbsolute(struct libinput_event *event){
	struct libinput_event_pointer *ptrevent = libinput_event_get_pointer_event(event);
	static double lastX = 0, lastY = 0;
	static bool initilized = false;
	if(ptrevent){
		PointerMotionRequest *request = new PointerMotionRequest;
		request->type = DEAR_PointerMotion_request;
		request->length = sizeof(*request);
		request->time = libinput_event_pointer_get_time(ptrevent);
		request->utime = libinput_event_pointer_get_time_usec(ptrevent);
		if(!initilized){
			lastX = libinput_event_pointer_get_dx(ptrevent);
			lastY = libinput_event_pointer_get_dy(ptrevent);
			initilized = true;
		}else{
			double curX = libinput_event_pointer_get_absolute_x(ptrevent);
			double curY = libinput_event_pointer_get_absolute_y(ptrevent);
			request->dx = curX - lastX;
			request->dy = curY - lastY;
			client->sendMessage(request, [](uv_write_t* req, int status){
				delete (PointerMotionRequest *)req->data;
				delete req;
			}, request);
			lastX = curX;
			lastY = curY;
		}
	}
}
void Input::pointerButton(struct libinput_event *event){
	struct libinput_event_pointer *ptrevent = libinput_event_get_pointer_event(event);
	if(ptrevent){
		PointerButtonRequest *request = new PointerButtonRequest;
		request->type = (libinput_event_pointer_get_button_state(ptrevent) == LIBINPUT_BUTTON_STATE_PRESSED) ? DEAR_PointerDown_request : DEAR_PointerUp_request;
		request->length = sizeof(*request);
		request->time = libinput_event_pointer_get_time(ptrevent);
		request->utime = libinput_event_pointer_get_time_usec(ptrevent);
		request->button = codeMap[libinput_event_pointer_get_button(ptrevent)];
		request->count = libinput_event_pointer_get_seat_button_count(ptrevent);		
		client->sendMessage(request, [](uv_write_t* req, int status){
			delete (PointerButtonRequest *)req->data;
			delete req;
		}, request);
	}
}
void Input::pointerAxis(struct libinput_event *event){
	struct libinput_event_pointer *ptrevent = libinput_event_get_pointer_event(event);
	if(ptrevent){
		PointerAxisRequest *request = new PointerAxisRequest;
		request->type = DEAR_PointerAxis_request;
		request->length = sizeof(*request);
		switch(libinput_event_pointer_get_axis_source(ptrevent)){
			case LIBINPUT_POINTER_AXIS_SOURCE_WHEEL:
				request->source = PointerAxisRequest::AxisSource::DEAR_POINTER_AXIS_SRC_Wheel;
			break;
			case LIBINPUT_POINTER_AXIS_SOURCE_FINGER:
				request->source = PointerAxisRequest::AxisSource::DEAR_POINTER_AXIS_SRC_Finger;
			break;
			case LIBINPUT_POINTER_AXIS_SOURCE_CONTINUOUS:
				request->source = PointerAxisRequest::AxisSource::DEAR_POINTER_AXIS_SRC_Continuous;
			break;
			default:
			break;
		}
		if(libinput_event_pointer_has_axis(ptrevent, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL)){
			request->axis = PointerAxisRequest::Axis::DEAR_POINTER_AXIS_Horizontal;
			request->valueH = libinput_event_pointer_get_axis_value(ptrevent, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL);
		}
		if(libinput_event_pointer_has_axis(ptrevent, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL)){
			request->axis = PointerAxisRequest::Axis::DEAR_POINTER_AXIS_Vertical;
			request->valueV = libinput_event_pointer_get_axis_value(ptrevent, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL);
		}
		client->sendMessage(request, [](uv_write_t* req, int status){
			delete (PointerAxisRequest *)req->data;
			delete req;
		}, request);
	}
}