/********************************************************************
	created:	2015/01/31
	created:	31:1:2015   17:20
	filename: 	D:\wangzi6147\Doc\git\VideoAbstract\IntrusionDetectLib\IntrusionDetectLib\264decoder.c
	file path:	D:\wangzi6147\Doc\git\VideoAbstract\IntrusionDetectLib\IntrusionDetectLib
	file base:	264decoder
	file ext:	c
	author:		PRIS
	
	purpose:	
*********************************************************************/

#include "264decoder.h"
#ifdef __cplusplus 
extern "C" {
#endif
const char *outfilename;
const char *outrecfilename;
const char *filename;
extern AVCodec h264_decoder;
AVCodec *codec;
AVCodecContext *c;
int frame, size, got_picture, len;
FILE *fin, *fout;
AVFrame *picture;
uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE], *inbuf_ptr;
char buf[1024];
DSPContext dsp;

void pgm_save(unsigned char *buf,int wrap, int xsize,int ysize,char *filename, char *recfilename)
{
    FILE *f;
    int i,j;

	int ci=0;

	static int framenum =0;
	/*
	if(framenum == 3*0)
	{
		
		f=fopen(filename,"r+");
		fprintf(f,"P5\n%d %d\n%d\n",xsize,ysize,255);
		//  for(i=0;i<ysize;i++)
		//      fwrite(buf + i * wrap,1,xsize,f);
		for(i=0;i<ysize;i++)
		{
			for (j=0; j < xsize; j++)
			{				
				fprintf(f, "%3d(%3d,%3d)",*(buf + i * wrap+j),i,j);
				if(ci++%5==0)
					fprintf(f, "\n");
			}
			fprintf(f, "\n");
		}
		fclose(f);
	}
	*/
	f=fopen(recfilename,"ab+");   
	
	for(i=0;i<ysize;i++)
	{
		fwrite(buf + i * wrap, 1, xsize, f );
//		for (j=0; j < xsize; j++)
//		{	   
//			fprintf(f, "%d(%d,%d)",*(buf + i * wrap+j),i,j);	 
//		}	  
	}
    fclose(f);
}

void init264decoder(){

	outfilename = "outrec.txt";
	outrecfilename = "outrec.yuv";
	filename = "test.264";
	codec = &h264_decoder;
	c = NULL;

	/* set end of buffer to 0 (this ensures that no overreading happens for damaged mpeg streams) */
	//memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);

	printf("Video decoding\n");

	/* find the mpeg1 video decoder */
	avcodec_init();
	c = avcodec_alloc_context();
	picture = avcodec_alloc_frame();
	//	 dsputil_init(&dsp, c);

	if (codec->capabilities&CODEC_CAP_TRUNCATED)
		c->flags |= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */

	/* For some codecs, such as msmpeg4 and mpeg4, width and height
	MUST be initialized there because this information is not
	available in the bitstream. */

	/* open it */


	if (avcodec_open(c, codec) < 0) {
		fprintf(stderr, "could not open codec\n");
		exit(1);
	}
	{
		H264Context *h = (H264Context *)c->priv_data;
		MpegEncContext *s = &h->s;
		s->dsp.idct_permutation_type = 1;
		dsputil_init(&s->dsp, c);
	}

}

