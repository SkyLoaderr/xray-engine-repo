// Occlusion.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//============================================================================
// tri_rast.cpp
//============================================================================

enum Sections {BOTTOM, TOP};

/* Order pts on Y coordinate
A,B,C are in increasing Y coordinates after return
*/

inline void orderPts(float A[3], float B[3], float C[3])
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

/* Find the closest min/max pixels of a point
Use min biasing 
*/
inline float minPixel(const float pt_val)
{
	float center = float(int(pt_val)) + 0.5f;
	// if pt is beyond center, pixel is the next , else current
	// next pixel = right/above
	return ( (pt_val > center) ? center + 1.0f: center );
}

inline float maxPixel(const float pt_val)
{
	float center = float(int(pt_val)) + 0.5f;
	// if pt is before/at center, pixel is previous, else current
	// prev pixel = left/below
	return ( (pt_val <= center) ? center - 1.0f: center );
}

/* Rasterize a scan line between given X point values, corresponding Z values
and current color
*/
void Scan1Line(unsigned char *Color, float *Depth, const int W, const int H,
			   int curY, float startX, float endX, float startZ, float endZ,
			   unsigned char cur_color[3])
{
	// X boundary in pixel coords
	float minX = minPixel(startX), maxX = maxPixel(endX);
	// some test cases gave min > max (ie pixel should not be plotted)
	if (minX > maxX) return;
	// interpolate Z to this start of boundary
	float Z		= startZ + (minX - startX)/(endX - startX) * (endZ - startZ);
	// incerement in Z / pixel wrt dX
	float dZ	= (endZ-startZ)/(endX-startX);
	// trunc the 0.5 in pixel coords to int, setup starting conditons
	int initX	= int(minX), finalX = int(maxX), i = curY*W+initX, ci=3*i;
	// compute the scanline
	for (; initX<=finalX; initX++) 
	{
		if (Z<(Depth[i])) { // update Z buffer, RGB
			Color[ci] = cur_color[0];
			Color[ci+1] = cur_color[1];
			Color[ci+2] = cur_color[2];
			Depth[i] = Z;
		}
		ci+=3; i++;
		Z += dZ;
	}
}

/* Rasterises 1 section of the triangle
a 'section' of a triangle is the poriton of a triangle between 
2 horizontal scan lines corresponding to 2 vertices of a triangle
p2.y >= p1.y, p1, p2 are start/end vertices
E1 E2 are the triangle edge differences of the 2 bounding edges for this 
section
*/
void RasterizeSection(unsigned char *Color, float *Depth, 
					  const int W, const int H, 
					  float *A, float *B, float *C,
					  unsigned char CurrentColor[3], Sections Sect)
{
	// Find the start/end Y pixel coord, set the starting pts for scan line ends
	float startY, endY, *startp1, *startp2;
	if (Sect == BOTTOM) { 
		startY	= minPixel(A[1]); endY = maxPixel(B[1]); 
		startp1 = startp2 = A;
	}
	else { 
		startY  = minPixel(B[1]); endY = maxPixel(C[1]); 
		startp1 = A; startp2 = B;
	}
	if (startY <= endY) 
	{
		// Find the edge differences
		float E1[3], E2[3];
		if (Sect == BOTTOM)	
		{
			E1[0] = B[0]-A[0]; E2[0] = C[0]-A[0];
			E1[1] = B[1]-A[1]; E2[1] = C[1]-A[1];
			E1[2] = B[2]-A[2]; E2[2] = C[2]-A[2];
		} else {
			E1[0] = C[0]-A[0]; E2[0] = C[0]-B[0];
			E1[1] = C[1]-A[1]; E2[1] = C[1]-B[1];
			E1[2] = C[2]-A[2]; E2[2] = C[2]-B[2];
		}

		// Compute the inverse slopes of the lines, ie rate of change of X by Y
		float mE1	= E1[0]/E1[1];
		float mE2	= E2[0]/E2[1];
		// Initial Y offset for left and right (due to pixel rounding)
		float e1_init_dY = startY - startp1[1], e2_init_dY = startY - startp2[1];
		float t;
		float leftX, leftZ, rightX, rightZ, left_dX, right_dX, left_dZ, right_dZ;
		// find initial values, step values
		if ( ((mE1<mE2)&&(Sect==BOTTOM)) || ((mE1>mE2)&&(Sect==TOP)) ) 
		{ 
			// E1 is on the Left
			// Initial Starting values for left (from E1)
			t		= e1_init_dY/E1[1]; // Initial fraction of offset
			leftX	= startp1[0] + E1[0]*t; left_dX = mE1;
			leftZ	= startp1[2] + E1[2]*t; left_dZ = E1[2]/E1[1];
			// Initial Ending values for right (from E2)
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
			// Initial Ending values for right (from E1)
			t		= e1_init_dY/E1[1]; // Initial fraction of offset
			rightX	= startp1[0] + E1[0]*t; right_dX = mE1;
			rightZ	= startp1[2] + E1[2]*t; right_dZ = E1[2]/E1[1];
		}	
		// Now scan all lines in this section
		for (; startY<=endY; startY++) 
		{
			Scan1Line(Color, Depth, W, H, int(startY), leftX, rightX, leftZ, rightZ, CurrentColor);
			leftX += left_dX; rightX += right_dX;
			leftZ += left_dZ; rightZ += right_dZ;
		}
	}
}

