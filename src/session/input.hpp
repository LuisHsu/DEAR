#ifndef DEAR_INPUT_DEF
#define DEAR_INPUT_DEF

extern "C"{
	#include <libudev.h>
	#include <libinput.h>
	#include <systemd/sd-login.h>
	#include <uv.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <linux/input.h>
}

#include <iostream>
#include <list>
#include <map>

#include <message/message.hpp>
#include <message/IPCClient.hpp>
#include <inputCode.hpp>

class Input{
public:
	Input(uv_loop_t *loop, IPCClient *client);
	~Input();
private:
	uv_loop_t *loop;
	uv_poll_t uvPoll;
	struct libinput *inputCtx;
	struct udev *udevCtx;
	IPCClient *client;
	std::list<struct libinput_device *> devices;

	void deviceAdd(struct libinput_event *event);
	void keyboardKey(struct libinput_event *event);
	void pointerMotion(struct libinput_event *event);
	void pointerMotionAbsolute(struct libinput_event *event);
	void pointerButton(struct libinput_event *event);
	void pointerAxis(struct libinput_event *event);

	static std::map<uint32_t, uint32_t> codeMap;
	static int open_restricted(const char *path, int flags, void *user_data);
	static void close_restricted(int fd, void *user_data);
};

#endif