int decode(unsigned char * buffer, size_t len, videoProcess * pro, id_image_t * ptr)
{
	///* the codec gives us the frame size, in samples */
 //   fin = fopen(filename, "rb");
 //   if (!fin) {
 //       fprintf(stderr, "could not open %s\n", filename);
 //       exit(1);
 //   }
 //   fout = fopen(outfilename, "wb");
 //   if (!fin) {
 //       fprintf(stderr, "could not open %s\n", outfilename);
 //       exit(1);
 //   }
	//fclose(fout);

	//fout = fopen(outrecfilename, "wb");
 //   if (!fin) {
 //       fprintf(stderr, "could not open %s\n", outrecfilename);
 //       exit(1);
 //   }
	//fclose(fout);

	frame = 0;
	size = len;
    for(;;) {
        //size = fread(inbuf, 1, INBUF_SIZE, fin);
        if (size == 0)
            break;

        /* NOTE1: some codecs are stream based (mpegvideo, mpegaudio)
           and this is the only method to use them because you cannot
           know the compressed data size before analysing it.

           BUT some other codecs (msmpeg4, mpeg4) are inherently frame
           based, so you must call them with all the data for one
           frame exactly. You must also initialize 'width' and
           'height' before initializing them. */

        /* NOTE2: some codecs allow the raw parameters (frame size,
           sample rate) to be changed at any frame. We handle this, so
           you should also take care of it */

        /* here, we use a stream based decoder (mpeg1video), so we
           feed decoder and see if it could decode a frame */
		inbuf_ptr = buffer;
        while (size > 0) {
            len = avcodec_decode_video(c, picture, &got_picture,
                                       inbuf_ptr, size);
            if (len < 0) {
                fprintf(stderr, "Error while decoding frame %d\n", frame);
                exit(1);
            }
            if (got_picture) {
                printf("saving frame %3d\n", frame);
                fflush(stdout);

                /* the picture is allocated by the decoder. no need to
                   free it */
              //  snprintf(buf, sizeof(buf), outfilename, frame);
				/*pgm_save(picture->data[0], picture->linesize[0],
						 c->width, c->height, outfilename, outrecfilename);
						 pgm_save(picture->data[1], picture->linesize[1],
						 c->width/2, c->height/2, outfilename, outrecfilename);
						 pgm_save(picture->data[2], picture->linesize[2],
						 c->width/2, c->height/2, outfilename, outrecfilename);*/
				if (pro->yuv2Mat(picture->data[0], picture->linesize[0], c->width, c->height)){
					ptr[frame].type = 1;
					ptr[frame].height = c->height;
					ptr[frame].width = c->width;
					ptr[frame].buf_size = ptr[frame].width*ptr[frame].height /2* 3;
					ptr[frame].buffer = new unsigned char[ptr[frame].buf_size];
					for (int i = 0; i < c->height; i++){
						for (int j = 0; j < c->width; j++){
							ptr[frame].buffer[i*c->width+j] = picture->data[0][i*picture->linesize[0]+j];
						}
					}
					for (int i = 0; i < c->height/2; i++){
						for (int j = 0; j < c->width / 2; j++){
							ptr[frame].buffer[c->height*c->width + i*c->width / 2 + j] = picture->data[1][i*picture->linesize[1] + j];
						}
					}
					for (int i = 0; i < c->height / 2; i++){
						for (int j = 0; j < c->width / 2; j++){
							ptr[frame].buffer[c->height*c->width/4*5 + i*c->width / 2 + j] = picture->data[2][i*picture->linesize[2] + j];
						}
					}
					frame++;
				}
            }
            size -= len;
            inbuf_ptr += len;
        }
    }
   // fclose(fin);
//	 fclose(fout);
	return frame;
}

void stop264decoder(videoProcess * pro){

	//	/* some codecs, such as MPEG, transmit the I and P frame with a
	//	latency of one frame. You must do the following to have a
	//	chance to get the last frame of the video */
	//#define NOTFOR264
	//#ifdef NOTFOR264
	//
	//	//    len = avcodec_decode_video(c, picture, &got_picture,
	//	//                               NULL, 0);
	//	len = avcodec_decode_video(c, picture, &got_picture,
	//		inbuf_ptr, 0);
	//	if (got_picture) {
	//		printf("saving last frame %3d\n", frame);
	//		fflush(stdout);
	//
	//		/* the picture is allocated by the decoder. no need to
	//		free it */
	//		//    snprintf(buf, sizeof(buf), outfilename, frame);
	//		/* pgm_save(picture->data[0], picture->linesize[0],
	//		c->width, c->height, outfilename, outrecfilename);
	//		pgm_save(picture->data[1], picture->linesize[1],
	//		c->width/2, c->height/2, outfilename, outrecfilename);
	//		pgm_save(picture->data[2], picture->linesize[2],
	//		c->width/2, c->height/2, outfilename, outrecfilename);*/
	//		if(pro->yuv2Mat(picture->data[0], picture->linesize[0], c->width, c->height))
	//			frame++;
	//	}
	//#endif

	avcodec_close(c);
	av_free(c);
	av_free(picture);
	//printf("\n");
}
#ifdef __cplusplus 
}
#endif