// cvScanProCam.cpp: Functions for structured light scanning.
//
// Overview:
//   This file implements the functions for structured lighting. The current implementation
//   supports a single projector-camera pair. The projector-camera system must be calibrated 
//   prior to running the scanning function. A 3D point cloud and depth map are recovered by
//   optical triangulation. Three scanning modes are implemented, including: (1) encoding only 
//   the projector columns, (2) encoding only the projector rows, (3) encoding both rows and 
//   columns. Two reconstruction methods are implemented, including: (1) "ray-plane" 
//   triangulation and (2) "ray-ray" triangulation. In the former, each optical ray from the 
//   camera is intersected with the corresponding projector column and/or row. In the later,
//   the corresponding optical rays from the camera and projector are intersected; in this 
//   case, the 3D point is assigned as the closest point to the two (generally skewed) rays.
//   
// Details:
//   Please read the SIGGRAPH 2009 course notes for additional details.
//
//     Douglas Lanman and Gabriel Taubin
//     "Build Your Own 3D Scanner: 3D Photography for Beginners"
//     ACM SIGGRAPH 2009 Course Notes
//
// Author:
//   Douglas Lanman
//   Brown University
//   July 2009

#include "stdafx.h"
#include <direct.h>
#include "cvStructuredLight.h"
#include "cvScanProCam.h"
#include "cvUtilProCam.h"

// Generate Gray codes.
int generateGrayCodes(int width, int height, 
					  IplImage**& gray_codes, 
					  int& n_cols, int& n_rows,
					  int& col_shift, int& row_shift, 
					  bool sl_scan_cols, bool sl_scan_rows){

	// Determine number of required codes and row/column offsets.
	if(sl_scan_cols){
		n_cols = (int)ceil(log2(width));
		col_shift = (int)floor((pow(2.0,n_cols)-width)/2);
	}
	else{
		n_cols = 0;
		col_shift = 0;
	}
	if(sl_scan_rows){
		n_rows = (int)ceil(log2(height));
		row_shift = (int)floor((pow(2.0,n_rows)-height)/2);
	}
	else{
		n_rows = 0;
		row_shift = 0;
	}	

	// Allocate Gray codes.
	gray_codes = new IplImage* [n_cols+n_rows+1];
	for(int i=0; i<(n_cols+n_rows+1); i++)
		gray_codes[i] = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 1);
	int step = gray_codes[0]->widthStep/sizeof(uchar);

	// Define first code as a white image.
	cvSet(gray_codes[0], cvScalar(255));

	// Define Gray codes for projector columns.
	for(int c=0; c<width; c++){
		for(int i=0; i<n_cols; i++){
			uchar* data = (uchar*)gray_codes[i+1]->imageData;
			if(i>0)
				data[c] = (((c+col_shift) >> (n_cols-i-1)) & 1)^(((c+col_shift) >> (n_cols-i)) & 1);
			else
				data[c] = (((c+col_shift) >> (n_cols-i-1)) & 1);
			data[c] *= 255;
			for(int r=1; r<height; r++)
				data[r*step+c] = data[c];	
		}
	}

	// Define Gray codes for projector rows.
	for(int r=0; r<height; r++){
		for(int i=0; i<n_rows; i++){
			uchar* data = (uchar*)gray_codes[i+n_cols+1]->imageData;
			if(i>0)
				data[r*step] = (((r+row_shift) >> (n_rows-i-1)) & 1)^(((r+row_shift) >> (n_rows-i)) & 1);
			else
				data[r*step] = (((r+row_shift) >> (n_rows-i-1)) & 1);
			data[r*step] *= 255;
			for(int c=1; c<width; c++)
				data[r*step+c] = data[r*step];	
		}
	}

	// Return without errors.
	return 0;
}

