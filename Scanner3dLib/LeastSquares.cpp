#include "LeastSquares.h"
/* ------------------------------------------------------------------------
 * FILE: least-squares.c
 * This program computes a linear model for a set of given data.
 *
 * PROBLEM DESCRIPTION:
 *  The method of least squares is a standard technique used to find
 *  the equation of a straight line from a set of data. Equation for a
 *  straight line is given by 
 *	 y = mx + b
 *  where m is the slope of the line and b is the y-intercept.
 *
 *  Given a set of n points {(x1,y1), x2,y2),...,xn,yn)}, let
 *      SUMx = x1 + x2 + ... + xn
 *      SUMy = y1 + y2 + ... + yn
 *      SUMxy = x1*y1 + x2*y2 + ... + xn*yn
 *      SUMxx = x1*x1 + x2*x2 + ... + xn*xn
 *
 *  The slope and y-intercept for the least-squares line can be 
 *  calculated using the following equations:
 *        slope (m) = ( SUMx*SUMy - n*SUMxy ) / ( SUMx*SUMx - n*SUMxx ) 
 *  y-intercept (b) = ( SUMy - slope*SUMx ) / n
 *
 * AUTHOR: Dora Abdullah (Fortran version, 11/96)
 * REVISED: RYL (converted to C, 12/11/96)
 * REVISED: Steve Hernandez 12/20/2011 - converted to utility
 * ---------------------------------------------------------------------- */

void FindLeastSquare(List *points, float *m, float *b)
{
	float SUMx = 0.0f;
	float SUMy = 0.0f;
	float SUMxy = 0.0f;
	float SUMxx = 0.0f;
	float n = (float)(points->Count());
	float slope,y_intercept;
	for(ListItem *li = points->list; li!=0; li=li->next)
	{
		Point2D *p = (Point2D *)li->data;
		SUMx += p->X;
		SUMy += p->Y;
		SUMxy += p->X * p->Y;
		SUMxx += p->X * p->X;
	}
    slope = ( SUMx*SUMy - n*SUMxy ) / ( SUMx*SUMx - n*SUMxx ) ;
    y_intercept  = ( SUMy - slope*SUMx ) / n;
	*m = slope;
	*b = y_intercept;

}