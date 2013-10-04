

#include <stdio.h>
#include "log.h"
#include "string.h"
//#include "porttype.h"
#include "color.h"

class camera;
class Plane;
class Vector3d;


#ifndef SCREENPOINT2D
#define SCREENPOINT2D
class Point2D {//: public Link{
public:
  long X, Y;   // The screen X and Y of point
  long Z;      // The 1/Z value

  Point2D()
  {
	  X=0;Y=0;Z=0;
  }
  int operator == ( Point2D &V ){
    return ((X==V.X)&&(Y == V.Y));}
  void operator  = ( Point2D &V )
  {
	  X= V.X;
	  Y= V.Y;
	  Z= V.Z;
  }
  void Set(int x, int y)
  {
	X=x;
	Y=y;
  }
};

#endif


#ifndef POINT3D
#define POINT3D


class point_3d {
public:
	//this is no longer a 3d point, this is extended to e 
	//a general point for 3d and 2d with color and intensity support
	float			 Wx,Wy,Wz;// world / object coords
	float			 Cx,Cy,Cz;// camera coords
	Color	 m_color; // the original unlit color
	Point2D  m_p2d; // the 2d screen point this originally came from
    int operator == (point_3d &test){
		if((Wx == test.Wx) && (Wy == test.Wy) && (Wz == test.Wz))
              {return 1;}else 
			  return 0;
	}    
	void  operator += (Vector3d v);
	void  operator -= (Vector3d v);
	void  operator = (point_3d p);
	point_3d operator +(Vector3d v);
	point_3d operator -(Vector3d v);
	Vector3d operator -(point_3d p);
	
    point_3d(){
		Wx= 0.0f;Wy=0.0f;Wz=0.0f;
		Cx= 0.0f;Cy=0.0f;Cz=0.0f;
	}
    point_3d(float x,float y,float z)
	{
		Wx= x;Wy=y;Wz=z;
		Cx= 0.0f;Cy=0.0f;Cz=0.0f;
	}
	void Project(Point2D &p); 
	void Project(Point2D &temp,camera *cam,int wid,int hei);

	void Set(float x,float y,float z){Wx = x;Wy = y;Wz = z;}
	int Parse(char *line);
	void Load(FILE *fp)
	{
		fread(&Wx,sizeof(float),1,fp);
		fread(&Wy,sizeof(float),1,fp);
		fread(&Wz,sizeof(float),1,fp);
	}

	void Save(FILE *fp)
	{
		fwrite(&Wx,sizeof(float),1,fp);
		fwrite(&Wy,sizeof(float),1,fp);
		fwrite(&Wz,sizeof(float),1,fp);
	}
	void Logit(){
		Log("P: x %f, y %f, z %f",Wx,Wy,Wz);
	}
	int ClassifyPoint(Plane *pln);
	double Distance(point_3d *neighbor);
};

int UniqueVert( point_3d &V, point_3d *List, int Range );
unsigned int GetVertIndex( point_3d &V, point_3d *List, unsigned int Range );

#endif
