#include "stdafx.h"
#include "occRasterizer.h"

const int BOTTOM = 0, TOP = 1;

void i_order	(float* A, float* B, float* C)
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
{	return iCeil(v);	}
int maxPixel(float v)
{	return iFloor(v);	}
void Vclamp(int& v, int a, int b)
{
	if (v<a)	v=a; else if (v>=b) v=b-1;
}
BOOL shared(occTri* T1, occTri* T2)
{
	if (0==T2)					return FALSE;
	if (T1==T2)					return TRUE;
	if (T1->adjacent[0]==T2)	return TRUE;
	if (T1->adjacent[1]==T2)	return TRUE;
	if (T1->adjacent[1]==T2)	return TRUE;
	return FALSE;
}

/* Rasterize a scan line between given X point values, corresponding Z values and current color
*/
void i_scan		(occTri* T, int curY, float startT, float endT, float startX, float endX, float startR, float endR, float startZ, float endZ)
{
	occTri**	pFrame	= Raster.get_frame();
	float*		pDepth	= Raster.get_depth();

	// guard-banding and clipping
	int minX	= minPixel(startX), maxX = maxPixel(endX);
	int minT	= maxPixel(startT), maxT = minPixel(endT);
	Vclamp		(minT,1,occ_dim0-1);
	Vclamp		(maxT,1,occ_dim0-1);
	if (minT >= maxT)		return;
	Vclamp		(minX,0,occ_dim0);
	Vclamp		(maxX,0,occ_dim0);
	int limLeft,limRight;
	if (minX >  maxX)	{ limLeft=maxX; limRight=minX;	}
	else				{ limLeft=minX; limRight=maxX;	}
	
	// interpolate
	float lenR	= endR - startR;
	float Zlen	= endZ - startZ;
	float Zstart= startZ + (minT - startR)/lenR * Zlen;		// interpolate Z to the start
	float Zend	= startZ + (maxT - startR)/lenR * Zlen;		// interpolate Z to the end
	float dZ	= (Zend-Zstart)/(maxT-minT);				// incerement in Z / pixel wrt dX
	int X		= minT;
	float Z		= Zstart;
	int	i		= curY*occ_dim0+X;
	
	// left connector
	for (; X<limLeft; X++, i++, Z+=dZ)
	{
		if (shared(T,pFrame[i-1])) {
			float ZR = (Z+pDepth[i-1])/2;
			if (ZR<pDepth[i])	{ pFrame[i]	= T; pDepth[i]	= ZR; }
		}
	}

	// compute the scanline + Y connectors
	for (; X<maxX; X++, i++, Z+=dZ) 
	{
		if (Z < pDepth[i])	{ pFrame[i]	= T; pDepth[i] = Z; }
	}
	
	// right connector
	for (X=maxT-1, Z=Zend-dZ, i=curY*occ_dim0+X; X>=limRight; X--, i--, Z-=dZ)
	{
		if (shared(T,pFrame[i+1])) {
			float ZR = (Z+pDepth[i+1])/2;
			if (ZR<pDepth[i])	{ pFrame[i]	= T; pDepth[i]	= ZR; }
		}
	}
}

void i_test_micro( int x, int y)
{
	occTri**	pFrame	= Raster.get_frame();
	float*		pDepth	= Raster.get_depth();
	
	if (x<1) return; else if (x>=occ_dim0-1)	return;
	if (y<1) return; else if (y>=occ_dim0-1)	return;
	int	pos		= y*occ_dim0+x;
	int	pos_up	= pos-occ_dim0;
	int	pos_down= pos+occ_dim0;
	
	occTri* T1	= pFrame[pos_up		];
	occTri* T2	= pFrame[pos_down	];
	if (T1 && shared(T1,T2))	
	{
		float ZR = (pDepth[pos_up]+pDepth[pos_down])/2;
		if (ZR<pDepth[pos])	{ pFrame[pos] = T1; pDepth[pos] = ZR; }
	}
}
void i_test		( int x, int y)
{
	i_test_micro	(x,y-1);
	i_test_micro	(x,y+1);
	i_test_micro	(x,y);
}

