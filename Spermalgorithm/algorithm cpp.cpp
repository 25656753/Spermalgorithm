#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

extern void tet1();
extern void splitimg();
extern void surfd();

//// shi-tomasi角点检测精子+KLT光流跟踪   

Mat frame, gray;
Mat prev_frame, prev_gray;
vector<Point2f> features; // shi-tomasi角点检测 - 特征数据

vector<Point2f> iniPoints; // 初始化特征数据
vector<Point2f> fpts[2]; // 保持当前帧和前一帧的特征点位置

vector<uchar> status; // 特征点跟踪成功标志位
vector<float> errors; // 跟踪时候区域误差和


vector<vector<Point2f>>  rsinitPoints;   // 初始化位置数组
vector<vector<Point2f>>  rsPoints;        //尾桢精子位置
vector<int>      rstime;     // 检测时间
vector<int>      rscount;  //首桢检测到精子数量
vector<int>      rs1count;  //尾桢跟踪到精子数量
vector <vector<double>>      rsdist;   //精子位移数组 

float thresholdration = 0.98f;


void drawFeature(Mat &inFrame);
void detectFeatures(Mat &inFrame, Mat &ingray);
void klTrackFeature();
void drawTrackLines();

//计算精子位移 s=|x|+|y|;
void calcdist(vector<vector<Point2f>>  orig, vector<vector<Point2f>> laig, vector <vector<double>>& rs);

int main(int argc, char** argv) {
	//surfd();
	//return 0;
	//VideoCapture capture(0);
	VideoCapture capture;
	//capture.open("C:/Users/25656/Desktop/new avi/20120101103537.mp4");
//	capture.open("C:/Users/25656/Desktop/20190801/110259-av-1.avi");
	capture.open("c:/vv/20190920112619.mp4");

	if (!capture.isOpened()) {
		printf("could not load video file...\n");
		return -1;
	}
	int i = 0;
	while (i < 10)
	{
		capture.read(frame);
		i++;
	}
	namedWindow("camera input", CV_WINDOW_AUTOSIZE);
	moveWindow("camera input", 200, 0);
	i = 0;
	while (capture.read(frame)) {
		//flip(frame, frame, 1);
		resize(frame, frame, Size(frame.cols / 2, frame.rows /2));
		cvtColor(frame, gray, COLOR_BGR2GRAY);
		if ((fpts[0].size() < (features.size()*thresholdration)) ||
			(iniPoints.size()==0)){
			detectFeatures(frame, gray);
		    
			if (features.size() >= 500) {
				thresholdration=0.90f;
			}
			else if (features.size() >= 300) {
				thresholdration = 0.92f;
				}
			else if (features.size() >= 100) {
				thresholdration = 0.97f;
			}
			

		

			if (fpts[0].size() > 0) //如果不是视pin第一个frame，删除掉之前历史数据，重新开始跟踪
			{
				rs1count.push_back(fpts[0].size());
				rstime.push_back(i * 50);
				rsinitPoints.push_back(iniPoints);
				rsPoints.push_back(fpts[0]);
				fpts[0].clear();
				iniPoints.clear();
				i = 0;
			}
			rscount.push_back(features.size());
			fpts[0].insert(fpts[0].end(), features.begin(), features.end());
			iniPoints.insert(iniPoints.end(), features.begin(), features.end());
		}
		else {
			i++;
			printf("跟踪成功精子数量...%d\n", fpts[0].size());
		}

		if (prev_gray.empty()) {
			gray.copyTo(prev_gray);
		}

		klTrackFeature();
		drawFeature(frame);

		// 更新前一帧数据
		gray.copyTo(prev_gray);
		frame.copyTo(prev_frame);
		imshow("camera input", frame);

		char c = waitKey(50);
		if (c == 27) {
			break;
		}
	}
	//视pin结束取得最后一次检测时长
	rs1count.push_back(fpts[0].size());
	rstime.push_back(i * 50);
	rsinitPoints.push_back(iniPoints);
	rsPoints.push_back(fpts[0]);

	calcdist(rsinitPoints, rsPoints, rsdist);


	ofstream  f_out("d:/sin.txt");
	f_out << "总共检测" << rsinitPoints.size() << "次" << endl;
	f_out << "------------------------------每次时长ms" << endl;
	f_out << Mat(rstime) << endl;
	f_out << "------------------------------每次首帜检测精子数目" << endl;
	f_out << Mat(rscount) << endl;
	f_out << "------------------------------每次末帜检测到精子数目" << endl;
	f_out << Mat(rs1count) << endl;
	f_out << "------------------------------距离数据 px " << endl;

	for (int j = 0; j < rsinitPoints.size(); j++)
	{
		f_out << "--------------------------------------------------------------------------" << j + 1 << endl;
		f_out << Mat(rsdist[j]) << endl;

	}
	f_out.close();
	cout << "game over" << endl;
	cv::waitKey(0);
	return 0;
}

void detectFeatures(Mat &inFrame, Mat &ingray) {
	double maxCorners = 5000;
	double qualitylevel = 0.005;
	double minDistance = 10;
	double blockSize = 3;
	double k = 0.04;

	goodFeaturesToTrack(ingray, features, maxCorners, qualitylevel, minDistance, Mat(), blockSize, false, k);

	cout << "detect features : " << features.size() << endl;
}

void drawFeature(Mat &inFrame) {
	for (size_t t = 0; t < fpts[0].size(); t++) {
		circle(inFrame, fpts[0][t], 3, Scalar(0, 0, 255), 1, 8, 0);
	}
}

void klTrackFeature() {
	// KLT
	calcOpticalFlowPyrLK(prev_gray, gray, fpts[0], fpts[1], status, errors);
	//printf("fpts[0]%d fpts[1]%d\n", fpts[0].size(), fpts[1].size());
	int k = 0;
	// 特征点过滤
	for (int i = 0; i < fpts[1].size(); i++) {
		double dist = abs(fpts[0][i].x - fpts[1][i].x) + abs(fpts[0][i].y - fpts[1][i].y);
		if (status[i] && (dist < 15)) {
			iniPoints[k] = iniPoints[i];
			fpts[1][k++] = fpts[1][i];
		}
	}
	// 保存特征点并绘制跟踪轨迹
	iniPoints.resize(k);
	fpts[1].resize(k);
	drawTrackLines();

	std::swap(fpts[1], fpts[0]);
}

void drawTrackLines() {
	for (size_t t = 0; t < fpts[1].size(); t++) {
		line(frame, iniPoints[t], fpts[1][t], Scalar(0, 255, 0), 1, 8, 0);
		circle(frame, fpts[1][t], 3, Scalar(0, 0, 255), 1, 8, 0);
	}
}
//计算初始原点和终点之前的距离
void calcdist(vector<vector<Point2f>>  orig, vector<vector<Point2f>> laig, vector <vector<double>>& rs)
{
	for (size_t t = 0; t < orig.size(); t++)
	{
		vector<double> dists;
		for (size_t k = 0; k < orig[t].size(); k++)
		{

			double dist = abs(orig[t][k].x - laig[t][k].x) + abs(orig[t][k].y - laig[t][k].y);
			dists.push_back(dist);
		}
		rs.push_back(dists);
	}
}