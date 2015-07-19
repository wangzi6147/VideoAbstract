#pragma  once
#include "opencv2\legacy\legacy.hpp"

using namespace std;

class areaDetect
{
public:
	areaDetect();
	~areaDetect();
	bool detect(CvRect r);
	CvRect rect;
};