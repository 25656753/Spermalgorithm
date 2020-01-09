#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <iostream>

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;


void surfd()
{


	


	Mat src, ingray, dst;
	src = imread("C:/Users/25656/Desktop/new avi/20120101103537/10.jpg");
	if (!src.data)
	{
		cout << "read image fail£¡\n";
		return;
	}
	cvtColor(src, ingray, COLOR_BGR2BGRA);
	
	Ptr<SURF> surf = SURF::create();
	vector<KeyPoint> keypoints;
	surf->detect(ingray, keypoints);
//	src.copyTo(dst);
	Mat t1;
	surf->compute(ingray, keypoints, t1);
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");
	vector<DMatch> matchs;
	
	drawKeypoints(src, keypoints, dst, Scalar(0, 0, 255));
	imshow("dst", dst);
	imshow("dstaa", t1);
	waitKey(0);

}