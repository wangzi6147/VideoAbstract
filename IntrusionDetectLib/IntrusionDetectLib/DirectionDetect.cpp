#include "DirectionDetect.h"

///@brief 计算向量的斜率
/// 
///@param[in] 向量起点p1，向量终点p2
///@return 斜率
float DirectionDetect::computeGradient(CvPoint p1,CvPoint p2)
{
	return (float) (p2.y - p1.y) / ( p2.x - p1.x ) ;
}

///@brief 计算向量的方向角
/// 
///@param[in] 向量起点p1，向量终点p2
///@return 方向角（度）
float DirectionDetect::computeTheta(CvPoint p1,CvPoint p2)
{
	float theta;
	float thetaCos;
	CvPoint p;
	CvPoint refPoint;
	refPoint.x=1;
	refPoint.y=0;
	p.x=p2.x-p1.x;
	p.y=p2.y-p1.y;

	if (p1.x==p2.x&&p1.y==p2.y)
		return 200;
	else 
		theta=atan(computeGradient(p1,p2))*180/M_PI;
		thetaCos=computeCos(p,refPoint);
		if(theta>0)
		{
			if (thetaCos<0)
			{
				return theta-180;
			}
			return theta;
		}
		else
		{
			if (thetaCos<0)
			{
				return theta+180;
			}
			return theta;
		}
}

///@brief 计算轨迹的方向角或者斜率
/// 
///@param[in] 轨迹vector指针，标志位flag（为GRADIENT时计算斜率，为THETA时计算方向角）,间隔delta个数据算一次
///@param[out] 方向角或斜率容器指针
///@return int
int DirectionDetect::computeTrajectory(vector<CvPoint>* trajectory,vector<float>* result,UINT flag,int delta)
{
	if (trajectory->size()<delta+1)
	{
		return -1;
	}
	if (flag==GRADIENT)
	{
		result->push_back(0);//第一个点无法计算斜率，赋值为0
		for (int i=delta;i<trajectory->size();i++)
		{
			result->push_back(computeGradient(trajectory->at(i-delta),trajectory->at(i)));
		}
		return 0;
	}
	else if (flag==THETA)
	{
		result->push_back(0);//第一个点无法计算方向角，赋值为0
		for (int i=delta;i<trajectory->size();i++)
		{
			result->push_back(computeTheta(trajectory->at(i-delta),trajectory->at(i)));
		}
		return 0;
	}
	else
	{
		return -2;
	}
}

///@brief 计算轨迹的方向角或者斜率
/// 
///@param[in] 前景的左上角坐标vector，以及前景的宽width，高height
///@param[out] 前景的中心坐标vector
///@return int
int DirectionDetect::computeCenterOfTrajectory(vector<CvPoint>* cornerPoint,int width,int height,vector<CvPoint>* centerPoint)
{
	CvPoint CP;
	if (cornerPoint->size()==0)
	{
		return -1;
	}
	for (int i=0;i<cornerPoint->size();i++)
	{
		CP.x=(cornerPoint->at(i).x)+width/2;
		CP.y=(cornerPoint->at(i).y)+height/2;

		centerPoint->push_back(CP);
	}
	return 0;
}


//0309测试
bool DirectionDetect::computeOrientByTwoFrame(vector<IntrOb>* befOb, vector<IntrOb>*curOb, int direction)
{
	CvPoint p1;
	CvPoint p2;
	//CvPoint p;
	//CvPoint refPoint;
	//refPoint.x=1;
	//refPoint.y=0;
	bool ifDetect = false;
	for (int i = 0; i<curOb->size(); i++)
	{
		for (int j = 0; j<befOb->size(); j++)
		{
			if (curOb->at(i).segID == befOb->at(j).segID)
			{
				//换算中心坐标
				//p1 = cvPoint(befOb->at(j).detecR.x, befOb->at(j).detecR.x);

				float temp1, temp2;
				p1.x = befOb->at(j).detecR.x + (befOb->at(j).detecR.width)*0.5;
				p1.y = befOb->at(j).detecR.y + (befOb->at(j).detecR.height)*0.5;
				p2.x = curOb->at(i).detecR.x + (curOb->at(i).detecR.width)*0.5;
				p2.y = curOb->at(i).detecR.y + (curOb->at(i).detecR.height)*0.5;
				//p.x=p2.x-p1.x;
				//p.y=p2.y-p1.y;
				temp1 = computeTheta(p1, p2);
				temp2 = computeModule(p1, p2);
				if (temp1 != 200)
				{
					temp1 = (befOb->at(j).module*befOb->at(j).orient + temp2*temp1) / (befOb->at(j).module + temp2);
					curOb->at(i).orient = temp1;


					curOb->at(i).module = (befOb->at(j).module + temp2) / 2;
					curOb->at(i).center = p2;
					//判断方向，temp1为200时不返回
					switch (direction)
					{
					case 0:
					case 2:
						if (curOb->at(i).orient<-45 && curOb->at(i).orient>-135)
							ifDetect = true;
						break;
					case 1:
						if (curOb->at(i).orient>45 && curOb->at(i).orient<135)
							ifDetect = true;
						break;
					case 3:
						if (curOb->at(i).orient<45 && curOb->at(i).orient>-45)
							ifDetect = true;
						break;
					case 4:
						if (curOb->at(i).orient<-135 || curOb->at(i).orient>135)
							ifDetect = true;
						break;
					default:
						break;
					}
					continue;
				}
				else
				{
					curOb->at(i).orient = befOb->at(j).orient;
					curOb->at(i).center = befOb->at(j).center;
					continue;
				}
			}
		}
	}
	return ifDetect;
}