// Decode Gray codes.
int decodeGrayCodes(int proj_width, int proj_height,
					IplImage**& gray_codes, 
					IplImage*& decoded_cols,
					IplImage*& decoded_rows,
					IplImage*& mask,
					int& n_cols, int& n_rows,
					int& col_shift, int& row_shift, 
					int sl_thresh){

	// Extract width and height of images.
	int cam_width  = gray_codes[0]->width;
	int cam_height = gray_codes[0]->height;

	// Allocate temporary variables.
	IplImage* gray_1      = cvCreateImage(cvSize(cam_width, cam_height), IPL_DEPTH_8U,  1);
	IplImage* gray_2      = cvCreateImage(cvSize(cam_width, cam_height), IPL_DEPTH_8U,  1);
	IplImage* bit_plane_1 = cvCreateImage(cvSize(cam_width, cam_height), IPL_DEPTH_8U,  1);
	IplImage* bit_plane_2 = cvCreateImage(cvSize(cam_width, cam_height), IPL_DEPTH_8U,  1);
	IplImage* temp        = cvCreateImage(cvSize(cam_width, cam_height), IPL_DEPTH_8U,  1);

	// Initialize image mask (indicates reconstructed pixels).
	cvSet(mask, cvScalar(0));

	// Decode Gray codes for projector columns.
	cvZero(decoded_cols);
	for(int i=0; i<n_cols; i++){

		// Decode bit-plane and update mask.
		cvCvtColor(gray_codes[2*(i+1)],   gray_1, CV_RGB2GRAY);
		cvCvtColor(gray_codes[2*(i+1)+1], gray_2, CV_RGB2GRAY);
		cvAbsDiff(gray_1, gray_2, temp);
		cvCmpS(temp, sl_thresh, temp, CV_CMP_GE);
		cvOr(temp, mask, mask);
		cvCmp(gray_1, gray_2, bit_plane_2, CV_CMP_GE);

		// Convert from gray code to decimal value.
		if(i>0)
			cvXor(bit_plane_1, bit_plane_2, bit_plane_1);
		else
			cvCopyImage(bit_plane_2, bit_plane_1);
		cvAddS(decoded_cols, cvScalar(pow(2.0,n_cols-i-1)), decoded_cols, bit_plane_1);
	}
	cvSubS(decoded_cols, cvScalar(col_shift), decoded_cols);

	// Decode Gray codes for projector rows.
	cvZero(decoded_rows);
	for(int i=0; i<n_rows; i++){

		// Decode bit-plane and update mask.
		cvCvtColor(gray_codes[2*(i+n_cols+1)],   gray_1, CV_RGB2GRAY);
		cvCvtColor(gray_codes[2*(i+n_cols+1)+1], gray_2, CV_RGB2GRAY);
		cvAbsDiff(gray_1, gray_2, temp);
		cvCmpS(temp, sl_thresh, temp, CV_CMP_GE);
		cvOr(temp, mask, mask);
		cvCmp(gray_1, gray_2, bit_plane_2, CV_CMP_GE);

		// Convert from gray code to decimal value.
		if(i>0)
			cvXor(bit_plane_1, bit_plane_2, bit_plane_1);
		else
			cvCopyImage(bit_plane_2, bit_plane_1);
		cvAddS(decoded_rows, cvScalar(pow(2.0,n_rows-i-1)), decoded_rows, bit_plane_1);
	}
	cvSubS(decoded_rows, cvScalar(row_shift), decoded_rows);

	// Eliminate invalid column/row estimates.
    // Note: This will exclude pixels if either the column or row is missing or erroneous.
	cvCmpS(decoded_cols, proj_width-1,  temp, CV_CMP_LE);
	cvAnd(temp, mask, mask);
	cvCmpS(decoded_cols, 0,  temp, CV_CMP_GE);
	cvAnd(temp, mask, mask);
	cvCmpS(decoded_rows, proj_height-1, temp, CV_CMP_LE);
	cvAnd(temp, mask, mask);
	cvCmpS(decoded_rows, 0,  temp, CV_CMP_GE);
	cvAnd(temp, mask, mask);
	cvNot(mask, temp);
	cvSet(decoded_cols, cvScalar(NULL), temp);
	cvSet(decoded_rows, cvScalar(NULL), temp);

	// Free allocated resources.
	cvReleaseImage(&gray_1);
	cvReleaseImage(&gray_2);
	cvReleaseImage(&bit_plane_1);
	cvReleaseImage(&bit_plane_2);
	cvReleaseImage(&temp);

	// Return without errors.
	return 0;
}

