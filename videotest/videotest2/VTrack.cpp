#include "stdafx.h"
#include "VTrack.h"
#include "LibMySql.h"

extern CDataMySql* m_MysqlHandle;
vector<Point2f> getFeaturePoints(Rect rc){
	vector<Point2f> points;
	float wshift = rc.width/6.0;
	float hshift = rc.height/6.0;//长宽都分为1/6
	for (int i = 1; i < 6; i++)
	{
		for (int j = 1; j < 6; j++)
		{
			points.push_back(Point2f(rc.x + wshift * i, rc.y + hshift * j));//在points尾部添加一个数据
		}
	}
	return points;
}

bool isPointInMat(Mat* mat, Point point)//判断点坐标是否在矩阵中
{
	if (point.x < 1 || point.y < 1 || mat->cols < point.x || mat->rows < point.y)
	{
		return false;
	}
	return true;
}
Rect mergeRect(Rect rc1, Rect rc2)//融合方形区域
{
	if (rc1.width == 0)
	{
		return rc2;
	}
	Rect rc;
	rc.x = (rc1.x + rc2.x) / 2;
	rc.width = (rc1.width + rc2.width) / 2;

	rc.y = (rc1.y + rc2.y) / 2;
	rc.height = (rc1.height + rc2.height) / 2;

	return rc;
}

Point getRectCenter(Rect rc)//获得方形区域中心
{
	return Point(rc.x + rc.width/2, rc.y + rc.height/2);
}


VTrack::VTrack(Size minBlobSize, Size maxBlobSize,  vector<Rect> roi, double paramsAlpha, int timeout)//跟踪初始化
{
    frameID = 0;
    backgroundIMG = NULL;
    backgroundIMG32 = NULL;
    grayFrameIMG = NULL;
    diffIMG = NULL;
    diffHist = NULL;
    colorIMG = NULL;
	preGrayFrameIMG = NULL;

    this->paramsAlpha = paramsAlpha;
    this->minBlobSize = minBlobSize;
    this->maxBlobSize = maxBlobSize; 
    this->historyTimeout =  timeout*CLOCKS_PER_SEC;
	this->ROI = roi;
	m_MysqlHandle=new CDataMySql;

}


VTrack::~VTrack(void)//析构函数
{
    delete backgroundIMG;
    delete backgroundIMG32;
    delete grayFrameIMG;

	delete preGrayFrameIMG;
    delete diffIMG;
    delete colorIMG;
    delete diffHist;
    vector<WorkBlob>().swap(historyBlobs);
}
void VTrack::DoProcessing(char *filename,/* char *dataname,*/ CString tablename)
{
	this->filename = filename;
	//this->dataname = dataname;
	this->objtablename = tablename;
	this->m_IfContinue = true;
	this->m_speed = 0.0;
	this->frameID = 0;
	this->frameCount = 200;
	m_threadControl = ::AfxBeginThread(DetectProcess,this);
}
void VTrack::DoProcessing()//重载
{
	CvCapture *capture = cvCreateFileCapture(filename.c_str());//读取视频文件
    Mat frame;
    IplImage* grabframe;
 //   Mat* bgIMG;
   // namedWindow("Frame");
  //  namedWindow("BG");
	//*capture >> frame;
	grabframe = cvQueryFrame(capture);//抓取一帧
	if (grabframe == NULL)
	{
		return;
	}
	frame = grabframe;
	ROI.clear();//ROI是一个vector
	ROI.push_back(Rect(0,0,grabframe->width,grabframe->height));//初始把一帧的整个区域添加进去
	
#if RECORD_VIDEO
	VideoWriter *vw = new VideoWriter("save.avi", CV_FOURCC('M', 'J', 'P', 'G'), 20, Size(frame.cols, frame.rows));
#endif

	frameCount = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);//总帧数
	//frameCount = 100;
    while(m_IfContinue && frameID<frameCount)
	{
        //*capture >> frame;
        grabframe = cvQueryFrame(capture);//获取一帧
		
		if (grabframe == NULL)
			break;
		
        frame = grabframe;//IplImage转换为Mat
       
       ProcessFrame(&frame);//跟踪处理
       /* bgIMG = vTracker->GetBackgroundIMG();*///获取背景
       // imshow("BG", *bgIMG);
       // imshow("Frame", frame);
#if RECORD_VIDEO
		*vw << frame ;
#endif
        //waitKey(3);//测试用，显示图片
		m_speed = (double)frameID/(double)frameCount;
    }
