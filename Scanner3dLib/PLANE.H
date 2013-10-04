
#ifndef PLANEDEFINED
#define PLANEDEFINED
#include "stdio.h"
#include "log.h"
#include "point3d.hpp"
class Plane {
public:
        float            a;
        float            b;
        float            c;
        float            d;
		void Set(float av,float bv, float cv, float dv)
		{
			a = av;
			b = bv;
			c = cv;
			d = dv;
		}
		Plane()
		{
			a=0.0f;
			b=0.0f;
			c=0.0f;
			d=0.0f;
		}
		void CalculatePlaneEquation(point_3d vertices[], int num_points);
		void Load(FILE *fp){
			fread(&a,sizeof(float),1,fp);
			fread(&b,sizeof(float),1,fp);
			fread(&c,sizeof(float),1,fp);
			fread(&d,sizeof(float),1,fp);
		}
		void Save(FILE *fp){
			fwrite(&a,sizeof(float),1,fp);
			fwrite(&b,sizeof(float),1,fp);
			fwrite(&c,sizeof(float),1,fp);
			fwrite(&d,sizeof(float),1,fp);
		}
		void Logit(){
			Log("Plane a,b,c,d  %f %f %f %f",a,b,c,d);
		}
};

#endif