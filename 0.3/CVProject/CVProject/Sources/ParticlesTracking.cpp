#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "D:\opencv\build\include\opencv2\opencv.hpp"

using namespace cv;
using namespace std;

static Point2f pt;
static bool addRemovePt = false;
Size winSize(25,25);
const int MAX_COUNT = 400;
TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03);
extern vector<Point2f> points[2];
extern int frame_number = 1;

void onMouse( int event, int x, int y, int flags, void* param );

void myHotspotDetector(vector<Point2f> myPoints);

void myGrid(Mat img);

int myFrameDisplay(bool needToInit, bool nightMode);

int myOpticalFlowCalc(bool needToInit, bool nightMode, VideoCapture cap);

int myKeyboard(bool needToInit, bool nightMode);

int main( int argc, char** argv )
{
    bool nightMode = false;
	bool needToInit = true;

	//CICLO FUNZIONE LETTURA E FUNZIONE DETECTION
	while(myFrameDisplay(needToInit, nightMode)!=1);
	
	/*
	for(;;)
    {


		//;
        Mat frame;
        cap >> frame;
        if( frame.empty() )
            break;

        frame.copyTo(image);
        cvtColor(image, gray, CV_BGR2GRAY);

        if( nightMode )
            image = Scalar::all(0);		//Tutti i canali a 0

        if( needToInit )
        {
            // automatic initialization
            goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
            cornerSubPix(gray, points[1], winSize, Size(-1,-1), termcrit);
            addRemovePt = false;
        }

        else if( !points[0].empty() )	//se ho già punti nel vettore delle feature
        {
            vector<uchar> status;
            vector<float> err;
            if(prevGray.empty())
                gray.copyTo(prevGray);
			
			calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize,
                                 3, termcrit, 0);	//calcolo vettore flusso ottico

            size_t i, k;
            
			for( i = k = 0; i < points[1].size(); i++ )		//per tutti i punti del flusso ottico
            {
                if( addRemovePt )
                {
                    if( norm(pt - points[1][i]) <= 5 )		//se il punto che voglio aggiungere con il mouse è vicino 
					{                                       //ad un punto già esistente nel flusso ottico
						addRemovePt = false;				//non lo aggiungo
                        continue;							//passo al ciclo successivo
                    }
                }

                if( !status[i] )							//se non ho trovato il flusso ottico
                    continue;								//passo al ciclo successivo

                points[1][k++] = points[1][i];
                circle( image, points[1][i], 3, Scalar(0,255,0), -1, 8);
            }//end for

            points[1].resize(k);
        }//end if


        if( addRemovePt && points[1].size() < (size_t)MAX_COUNT )
        {
            vector<Point2f> tmp;
            tmp.push_back(pt);
            cornerSubPix( gray, tmp, winSize, cvSize(-1,-1), termcrit);
            points[1].push_back(tmp[0]);
            addRemovePt = false;
        }

		if (frame_number%58 == 0)				//reinizializzo i punti in automatico
			{ needToInit = true; }				//ogni 2 secondi
		else { needToInit = false; }
		

        imshow("LK Demo", image);

        char c = (char)waitKey(10);

        if( c == 27 )
            break;								//se il tasto è esc, esci dal loop for principale
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
        }//end for principale


        //if(waitKey(30)>0)
        	//break;
    }
    */

	system ("PAUSE");
	return 0;
}

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

int myFrameDisplay(bool needToInit, bool nightMode)
{
    VideoCapture cap;
	cap.open("../2low.avi");

    if( !cap.isOpened() )
    {
        cout << "Could not initialize capturing...\n";
        return 1;
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

	do { frame_number += 1; }
	while(myOpticalFlowCalc(needToInit, nightMode, cap)!=1);

	return 0;
}

int myOpticalFlowCalc(bool needToInit, bool nightMode, VideoCapture cap)
{
	if (needToInit = true && frame_number>1)
	needToInit = false;

	Mat frame, image, gray, prevGray;
	int frame_number = 1; 
    
	cap >> frame;
    if( frame.empty() ) return 1;

	frame.copyTo(image);
    cvtColor(image, gray, CV_BGR2GRAY);

	if( needToInit )
        {
            // automatic initialization
            goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
            cornerSubPix(gray, points[1], winSize, Size(-1,-1), termcrit);
            addRemovePt = false;
        }

	else if( !points[1].empty() )	//se ho già punti nel vettore delle feature
        {
            vector<uchar> status;
            vector<float> err;
            if(prevGray.empty())
                gray.copyTo(prevGray);
			
			calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize,
                                 3, termcrit, 0);	//calcolo vettore flusso ottico

            size_t i, k;
            
		for( i = k = 0; i < points[1].size(); i++ )		//per tutti i punti del flusso ottico
            {
				if( addRemovePt )
                {
                    if( norm(pt - points[1][i]) <= 5 )		//se il punto che voglio aggiungere con il mouse è vicino 
					{                                       //ad un punto già esistente nel flusso ottico
						addRemovePt = false;				//non lo aggiungo
                        continue;							//passo al ciclo successivo
                    }
                }

                if( !status[i] )							//se non ho trovato il flusso ottico
                    continue;								//passo al ciclo successivo

                points[1][k++] = points[1][i];
                circle( image, points[1][i], 3, Scalar(0,255,0), -1, 8);
            }//end for

            points[1].resize(k);
        }//end if


	if( addRemovePt && points->size() < (size_t)MAX_COUNT )
    {
        vector<Point2f> tmp;
        tmp.push_back(pt);
        cornerSubPix( gray, tmp, winSize, cvSize(-1,-1), termcrit);
		points->push_back(tmp[0]);
        addRemovePt = false;
    }

	if (frame_number%58 == 0)				//reinizializzo i punti in automatico
		{ needToInit = true; }				//ogni 2 secondi
	else { needToInit = false; }
		

    imshow("LK Demo", image);

	switch (myKeyboard(needToInit, nightMode))
	{
		case '0': 
			break;
		case '0.5': 
			points[1].erase(points[1].begin(), points[1].end());
			points[0].erase(points[0].begin(), points[0].end());
			break;
		case '1': 
			return 1;
			break;
		default:
			break;
	}

    std::swap(points[1], points[0]);		//mantengo i punti calcolati con il flusso ottico nel vettore points[0]
    swap(prevGray, gray);					
	//frame_number += 1;
	//cout << frame_number;

	return 0;
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
            break;
        case 'c':
			return 0.5;
			break;
        case 'n':
            nightMode = !nightMode;
            break;
        default:
            break;
		}

}