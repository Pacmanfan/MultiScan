/*
	These are some routines used to determine intersections and such for
	various ray tracing tasks.

*/


#include "plane.h"
#include "point3d.hpp"
#ifndef RTUTIL
#define RTUTIL

bool IntersectPlane(Plane *pln, point_3d *start,Vector3d *dir,point_3d *intersection);
short IntersectSphere(point_3d *start,point_3d *end,point_3d *intersect, point_3d *center,float radius);
bool insphere(point_3d *spherecenter,float sprad,point_3d *tstcenter);

#endif 