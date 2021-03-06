#pragma once

#include "videoProcess.h"
#include "interface_server.h"

#ifdef __cplusplus 
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "dsputil.h"
#include "h264.h"


#define PI 3.14159265358979323846

#ifdef HAVE_AV_CONFIG_H
#undef HAVE_AV_CONFIG_H
#endif

#include "avcodec.h"
#include "define.h"

#define INBUF_SIZE 4096

void init264decoder();

void stop264decoder(videoProcess * pro);

void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize, char *filename, char *recfilename);

int decode(unsigned char * buffer, size_t len, videoProcess * pro, id_image_t * ptr);

#ifdef __cplusplus 
}
#endif
