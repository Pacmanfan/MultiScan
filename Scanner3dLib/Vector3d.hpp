// vector 3d header plus plus file
// targeted at a 32bit OS


#ifndef VECTOR3D
#define VECTOR3D	
//#include <math.h>
//#include <stdio.h>
//#include <stdlib.h>
#include "point3d.hpp"
#include "log.h"
#include "math3d.h"
//#include "time.h"

enum {_zip,_Local,_World,_Camera};
//class point_3d;
class Vector3d
{
public:
float x,y,z,d;
float Tx,Ty,Tz,Td;

    Vector3d (){*this = 0.0f;}
    Vector3d (float nx,float ny,float nz){Set(nx,ny,nz);}
    inline void Set(float nx ,float ny ,float nz){x= nx; y = ny; z = nz;}
	inline void Setx(float xx){x = xx;}
	inline void Sety(float yy){y = yy;}
	inline void Setz(float zz){z = zz;}
    inline float Getx(){return x;}
    inline float Gety(){return y;}
    inline float Getz(){return z;}
    operator float() // magnitude of vector
    {
       float Mag = (float)sqrt((x*x)+(y*y)+(z*z));
       return Mag;
    }

    inline float Mag(){return float(*this);}  //returns mag of vect
 
    inline float Dot(Vector3d &v) //dot product 
    {
	    float dp = ( x * v.x ) +
		           ( y * v.y ) +
		     	   ( z * v.z );
	    return dp;
    }

    inline float MyDot(Vector3d &v) //dot product 
    {
	    float dp = ( x * v.x ) +
		           ( y * v.y ) +
		     	   ( z * v.z );
		if(dp < 0.0f){
			return 0;
		}else return dp;
    }

    inline float Angle(Vector3d &v) // returns angle bett. this and other vect. in rad.
    {
	    float Radians = (float)acos(this->Dot(v) / (Mag() * v.Mag() ));
	    return Radians;
    }

    inline float CosTheta(Vector3d &v) //returns cos bett. this and other.
    {
	    float CosA = this->Dot(v) / ( Mag() * v.Mag());

	    return CosA;
    }

    inline void Make3dVect(point_3d *term,point_3d * init,short pnt)
    {
		// this function creates a vector from two points in 3D space
                switch(pnt){
		case _World:
			x = term->Wx - init->Wx;
			y = term->Wy - init->Wy;
			z = term->Wz - init->Wz;
			break;
		case _Camera:
			x = term->Cx - init->Cx;
			y = term->Cy - init->Cy;
			z = term->Cz - init->Cz;
			break;
		}
    }  

    inline Vector3d &operator = (float scaler)
    {
	    x = y = z = scaler;
	    return *this;
    }
 
    inline Vector3d operator + (Vector3d &v)
    {
	    Vector3d r;
    	r.x = x + v.x;
	    r.y = x + v.y;
	    r.z = x + v.z;
	    return r;
    }

   inline Vector3d operator - (Vector3d &v)
   {
	    Vector3d r;
	    r.x = x - v.x;
	    r.y = x - v.y;
	    r.z = x - v.z;
    	return r;
   }

   inline Vector3d &operator += (Vector3d &v)
   {
    	x += v.x; y += v.y; z += v.z;
    	return *this;
   }

   inline Vector3d &operator -= (Vector3d &v)
   {
	    x -= v.x; y -= v.y; z -= v.z;
    	return *this;
   }
/*
   inline Vector3d operator * (Vector3d &v)  // returns cross prod.
   {
    	Vector3d r;
	    x += v.x; y += v.y; z += v.z;
	    return *this;
   }
*/
   inline Vector3d &operator *= (Vector3d &v)// returns cross prod.
   {
	  float ox = x, oy = y, oz = z;
	  x = (oy * v.z) - (oz * v.y);
	  y = (oz * v.x) - (ox * v.z);
	  z = (ox * v.y) - (oy * v.x);
      return *this;
   }

   inline Vector3d operator * (float scaler) //changes scaler
   {
	    Vector3d R;
	    R.x = x * scaler;
	    R.y = y * scaler;
	    R.z = z * scaler;
        return R;
   }

   inline Vector3d &operator *= (float scaler)//changes scaler
   {
    	x *= scaler;
        y *= scaler;
        z *= scaler;
    	return *this;
   }

   inline void Vector3d::Normalize()  
   {
    	float oneoverdist  = 1.0f / Mag();
	    (*this) *= oneoverdist;
   }
   void Print(){Log("%f, %f, %f",x,y,z);}
   Vector3d Perturb(float percent){
	   Vector3d ptb;
		//ptb.x =  x;
//		ptb += percent * (x * rand()/ );
	//
		ptb.x = x + ((((float)(rand()%10)) /10.0f) * 2.0f)  - ((((float)(rand()%10)) /10.0f) * 2.0f)/2.0f;
		ptb.y = y  + ((((float)(rand()%10)) /10.0f) * 2.0f) - ((((float)(rand()%10)) /10.0f) * 2.0f)/2.0f;
		ptb.z = z + ((((float)(rand()%10)) /10.0f) * 2.0f)  - ((((float)(rand()%10)) /10.0f) * 2.0f)/2.0f;
		return ptb;
   }
};
#endif