#if RECORD_VIDEO
	vw->release();
#endif

	//cvReleaseVideoWriter(&videoWriter);
	if (m_IfContinue)
	{
		m_speed = 1.0;
		//SaveData(dataname,&m_speed);
		SaveDatabase(objtablename,&m_speed,&m_IfContinue);
	}
	cvReleaseCapture(&capture);
	if (m_speed<2)
	{
		bool ExistObjectTable = true;
		m_MysqlHandle->IfExistTable(objtablename,&ExistObjectTable);
		if (ExistObjectTable)
		{
			m_MysqlHandle->DropTable(objtablename);
			cvWaitKey(5);
		}
	}
}
void VTrack::StopProcessing()
{
	m_IfContinue = false;
	WaitForSingleObject(m_threadControl->m_hThread,INFINITE);//进程同步
}
double VTrack::VTrackSpeed()
{
	return this->m_speed;//返回处理进度
	//return (double)frameID/(double)frameCount;
}
void VTrack::ProcessFrame(Mat* frame)
{
	if (frameID == 0)
	{
		InitMemory(frame);
	}
	//else if (frameID % 3 == 0)
	//{
		assert(frame->channels() == 3);//assert宏的原型定义在<assert.h>中，其作用是如果它的条件返回错误，则终止程序执行

		cvtColor(*frame, *grayFrameIMG, CV_BGR2GRAY);

		frame->copyTo(*colorIMG);//当前帧的3通道图

		UpdateBackground(grayFrameIMG);

		CleanHistroyBlob();


		vector<Rect> blobs;

		blobs = FindBlob(diffIMG);//将一帧画面中所有的运动blob都找出来

		UpdateHistoryBlob(blobs);
	//}
	
    frameID++;
	
}

void VTrack::SaveData(string filename,double *speed){
    //flush
    for (int i = 0; i < historyBlobs.size(); i++)
    {
        if (historyBlobs[i].centers.size() > 2) //保证是一个运动的目标
        {
            vHandler.AddWorkBlob(historyBlobs[i]);
        }
    }
    vHandler.StoreVFrame(frameID);
    vHandler.WriteData(filename);
}

void VTrack::SaveDatabase(CString objtablename,double *speed,bool *ifContinue)
{
	//flush
	for (int i = 0; i < historyBlobs.size() && *ifContinue; i++)
	{
		if (historyBlobs[i].centers.size() > 2) //保证是一个运动的目标
		{
			vHandler.AddWorkBlob(historyBlobs[i]);
		}
	}
	if(*ifContinue)
	{
		vHandler.StoreVFrame(frameID);
		vHandler.WriteDatabase(objtablename,speed,ifContinue);
	}
	
}
void VTrack::UpdateBackground(Mat* frame){
    if (frameID == 0)
    {
        frame->convertTo(*backgroundIMG32, CV_32FC1);//32位浮点型数据
    }
	myGaussModel(*grayFrameIMG, *diffIMG, 0.01);
	Mat *temp = diffIMG;
	adaptiveThreshold(*diffIMG, *diffIMG, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 11, 6);//自适应阈值，能比较清晰描述轮廓
	//cvThreshold(diffIMG, temp, 10, 255.0, CV_THRESH_BINARY);///<前景图像二值化
    dilate(*diffIMG, *diffIMG, Mat());  //4ms
	erode(*diffIMG, *diffIMG, Mat());//腐蚀
	dilate(*diffIMG, *diffIMG, Mat());//膨胀


    //erode(*diffIMG, *diffIMG, Mat());
    //dilate(*diffIMG, *diffIMG, Mat());
    //diffIMG->copyTo(*backgroundIMG);//？
	//imshow("diffIMG1", *diffIMG);
}

static void cFindContours( Mat& image, vector<vector<Point> >& contours, int mode, int method, Point offset=Point())
{
	MemStorage storage(cvCreateMemStorage());
	CvMat _image = image;
	CvSeq* _contours = 0;
	
	cvFindContours(&_image, storage, &_contours, sizeof(CvContour), mode, method, offset);
	if( !_contours )
	{
		contours.clear();
		return;
	}
	Seq<CvSeq*> all_contours(cvTreeToNodeSeq( _contours, sizeof(CvSeq), storage ));
	size_t i, total = all_contours.size();
	contours.resize(total);
	SeqIterator<CvSeq*> it = all_contours.begin();
	for( i = 0; i < total; i++, ++it )
	{
		CvSeq* c = *it;
		((CvContour*)c)->color = (int)i;
		Seq<Point>(c).copyTo(contours[i]);
	}
}

