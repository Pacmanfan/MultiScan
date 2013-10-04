#include "plane.h"
#include "point3d.hpp"
#include "Vector3d.hpp"

void Plane::CalculatePlaneEquation(point_3d *vertices, int num_points)
{
    float           len;
    int             i;
    Vector3d          ref, norm, v1, v2;
    point_3d          *vert1, *vert2;

    ref.x = ref.y = ref.z = 0.0;
    norm.x = norm.y = norm.z = 0.0;

    for(i=0; i<num_points; i++) 
	{
		vert1 = &vertices[i];
		vert2 = &vertices[(i+1) % num_points];

		v1.x = vert1->Wx;
		v1.y = vert1->Wy;
		v1.z = vert1->Wz;

		v2.x = vert2->Wx;
		v2.y = vert2->Wy;
		v2.z = vert2->Wz;

        norm.x += (v1.y - v2.y)*(v1.z + v2.z);
        norm.y += (v1.z - v2.z)*(v1.x + v2.x);
        norm.z += (v1.x - v2.x)*(v1.y + v2.y);
        ref.x += v1.x;
        ref.y += v1.y;
        ref.z += v1.z;
    }

	len = norm.Mag();
    a = norm.x / len;
    b = norm.y / len;
    c = norm.z / len;
    len *= num_points;
    d = -ref.Dot(norm) / len;
}
