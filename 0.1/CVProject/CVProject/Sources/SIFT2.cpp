// SIFT2.cpp : Defines the entry point for the console application.
//
//#include "stdafx.h"

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv2/features2d/features2d.hpp>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>

using namespace std;
using namespace cv;

int main()
{
	cv::initModule_nonfree();

	Mat frame, output;
	cv::VideoCapture capture("../2low.avi");

	if (!capture.isOpened())
		return 0;
	capture.read(frame);

	//printf("%s",filename);

	int count = 0;
	char filename[200];
	Ptr<FeatureDetector> detector = cv::FeatureDetector::create("SIFT");
	vector<KeyPoint> keypoints;
	bool stop = true;

	while (stop)
	{

		detector->detect(frame, keypoints);
		drawKeypoints(frame, keypoints, output);
		namedWindow("Output");
		imshow("Output", output);
		imshow("frame", frame);

		if (!capture.read(frame)) stop = false;
		waitKey(2);

	}



	return 0;
}