// Illuminate object with a structured light sequence.
int slScan(CvCapture* capture, 
		   IplImage**& proj_codes, IplImage**& cam_codes,
		   int n_cols, int n_rows, 
		   struct slParams* sl_params,
		   struct slCalib*  sl_calib){

	// Create a window to display captured frames.
	IplImage* cam_frame  = cvQueryFrame2(capture, sl_params);
	IplImage* proj_frame = cvCreateImage(cvSize(sl_params->proj_w, sl_params->proj_h), IPL_DEPTH_8U, 1);
	cvNamedWindow("camWindow", CV_WINDOW_AUTOSIZE);
	HWND camWindow = (HWND)cvGetWindowHandle("camWindow");
	BringWindowToTop(camWindow);
	cvWaitKey(1);

	// Allocate storage for captured images.
	cam_codes = new IplImage* [2*(n_cols+n_rows+1)];
	for(int i=0; i<2*(n_cols+n_rows+1); i++)
		cam_codes[i] = cvCloneImage(cam_frame);

	// Capture structured light sequence.
    // Note: Assumes sequence is binary, so code and its inverse can be compared.
	IplImage* inverse_code = cvCloneImage(proj_codes[0]);
	for(int i=0; i<(n_cols+n_rows+1); i++){

		// Display code.
		cvCopy(proj_codes[i], proj_frame);
		cvScale(proj_frame, proj_frame, 2.*(sl_params->proj_gain/100.), 0);
		cvShowImage("projWindow", proj_frame);
		cvWaitKey(sl_params->delay);
		cam_frame = cvQueryFrameSafe(capture, sl_params, false);
		cvScale(cam_frame, cam_frame, 2.*(sl_params->cam_gain/100.), 0);
		cvShowImageResampled("camWindow", cam_frame, sl_params->window_w, sl_params->window_h);
		cvCopyImage(cam_frame, cam_codes[2*i]);

		// Display inverse code.
		cvSubRS(proj_codes[i], cvScalar(255), inverse_code);
		cvCopy(inverse_code, proj_frame);
		cvScale(proj_frame, proj_frame, 2.*(sl_params->proj_gain/100.), 0);
		cvShowImage("projWindow", proj_frame);
		cvWaitKey(sl_params->delay);
		cam_frame = cvQueryFrameSafe(capture, sl_params, false);
		cvScale(cam_frame, cam_frame, 2.*(sl_params->cam_gain/100.), 0);
		cvShowImageResampled("camWindow", cam_frame, sl_params->window_w, sl_params->window_h);
		cvCopyImage(cam_frame, cam_codes[2*i+1]);
	}

	// Display black projector image.
	cvSet(inverse_code, cvScalar(0, 0, 0));
	cvShowImage("projWindow", inverse_code);
	cvWaitKey(1);

	// Free allocated resources.
	cvReleaseImage(&inverse_code);
	cvReleaseImage(&proj_frame);

	// Return without errors.
	cvDestroyWindow("camWindow");
	return 0;
}

// Display a structured lighting decoding result (i.e., projector column/row to camera pixel correspondences).
int displayDecodingResults(IplImage*& decoded_cols, 
						   IplImage*& decoded_rows, 
						   IplImage*& mask,
						   struct slParams* sl_params){

	// Create a window to display correspondences.
	cvNamedWindow("camWindow", CV_WINDOW_AUTOSIZE);
	HWND camWindow = (HWND)cvGetWindowHandle("camWindow");
	BringWindowToTop(camWindow);
	cvWaitKey(1);

	// Allocate image arrays.
	IplImage* temp_1 = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_8U, 1);
	IplImage* temp_2 = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_8U, 3);
	
	// Display decoded projector columns.
	if(sl_params->scan_cols){
		cvConvertScale(decoded_cols, temp_1, 255.0/sl_params->proj_w, 0);
		colorizeWinter(temp_1, temp_2, mask);
		cvShowImageResampled("camWindow", temp_2, sl_params->window_w, sl_params->window_h);
		printf("Displaying the decoded columns; press any key (in 'camWindow') to continue.\n");
		cvWaitKey(0);
	}

	// Display decoded projector rows.
	if(sl_params->scan_rows){
		cvConvertScale(decoded_rows, temp_1, 255.0/sl_params->proj_h, 0);
		colorizeWinter(temp_1, temp_2, mask);
		cvShowImageResampled("camWindow", temp_2, sl_params->window_w, sl_params->window_h);
		printf("Displaying the decoded rows; Press any key (in 'camWindow') to continue.\n");
		cvWaitKey(0);
	}

	// Free allocated resources.
	cvReleaseImage(&temp_1);
	cvReleaseImage(&temp_2);
	
	// Return without errors.
	cvDestroyWindow("camWindow");
	return 0;
}