vector<Rect> VTrack::FindBlob(Mat* binIMG){


    vector<Rect> blobs;
    vector<vector<Point> > contours;

    cFindContours(*binIMG, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);  //78ms OpenCV 之轮廓查找函数

    for (int i = 0; i < contours.size(); i++)
    {
		
        Rect rc = boundingRect(Mat(contours[i]));//生成矩形边界

			blobs.push_back(rc);

        
        //rectangle(*frame, rc, Scalar(255,255,0));
    }

    bool isAgain = true;
    int r1, r2, b1, b2; //right and bottom
    int mergeLength = 0;
    while(isAgain){
        isAgain = false;
        for (int i = 0; i < blobs.size(); i++)
        {
            for (int j = i + 1; j < blobs.size(); j++)
            {
                r1 = blobs[i].x + blobs[i].width;
                r2 = blobs[j].x + blobs[j].width;
                b1 = blobs[i].y + blobs[i].height;
                b2 = blobs[j].y + blobs[j].height;
                if (blobs[i].x < r2 + mergeLength && 
                    blobs[j].x < r1 + mergeLength &&
                    blobs[i].y < b2 + mergeLength &&
                    blobs[j].y < b1 + mergeLength )
                {
                    blobs[i].x = MIN(blobs[i].x, blobs[j].x);
                    blobs[i].y = MIN(blobs[i].y, blobs[j].y);
                    r1 = MAX(r1, r2);
                    b1 = MAX(b1, b2);
                    blobs[i].width = r1 - blobs[i].x;
                    blobs[i].height = b1 - blobs[i].y;
                    isAgain = true;
                    blobs[j] = blobs.back();
                    blobs.pop_back();
                    j--;
                }
            }
        }
    }

    //排除太小或者太大的
    for (int i = 0; i < blobs.size(); i++)
    {
        if (blobs[i].width*  blobs[i].height <750 )
        {
            blobs[i] = blobs.back();
            blobs.pop_back();
            i--;
        }
    }

	//排除roi以外的
	
	if (ROI.size())
	{
		vector<Rect> tmpBlobs;
		for (int i = 0; i < ROI.size(); i++)
		{
			Rect rc = ROI[i];
			for (int j = 0; j < blobs.size(); j++)
			{
				Point center = getRectCenter(blobs[j]);
				if (rc.contains(center))
				{
					tmpBlobs.push_back(blobs[j]);
				}
			}
		}
		return tmpBlobs;
	}else {
		return blobs;//把当前帧中所有的运动对象都找出来（矩形vector）
	}
    
}

double GetOverLap(Rect rc1, Rect rc2){
    int r1, r2, b1 ,b2;
    r1 = rc1.x + rc1.width;
    b1 = rc1.y + rc1.height;
    r2 = rc2.x + rc2.width;
    b2 = rc2.y + rc2.height;
    if(rc1.x < r2 && rc2.x < r1 &&
        rc1.y < b2 && rc2.y < b1 ){
            int left = MAX(rc1.x, rc2.x);
            int top =MAX(rc1.y, rc2.y);
            int right = MIN(r1, r2);
            int bottom = MIN(b1, b2);
            int area3 = (right - left) * (bottom - top);
            return area3 * 1.0 / (rc1.area() + rc2.area() - area3);
    }
    return 0;
}

Scalar VTrack::GetRectColor(Rect rc){
    Rect tmpRC =rc;
    int w_s = rc.width/4;
    int h_s = rc.height/4;
    tmpRC.x = tmpRC.x + w_s;
    tmpRC.y = tmpRC.y + h_s;
    tmpRC.width = tmpRC.width/2;
    tmpRC.height = tmpRC.height/2;
    Mat roiMat = (*colorIMG)(tmpRC);
    Scalar s = mean(roiMat);
    return s;
}


