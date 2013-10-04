#include "CameraCalibration.h"

CameraCalibration::CameraCalibration(void)
{
	successes = 0;
}

CameraCalibration::~CameraCalibration(void)
{

}
void CameraCalibration::StartCalibration(int xsz,int ysz,int numsuccess)
{
	successes =0;
	reqsuccesses = numsuccess;
    numSquares = xsz * ysz;
    board_sz = Size(xsz, ysz);
	board_w = xsz;
	board_h = ysz;
	corners = new CvPoint2D32f[ numSquares ];
	CvMat* image_points		= cvCreateMat( numsuccess*numSquares, 2, CV_32FC1 );
	CvMat* object_points		= cvCreateMat( numsuccess*numSquares, 3, CV_32FC1 );
	CvMat* point_counts			= cvCreateMat( numsuccess, 1, CV_32SC1 );
	CvMat* intrinsic_matrix		= cvCreateMat( 3, 3, CV_32FC1 );
	CvMat* distortion_coeffs	= cvCreateMat( 5, 1, CV_32FC1 );

    for(int j=0;j<numSquares;j++)
        obj.push_back(Point3f((float)(j/xsz), (float)(j%ysz), 0.0f));

}
bool CameraCalibration::FeedImage(IplImage *frame)
{
	int corner_count =0;
	int found = cvFindChessboardCorners( frame, board_sz, corners,
				&corner_count, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS );
	int step;

	if(successes = reqsuccesses)return true;


//http://dasl.mem.drexel.edu/~noahKuntz/openCVTut10.html
	if(found)
	{
		IplImage *gray_image = cvCreateImage(cvSize(frame->width,frame->height),frame->depth,1);
		//convert it to single channel grey scale
		cvCvtColor(frame, gray_image,CV_BGR2GRAY);

		cvFindCornerSubPix( gray_image, corners, corner_count, cvSize( 11, 11 ), 
			cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
		
		cvDrawChessboardCorners( frame, board_sz, corners, corner_count, found );

			// If we got a good board, add it to our data
			if( corner_count == numSquares ){
				step = successes*numSquares;
				for( int i=step, j=0; j < numSquares; ++i, ++j ){
					CV_MAT_ELEM( *image_points, float, i, 0 ) = corners[j].x;
					CV_MAT_ELEM( *image_points, float, i, 1 ) = corners[j].y;
					CV_MAT_ELEM( *object_points, float, i, 0 ) = (float)(j/board_w);
					CV_MAT_ELEM( *object_points, float, i, 1 ) = (float)(j%board_w);
					CV_MAT_ELEM( *object_points, float, i, 2 ) = 0.0f;
				}
				CV_MAT_ELEM( *point_counts, int, successes, 0 ) = numSquares;
				successes++;
			}
		cvReleaseImage(&gray_image);
	}
	if(successes = reqsuccesses)
	{
		int board_n = numSquares;
		// Allocate matrices according to how many chessboards found
		CvMat* object_points2 = cvCreateMat( successes*board_n, 3, CV_32FC1 );
		CvMat* image_points2 = cvCreateMat( successes*board_n, 2, CV_32FC1 );
		CvMat* point_counts2 = cvCreateMat( successes, 1, CV_32SC1 );
		
		// Transfer the points into the correct size matrices
		for( int i = 0; i < successes*board_n; ++i ){
			CV_MAT_ELEM( *image_points2, float, i, 0) = CV_MAT_ELEM( *image_points, float, i, 0 );
			CV_MAT_ELEM( *image_points2, float, i, 1) = CV_MAT_ELEM( *image_points, float, i, 1 );
			CV_MAT_ELEM( *object_points2, float, i, 0) = CV_MAT_ELEM( *object_points, float, i, 0 );
			CV_MAT_ELEM( *object_points2, float, i, 1) = CV_MAT_ELEM( *object_points, float, i, 1 );
			CV_MAT_ELEM( *object_points2, float, i, 2) = CV_MAT_ELEM( *object_points, float, i, 2 );
		}

		for( int i=0; i < successes; ++i ){
			CV_MAT_ELEM( *point_counts2, int, i, 0 ) = CV_MAT_ELEM( *point_counts, int, i, 0 );
		}
		cvReleaseMat( &object_points );
		cvReleaseMat( &image_points );
		cvReleaseMat( &point_counts );

		// At this point we have all the chessboard corners we need
		// Initiliazie the intrinsic matrix such that the two focal lengths
		// have a ratio of 1.0

		CV_MAT_ELEM( *intrinsic_matrix, float, 0, 0 ) = 1.0;
		CV_MAT_ELEM( *intrinsic_matrix, float, 1, 1 ) = 1.0;

		// Calibrate the camera
		cvCalibrateCamera2( object_points2, image_points2, point_counts2, cvGetSize( frame ), 
			intrinsic_matrix, distortion_coeffs, NULL, NULL, CV_CALIB_FIX_ASPECT_RATIO ); 

		// Save the intrinsics and distortions
		cvSave( "Intrinsics.xml", intrinsic_matrix );
		cvSave( "Distortion.xml", distortion_coeffs );

		// Example of loading these matrices back in
		CvMat *intrinsic = (CvMat*)cvLoad( "Intrinsics.xml" );
		CvMat *distortion = (CvMat*)cvLoad( "Distortion.xml" );

		// Build the undistort map that we will use for all subsequent frames
		mapx = cvCreateImage( cvGetSize( frame ), IPL_DEPTH_32F, 1 );
		mapy = cvCreateImage( cvGetSize( frame ), IPL_DEPTH_32F, 1 );
		cvInitUndistortMap( intrinsic, distortion, mapx, mapy );
		return true;
	}
	return false;
}
void CameraCalibration::EndCalibration()
{

}
