
#ifndef CAMERAINCLUDED
#define CAMERAINCLUDED

#include "math3d.h"
#include "vector3d.hpp"
#include "plane.h"
#include <stdio.h>
#include "log.h"

class camera{
public:
	char		name[256];
	float		viewing_distance;  // the distance of user to the screen; 
	Matrix3D    global_view;// the global inverse world to camera matrix.
	long		id;
	Plane		top,bottom,left,right,nearplane,farplane;

	camera()
	{
		if(!tablesbuilt)Build_Look_Up_Tables();
		strcpy(name,"camera");
		viewing_distance = 350.0f;//350.0f; // default view dist. 1700
		global_view.Initialize ();
		global_view.Translate (0,0,0 );
		//global_view.Rotate ( 90, 1, 1 );
	}
	camera(float x,float y, float z, float xang,float yang, float zang){
		if(!tablesbuilt)Build_Look_Up_Tables();
		viewing_distance = 500.0f;//350.0f; // default view dist.
		global_view.Translate ( x, y, z );
		global_view.Rotate ( xang, yang, zang );
	}
	void Origin()
	{
		global_view.Initialize ();
		global_view.Translate (0,0,0);
		global_view.Rotate ( 0, 1, 1 );
	}

	void Origin2()
	{
		global_view.Initialize ();

		//global_view.Translate (0,0,0);
		//global_view.Rotate ( 0, 1, 1 );
		point_3d pnt;
		pnt.Set(0,1,0); //look down the y vector
		Vector3d vec;
		vec.Set(0,0,1); // set the z axis as up
		LookAt(&pnt,&vec);
	}
	void Rotate(float x,float y,float z){
		global_view.Rotate ( x,y,z );	
	}
	void Translate(float x,float y,float z){
		global_view.Translate (x,y,z);	
	}
	void GetPosition(point_3d *p){ // in world coords
		global_view.GetPosition(p->Wx ,p->Wy ,p->Wz);
	}
	void GetPosition(float *x,float *y,float *z){
		global_view.GetPosition(*x ,*y ,*z);
	}
	void SetPosition(point_3d *p)
	{ //in WC
		Log("setting camera pos %f %f %f",p->Wx ,p->Wy ,p->Wz );
		global_view.SetPosition(p->Wx ,p->Wy ,p->Wz );
	}
	void SetPosition(float x, float y, float z)
	{
		Log("setting camera pos %f %f %f",x ,y ,z );
		global_view.SetPosition(x ,y ,z );
	}
	void LookAt(point_3d *p,Vector3d *up)
	{ //works great
		camera tmp;
		Vector3d xvec; // side
		Vector3d forz; // forward z
		point_3d initial,curpos,curpos2;
		GetPosition(&curpos);
		forz = *p - curpos;
		forz.Normalize();
		
		curpos2.Wx = global_view.Matrix[0][3];
		curpos2.Wy = global_view.Matrix[1][3];
		curpos2.Wz = global_view.Matrix[2][3];

		up->Normalize();//make sure it's length of 1
		xvec = forz; // set equal
		xvec *= *up; // cross product
		*up = xvec;
		*up *= forz;// to re-correct the "up" vector to true orthaganality
		*up *= -1.0f;
		global_view.Initialize();	
		
		global_view.Matrix[0][0] = xvec.x;
		global_view.Matrix[1][0] = xvec.y;
		global_view.Matrix[2][0] = xvec.z;
  
		global_view.Matrix[0][1] = up->x;
		global_view.Matrix[1][1] = up->y;
		global_view.Matrix[2][1] = up->z; 

		global_view.Matrix[0][2] = forz.x;
		global_view.Matrix[1][2] = forz.y;
		global_view.Matrix[2][2] = forz.z;

		tmp.global_view.Initialize();
		tmp.SetPosition(curpos.Wx,curpos.Wy,curpos.Wz);
		global_view.MergeMatrices ( global_view.Matrix, tmp.global_view.Matrix);
	}
	void Orbit(point_3d *center,float dist,float angle){ // angle in deg.
		//convert angle and dist to x -y plane orbit
		//'center->Wx
		float xp,yp;
		Vector3d up;
		up.Set(0,0,1);
		xp = dist * COS(angle);
		yp = dist * SIN(angle);
		SetPosition(xp + center->Wx, yp + center->Wy, center->Wz);	
		LookAt(center,&up);
	}
	int Load(FILE *fp){
		fread(&name,255,1,fp);//255 char name	//read the camera name
		fread(&viewing_distance,sizeof(float),1,fp);//read the viewing distance
		fread(&id,sizeof(long),1,fp);
		global_view.Load(fp);//read the global_view matrix
		return 1;
	}
	int Load(char *file){
		if(file==0)return 0;
		FILE *fp=0;
		fp = fopen(file,"rb");
		if(fp==0)return 0;
		Load(fp);
		fclose(fp);
		return 1;
	}
	int Save(FILE *fp){
		fwrite(&name,255,1,fp);//255 char name	//write the camera name
		fwrite(&viewing_distance,sizeof(float),1,fp);//write the viewing distance
		fwrite(&id,sizeof(long),1,fp);
		global_view.Save(fp);//write the global_view matrix
		return 1;
	}

	int Save(char *file){
		if(file==0)return 0;
		FILE *fp=0;
		fp = fopen(file,"wb");
		if(fp==0)return 0;
		Save(fp);
		fclose(fp);
		return 1;
	}	
};

#endif