void VTrack::TrackHistroyBlob(){
	if (frameID == 0)
	{
		preGrayFrameIMG = new Mat(grayFrameIMG->rows, grayFrameIMG->cols, CV_8UC1);
		grayFrameIMG->copyTo(*preGrayFrameIMG);
		return;
	}

	if (historyBlobs.size() == 0)
	{
		return;
	}
	if (frameID % 3 == 0)
	{
		vector<Point2f> points[2];
		for (int i = 0; i < historyBlobs.size(); i++){
			vector<Point2f> bp = getFeaturePoints(historyBlobs[i].rcs.back());
			points[0].insert(points[0].end(), bp.begin(), bp.end());
		}
		vector<uchar> status;
		vector<float> err;
		calcOpticalFlowPyrLK(*preGrayFrameIMG, *grayFrameIMG, points[0], points[1], status, err);
		grayFrameIMG->copyTo(*preGrayFrameIMG);

		Mat drawImg = colorIMG->clone();
		Rect trackedRC;
		for (int i = 0; i < historyBlobs.size(); i++){
			vector<Point2f> rcp;
			for (int j = i * 25; j < (i + 1) * 25; j++)
			{
				if (status[j] && isPointInMat(grayFrameIMG, points[1][j]))
				{
					rcp.push_back(points[1][j]);
				}
			}
			if (rcp.size() > 10)
			{
				trackedRC = boundingRect(Mat(rcp));
				historyBlobs[i].trackedRC = historyBlobs[i].rcs.back();
				Point center1 = getRectCenter(historyBlobs[i].trackedRC);
				Point center2 = getRectCenter(trackedRC);
				historyBlobs[i].trackedRC.x += center2.x - center1.x;
				historyBlobs[i].trackedRC.y += center2.y - center1.y;
			}
			else {
				historyBlobs[i].trackedRC = Rect(0, 0, 0, 0);
			}
			WorkBlob wb = historyBlobs[i];
			rectangle(drawImg, historyBlobs[i].trackedRC, Scalar(0, 255, 0));
			rectangle(drawImg, historyBlobs[i].rcs.back(), Scalar(255, 0, 0));
		}

		imshow("LK", drawImg);
	}
	
}

void VTrack::CleanHistroyBlob(){
	//清理历史队列中超时的workblob，并将其追加到保存队列中
	clock_t now = clock();
	for (int i = 0; i < historyBlobs.size(); i++)
	{
		if(now - historyBlobs[i].lastUpdateTime > historyTimeout){
			//do sth for append historyBlobs[i]
			if (historyBlobs[i].centers.size() > 2) //保证是一个运动的目标
			{
				vHandler.AddWorkBlob(historyBlobs[i]);
			}


			//delete it
			historyBlobs[i] = historyBlobs.back();
			historyBlobs.pop_back();
			i--;
		}
	}
}

void VTrack::UpdateHistoryBlob(vector<Rect>& currentRC){//currentRC为当前这一帧找出的所有运动物体
    clock_t now = clock();

    //vector<MatND> blobHist;
    int channels[1] = {0};
    int histSize[2] = {30, 32};
    float sranges[2] = { 0, 255 };
    const float* ranges[] = { sranges };

    double maxOverlapValue;
    double overlapValue;
    double maxHistValue;
    double histValue;
    int matchOLID;
    int matchHistogramID;

    int blobDistance;
    Point blobCenter;
    Rect histRC;      //历史区域最新的Rect
    Point histCenter; //历史区域最新的中心点
    for (int i = 0; i < currentRC.size(); i++)
    {
        MatND histMat;
        Mat roiMat = (*grayFrameIMG)(currentRC[i]).clone();
        calcHist(&roiMat, 1, channels, Mat(), histMat, 1, histSize, ranges);
        blobCenter.x = currentRC[i].x + currentRC[i].width/2;
        blobCenter.y = currentRC[i].y + currentRC[i].height/2;


        maxOverlapValue = 0.1;
        maxHistValue = 0.5;
        matchOLID = -1;
        matchHistogramID = -1;
        
        for (int j = 0; j < historyBlobs.size(); j++)
        {
            //1.找重叠最多的
            histRC = historyBlobs[j].rcs.back();//返回末尾元素的引用
            overlapValue = GetOverLap(currentRC[i], histRC);//并非绝对面积，而是重叠面积与两块合起来的总面积的和的比值
            if (maxOverlapValue < overlapValue)//大于0.1即认为找到
            {
                matchOLID = j;//找到当前帧的运动物体在以前出现过
                maxOverlapValue = overlapValue;
            }
        }
        
        if (matchOLID == -1)
        {
            for (int j = 0; j < historyBlobs.size(); j++)
            {
                //2.找匹配度最高的 //测试发现这块基本上用不上
                histRC = historyBlobs[j].rcs.back();
                histCenter = historyBlobs[j].centers.back();
                blobDistance = (blobCenter.x - histCenter.x)*(blobCenter.x - histCenter.x) + (blobCenter.y - histCenter.y)*(blobCenter.y - histCenter.y);
                if (blobDistance > 400)
                {
                    continue;
                }
                double compareArea = histRC.area() * 1.0 / currentRC[i].area();
                if (compareArea > 2 || compareArea < 0.5)
                {
                    continue;
                }

                histValue = compareHist(histMat, historyBlobs[j].histMat, CV_COMP_INTERSECT);
                if (histValue > maxHistValue)
                {
                    maxHistValue = histValue;
                    matchHistogramID = j;
                }
            }
            matchOLID = matchHistogramID;
        }

        if (matchOLID == -1)
        {
            //这是出现的新物体，创建新的对象
            WorkBlob wb;
            wb.centers.push_back(blobCenter);
            wb.lastUpdateTime = now;
            wb.histMat = histMat.clone();
            wb.rcs.push_back(currentRC[i]);
            wb.frameIDs.push_back(frameID);
            wb.rgb.push_back(GetRectColor(currentRC[i]));
            historyBlobs.push_back(wb);
        }else {
            //merge
			//将当前帧的物体加进去
            historyBlobs[matchOLID].centers.push_back(blobCenter);
            historyBlobs[matchOLID].rcs.push_back(currentRC[i]);
            historyBlobs[matchOLID].lastUpdateTime = now;
            historyBlobs[matchOLID].histMat = histMat.clone();
            historyBlobs[matchOLID].frameIDs.push_back(frameID);
            historyBlobs[matchOLID].rgb.push_back(GetRectColor(currentRC[i]));
        }

    }
}


