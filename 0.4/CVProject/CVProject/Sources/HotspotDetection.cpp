
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

extern int frame_number = 1;
bool addRemovePt = false;
Point2f pt;


int myRoutine();
void myHotspotDetector(vector<Point2f> myPoints, int myCapHeight, int myCapWidth);
void onMouse( int event, int x, int y, int flags, void* param );
int myKeyboard(bool needToInit, bool nightMode);


int main( int argc, char** argv )
{
	myRoutine();

	system("PAUSE");
	return 0;
}


int myRoutine()
{
	VideoCapture cap;
    TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03);
    Size winSize(10,10);

    const int MAX_COUNT = 400;
    bool needToInit = false;
    bool nightMode = false;

	//cap.open("../2low.avi");
	cap.open("D:/ktmfbz/Desktop/crowd1.avi");

    if( !cap.isOpened() )
    {
        cout << "Could not initialize capturing...\n";
        return 1;
	}
	else
	{
    // print a welcome message, and the OpenCV version
    cout << "Welcome to Hotspot detection, using OpenCV version %s\n", CV_VERSION;

    cout << "\nHot keys: \n"
            "\tESC - quit the program\n"
            "\tr - auto-initialize tracking\n"
            "\tc - delete all the points\n"
            "\tn - switch the \"night\" mode on/off\n"
            "To add/remove a feature point click it\n";
	}

	int stat = 0;

	Mat image, gray, prevGray;

	namedWindow("Feature tracking", 1);
	setMouseCallback("Feature tracking", onMouse, 0);

	namedWindow("Hotspot detection", 1);

	vector<Point2f> points[2];

	for(;;)
	{		
		if(frame_number%(int)(cap.get(CV_CAP_PROP_FPS)*1)==0 || frame_number==1)	//reinitialize if we are at the first frame or every 2 sec; frmae_rate->CV_CAP_PROP_FPS
		{ needToInit = true; }
		else 
		{ needToInit = false; }

		Mat frame;

		cap >> frame;
		if(frame.empty()) break;
			
		frame.copyTo(image);
		cvtColor(image, gray, CV_BGR2GRAY);

		if(nightMode) image = Scalar::all(0);
	
		if(needToInit)
		{
			//automatic initializiation
			goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
			cornerSubPix(gray, points[1], winSize, Size(-1,-1), termcrit);
			addRemovePt = false;
		}
		else if(!points[0].empty())
		{
			vector<uchar> status;
			vector<float> err;
			if(prevGray.empty())
				gray.copyTo(prevGray);
			calcOpticalFlowPyrLK(prevGray,gray, points[0], points[1], status, err, winSize, 3, termcrit, 0);
		
			size_t i, k;
			for (i = k = 0; i < points[1].size(); i++)
			{
				if(addRemovePt)
				{
					if(norm(pt - points[1].at(i)) <= 5)
					{
						addRemovePt = false;
						continue;
					}
				}

				if(!status[i])
					continue;

				points[1][k++] = points[1][i];
				cv::circle(image, points[1].at(i), 3, Scalar(0, 255, 0), -1, 8);
			}//end for

			points[1].resize(k);
		}//end else if

		if(addRemovePt && points[1].size()<(size_t)MAX_COUNT)
		{
			vector<Point2f> myTmp;
			myTmp.push_back(pt);
			cornerSubPix(gray, myTmp, winSize, cvSize(-1,-1), termcrit);
			points[1].push_back(myTmp[0]);
			addRemovePt = false;
		}
		
		needToInit = false;

		imshow("Feature tracking", image);

		myKeyboard(needToInit, nightMode);
		if(frame_number>1)
		{ myHotspotDetector(points[1], (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT), (int)cap.get(CV_CAP_PROP_FRAME_WIDTH)); }

		std::swap(points[1], points[0]);
		swap(prevGray, gray);

		frame_number += 1;

	}

	return 0;
}

