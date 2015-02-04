/********************************************************************
	created:	2015/01/21
	created:	21:1:2015   15:07
	filename: 	D:\wangzi6147\Doc\video project\detect\testProj\testProj\testProj\interface_server.cpp
	file path:	D:\wangzi6147\Doc\video project\detect\testProj\testProj\testProj
	file base:	interface_server
	file ext:	cpp
	author:		PRIS
	
	purpose:	
*********************************************************************/

#include "interface_server.h"
#include "JsonParser.h"
#include "videoProcess.h"

videoProcess * pro;

int IntrusionDetectStart(char const *config, void *tag){
	JsonParser jsonParser;
	jsonParser.parseJsonFromString(config);
	pro = new videoProcess(jsonParser);
	return 0;
}

int IntrusionDetectPushStream(id_stream_t *stream, PFNInstrusionDetectCB cb){
	Mat frame;
	id_image_t temps[1 << 8];
	int count = pro->processStream(stream->buffer, stream->len, temps);
	cb(temps, count, NULL);
	for (int i = 0; i < count; i++)
		delete[] temps[i].buffer;
	return 0;
}

int IntrusionDetectOpenFile(const char * filename, PFNInstrusionDetectCB cb){
	VideoCapture capture(filename);
	Mat frame;
	int count = 0;
	id_image_t temps[1 << 8];
	while (capture.read(frame)){
		if (pro->process(frame)){
			temps[count].type = 2;
			temps[count].height = frame.size().height;
			temps[count].width = frame.size().width;
			temps[count].buf_size = temps[count].width*temps[count].height * 3;
			temps[count].buffer = new unsigned char[temps[count].buf_size];
			for (int i = 0; i<temps[count].buf_size; i++)
				temps[count].buffer[i] = frame.data[i];
			count++;
		}
		else if (count > 0){
			cb(temps, count, NULL);
			for (int j = 0; j < count; j++)
				delete[] temps[j].buffer;
			count = 0;
		}
	}
	capture.release();
	return 0;
}

int IntrusionDetectStop(){
	if (pro != NULL)
		delete pro;
	//destroyAllWindows();
	return 0;
}