// Display a depth map.
int displayDepthMap(CvMat*& depth_map,
					IplImage*& mask,
				    struct slParams* sl_params){

	// Create a window to display depth map.
	cvNamedWindow("camWindow", CV_WINDOW_AUTOSIZE);
	HWND camWindow = (HWND)cvGetWindowHandle("camWindow");
	BringWindowToTop(camWindow);
	cvWaitKey(1);

	// Allocate image arrays.
	IplImage* temp_1 = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_8U, 1);
	IplImage* temp_2 = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_8U, 3);

	// Create depth map image (scaled to distance range).
	for(int r=0; r<sl_params->cam_h; r++){
		for(int c=0; c<sl_params->cam_w; c++){
			uchar* temp_1_data = (uchar*)(temp_1->imageData + r*temp_1->widthStep);
			uchar* mask_data   = (uchar*)(mask->imageData + r*mask->widthStep);
			if(mask_data[c])
				temp_1_data[c] = 
					255-int(255.0*(depth_map->data.fl[sl_params->cam_w*r+c]-sl_params->dist_range[0])/
						(sl_params->dist_range[1]-sl_params->dist_range[0]));
			else
				temp_1_data[c] = 0;
		}
	}
	
	// Display depth map.
	colorizeWinter(temp_1, temp_2, mask);
	cvShowImageResampled("camWindow", temp_2, sl_params->window_w, sl_params->window_h);
	printf("Displaying the depth map; press any key (in 'camWindow') to continue.\n");
	cvWaitKey(0);

	// Release allocated resources.
	cvReleaseImage(&temp_1);
	cvReleaseImage(&temp_2);

	// Return without errors.
	cvDestroyWindow("camWindow");
	return 0;
}

