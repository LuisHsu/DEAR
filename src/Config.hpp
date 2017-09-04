#ifndef CONFIG_DEF
#define CONFIG_DEF

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>

class Config{
public:
	static void readconf();
	// Backend
	static uint32_t dispWidth;
	static uint32_t dispHeight;
	static int32_t dispX;
	static int32_t dispY;
	static uint32_t dispBorder;
};

#endif