void VTrack::UpdateHistoryBlob2(vector<Rect>& currentRC){
	clock_t now = clock();
	double maxOverlapValue;
	int matchOLID;
	double overlapValue;
	
	for (int i = 0; i < historyBlobs.size(); i++)
	{
		maxOverlapValue = 0.1;
		matchOLID = -1;
		for (int j = 0; j < currentRC.size(); j++)
		{
			overlapValue = GetOverLap(currentRC[j], historyBlobs[i].trackedRC);
			if (maxOverlapValue < overlapValue)
			{
				matchOLID = j;
				maxOverlapValue = overlapValue;
			}
		}

		

		//找到更新
		if (matchOLID > -1)
		{
			Rect mergeRC = mergeRect(historyBlobs[i].trackedRC, currentRC[matchOLID]);

			Point blobCenter;
			blobCenter.x = mergeRC.x + mergeRC.width/2;
			blobCenter.y = mergeRC.y + mergeRC.height/2;

			historyBlobs[i].centers.push_back(blobCenter);
			historyBlobs[i].rcs.push_back(mergeRC);
			historyBlobs[i].lastUpdateTime = now;
			//historyBlobs[i].histMat = histMat.clone();
			historyBlobs[i].frameIDs.push_back(frameID);
			historyBlobs[i].rgb.push_back(GetRectColor(mergeRC));
			historyBlobs[i].blobMat = (*grayFrameIMG)(mergeRC).clone();
			//从currentRC中删除
			currentRC[matchOLID] = currentRC.back();
			currentRC.pop_back();
		}else {
			//没有更新
			Rect trackedRC = historyBlobs[i].trackedRC;
			if ((trackedRC.width < minBlobSize.width && trackedRC.width > maxBlobSize.width) &&
				trackedRC.height < minBlobSize.height && trackedRC.height > maxBlobSize.height)
			{
				//Mat result;
				//Mat patch = (*grayFrameIMG)(historyBlobs[i].trackedRC).clone();
				////Mat resizePatch = patch.res
				////patch.resize();
				//resize(patch, patch, Size(historyBlobs[i].blobMat.cols, historyBlobs[i].blobMat.rows));
				////int result_cols =  patch.cols - historyBlobs[i].blobMat.cols + 1;
				////int result_rows = patch.rows - historyBlobs[i].blobMat.rows + 1; 
				//result.create( 1, 1, CV_32FC1 );
				//matchTemplate( patch, historyBlobs[i].blobMat, result, CV_TM_CCOEFF_NORMED);
				////normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
				//float matchValue = *((float *)(result.data));
				
				//imshow("patch", patch);
				//imshow("blobMat", historyBlobs[i].blobMat);
				
				//if (matchValue > 0.0)
				//{
					Rect mergeRC = historyBlobs[i].trackedRC;

					Point blobCenter;
					blobCenter.x = mergeRC.x + mergeRC.width/2;
					blobCenter.y = mergeRC.y + mergeRC.height/2;
					printf("ID:%d x:%d y:%d\n", i, blobCenter.x, blobCenter.y);
					historyBlobs[i].centers.push_back(blobCenter);
					historyBlobs[i].rcs.push_back(mergeRC);
					historyBlobs[i].lastUpdateTime = now;
					//historyBlobs[i].histMat = histMat.clone();
					historyBlobs[i].frameIDs.push_back(frameID);
					historyBlobs[i].rgb.push_back(GetRectColor(mergeRC));
					historyBlobs[i].blobMat = (*grayFrameIMG)(mergeRC).clone();
				//}
			}
			
		}
	}

	//新加入的区域
	for (int i = 0; i < currentRC.size(); i++)
	{
		WorkBlob wb;
		Point blobCenter;
		blobCenter.x = currentRC[i].x + currentRC[i].width/2;
		blobCenter.y = currentRC[i].y + currentRC[i].height/2;
		wb.centers.push_back(blobCenter);
		wb.lastUpdateTime = now;
		//wb.histMat = histMat.clone();
		wb.rcs.push_back(currentRC[i]);
		wb.frameIDs.push_back(frameID);
		wb.rgb.push_back(GetRectColor(currentRC[i]));
		wb.trackedRC = Rect(0,0,0,0);
		wb.blobMat = (*grayFrameIMG)(currentRC[i]).clone();
		historyBlobs.push_back(wb);
	}
}

