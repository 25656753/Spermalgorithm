#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

int smin = 15;
int vmin = 40;
int vmax = 256;
int bins = 16;
int main1(int argc, char** argv) {
	VideoCapture capture;
	capture.open("C:/Users/25656/Desktop/20190801/110259-av-1.avi");
	if (!capture.isOpened()) {
		printf("could not find video data file...\n");
		return -1;
	}
	namedWindow("CAMShift Tracking", CV_WINDOW_AUTOSIZE);
	namedWindow("ROI Histogram", CV_WINDOW_AUTOSIZE);

	bool firstRead = true;
	float hrange[] = { 0, 180 };
	const float* hranges = hrange;
	Rect selection;
	Mat frame, hsv, hue, mask, hist, backprojection;
	Mat drawImg = Mat::zeros(300, 300, CV_8UC3);
	while (capture.read(frame)) {
		if (firstRead) {
			Rect2d first = selectROI("CAMShift Tracking", frame);
			selection.x = first.x;
			selection.y = first.y;
			selection.width = first.width;
			selection.height = first.height;
			printf("ROI.x= %d, ROI.y= %d, width = %d, height= %d", selection.x, selection.y, selection.width, selection.height);
		}
		// convert to HSV
		cvtColor(frame, hsv, COLOR_BGR2HSV);
		inRange(hsv, Scalar(0, smin, vmin), Scalar(180, vmax, vmax), mask);
		hue = Mat(hsv.size(), hsv.depth());
		int channels[] = { 0, 0 };
		mixChannels(&hsv, 1, &hue, 1, channels, 1);

		if (firstRead) {
			// ROI ÷±∑ΩÕºº∆À„
			Mat roi(hue, selection);
			Mat maskroi(mask, selection);
			calcHist(&roi, 1, 0, maskroi, hist, 1, &bins, &hranges);
			normalize(hist, hist, 0, 255, NORM_MINMAX);

			// show histogram
			int binw = drawImg.cols / bins;
			Mat colorIndex = Mat(1, bins, CV_8UC3);
			for (int i = 0; i < bins; i++) {
				colorIndex.at<Vec3b>(0, i) = Vec3b(saturate_cast<uchar>(i * 180 / bins), 255, 255);
			}
			cvtColor(colorIndex, colorIndex, COLOR_HSV2BGR);
			for (int i = 0; i < bins; i++) {
				int  val = saturate_cast<int>(hist.at<float>(i)*drawImg.rows / 255);
				rectangle(drawImg, Point(i*binw, drawImg.rows), Point((i + 1)*binw, drawImg.rows - val), Scalar(colorIndex.at<Vec3b>(0, i)), -1, 8, 0);
			}
		}

		// back projection
		calcBackProject(&hue, 1, 0, hist, backprojection, &hranges);
		// CAMShift tracking
		backprojection &= mask;
		RotatedRect trackBox = CamShift(backprojection, selection, TermCriteria((TermCriteria::COUNT | TermCriteria::EPS), 10, 1));

		// draw location on frame;
		ellipse(frame, trackBox, Scalar(0, 0, 255), 3, 8);

		if (firstRead) {
			firstRead = false;
		}
		imshow("CAMShift Tracking", frame);
		imshow("ROI Histogram", drawImg);
		char c = waitKey(50);// ESC
		if (c == 27) {
			break;
		}
	}

	capture.release();
	waitKey(0);
	return 0;
}