#include "stdafx.h"
#include "occRasterizer.h"

const int BOTTOM = 0, TOP = 1;

void occRasterizer::i_order	(float* A, float* B, float* C)
{
	float *min, *max, *mid;
	float t1[3],t2[3],t3[3];
	if (A[1] <= B[1])
	{
        if (B[1] <= C[1]) 
		{  
			min = A; mid = B; max = C; 
		}
        else // C < B
			if   (A[1] <= C[1]) 
			{
				min = A; mid = C; max = B; 
			}
			else 
			{ 
				min = C; mid = A; max = B; 
			}
	}
	else // B < A
	{
		if (A[1] <= C[1]) 
		{ 
			min = B; mid = A; max = C; 
		}
		else // C < A
			if (B[1] <= C[1]) 
			{ 
				min = B; mid = C; max = A; 
			}
			else 
			{ 
				min = C; mid = B; max = A; 
			}
	}
	
	t1[0]	= min[0];	t2[0]	= mid[0];	t3[0]	= max[0];
	t1[1]	= min[1];	t2[1]	= mid[1];	t3[1]	= max[1];
	t1[2]	= min[2];	t2[2]	= mid[2];	t3[2]	= max[2];
	A[0]	= t1[0];	B[0]	= t2[0];	C[0]	= t3[0];
	A[1]	= t1[1];	B[1]	= t2[1];	C[1]	= t3[1];
	A[2]	= t1[2];	B[2]	= t2[2];	C[2]	= t3[2];
}

// Find the closest min/max pixels of a point
int minPixel(float v)
{	return int(ceilf(v));	}
int maxPixel(float v)
{	return int(floorf(v));	}

/* Rasterize a scan line between given X point values, corresponding Z values
and current color
*/
void i_scan	(occRasterizer* OCC, occTri* T, int curY, float startX, float endX, float startZ, float endZ)
{
	occTri**	pFrame	= OCC->get_frame();
	float*		pDepth	= OCC->get_depth();

	// guard-banding and clipping
	int minX	= minPixel(startX), maxX = maxPixel(endX);
	if (minX >= occ_dim0)	return;
	if (maxX <= 0)			return;
	if (minX <= 0)			minX = 0;
	if (maxX >= occ_dim0)	maxX = occ_dim0-1;
	if (minX >= maxX)		return;

	// interpolate
	float Z		= startZ + (minX - startX)/(endX - startX) * (endZ - startZ);	// interpolate Z to this start of boundary
	float dZ	= (endZ-startZ)/(endX-startX);									// incerement in Z / pixel wrt dX
	int initX	= int(minX), finalX = int(maxX), i = curY*occ_dim0+initX;		// trunc the 0.5 in pixel coords to int, setup starting conditons

	// compute the scanline
	for (; initX<finalX; initX++, i++, Z+=dZ) 
	{
		if (Z < pDepth[i]) 
		{	
			// update Z buffer + Frame buffer
			pFrame[i]	= T;
			pDepth[i]	= Z;
		}
	}
}


/* 
Rasterises 1 section of the triangle
a 'section' of a triangle is the poriton of a triangle between 
2 horizontal scan lines corresponding to 2 vertices of a triangle
p2.y >= p1.y, p1, p2 are start/end vertices
E1 E2 are the triangle edge differences of the 2 bounding edges for this 
section
*/

float maxp(float a, float b)
{	return a>b ? a:b;		}
float minp(float a, float b)
{	return a<b ? a:b;		}

void i_iterate	(occRasterizer* OCC, occTri* T, int startY, int endY, float leftX, float left_dX, float rightX, float right_dX, float leftZ, float left_dZ, float rightZ, float right_dZ)
{
	// Now scan all lines in this section
	float lhx = left_dX/2;	leftX	+= lhx;	// half pixel
	float rhx = right_dX/2;	rightX	+= rhx;	// half pixel
	for (; startY<=endY; startY++) 
	{
		i_scan	(OCC, T, int(startY), maxp(leftX-lhx,leftX+lhx), minp(rightX-rhx,rightX+rhx), leftZ, rightZ);
		leftX	+= left_dX; rightX += right_dX;
		leftZ	+= left_dZ; rightZ += right_dZ;
	}
}