void VTrack::InitMemory(Mat* frame)
{
    backgroundIMG32 = new Mat(frame->rows, frame->cols, CV_32FC1);
    backgroundIMG = new Mat(frame->rows, frame->cols, CV_8UC1);
    grayFrameIMG = new Mat(frame->rows, frame->cols, CV_8UC1);

    diffIMG  = new Mat(frame->rows, frame->cols, CV_8UC1);
    diffHist  = new Mat(frame->rows, frame->cols, CV_8UC1);
    diffHist->setTo(Scalar(0,0,0,0));
    
    colorIMG = new Mat(frame->rows, frame->cols, CV_8UC3);

}

Mat* VTrack::GetBackgroundIMG(){
    return backgroundIMG;
}


VBlob::VBlob(){
	id = -1;
	//startFrm = endFrm = -1;
}

VBlob::VBlob(WorkBlob& wb, int id){
    this->centers = wb.centers;
    this->frameIDs = wb.frameIDs;
    this->rcs = wb.rcs;
    this->id = id;
    //处理多帧
    //this->rgbValue = ?;
    int r = 0, g = 0, b = 0;
    int start = 0;
    int end = wb.centers.size();
    if (end > 24) //取中间6帧
    {
        start = end/2 - 3;
        end = start + 6;
    }else {
        start = end/4;
        end = end - start;
    }
    for (int i = start; i < end; i++)
    {
        b += wb.rgb[i][0];
        g += wb.rgb[i][1];
        r += wb.rgb[i][2];
    }
    end = end - start;
    b = b/end;
    g = g/end;
    r = r/end;
    this->rgbValue = Scalar(b,g,r);
}

VBlob::~VBlob(){
    vector<Rect>().swap(rcs);
    vector<Point>().swap(centers);
    vector<int>().swap(frameIDs);
}
/*
void VBlob::AddBlobInfo(int frmId, Rect pos, Point center){
	//if (frmId > endFrm){
	//	if(endFrm > 0)
	//		frmSegment.push_back(Point(startFrm,endFrm));
	//	startFrm = endFrm = frmId;
	//}
	//else
	//	endFrm = frmId;
	//blobInfo.push_back(Rect(x,y,width,height));
	//id = blobId;
	BlobInfo temp;
	//bool isRepeat;
	if(blobInfo.empty()){
		temp.startFrm = frmId;
	}else if(frmId -1 > blobInfo.back().currentFrm){
		temp.startFrm = frmId;
		frmSegment.push_back(Point(blobInfo.back().startFrm, blobInfo.back().currentFrm));
	}else{
		temp.startFrm = blobInfo.back().startFrm;
	}
	temp.position = pos;
	temp.center = center;
	temp.currentFrm = frmId;
	blobInfo.push_back(temp);
}

void VBlob::AppendBlobInfo(VBlob blob){
	for(int i = 0; i < blob.frmSegment.size(); i ++){
		frmSegment.push_back(blob.frmSegment[i]);
		blobInfo.push_back(blob.blobInfo[i]);
	}
	//blob.~VBlob();//合理吗？ 
}

void VBlob::ConvertWorkBlob(WorkBlob wblob, int Bid){
	frmSegment[0].x = wblob.frameIDs[0];//不会有重复出现，frmSegment将只有1个元素。。
	frmSegment[0].y = wblob.frameIDs.back();
	id = Bid;
	for (int i = 0; i < wblob.frameIDs.size(); i++)
	{
		AddBlobInfo(wblob.frameIDs[i], wblob.rcs[i], wblob.centers[i]);
	}
}
*/
Rect VBlob::GetFrameRect(int frmId){
	for (int i = 0; i < frameIDs.size(); i++){
		if(frmId == frameIDs[i]){
            return rcs[i];
        }
		
	}
	return Rect(0, 0, 0, 0);
}