// Reconstruct the point cloud and the depth map from a structured light sequence.
int reconstructStructuredLight(struct slParams* sl_params, 
					           struct slCalib* sl_calib,
							   IplImage*& texture_image,
							   IplImage*& gray_decoded_cols, 
							   IplImage*& gray_decoded_rows, 
						       IplImage*& gray_mask,
							   CvMat*&    points,
							   CvMat*&    colors,
							   CvMat*&    depth_map,
							   CvMat*&    mask){
	
	// Define pointers to various image data elements (for fast pixel access).
	int cam_nelems                 = sl_params->cam_w*sl_params->cam_h;
	int proj_nelems                = sl_params->proj_w*sl_params->proj_h;
	uchar*  background_mask_data   = (uchar*)sl_calib->background_mask->imageData;
	int     background_mask_step   = sl_calib->background_mask->widthStep/sizeof(uchar);
	uchar*  gray_mask_data         = (uchar*)gray_mask->imageData;
	int     gray_mask_step         = gray_mask->widthStep/sizeof(uchar);
	ushort* gray_decoded_cols_data = (ushort*)gray_decoded_cols->imageData;
	int     gray_decoded_cols_step = gray_decoded_cols->widthStep/sizeof(ushort);
	ushort* gray_decoded_rows_data = (ushort*)gray_decoded_rows->imageData;
	int     gray_decoded_rows_step = gray_decoded_rows->widthStep/sizeof(ushort);

	// Create a temporary copy of the background depth map.
	CvMat* background_depth_map = cvCloneMat(sl_calib->background_depth_map);

	// By default, disable all pixels.
	cvZero(mask);

	// Reconstruct point cloud and depth map.
	for(int r=0; r<sl_params->cam_h; r++){
		for(int c=0; c<sl_params->cam_w; c++){

			// Reconstruct current point, if mask is non-zero.
			if(gray_mask_data[r*gray_mask_step+c]){

				// Reconstruct using either "ray-plane" or "ray-ray" triangulation.
				if(sl_params->mode == 1){

					// Allocate storage for row/column reconstructed points and depths.
					float point_cols[3], point_rows[3];
					float depth_cols, depth_rows;
				
					// Intersect camera ray with corresponding projector column.
					if(sl_params->scan_cols){
						float q[3], v[3], w[4];
						int rc = (sl_params->cam_w)*r+c;
						for(int i=0; i<3; i++){
							q[i] = sl_calib->cam_center->data.fl[i];
							v[i] = sl_calib->cam_rays->data.fl[rc+cam_nelems*i];
						}
						int corresponding_column = gray_decoded_cols_data[r*gray_decoded_cols_step+c];
						for(int i=0; i<4; i++)
							w[i] = sl_calib->proj_column_planes->data.fl[4*corresponding_column+i];
						intersectLineWithPlane3D(q, v, w, point_cols, depth_cols);
					}

					// Intersect camera ray with corresponding projector row.
					if(sl_params->scan_rows){
						float q[3], v[3], w[4];
						int rc = (sl_params->cam_w)*r+c;
						for(int i=0; i<3; i++){
							q[i] = sl_calib->cam_center->data.fl[i];
							v[i] = sl_calib->cam_rays->data.fl[rc+cam_nelems*i];
						}
						int corresponding_row = gray_decoded_rows_data[r*gray_decoded_rows_step+c];
						for(int i=0; i<4; i++)
							w[i] = sl_calib->proj_row_planes->data.fl[4*corresponding_row+i];
						intersectLineWithPlane3D(q, v, w, point_rows, depth_rows);
					}

					// Average points of intersection (if row and column scanning are both enabled).
					// Note: Eliminate any points that differ between row and column reconstructions.
					if( sl_params->scan_cols && sl_params->scan_rows){
						if(abs(depth_cols-depth_rows) < sl_params->dist_reject){
							depth_map->data.fl[sl_params->cam_w*r+c] = (depth_cols+depth_rows)/2;
							for(int i=0; i<3; i++)
								points->data.fl[sl_params->cam_w*r+c+cam_nelems*i] = (point_cols[i]+point_rows[i])/2;
						}
						else
							gray_mask_data[r*gray_mask_step+c] = 0;
					}
					else if(sl_params->scan_cols){
						depth_map->data.fl[sl_params->cam_w*r+c] = depth_cols;
						for(int i=0; i<3; i++)
							points->data.fl[sl_params->cam_w*r+c+cam_nelems*i] = point_cols[i];
					}
					else if(sl_params->scan_rows){
						depth_map->data.fl[sl_params->cam_w*r+c] = depth_rows;
						for(int i=0; i<3; i++)
							points->data.fl[sl_params->cam_w*r+c+cam_nelems*i] = point_rows[i];
					}
					else
						gray_mask_data[r*gray_mask_step+c] = 0;
				}
				else{

					// Reconstruct surface using "ray-ray" triangulation.
					int corresponding_column = gray_decoded_cols_data[r*gray_decoded_cols_step+c];
					int corresponding_row    = gray_decoded_rows_data[r*gray_decoded_rows_step+c];
					float q1[3], q2[3], v1[3], v2[3], point[3], depth = 0;
					int rc_cam  = (sl_params->cam_w)*r+c;
					int rc_proj = (sl_params->proj_w)*corresponding_row+corresponding_column;
					for(int i=0; i<3; i++){
						q1[i] = sl_calib->cam_center->data.fl[i];
						q2[i] = sl_calib->proj_center->data.fl[i];
						v1[i] = sl_calib->cam_rays->data.fl[rc_cam+cam_nelems*i];
						v2[i] = sl_calib->proj_rays->data.fl[rc_proj+proj_nelems*i];
					}
					intersectLineWithLine3D(q1, v1, q2, v2, point);
					for(int i=0; i<3; i++)
						depth += v1[i]*(point[i]-q1[i]);
					depth_map->data.fl[rc_cam] = depth;
					for(int i=0; i<3; i++)
						points->data.fl[rc_cam+cam_nelems*i] = point[i];
				}

				// Assign color using provided texture image.
				// Note: Color channels are ordered as RGB, rather than OpenCV's default BGR.
				uchar* texture_image_data = (uchar*)(texture_image->imageData + r*texture_image->widthStep);
				for(int i=0; i<3; i++)
					colors->data.fl[sl_params->cam_w*r+c+cam_nelems*i] = (float)texture_image_data[3*c+(2-i)]/(float)255.0;

				// Update valid pixel mask (e.g., points will only be saved if valid).
				mask->data.fl[sl_params->cam_w*r+c] = 1;

				// Reject any points outside near/far clipping planes.
				if(depth_map->data.fl[sl_params->cam_w*r+c] < sl_params->dist_range[0] ||
				   depth_map->data.fl[sl_params->cam_w*r+c] > sl_params->dist_range[1]){
					gray_mask_data[r*gray_mask_step+c] = 0;
					mask->data.fl[sl_params->cam_w*r+c] = 0;
					depth_map->data.fl[sl_params->cam_w*r+c] = 0;
					for(int i=0; i<3; i++)
						points->data.fl[sl_params->cam_w*r+c+cam_nelems*i] = 0;
					for(int i=0; i<3; i++)
						colors->data.fl[sl_params->cam_w*r+c+cam_nelems*i] = 0;
				}

				// Reject background points.
				// Note: Currently only uses depth to determine foreground vs. background pixels.
				float depth_difference = 
					background_depth_map->data.fl[sl_params->cam_w*r+c] - 
					depth_map->data.fl[sl_params->cam_w*r+c];
				if(depth_difference < sl_params->background_depth_thresh && 
				   gray_mask_data[r*gray_mask_step+c] && 
				   background_mask_data[r*background_mask_step+c]){
					gray_mask_data[r*gray_mask_step+c] = 0;
					mask->data.fl[sl_params->cam_w*r+c] = 0;
					depth_map->data.fl[sl_params->cam_w*r+c] = 0;
					for(int i=0; i<3; i++)
						points->data.fl[sl_params->cam_w*r+c+cam_nelems*i] = 0;
					for(int i=0; i<3; i++)
						colors->data.fl[sl_params->cam_w*r+c+cam_nelems*i] = 0;
				}
			}
		}
	}

	// Release allocated resources.
	cvReleaseMat(&background_depth_map);

	// Return without errors.
	return 0;
}

