#include <windows.h>
#include <interface_server.h>
#include <stdint.h>
#include <stdio.h>

#define INBUF_SIZE 4096
#define FF_INPUT_BUFFER_PADDING_SIZE 8

//int save(id_image_t *imgs, size_t imgs_count, void *tag){
//	static int count = 0;
//	printf("detected %d frames.\n", imgs_count);
//	for (int i = 0; i < imgs_count; i++){
//		Mat m(imgs[i].height, imgs[i].width, CV_8UC3, imgs[i].buffer);
//		std::ostringstream oss;
//		oss << "..\\imgs\\ " << i + count << ".jpg";
//		std::string result = oss.str();
//		imwrite(result, m);
//	}
//	count = count + imgs_count;
//	return 0;
//}

int saveYUV(id_image_t *imgs, size_t imgs_count, void *tag){
	//remove("..\\imgs\\out.yuv");
	FILE * f = fopen("..\\imgs\\out.yuv", "ab+");
	for (int i = 0; i < imgs_count; i++){
		fwrite(imgs[i].buffer, 1, imgs[i].height*imgs[i].width / 2 * 3, f);
	}
	fclose(f);
	return 0;
}

int main(){

	//JSON格式配置文件，目前只支持定义一个area。
	//这里的示例direction为down2up,right2left；area为视频左半区域。
	const char config[] = "{\"version\": \"1\",\"alg-params\": [{\"direction\": [\"down2up\",\"right2left\"],\"target\": [\"person\"],\"areas\": [{\"left\": \"0\",\"top\": \"0\",\"width\": \"320\",\"height\": \"480\"}]}]}";
	
	//回调函数，以保存每一帧的图片为例。
	PFNInstrusionDetectCB cb = &saveYUV;
	
	//初始化检测，并传入JSON配置。
	if (IntrusionDetectStart(config, NULL) == 0)
		printf("Detection start.\n");
	
	//以264视频流的形式的形式加载视频，并进行检测。如果有检测到的目标，会调用回调函数。
	FILE *fin = fopen("..\\videos\\015.264", "rb");
	uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
	memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);
	int size = 1;
	id_stream_t * stream = new id_stream_t();
	while (size > 0){
		size = fread(inbuf, 1, INBUF_SIZE, fin);
		stream->type = 1;
		stream->buffer = inbuf;
		stream->len = INBUF_SIZE;
		IntrusionDetectPushStream(stream, cb);
	}
	delete stream;
	
	//以文件的形式加载视频，并进行检测。如果有检测到的目标，会调用回调函数。
	//加载裸视频流的形式还未完成，会在以后的版本更新。
	//IntrusionDetectOpenFile("..\\videos\\00015_H_fusion.avi", cb);
	
	//停止检测，释放内存。
	if(IntrusionDetectStop()==0)
		printf("Done.\n");

	system("pause");
	return 0;
}