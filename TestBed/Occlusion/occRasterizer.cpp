// occRasterizer.cpp: implementation of the occRasterizer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "occRasterizer.h"

occRasterizer	Raster;

void __stdcall fillDW_8x	(LPDWORD ptr, DWORD size, DWORD value)
{
	LPDWORD end = ptr+size;
	for (; ptr!=end; ptr+=8)
	{
		ptr[0]	= value;
		ptr[1]	= value;
		ptr[2]	= value;
		ptr[3]	= value;
		ptr[4]	= value;
		ptr[5]	= value;
		ptr[6]	= value;
		ptr[7]	= value;
	}
}

template <int dim>
void propagade_depth			(LPVOID p_dest, LPVOID p_src)
{
	float*	dest = (float*)p_dest;
	float*	src	 = (float*)p_src;

	for (int y=0; y<dim; y++)
	{
		for (int x=0; x<dim; x++)
		{
			float*	base0		= src + (y*2+0)*(dim*2) + (x*2);
			float*  base1		= src + (y*2+1)*(dim*2) + (x*2);
			float	f1			= base0[0];
			float	f2			= base0[1];
			float	f3			= base1[0];
			float	f4			= base1[1];
			float	f			= f1;
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

void occRasterizer::propagade	()
{
	propagade_depth<occ_dim1>	(bufDepth_1,bufDepth_0);
	propagade_depth<occ_dim2>	(bufDepth_2,bufDepth_1);
	propagade_depth<occ_dim3>	(bufDepth_3,bufDepth_2);
}