// Run the background capture (used to eliminate background points from reconstructions).
int runBackgroundCapture(CvCapture* capture, 
						 struct slParams* sl_params, 
						 struct slCalib* sl_calib){

	// Generate Gray codes.
	IplImage** proj_gray_codes = NULL;
	int gray_ncols, gray_nrows;
	int gray_colshift, gray_rowshift;
	generateGrayCodes(sl_params->proj_w, sl_params->proj_h, proj_gray_codes, 
		gray_ncols, gray_nrows, gray_colshift, gray_rowshift, 
		sl_params->scan_cols, sl_params->scan_rows);

	// Capture live image stream (e.g., for adjusting object placement).
	printf("Remove object, then press any key (in 'camWindow') to scan.\n");
	camPreview(capture, sl_params, sl_calib);

	// Illuminate the background using the Gray code sequence.
	printf("Displaying the structured light sequence...\n");
	IplImage** cam_gray_codes = NULL;
	slScan(capture, proj_gray_codes, cam_gray_codes, gray_ncols, gray_nrows, sl_params, sl_calib);

	// Save white image for background model.
	cvCopyImage(cam_gray_codes[0], sl_calib->background_image);

	// Decode the structured light sequence.
	printf("Decoding the structured light sequence...\n");
	IplImage* gray_decoded_cols = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_16U, 1);
	IplImage* gray_decoded_rows = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_16U, 1);
	decodeGrayCodes(sl_params->proj_w, sl_params->proj_h,
					cam_gray_codes, 
					gray_decoded_cols, gray_decoded_rows, sl_calib->background_mask,
					gray_ncols, gray_nrows, 
					gray_colshift, gray_rowshift, 
					sl_params->thresh);

	// Reconstruct the point cloud and depth map.
	printf("Reconstructing the point cloud and the depth map...\n");
	CvMat *points  = cvCreateMat(3, sl_params->cam_h*sl_params->cam_w, CV_32FC1);
	CvMat *colors  = cvCreateMat(3, sl_params->cam_h*sl_params->cam_w, CV_32FC1);
	CvMat *mask    = cvCreateMat(1, sl_params->cam_h*sl_params->cam_w, CV_32FC1);
	reconstructStructuredLight(sl_params, sl_calib, 
							   cam_gray_codes[0],
		                       gray_decoded_cols, gray_decoded_rows, sl_calib->background_mask,
							   points, colors, sl_calib->background_depth_map, mask);

	// Free allocated resources.
	cvReleaseImage(&gray_decoded_cols);
	cvReleaseImage(&gray_decoded_rows);
	cvReleaseMat(&points);
	cvReleaseMat(&colors);
	cvReleaseMat(&mask);
	for(int i=0; i<(gray_ncols+gray_nrows+1); i++)
		cvReleaseImage(&proj_gray_codes[i]);
	delete[] proj_gray_codes;
	for(int i=0; i<2*(gray_ncols+gray_nrows+1); i++)
		cvReleaseImage(&cam_gray_codes[i]);
	delete[] cam_gray_codes;
	
	// Return without errors.
	return 0;
}

