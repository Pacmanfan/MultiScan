//#include "porttype.h"
//#include "stdafx.h"
#include "point3d.hpp"
#include "camera.h"
#include "log.h"
#include "plane.h"

#ifndef _WIN32
//CPoint ProjectedPoint; // the returned projected point
#endif
void point_3d::operator += (Vector3d v){
		Wx +=v.x;Wy +=v.y;Wz +=v.z;
}
void point_3d::operator -= (Vector3d v){
		Wx -=v.x;Wy -=v.y;Wz -=v.z;
}
void point_3d::operator = (point_3d p)
{
	Wx = p.Wx;
	Wy = p.Wy;
	Wz = p.Wz;
	Cx = p.Cx;
	Cy = p.Cy;
	Cz = p.Cz;
}
point_3d point_3d::operator + (Vector3d v){
		point_3d temp;
		temp.Wx +=v.x;temp.Wy +=v.y;temp.Wz +=v.z;
		return temp;
}
point_3d point_3d::operator - (Vector3d v){
		point_3d temp;
		temp.Wx -=v.x;temp.Wy -=v.y;temp.Wz -=v.z;
		return temp;
}

Vector3d point_3d::operator -(point_3d p){
	Vector3d v;
	v.x = Wx - p.Wx;
	v.y = Wy - p.Wy;
	v.z = Wz - p.Wz;
	return v;
}


double point_3d::Distance(point_3d *n){
	double dist = 0;
	dist = sqrt( ((Wx - n->Wx)*(Wx - n->Wx)) + 
				 ((Wy - n->Wy)*(Wy - n->Wy)) + 
				 ((Wz - n->Wz)*(Wz - n->Wz)) );
	dist = fabs(dist);
	return dist;
}
void UnProject(Point2D &p,point_3d *out, camera *cam, int Wid,int Hei){

	//take this point from screen space to camera space
	float voodoo;
	float OneOverZ;
	OneOverZ = 1/out->Cz;
	voodoo = cam->viewing_distance *OneOverZ;
	out->Cx = (((float)p.X) / voodoo)  -  (((float)Wid/2.0f)/voodoo);
	out->Cy = (((float)p.Y)  / voodoo) -  (((float)Hei/2.0f)/voodoo);
}

/*
void point_3d::Project(Point2D &temp){
	static float voodoo;
	static float OneOverZ;
	OneOverZ = 1/Cz;
	voodoo = FramePrefs->RenderCamera->viewing_distance *OneOverZ;
	temp.X = QuickFist( Cx * voodoo)+FramePrefs->HWidth;
	temp.Y = QuickFist( Cy * voodoo)+FramePrefs->HHeight;
	temp.Z = (long)((double)OneOverZ*((double)(1<<ZSTEP_PREC)));
	temp.R = GetRValue(originalcolor);
	temp.G = GetGValue(originalcolor);
	temp.B = GetBValue(originalcolor);
}
*/
/*
void point_3d::Project(Point2D &temp,camera *cam,int wid,int hei){
	static float voodoo;
	static float OneOverZ;
	OneOverZ = 1/Cz;
	voodoo = cam->viewing_distance *OneOverZ;
	temp.X = (float)( Cx * voodoo)+ (wid/2);
	temp.Y = (float)( Cy * voodoo)+ (hei/2);
	temp.Z = (long)((double)OneOverZ*((double)(1<<ZSTEP_PREC)));
	temp.R = GetRValue(originalcolor);
	temp.G = GetGValue(originalcolor);
	temp.B = GetBValue(originalcolor);
}
*/
/*

int point_3d::ClassifyPoint(Plane *pln){
        float            dist;
        dist = pln->a*Wx + pln->b*Wy + pln->c*Wz + pln->d;
        if(dist > c_plane_eps)
                return BSP_INFRONT;
        else if(dist < -c_plane_eps)
                return BSP_BEHIND;
        else
                return BSP_ONPLANE;
}
*/

