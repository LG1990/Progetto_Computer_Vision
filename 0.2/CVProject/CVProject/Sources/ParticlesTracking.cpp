#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Point2f pt;
bool addRemovePt = false;

void onMouse( int event, int x, int y, int flags, void* param )
{
    if( event == CV_EVENT_LBUTTONDOWN )
    {
        pt = Point2f((float)x,(float)y);
        addRemovePt = true;
    }
}

void myHotspotDetector(vector<Point2f> myPoints)
{
	vector<int> j;
	for(int i=0; i < myPoints.size(); i++)
	{
		if(myPoints[i].x < 40 && myPoints[i].y < 30)
		{
			j.push_back(i);
			cout << j.size();
		}

	}

	//waitKey(30000);
}

void myGrid(Mat img)
{
	int thickness = 2;
	int lineType = 8;
	for(int x=40; x<320; x+=40)
	{
		line(img, Point(x,0), Point(x,240), Scalar(255,0,0),thickness, lineType);
	}
	for(int y=30; y<240; y+=30)
	{
		line(img, Point(0,y), Point(320,y), Scalar(255,0,0),thickness, lineType);
	}

}

int main( int argc, char** argv )
{
    VideoCapture cap;
    TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03);
    Size winSize(25,25);

    const int MAX_COUNT = 400;
    bool needToInit = true;
    bool nightMode = false;
    cap.open("../2low.avi");

    if( !cap.isOpened() )
    {
        cout << "Could not initialize capturing...\n";
        return 0;
    }

    // print a welcome message, and the OpenCV version
    cout << "Welcome to lkdemo, using OpenCV version %s\n", CV_VERSION;

    cout << "\nHot keys: \n"
            "\tESC - quit the program\n"
            "\tr - auto-initialize tracking\n"
            "\tc - delete all the points\n"
            "\tn - switch the \"night\" mode on/off\n"
            "To add/remove a feature point click it\n";

    namedWindow( "LK Demo", 1 );
    setMouseCallback( "LK Demo", onMouse, 0 );

    Mat gray, prevGray, image;
    vector<Point2f> points[2];
	int frame_number = 1; 
    
	for(;;)
    {
        Mat frame;
        cap >> frame;
        if( frame.empty() )
            break;

        frame.copyTo(image);
        cvtColor(image, gray, CV_BGR2GRAY);

        if( nightMode )
            image = Scalar::all(0);

        if( needToInit )
        {
            // automatic initialization
            goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
            cornerSubPix(gray, points[1], winSize, Size(-1,-1), termcrit);
            addRemovePt = false;
        }

        else if( !points[0].empty() )
        {
            vector<uchar> status;
            vector<float> err;
            if(prevGray.empty())
                gray.copyTo(prevGray);
			
			calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize,
                                 3, termcrit, 0);

            size_t i, k;
            
			for( i = k = 0; i < points[1].size(); i++ )
            {
                if( addRemovePt )
                {
                    if( norm(pt - points[1][i]) <= 5 )
                    {
                        addRemovePt = false;
                        continue;
                    }
                }

                if( !status[i] )
                    continue;

                points[1][k++] = points[1][i];
                circle( image, points[1][i], 3, Scalar(0,255,0), -1, 8);
            }
            points[1].resize(k);
        }

		myGrid(image);

        if( addRemovePt && points[1].size() < (size_t)MAX_COUNT )
        {
            vector<Point2f> tmp;
            tmp.push_back(pt);
            cornerSubPix( gray, tmp, winSize, cvSize(-1,-1), termcrit);
            points[1].push_back(tmp[0]);
            addRemovePt = false;
        }

		if (frame_number%58 == 0)
			{ needToInit = true; }
		else { needToInit = false; }

		myHotspotDetector(points[0]);

        imshow("LK Demo", image);

        char c = (char)waitKey(10);
        if( c == 27 )
            break;
        switch( c )
        {
        case 'r':
            needToInit = true;
            break;
        case 'c':
            //points[1].clear();
            points[1].erase(points[1].begin(), points[1].end());
            points[0].erase(points[0].begin(), points[0].end());
            break;
        case 'n':
            nightMode = !nightMode;
            break;
        default:
            ;
        }

        std::swap(points[1], points[0]);
        swap(prevGray, gray);
		frame_number += 1;
		//cout << frame_number;

        //if(waitKey(30)>0)
        	//break;
    }
    
	system ("PAUSE");
	return 0;
}