void DirectionDetect::drawOrient(Mat mat, vector<IntrOb>*curOb)
{
	CvScalar color = CV_RGB(0, 255, 255);
	//IplImage* curIplImage=new IplImage;
	//*curIplImage=*curMatImage;
	double r;
	for (int i = 0; i<curOb->size(); i++)
	{
		r = ((curOb->at(i).detecR.width>curOb->at(i).detecR.height) ? curOb->at(i).detecR.height : curOb->at(i).detecR.width) / 2;
		//cvCircle(curIplImage,curOb->at(i).center,cvRound(r), color, 3, CV_AA, 0 );
		if (curOb->at(i).orient != 0)
		{
			/*IplImage * p = new IplImage();
			*p = mat;*/
			line(mat, curOb->at(i).center, cvPoint(cvRound(curOb->at(i).center.x + r*cos(curOb->at(i).orient*CV_PI / 180)),
				cvRound(curOb->at(i).center.y + r*sin(curOb->at(i).orient*CV_PI / 180))), Scalar(255, 0, 0), 3);
			/*cvLine(p, curOb->at(i).center, cvPoint(cvRound(curOb->at(i).center.x + r*cos(curOb->at(i).orient*CV_PI / 180)),
			cvRound( curOb->at(i).center.y + r*sin(curOb->at(i).orient*CV_PI/180))), color, 3, CV_AA, 0 );
			delete p;*/
		}

		//if (cvRound(curOb->at(i).center.x + r*cos(curOb->at(i).orient*CV_PI/180))<0)
		//{
		//	cout<<curOb->at(i).orient<<","<<r<<endl;
		//}
		//cout<<cvRound(curOb->at(i).center.x + r*cos(curOb->at(i).orient*CV_PI/180))<<","<<cvRound( curOb->at(i).center.y - r*sin(curOb->at(i).orient*CV_PI/180))<<endl;
	}
	//*curMatImage=curIplImage;
	//delete curIplImage;
}


float DirectionDetect::computeCos(CvPoint p1,CvPoint p2)
{
	return (float)(p1.x*p2.x+p1.y*p2.y)/(sqrt(double(p1.x*p1.x+p1.y*p1.y))*sqrt(double(p2.x*p2.x+p2.y*p2.y)));
}


int DirectionDetect::computeModule(CvPoint p1,CvPoint p2)
{
	return (int)sqrt((double)((p2.x-p1.x)*(p2.x-p1.x)+(p2.y-p1.y)*(p2.y-p1.y)));
}



bool DirectionDetect::initHistoryData(int deltaFrame)
{
	if (deltaFrame<1)
	{
		return false;
	}
	delta=deltaFrame;
	historyData.empty();
	return true;
}


bool DirectionDetect::computeOrient(vector<IntrOb>*curOb, int direction)//用隔一定数量的两帧数据计算方向
{
	bool ifDetect=0;
	if (delta<1)
	{
		return false;
	}
	if (historyData.size()<delta)
	{
		historyData.push(*curOb);
	}
	if (historyData.size()==delta)
	{
		ifDetect = computeOrientByTwoFrame(&historyData.front(), curOb, direction);


		historyData.push(*curOb);
		historyData.pop();
	}
	return ifDetect;
}