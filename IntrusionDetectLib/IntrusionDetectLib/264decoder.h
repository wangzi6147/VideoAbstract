#pragma once
#ifndef  _264_DECODER_H_
#define  _264_DECODER_H_

#include "videoProcess.h"

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

int decode(unsigned char * buffer, size_t len, videoProcess * pro);

#ifdef __cplusplus 
}
#endif


#endif //  264_DECODER_H