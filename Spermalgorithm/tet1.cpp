#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;
void tet1()
{


	Point2f corners[4];


	Mat src, ingray,dst;
	src=imread("C:/Users/25656/Desktop/new avi/20120101103537/10.jpg");
	if (!src.data)
	{
		cout << "read image fail£¡\n";
		return;
	}
	
	cvtColor(src, ingray, COLOR_RGB2GRAY);
	double maxCorners = 5000;
	double qualitylevel = 0.005;
	double minDistance = 10;
	double blockSize = 3;
	double k = 0.04;
	vector<Point2f> features;
	goodFeaturesToTrack(ingray, features, maxCorners, qualitylevel, minDistance, Mat(), blockSize, false, k);
	
	/*float centerx, centery;
	centery = src.rows / 2;
	centerx = src.cols / 2;
	Rect rect(centerx-200, centery-200, 400,400);
	
	
	Mat mask=Mat::zeros(src.size(),CV_8UC1);

	mask(rect).setTo(255);

	src(rect).copyTo(dst);*/
	
	src.copyTo(dst);
	for (size_t i = 0; i < features.size(); i++)
	{
		circle(dst, features[i], 3,  Scalar(0,0,255), 1, 8);
	}

	imshow("srcimg", src);
	

	

imshow("dstimg", dst);
	waitKey(0);


}