//----------------------------------------------------------------------------
// Given 3D triangle ABC in screen space with clipped coordinates within the following
// bounds: x in [0,W], y in [0,H], z in [0,1]. The origin for (x,y) is in the bottom
// left corner of the pixel grid. z=0 is the near plane and z=1 is the far plane,
// so lesser values are closer. The coordinates of the pixels are evenly spaced
// in x and y 1 units apart starting at the bottom-left pixel with coords
// (0.5,0.5). In other words, the pixel sample point is in the center of the
// rectangular grid cell containing the pixel sample. The framebuffer has
// dimensions width x height (WxH). The Color buffer is a 1D array (row-major
// order) with 3 unsigned chars per pixel (24-bit color). The Depth buffer is 
// a 1D array (also row-major order) with a float value per pixel
// For a pixel location (x,y) we can obtain
// the Color and Depth array locations as: Color[(((int)y)*W+((int)x))*3] 
// (for the red value, green is offset +1, and blue is offset +2 and
// Depth[((int)y)*W+((int)x)]. Fills the pixels contained in the triangle
// with the global current color and the properly linearly interpolated depth
// value (performs Z-buffer depth test before writing new pixel). 
// Pixel samples that lie inside the triangle edges are filled with
// a bias towards the minimum values (samples that lie exactly on a triangle
// edge are filled only for minimum x values along a horizontal span and for
// minimum y values, samples lying on max values are not filled).
//---------------------------------------------------------------------------
void RasterizeTriangle_FLAT(unsigned char *Color, float *Depth, const int W, 
							const int H, float A[3], float B[3], float C[3],
							unsigned char CurrentColor[3])
{
	// Create local copy of the vertices
	// Since I am reordering the vertices, changing A,B,C screws up the model
	float a[3],  b[3],  c[3];
	a[0] = A[0]; a[1] = A[1]; a[2] = A[2];
	b[0] = B[0]; b[1] = B[1]; b[2] = B[2];
	c[0] = C[0]; c[1] = C[1]; c[2] = C[2];
	
	// Order the vertices by Y
	orderPts		(a,b,c);
	// Rasterise First Section
	RasterizeSection(Color, Depth, W, H, a, b, c, CurrentColor, BOTTOM);
	// Rasterise Second Section
	RasterizeSection(Color, Depth, W, H, a, b, c, CurrentColor, TOP);
	
}

int main(int argc, char* argv[])
{
	printf("Hello World!\n");
	return 0;
}

