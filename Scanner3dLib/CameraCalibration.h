#pragma once
//http://dasl.mem.drexel.edu/~noahKuntz/openCVTut10.html
//http://www.aishack.in/2010/07/calibrating-undistorting-with-opencv-in-c-oh-yeah/
/*
This class is used to correct for camera distortions.
A series of chessboard images are fed into this algorithm
when we have enough images, a distortion map is created
this distortion map can then be used to 
un-distort new images before they are fed into the algorithm
this will hopefully improve the accuracy of the 3d scanner.
*/
#include <cv.h>
#include <highgui.h>
using namespace cv;
using namespace std;
class CameraCalibration
{
public:
 //   vector<vector<Point3f>> object_points;
//    vector<vector<Point2f>> image_points;
    //vector<Point2f> corners;
	CvMat* image_points;	
	CvMat* object_points;	
	CvMat* point_counts;	
	CvMat* intrinsic_matrix;
	CvMat* distortion_coeffs;

	IplImage* mapx;// = cvCreateImage( cvGetSize( frame ), IPL_DEPTH_32F, 1 );
	IplImage* mapy;// = cvCreateImage( cvGetSize( frame ), IPL_DEPTH_32F, 1 );

	CvPoint2D32f* corners;
	vector<Point3f> obj; // the grid in world coordinates

    int successes;
	int reqsuccesses; // required successes

    int numSquares;
    Size board_sz;
	int board_w,board_h;
	//int board_n;


	CameraCalibration(void);
	~CameraCalibration(void);
	void StartCalibration(int xsz,int ysz,int numsuccess);
	bool FeedImage(IplImage *frame);
	void EndCalibration();
	bool Save(char * filename);
	bool Load(char * filename);
};
