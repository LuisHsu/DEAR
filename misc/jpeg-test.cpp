#include <iostream>
#include <cstdio>

extern "C"{
	#include <jpeglib.h>
}

int main(void){
	struct jpeg_decompress_struct jpegInfo;
	struct jpeg_error_mgr jpegErr;

	jpegInfo.err = jpeg_std_error(&jpegErr);
	jpeg_create_decompress(&jpegInfo);

	FILE *fin = fopen("test.jpg", "rb");
	jpeg_stdio_src(&jpegInfo, fin);
	jpeg_read_header(&jpegInfo, TRUE);

	return 0;
}