#ifndef __INSTRUSION_DETECT_ALG_H__
#define __INSTRUSION_DETECT_ALG_H__
#ifdef DLL_API  
#else  
#define DLL_API __declspec(dllexport)  
#endif  
/*,
算法参数初步定为JSON格式，这样的好处是以后算法增加参数或者其他功能，函数本身签名就不需要修改了

JSON串结构如下
version：为版本号，当前为1，如果以后参数有重要扩充或修改，可以顺序增加
alg-params：对应的值为数组，数组中每一项代表一个调用者感兴趣的区域
区域结构：
	direction代表方向，可以一次选择多个, 候选字符为 all left2right right2left up2down down2up
		例如：
		all : 所有方向
		up2down|down2up：从上到下或从下到上的方向
	target代表检测目标，候选字符为 person animal all
	areas代表区域，当前为矩形区域。如果此区域数组为多个，则代表这几个区域有相同的方向和目标
		left : 矩形左上角的X坐标
		top  : 矩形左上角的Y坐标
		width : 矩形区域长度
		height : 矩形区域高度

JSON示例如下：
{
    "version": "1",
    "alg-params": [
        {
            "direction": [
                "up2down",
                "down2up"
            ],
            "target": [
                "person"
            ],
            "areas": [
                {
                    "left": "0",
                    "top": "0",
                    "width": "100",
                    "height": "100"
                },
                {
                    "left": "100",
                    "top": "100",
                    "width": "200",
                    "height": "200"
                }
            ]
        },
        {
            "direction": [
                "all"
            ],
            "target": [
                "all"
            ],
            "areas": [
                {
                    "left": "500",
                    "top": "500",
                    "width": "600",
                    "height": "600"
                }
            ]
        }
    ]
}


*/

/*
 * 流定义，当前type为1代表H264 frame, 以0x0001开头，每个I帧前，有SPS、PPS帧。
*/
typedef struct _id_stream_t
{
	int type; // 1 : H264
	unsigned char *buffer;
	size_t len;
} id_stream_t;


/*
 * 图像定义，当前type为1代表YUV420SP格式
*/
typedef struct _id_image_t
{
	int type; // 1：YUV420SP(Android)；2：RGB
	int width;
	int height;
	int buf_size;
	unsigned char *buffer;
} id_image_t;


/*
 * @brief 检测到入侵后的回掉函数
 * param imgs 触发入侵检测警报的图像数组
 *			  数组的buffer由算法库进行管理，调用者如果在回调返回后还要使用，则需要把它们复制到自己的内存块
 * param imgs_count 数组中图像的个数
 * param tag 
 */
typedef int(*PFNInstrusionDetectCB)(id_image_t *imgs, size_t imgs_count, void *tag);

/**
 * @brief 启动入侵检测。为同步操作
 * @param config 入侵检测的参数，为json格式
 * @param tag 调用者指定的回调函数的透明参数
			  当前设定1分钟内只引发一次警报回调
 * @return 返回值0代表成功，其他代表错误，具体错误码由算法库给出
 */
DLL_API int IntrusionDetectStart(
	char const *config,
	void *tag
	);

/**
 * @brief 停止检测。为同步操作
 * @return 返回值0代表成功
 */
DLL_API int IntrusionDetectStop(
	);

/**
 * @brief 检测过程中的推流操作。为同步操作。此函数要尽可能快的返回
 * @stream 视频流，当前为RTP协议包装的H264格式，算法库应该在函数调用返回前
		   复制数组中的数据到自己的缓冲区
 * @return 返回值0代表成功
 */
DLL_API int IntrusionDetectPushStream(
	id_stream_t *stream
	);

//************************************
// Method:    IntrusionDetectOpenFile
// FullName:  IntrusionDetectOpenFile
// Access:    public 
// Returns:   int 返回值0代表成功
// Qualifier:
// Parameter: const char * filename 视频文件路径
// Parameter: PFNInstrusionDetectCB cb 检测到入侵的回调函数
//************************************
DLL_API int IntrusionDetectOpenFile(const char * filename, PFNInstrusionDetectCB cb);
#endif

