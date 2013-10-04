//-----------------MATH3D.H
//contains all the neccesary math function for the 3d engine

#ifndef MATH3D
#define MATH3D
#include "math.h"
#include "stdlib.h" 
#include <stdio.h>

#define PI 3.1415926535
#define PIDIV180  PI / 180 //rad per degree
#define RADPERDEG  PIDIV180 //rad per degree
#define DEGPERRAD  180 / PI //degree per rad
#define PIDIV2 1.570796327
#define PIMUL2 6.283185307

#define Hither .5f
#define Yon 17000.0f
#define numentrys 1024
#define stepprecfact 2.844444444f

extern float sinlook[numentrys],coslook[numentrys];
//float SIN(float n);
//float COS(float n);
#define SIN(n)(sinlook[int(n*stepprecfact)])
#define COS(n)(coslook[int(n*stepprecfact)])
//float COS(float n){return coslook[int(n*stepprecfact)];}

//#define COS(a) ( cos_look [a & (numentrys - 1)] )
//#define SIN(a) ( sin_look [a & (numentrys - 1)] )

float Random(float n);//{return ((1/RAND_MAX*rand())*n);}
extern bool tablesbuilt;
void Build_Look_Up_Tables(void);
class point_3d;//forward def
class Vector3d;//forward def
class Matrix3D 
{
public:
  void InitMat ( float Mat [ 4 ] [ 4 ] );
  void MergeMatrix ( float NewMatrix [ 4 ] [ 4 ] );
  void MergeMatrices ( float Dest [ 4 ] [ 4 ], float Source [ 4 ] [ 4 ] );
  float Matrix [4][4];
  float RMatrix [4][4];

  Matrix3D ()
     {
     Initialize ();
     }
  void Set(float x,float y,float z,short row);
  void SetRow(float x,float y,float z,short row);
  void Get(float *x,float *y,float *z,short row);
  void GetRow(float *x,float *y,float *z,short col);
  void Rotate ( float Xa, float Ya, float Za );
  void Translate ( float Xt, float Yt, float Zt );
  void SetPosition(float x,float y,float z);
  void GetPosition(float &x,float &y, float &z);
  void SetAngles(float x,float y,float z);

  void Scale ( float Xs, float Ys, float Zs );
  void Shear ( float Xs, float Ys );
  void Initialize ();
  void Print();
  void Transform(point_3d &V);
  void Load(FILE *fp);
  void Save(FILE *fp);
  void Inverse(Matrix3D &dest);// calc the inverse
  point_3d &Untransform ( point_3d &V );
  Vector3d &Transform ( Vector3d &V );
  void TransformWorld(point_3d &V);
};
#endif // Math3d
  