VFrame::VFrame(){
	id = -1;
    blobCount = 0;
}
VFrame::~VFrame(){
	vector<int>().swap(blobIdx);
}

void VFrame::AddBlobId(int blobId){
    assert(id != -1);
	blobIdx.push_back(blobId);
    blobCount++;
}

vector<int> VFrame::GetBlobId(){
	return blobIdx;
}


VHandleData::VHandleData(){
	//isBlob = isBlob;
    frameCount = 0;
    blobCount = 0;
}
VHandleData::~VHandleData(){
	//if(isBlob)
	//	vector<VBlob>().swap(blobs);
	//else
	//	vector<VFrame>().swap(frames);
	vector<VBlob>().swap(blobs);
	vector<VFrame>().swap(frames);
}




/************************************************************************/
/*默认id从0开始顺序编号                                                 */
/************************************************************************/
/*
void VHandleData::StoreData(VBlob blob){
	if(blob.id > blobs.size())
		blobs.push_back(blob);
	else{//【3】基本不会出现
		blobs[blob.id].AppendBlobInfo(blob);
	}
}

void VHandleData::StoreData(vector<VBlob> blobs){
	//frames.push_back(frame);
	//VFrame tempfrm;
	//frames.reserve(blobs.size());
	for (int i = 0; i < blobs.size(); i++)
	{
		for(int j = 0; j < blobs[i].blobInfo.size(); j++)
		{
			frames[blobs[i].blobInfo[j].currentFrm].blobIdx.push_back(blobs[i].id);
			//tempfrm.id = blobs[i].blobInfo[j].currentFrm;
			//tempfrm.blobIdx.push_back(blobs[i].id);
		}
	}
}
*/
void VHandleData::StoreVFrame(int frameCount){
    this->frameCount = frameCount;
    frames = vector<VFrame>(frameCount);
    for (int i = 0; i < frameCount; i++)
    {
        frames[i].id = i;
    }

    for (int i = 0; i < blobCount; i++)
    {
        for (int j = 0; j < blobs[i].frameIDs.size(); j++)
        {
            frames[blobs[i].frameIDs[j]].AddBlobId(i);
        }
    }
}

void VHandleData::AddWorkBlob(WorkBlob wb){
    VBlob vb(wb, blobCount++);
    this->blobs.push_back(vb);
}
void VHandleData::WriteData(string filename){
    if (blobs.empty())
    {
        return;
    }
	ofstream ofs(filename.c_str());
	if(!ofs.is_open() ){
		return;
    }

	ofs << blobCount << endl;//blob个数
	for (int i = 0; i < blobCount; i++)
	{
		ofs << blobs[i].id << "\t" << blobs[i].frameIDs.size() << endl;
        ofs << blobs[i].rgbValue[0] << "\t"
            << blobs[i].rgbValue[1] << "\t"
            << blobs[i].rgbValue[2] << endl;
		for (int j = 0; j < blobs[i].frameIDs.size(); j++)
		{

			ofs << blobs[i].frameIDs[j] << "\t"
                << blobs[i].centers[j].x  << "\t"
				<< blobs[i].centers[j].y << endl;
			ofs << blobs[i].rcs[j].x << "\t"
				<< blobs[i].rcs[j].y << "\t"
				<< blobs[i].rcs[j].width << "\t"
				<< blobs[i].rcs[j].height << endl;
		}	
	}
	
	ofs << frameCount << endl;
	/*for(int i = 0; i < frameCount; i++)
	{
		ofs << frames[i].id << "\t" << frames[i].blobCount << endl;
		for (int j = 0; j < frames[i].blobCount; j++)
		{
			ofs << frames[i].blobIdx[j] << "\t";
		}
        if (frames[i].blobCount > 0)
        {
            ofs << endl;
        }
		
	}*/
	
	ofs.close();

	////--------------写数据库--------------------//
	//CString objtablename;
	//objtablename = "ObjectTable";
	//m_MysqlHandle->CreateObjectTable(objtablename);
	//NewTraceTable object;
	//for (int i = 0; i < blobCount; i++)
	//{
	//	object.segID = blobs[i].id;
	//	for (int j = 0; j < blobs[i].frameIDs.size(); j++)
	//	{
	//		object.origFrame = blobs[i].frameIDs[j];
	//		object.CentX     = blobs[i].centers[j].x;
	//		object.CentY     = blobs[i].centers[j].y;
	//		object.nLeft     = blobs[i].rcs[j].x;
	//		object.nTop      = blobs[i].rcs[j].y;
	//		object.nRight    = object.nLeft + blobs[i].rcs[j].width;
	//		object.nBottom   = object.nTop  + blobs[i].rcs[j].height;
	//		m_MysqlHandle->InsertData2ObjectTable(object,objtablename);
	//	}	
	//}
}

