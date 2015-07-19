#include "areaDetect.h"
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <stdio.h>

using namespace std;
using namespace cv;

areaDetect::areaDetect(){}

areaDetect::~areaDetect(){}

bool areaDetect::detect(CvRect r)
{
	int maxx = min(r.x+r.width,rect.x+rect.width);
	int maxy = min(r.y+r.height,rect.y+rect.height);
	int minx = max(r.x,rect.x);
	int miny = max(r.y,rect.y);
	
	if (minx > maxx || miny > maxy)
		return false;
	else
	if (abs(maxx - minx)*abs(maxy - miny)>rect.width*rect.height*0.1 || abs(maxx - minx)*abs(maxy - miny)>r.width*r.height*0.1)
			return true;
		else
			return false;
	
}