void i_section	(occRasterizer* OCC, float *A, float *B, float *C, occTri* T, int Sect)
{
	// Find the start/end Y pixel coord, set the starting pts for scan line ends
	int		startY, endY;
	float	*startp1, *startp2;
	float	E1[3], E2[3];

	if (Sect == BOTTOM) { 
		startY	= minPixel(A[1]); endY = maxPixel(B[1]); 
		startp1 = startp2 = A;
		
		// check 'endY' for out-of-tiangle 
		int test = maxPixel(C[1]);
		if (endY>=test) endY --;
		if (startY > endY) return;

		// Find the edge differences
		E1[0] = B[0]-A[0]; E2[0] = C[0]-A[0];
		E1[1] = B[1]-A[1]; E2[1] = C[1]-A[1];
		E1[2] = B[2]-A[2]; E2[2] = C[2]-A[2];
	}
	else { 
		startY  = minPixel(B[1]); endY = maxPixel(C[1]); 
		startp1 = A; startp2 = B;
		
		// check 'startY' for out-of-tiangle 
		int test = minPixel(A[1]);
		if (startY<test) startY ++;
		if (startY > endY) return;

		// Find the edge differences
		E1[0] = C[0]-A[0]; E2[0] = C[0]-B[0];
		E1[1] = C[1]-A[1]; E2[1] = C[1]-B[1];
		E1[2] = C[2]-A[2]; E2[2] = C[2]-B[2];
	}
	
	// Compute the inverse slopes of the lines, ie rate of change of X by Y
	float mE1	= E1[0]/E1[1];
	float mE2	= E2[0]/E2[1];
	
	// Initial Y offset for left and right (due to pixel rounding)
	float	e1_init_dY = startY - startp1[1], e2_init_dY = startY - startp2[1];
	float	t,leftX, leftZ, rightX, rightZ, left_dX, right_dX, left_dZ, right_dZ;
	
	// find initial values, step values
	if ( ((mE1<mE2)&&(Sect==BOTTOM)) || ((mE1>mE2)&&(Sect==TOP)) ) 
	{ 
		// E1 is on the Left
		// Initial Starting values for left (from E1)
		t		= e1_init_dY/E1[1]; // Initial fraction of offset
		leftX	= startp1[0] + E1[0]*t; left_dX = mE1;
		leftZ	= startp1[2] + E1[2]*t; left_dZ = E1[2]/E1[1];

		// Initial Ending values for right	(from E2)
		t		= e2_init_dY/E2[1]; // Initial fraction of offset
		rightX	= startp2[0] + E2[0]*t; right_dX = mE2;
		rightZ	= startp2[2] + E2[2]*t; right_dZ = E2[2]/E2[1];
	}
	else { 
		// E2 is on left
		// Initial Starting values for left (from E2)
		t		= e2_init_dY/E2[1]; // Initial fraction of offset
		leftX	= startp2[0] + E2[0]*t; left_dX = mE2;
		leftZ	= startp2[2] + E2[2]*t; left_dZ = E2[2]/E2[1];

		// Initial Ending values for right	(from E1)
		t		= e1_init_dY/E1[1]; // Initial fraction of offset
		rightX	= startp1[0] + E1[0]*t; right_dX = mE1;
		rightZ	= startp1[2] + E1[2]*t; right_dZ = E1[2]/E1[1];
	}

	// Now scan all lines in this section
	float lhx = left_dX/2;	leftX	+= lhx;	// half pixel
	float rhx = right_dX/2;	rightX	+= rhx;	// half pixel
	for (; startY<=endY; startY++) 
	{
		i_scan	(OCC, T, int(startY), maxp(leftX-lhx,leftX+lhx), minp(rightX-rhx,rightX+rhx), leftZ, rightZ);
		leftX	+= left_dX; rightX += right_dX;
		leftZ	+= left_dZ; rightZ += right_dZ;
	}
}

void occRasterizer::rasterize	(occTri* T)
{
	// Create local copy of the vertices
	// Since I am reordering the vertices, changing A,B,C screws up the model
	float a[3],  b[3],  c[3];
	a[0] = T->raster[0].x; a[1] = T->raster[0].y; a[2] = T->raster[0].z;
	b[0] = T->raster[1].x; b[1] = T->raster[1].y; b[2] = T->raster[1].z;
	c[0] = T->raster[2].x; c[1] = T->raster[2].y; c[2] = T->raster[2].z;
	
	i_order				(a, b, c);					// Order the vertices by Y
	i_section			(this,a, b, c, T,BOTTOM);	// Rasterise First Section
	//i_section			(this,a, b, c, T,TOP);		// Rasterise Second Section
}