void VHandleData::ReadData(string filename){
	ifstream ifs(filename.c_str());
	stringstream ss;
	int blobsize,frmlen,frmid;
	Rect temprc;
	Point center;
//	int frmsize,bloblen,blobid;

	if (!ifs.is_open())
		return;

	ss << ifs.rdbuf();

	
	ss >> blobsize;
    blobCount = blobsize;
    blobs = vector<VBlob>(blobsize);
	for (int i = 0; i < blobsize; i++)
	{
        
		ss >> blobs[i].id >> frmlen;
        ss >> blobs[i].rgbValue[0] >> blobs[i].rgbValue[1] >> blobs[i].rgbValue[2];
        cout << "B:" << blobs[i].rgbValue[0] << " G:" << blobs[i].rgbValue[1] << " R:" << blobs[i].rgbValue[2] << endl;
		for (int j = 0; j < frmlen; j++)
		{
			
			ss >> frmid >> center.x >> center.y;
			ss >> temprc.x >> temprc.y >> temprc.width >> temprc.height;
            blobs[i].centers.push_back(center);
            blobs[i].rcs.push_back(temprc);
            blobs[i].frameIDs.push_back(frmid);
		}
		//blobs.push_back(tempblob);
	}

	
	ss >> frmlen;
    this->StoreVFrame(frmlen);
    /*frames = vector<VFrame>(frmlen);
	for (int i = 0; i < frmlen; i++)
	{
        //VFrame tempfrm;
		ss >> frames[i].id >> bloblen;
		for (int j = 0; j < bloblen; j++)
		{
			ss >> blobid;
			frames[i].blobIdx.push_back(blobid);
		}
        frames[i].blobCount = bloblen;
		//frames.push_back(tempfrm);
	}*/
	
	ifs.close();
}
void VHandleData::WriteDatabase(CString objtablename,double *speed,bool *ifContinue)
{
	if (blobs.empty() || !(*ifContinue))
	{
		return;
	}
	//--------------写数据库--------------------//
	m_MysqlHandle->CreateObjectTable(objtablename);
	NewTrackParam object;
	int i;
	for (i = 0; i < blobCount && *ifContinue; i++)
	{
		object.segID = blobs[i].id;
		for (int j = 0; j < blobs[i].frameIDs.size() && *ifContinue; j++)
		{
			object.origFrame = blobs[i].frameIDs[j];
			object.CentX     = blobs[i].centers[j].x;
			object.CentY     = blobs[i].centers[j].y;
			object.nLeft     = blobs[i].rcs[j].x;
			object.nTop      = blobs[i].rcs[j].y;
			object.nRight    = object.nLeft + blobs[i].rcs[j].width;
			object.nBottom   = object.nTop  + blobs[i].rcs[j].height;
			m_MysqlHandle->InsertData2ObjectTable(object,objtablename);
		}
		*speed = (double)i/(double)blobCount + 1.0;
	}
	if (i!=blobCount)
	{
		m_MysqlHandle->DropTable(objtablename);
		*speed = 0;
		return;
	}
	*speed = 2;
	
}
UINT DetectProcess(LPVOID pParam)
{
	VTrack *pTrackHandle = (VTrack*)pParam;
	pTrackHandle->DoProcessing();
	return 0;
}