// occRasterizer.cpp: implementation of the occRasterizer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "occRasterizer.h"

ENGINE_API occRasterizer	Raster;

void __stdcall fillDW_8x	(LPDWORD ptr, DWORD size, DWORD value)
{
	LPDWORD end = ptr+size;
	for (; ptr!=end; ptr+=2)
	{
		ptr[0]	= value;
		ptr[1]	= value;
	}
}

IC void propagade_depth			(LPVOID p_dest, LPVOID p_src, int dim)
{
	int*	dest = (int*)p_dest;
	int*	src	 = (int*)p_src;

	for (int y=0; y<dim; y++)
	{
		for (int x=0; x<dim; x++)
		{
			int*	base0		= src + (y*2+0)*(dim*2) + (x*2);
			int*	base1		= src + (y*2+1)*(dim*2) + (x*2);
			int		f1			= base0[0];
			int		f2			= base0[1];
			int		f3			= base1[0];
			int		f4			= base1[1];
			int		f			= f1;
			if (f2>f)	f		= f2;
			if (f3>f)	f		= f3;
			if (f4>f)	f		= f4;
			dest[y*dim+x]		= f;
		}
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

occRasterizer::occRasterizer	()
{
	
}

occRasterizer::~occRasterizer	()
{
	
}

void occRasterizer::clear		()
{
	DWORD size	= occ_dim*occ_dim;
	float f		= 1.f;
	fillDW_8x	(LPDWORD(bufFrame),		size,0);
	fillDW_8x	(LPDWORD(bufDepth),		size,*LPDWORD(&f));
}

IC BOOL shared(occTri* T1, occTri* T2)
{
	if (T1==T2)					return TRUE;
	if (T1->adjacent[0]==T2)	return TRUE;
	if (T1->adjacent[1]==T2)	return TRUE;
	if (T1->adjacent[2]==T2)	return TRUE;
	return FALSE;
}

void occRasterizer::propagade	()
{
	// Clip-and-propagade zero level
	for (int y=0; y<occ_dim_0; y++)
	{
		for (int x=0; x<occ_dim_0; x++)
		{
			int					ox=x+2, oy=y+2;
			
			// Y-connect
			int	pos		= oy*occ_dim+ox;
			int	pos_up	= pos-occ_dim;
			int	pos_up2	= pos_up-occ_dim;
			int	pos_down= pos+occ_dim;
			
			occTri**	pFrame	= get_frame	();
			float*		pDepth	= get_depth	();
			occTri* Td1			= pFrame	[pos_down];
			if (Td1) {
				// We has pixel 1scan down
				if (shared(Td1,pFrame[pos_up]))
				{
					// We has pixel 1scan up
					float ZR			= (pDepth[pos_down]+pDepth[pos_up])/2;
					if (ZR<pDepth[pos])	{ pFrame[pos] = Td1; pDepth[pos] = ZR; }
				} else if (shared(Td1,pFrame[pos_up2])) 
				{
					// We has pixel 2scan up
					float ZR			= (pDepth[pos_down]+pDepth[pos_up2])/2;
					if (ZR<pDepth[pos])	{ pFrame[pos] = Td1; pDepth[pos] = ZR; }
				}
			}

			//
			float d				= pDepth[pos];
			clamp				(d,-1.9f,1.9f);
			bufDepth_0[y][x]	= d2int(d);
		}
	}

	// Propagade other levels
	propagade_depth	(bufDepth_1,bufDepth_0,occ_dim_1);
	propagade_depth	(bufDepth_2,bufDepth_1,occ_dim_2);
	propagade_depth	(bufDepth_3,bufDepth_2,occ_dim_3);
}

BOOL occRasterizer::test		(float _x0, float _y0, float _x1, float _y1, float _z)
{
	float d2	= occ_dim_0/2;
	int x0		= iFloor(_x0*d2); clamp(x0,0,occ_dim_0-1);
	int x1		= iCeil (_x1*d2); clamp(x1,0,occ_dim_0-1);
	int y0		= iFloor(_y0*d2); clamp(y0,0,occ_dim_0-1);
	int y1		= iCeil (_y1*d2); clamp(y1,0,occ_dim_0-1);
	int	z		= d2int_up	(_z)+1;
	
	for (int y=y0; y<=y1; y++)
	{
		for (int x=x0; x<=x1; x++)
		{
			if (z<bufDepth_0[y][x])	return TRUE;
		}
	}
	return FALSE;
}