// Run the structured light scanner.
int runStructuredLight(CvCapture* capture, 
					   struct slParams* sl_params, 
					   struct slCalib* sl_calib, 
					   int scan_index){

	// Generate Gray codes.
	IplImage** proj_gray_codes = NULL;
	int gray_ncols, gray_nrows;
	int gray_colshift, gray_rowshift;
	generateGrayCodes(sl_params->proj_w, sl_params->proj_h, proj_gray_codes, 
		gray_ncols, gray_nrows, gray_colshift, gray_rowshift, 
		sl_params->scan_cols, sl_params->scan_rows);

	// Capture live image stream (e.g., for adjusting object placement).
	printf("Position object, then press any key (in 'camWindow') to scan.\n");
	camPreview(capture, sl_params, sl_calib);

	// Illuminate the object using the Gray code sequence.
	printf("Displaying the structured light sequence...\n");
	IplImage** cam_gray_codes = NULL;
	slScan(capture, proj_gray_codes, cam_gray_codes, 
		   gray_ncols, gray_nrows, sl_params, sl_calib);

	// Create output directory (if output enabled).
	char str[1024], outputDir[1024];
	if(sl_params->save){
		sprintf(outputDir, "%s\\%s\\%0.2d", sl_params->outdir, sl_params->object, scan_index);
		_mkdir(outputDir);
	}

	// Save the current calibration parameters (if output enabled).
	if(sl_calib->cam_intrinsic_calib && sl_params->save){
		sprintf(str,"%s\\cam_intrinsic.xml", outputDir);	
		cvSave(str, sl_calib->cam_intrinsic);
		sprintf(str,"%s\\cam_distortion.xml", outputDir);
		cvSave(str, sl_calib->cam_distortion);
	}
	if(sl_calib->proj_intrinsic_calib && sl_params->save){
		sprintf(str,"%s\\proj_intrinsic.xml", outputDir);	
		cvSave(str, sl_calib->proj_intrinsic);
		sprintf(str,"%s\\proj_distortion.xml", outputDir);
		cvSave(str, sl_calib->proj_distortion);
	}
	if(sl_calib->procam_extrinsic_calib && sl_params->save){
		sprintf(str,"%s\\cam_extrinsic.xml", outputDir);	
		cvSave(str, sl_calib->cam_extrinsic);
		sprintf(str,"%s\\proj_extrinsic.xml", outputDir);	
		cvSave(str, sl_calib->proj_extrinsic);
	}
	if(sl_params->save){
		sprintf(str,"%s\\config.xml", outputDir);
		writeConfiguration(str, sl_params);
	}

	// Save the capture image sequence (if enabled).
	if(sl_params->save){
		printf("Saving the structured light sequence...\n");
		for(int i=0; i<2*(gray_ncols+gray_nrows+1); i++){
			sprintf(str, "%s\\%0.2d.png", outputDir, i);
			cvSaveImage(str, cam_gray_codes[i]);
		}
	}

	// Decode the structured light sequence.
	printf("Decoding the structured light sequence...\n");
	IplImage* gray_decoded_cols = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_16U, 1);
	IplImage* gray_decoded_rows = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_16U, 1);
	IplImage* gray_mask         = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_8U,  1);
	decodeGrayCodes(sl_params->proj_w, sl_params->proj_h,
					cam_gray_codes, 
					gray_decoded_cols, gray_decoded_rows, gray_mask,
					gray_ncols, gray_nrows, 
					gray_colshift, gray_rowshift, 
					sl_params->thresh);

	// Display and save the correspondences.
	if(sl_params->display)
		displayDecodingResults(gray_decoded_cols, gray_decoded_rows, gray_mask, sl_params);

	// Reconstruct the point cloud and depth map.
	printf("Reconstructing the point cloud and the depth map...\n");
	CvMat *points    = cvCreateMat(3, sl_params->cam_h*sl_params->cam_w, CV_32FC1);
	CvMat *colors    = cvCreateMat(3, sl_params->cam_h*sl_params->cam_w, CV_32FC1);
	CvMat *depth_map = cvCreateMat(sl_params->cam_h, sl_params->cam_w, CV_32FC1);
	CvMat *mask      = cvCreateMat(1, sl_params->cam_h*sl_params->cam_w, CV_32FC1);
	reconstructStructuredLight(sl_params, sl_calib, 
							   cam_gray_codes[0],
		                       gray_decoded_cols, gray_decoded_rows, gray_mask,
							   points, colors, depth_map, mask);

	// Display and save the depth map.
	if(sl_params->display)
		displayDepthMap(depth_map, gray_mask, sl_params);
	if(sl_params->save){
		printf("Saving the depth map...\n");
		IplImage* depth_map_image = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_8U, 1);
		for(int r=0; r<sl_params->cam_h; r++){
			for(int c=0; c<sl_params->cam_w; c++){
				char* depth_map_image_data = (char*)(depth_map_image->imageData + r*depth_map_image->widthStep);
				if(mask->data.fl[sl_params->cam_w*r+c])
					depth_map_image_data[c] = 
						255-int(255*(depth_map->data.fl[sl_params->cam_w*r+c]-sl_params->dist_range[0])/
							(sl_params->dist_range[1]-sl_params->dist_range[0]));
				else
					depth_map_image_data[c] = 0;
			}
		}
		CvMat* dist_range = cvCreateMat(1, 2, CV_32FC1);
		cvmSet(dist_range, 0, 0, sl_params->dist_range[0]);
		cvmSet(dist_range, 0, 1, sl_params->dist_range[1]);
		char str[1024];
		sprintf(str, "%s\\depth_map.png", outputDir);
		cvSaveImage(str, depth_map_image);
		sprintf(str, "%s\\depth_map_range.xml", outputDir);
		cvSave(str, dist_range);
		cvReleaseImage(&depth_map_image);
		cvReleaseMat(&dist_range);
	}

	// Save the texture map.
	printf("Saving the texture map...\n");
	sprintf(str, "%s\\%s\\%s_%0.2d.png", sl_params->outdir, sl_params->object, sl_params->object, scan_index);
	cvSaveImage(str, cam_gray_codes[0]);

	// Save the point cloud.
	printf("Saving the point cloud...\n");
	sprintf(str, "%s\\%s\\%s_%0.2d.wrl", sl_params->outdir, sl_params->object, sl_params->object, scan_index);
	if(savePointsVRML(str, points, NULL, colors, mask)){
		printf("Scanning was not successful and must be repeated!\n");
		return -1;
	}

	// Free allocated resources.
	cvReleaseImage(&gray_decoded_cols);
	cvReleaseImage(&gray_decoded_rows);
	cvReleaseImage(&gray_mask);
	cvReleaseMat(&points);
	cvReleaseMat(&colors);
	cvReleaseMat(&depth_map);
	cvReleaseMat(&mask);
	for(int i=0; i<(gray_ncols+gray_nrows+1); i++)
		cvReleaseImage(&proj_gray_codes[i]);
	delete[] proj_gray_codes;
	for(int i=0; i<2*(gray_ncols+gray_nrows+1); i++)
		cvReleaseImage(&cam_gray_codes[i]);
	delete[] cam_gray_codes;
	
	// Return without errors.
	return 0;
}