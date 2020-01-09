
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void splitimg()
{
	Mat frame;
	int framepos = 1;
	VideoCapture capture;
	capture.open("C:/Users/25656/Desktop/new avi/20120101103537.mp4");
	if (!capture.isOpened()) {
		printf("could not load video file...\n");
		return;
	}
	while (capture.isOpened())
	{
		capture >> frame;
		if (!frame.data)
		{
			printf("could not load video file...\n");
			return;
		}
		imwrite("C:/Users/25656/Desktop/new avi/20120101103537/" + to_string(framepos) + ".jpg", frame);
		framepos++;

	} 
	cout << "game over" << endl;
	waitKey(0);
}