void myHotspotDetector(vector<Point2f> myPoints, int myCapHeight, int myCapWidth)
{
	//Create a 2D map with the feature points
	Mat my2dMap(myCapHeight,myCapWidth,CV_8UC1(0));
	my2dMap = Scalar(0);
	for (int i = 0; i < myPoints.size(); i++)
	{
		cv::circle(my2dMap, myPoints.at(i), 3, Scalar(255), -1, 8);
	}
	
	//Opening and closure operations on binary map of feature points
	//CODE HERE
	Mat myKernel = getStructuringElement(MORPH_RECT, Size(7, 7), Point(3, 3) );  
	//cv::morphologyEx(my2dMap, my2dMap, MORPH_OPEN, myKernel);
	dilate(my2dMap, my2dMap, myKernel, Point(-1,-1), 2, 0);

	//Gaussian filtering on morphed image
	//CODE HERE
	GaussianBlur(my2dMap,my2dMap,Size(31, 31),-1,-1);
	
	//Hotspot candidate in every point with value higher than 0.99(*255)
	//CODE HERE
	vector<Point3f> myHSPoints;

	for (int rows = 0; rows < my2dMap.rows; rows++)
	{
		for (int cols = 0; cols < my2dMap.cols; cols++)
		{
			if(my2dMap.at<uchar>(rows,cols)>0.995*255)
			{ 
				myHSPoints.push_back(Point3f(rows,cols,0)); 
				//cout << rows << ", " << cols << endl; 
			}
		}
	}
	
	//assign a belonging zone to each hotspot detected
	int zone_number = 0;
	vector<int> index;

	if(!myHSPoints.empty())
	{
		for (int v = 0; v < myHSPoints.size(); v++)
		{
			if(myHSPoints[v].z==0)
			{
				for (int i = 0; i <  myHSPoints.size(); i++)
				{
					if(abs(myHSPoints[v].x-myHSPoints[i].x) < 50 && i!=v)
					{ index.push_back(i); }
				}
				for (int i = 0; i < index.size(); i++)
				{
					if(myHSPoints[v].z==0){ myHSPoints[v].z = zone_number + 1; }
					if(abs(myHSPoints[v].y-myHSPoints[index[i]].y) < 50)
					{ 
						myHSPoints[index[i]].z = myHSPoints[v].z;
					}
				}
				index.clear();
				index.shrink_to_fit();
				zone_number++;
			}
		}
	}

	//find the median of each zone
	vector<Point3f> myHSMedianPoint;
	int x, y, den;
	
	for (int v = 0; v < zone_number; v++)
	{
		x  =  0;
		y  =  0;
		den = 0;
		for (int i = 0; i < myHSPoints.size(); i++)
		{
			if(myHSPoints[i].z == v)
			{ 
				//media di tutte le x di tutti i punti appartenenti ad una certa zona
				x += myHSPoints[i].x;
				//media di tutte le y di tutti i punti appartenenti ad una certa zona
				y += myHSPoints[i].y;
				den++;
			}
		}
		//punto medio :-> (x, y, # zona)
		//solo se il numero di punti per zona è sufficientemente alto
		if(den>4)
		{ myHSMedianPoint.push_back(Point3f(x/den, y/den, den)); 
			cout << frame_number << "\t->\t" << x/den << ", " << y/den << endl; }
	}
	cout << "\n" << endl;

	Mat my2dColorMap;
	cvtColor(my2dMap, my2dColorMap, CV_GRAY2BGR);

	//if(!myHSPoints.empty())
	//{
	//	for (int i = 0; i < myHSPoints.size(); i++)
	//	{
	//		cv::circle(my2dColorMap, Point(myHSPoints.at(i).x, myHSPoints.at(i).y), 3, Scalar(0,0,255), -1, 0);
	//	}
	//}
		
	if(!myHSMedianPoint.empty())
	{
		for (int i = 0; i < myHSMedianPoint.size(); i++)
		{
			cv::circle(my2dColorMap, Point(myHSMedianPoint.at(i).x, myHSMedianPoint.at(i).y), 15, Scalar(0,0,255), -1, 0);
		}
	}

	cv::imshow("Hotspot detection", my2dColorMap);

	//Display map of gravitation (->red=higher #, ->blu=lower #)
	//CODE HERE
	cvtColor(my2dMap, my2dMap, CV_GRAY2BGR);
	Mat myKernel2 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(2, 2) );  
	//dilate(my2dMap, my2dMap, myKernel2, Point(-1,-1), 2, 0);
	morphologyEx(my2dMap,my2dMap,MORPH_CLOSE,myKernel,Point(-1,-1));

	for (int i = 0; i < my2dMap.rows; i++)
	{
		for (int j = 0; j < my2dMap.cols; j++)
		{
			if (my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 0] < 51 &&
				my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 1] < 51 &&
				my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 2] < 51)
			{
				my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 0] = 200;
				my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 1] = 10;
				my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 2] = 10;
				continue;
			}
			
			else if (my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 0] < 102 &&
					 my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 1] < 102 &&
					 my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 2] < 102)
			{
				my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 0] = 100;
				my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 1] = 180;
				my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 2] = 10;
				continue;
			}
			
			else if (my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 0] < 153 &&
					 my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 1] < 153 &&
					 my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 2] < 153)
			{
				my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 0] = 80;
				my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 1] = 220;
				my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 2] = 50;
				continue;
			}
			
			else if (my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 0] < 204 &&
					 my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 1] < 204 &&
					 my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 2] < 204)
			{
				my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 0] = 50;
				my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 1] = 50;
				my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 2] = 180;
				continue;
			}
			
			else if (my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 0] < 250 &&
					 my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 1] < 250 &&
					 my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 2] < 250)
			{
				my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 0] = 10;
				my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 1] = 20;
				my2dMap.data[my2dMap.step*i + my2dMap.channels()*j + 2] = 240;
				continue;
			}
		}
	}

	namedWindow("Density",1);
	imshow("Density",my2dMap);

	return;
}

void onMouse( int event, int x, int y, int flags, void* param )
{
    if( event == CV_EVENT_LBUTTONDOWN )
    {
        pt = Point2f((float)x,(float)y);
        addRemovePt = true;
    }
}

int myKeyboard(bool needToInit, bool nightMode)
{
	    char c = (char)waitKey(10);

        if( c == 27 )
            return 1;								//se il tasto è esc, esci dal loop for principale
        switch( c )
        {
        case 'r':
            needToInit = true;
			return 0;
            break;
        case 'c':
			return 2;
			break;
        case 'n':
            nightMode = !nightMode;
			return 0;
            break;
        default:
			return 0;
			break;
		}
}