void i_line		( int x1, int y1, int x2, int y2 )
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x2 >= x1 ? 1 : -1;
    int sy = y2 >= y1 ? 1 : -1;
	
    if ( dy <= dx ){
        int d  = ( dy << 1 ) - dx;
        int d1 = dy << 1;
        int d2 = ( dy - dx ) << 1;
		
		i_test(x1,y1);
		
        for  (int x = x1 + sx, y = y1, i = 1; i <= dx; i++, x += sx){
            if ( d > 0){
                d += d2; y += sy;
            }else
                d += d1;
			i_test(x,y);
        }
    }else{
        int d  = ( dx << 1 ) - dy;
        int d1 = dx << 1;
        int d2 = ( dx - dy ) << 1;
		
		i_test(x1,y1);
        for  (int x = x1, y = y1 + sy, i = 1; i <= dy; i++, y += sy ){
            if ( d > 0){
                d += d2; x += sx;
            }else
                d += d1;
			i_test(x,y);
        }
    }
}
IC void i_edge ( float x1, float y1, float x2, float y2)
{
	i_line	(iFloor(x1),iFloor(y1),iFloor(x2),iFloor(y2));
}


/* 
Rasterises 1 section of the triangle a 'section' of a triangle is the portion of a triangle between 
2 horizontal scan lines corresponding to 2 vertices of a triangle
p2.y >= p1.y, p1, p2 are start/end vertices
E1 E2 are the triangle edge differences of the 2 bounding edges for this section
*/

IC float maxp(float a, float b)
{	return a>b ? a:b;		}
IC float minp(float a, float b)
{	return a<b ? a:b;		}

IC void i_section	(float *A, float *B, float *C, occTri* T, int Sect, BOOL bMiddle)
{
	// Find the start/end Y pixel coord, set the starting pts for scan line ends
	int		startY, endY;
	float	*startp1, *startp2;
	float	E1[3], E2[3];

	if (Sect == BOTTOM) { 
		startY	= minPixel(A[1]); endY = maxPixel(B[1])-1; 
		startp1 = startp2 = A;
		if (bMiddle)	endY ++;
		
		// check 'endY' for out-of-triangle 
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
		if (bMiddle)	startY --;
		
		// check 'startY' for out-of-triangle 
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
	float	e1_init_dY = float(startY) - startp1[1], e2_init_dY = float(startY) - startp2[1];
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
		float	minT	= minp(leftX-lhx,leftX+lhx);
		float	maxT	= maxp(rightX-rhx,rightX+rhx);
		float	minX	= maxp(leftX-lhx,leftX+lhx);
		float	maxX	= minp(rightX-rhx,rightX+rhx);
		i_scan	(T, int(startY), minT,maxT,minX,maxX, leftX-lhx, rightX-rhx, leftZ, rightZ);
		leftX	+= left_dX; rightX += right_dX;
		leftZ	+= left_dZ; rightZ += right_dZ;
	}
}

void __stdcall i_section_b0	(float *A, float *B, float *C, occTri* T)
{	i_section	(A,B,C,T,BOTTOM,0);	}
void __stdcall i_section_b1	(float *A, float *B, float *C, occTri* T)
{	i_section	(A,B,C,T,BOTTOM,1);	}
void __stdcall i_section_t0	(float *A, float *B, float *C, occTri* T)
{	i_section	(A,B,C,T,TOP,0);	}
void __stdcall i_section_t1	(float *A, float *B, float *C, occTri* T)
{	i_section	(A,B,C,T,TOP,1);	}

void occRasterizer::rasterize	(occTri* T)
{
	// Create local copy of the vertices
	// Since I am reordering the vertices, changing A,B,C screws up the model
	float a[3],  b[3],  c[3];
	a[0] = T->raster[0].x; a[1] = T->raster[0].y; a[2] = T->raster[0].z;
	b[0] = T->raster[1].x; b[1] = T->raster[1].y; b[2] = T->raster[1].z;
	c[0] = T->raster[2].x; c[1] = T->raster[2].y; c[2] = T->raster[2].z;
	
	i_order				(a, b, c);			// Order the vertices by Y
	if (b[1]-floorf(b[1])>0.5f)	
	{
		i_section_b1	(a, b, c, T);	// Rasterise First Section
		i_section_t0	(a, b, c, T);	// Rasterise Second Section
	} else {
		i_section_b0	(a, b, c, T);	// Rasterise First Section
		i_section_t1	(a, b, c, T);	// Rasterise Second Section
	}

	// Rasterize (and Y-connect) edges
	i_edge			(a[0],a[1],b[0],b[1]);
	i_edge			(a[0],a[1],c[0],c[1]);
	i_edge			(b[0],b[1],c[0],c[1]);
}
