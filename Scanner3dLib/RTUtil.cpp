
#include "rtutil.hpp"
#include "vector3d.hpp"

class TestPoint{public:	double X,Y;};
static TestPoint TstPnt[4]; //for the crossing test
static long numTstPnt;//for the crossing test

int CrossingsTest(double PntX, double PntY ){
	register int	j, yflag0, yflag1, inside_flag, xflag0 ;
	register double ty, tx;// *vtx0, *vtx1 ;
	register int	line_flag ;
	short index=0;	
    tx = PntX;//point[X] ;
    ty = PntY;//point[Y] ;
	TestPoint  vtx0,vtx1;
    vtx0 = TstPnt[numTstPnt-1] ;
    yflag0 = ( vtx0.Y >= ty ) ;
    vtx1 = TstPnt[0];
    inside_flag = 0 ;
    line_flag = 0 ;
    for ( j = numTstPnt+1 ; --j ; ) {
	yflag1 = ( vtx1.Y >= ty ) ;
	if ( yflag0 != yflag1 ) {
	    xflag0 = ( vtx0.X >= tx ) ;
	    if ( xflag0 == ( vtx1.X >= tx ) ) {
			if ( xflag0 ) inside_flag = !inside_flag ;
	    } else {
			if ( (vtx1.X - (vtx1.Y-ty)*
				 ( vtx0.X-vtx1.X)/(vtx0.Y-vtx1.Y)) >= tx ) {
				inside_flag = !inside_flag ;
				}
	    }
	    if ( line_flag ) goto Exit ;
            line_flag = 1 ;
	}

	/* move to next pair of vertices, retaining info as possible */
	yflag0 = yflag1 ;
	vtx0 = vtx1 ;
	vtx1 = TstPnt[++index];
    }
    Exit: ;
    return( inside_flag ) ;
}
bool IntersectPlane(Plane *pln, point_3d *start,Vector3d *dir,point_3d *intersection){
	//intersect a plane with a ray in world space
	bool retval = false;
	double deltaX,deltaY,deltaZ,t,T,S;
	double A,B,C,D;//the polygon plane
	double denom;
	
	A = pln->a ;
	B = pln->b;
	C = pln->c;
	D = pln->d;
	deltaX = dir->x ;// end->Wx - start->Wx;
	deltaY = dir->y ;// end->Wy - start->Wy;
	deltaZ = dir->z ;// end->Wz - start->Wz;
	
	denom = (A*deltaX + B*deltaY + C*deltaZ);
	
	if(denom == 0.0){//ray is parallel, no intersection
		retval = false;
		return retval;
	}
	T = (-1)/denom;
	S = (A*start->Wx + B*start->Wy + C*start->Wz);	
	t =  (S + D)*T;
	//at this point we have a possible intersection
	//project to a major world axis and test for containment in the poly
	intersection->Wx = (float)(start->Wx + (t * deltaX));
	intersection->Wy = (float)(start->Wy + (t * deltaY));
	intersection->Wz = (float)(start->Wz + (t * deltaZ));
	return true;
}

short IntersectSphere(point_3d *start,point_3d *end,point_3d *intersect,
				 point_3d *center,float radius){
	short retval =0;
	float EO;//EO is distance from start of ray to center of sphere
	float d,disc,v;//v is length of direction ray
	Vector3d V,temp;//V is unit vector of the ray
	
	temp.Set(center->Wx - start->Wx,center->Wy - start->Wy,	center->Wz - start->Wz);

	EO = float(temp); // unnormalized length
	V.Set(end->Wx - start->Wx,end->Wy - start->Wy,end->Wz - start->Wz);
	v = float(V);// magnitude of direction vector
	V.Normalize();// normalize the direction vector
	disc = (radius*radius) - ((EO*EO) - (v*v));
	if(disc < 0.0f){
		retval =0;// no intersection
	}else{ // compute the intersection point
		Log("i");
		retval = 1;
		d = (float)sqrt(disc);
		intersect->Wx = start->Wx + ((v-d)*V.Getx());
		intersect->Wy = start->Wy + ((v-d)*V.Gety());
		intersect->Wz = start->Wz + ((v-d)*V.Getz());
	}
	return retval;
}