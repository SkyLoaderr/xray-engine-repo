// Gauss.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <d3dtypes.h>
#pragma hdrstop

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#define DLL_API			__declspec(dllexport)

//---------------------------------------------------------------------------
extern "C" float DLL_API gauss [7][7] =
{
	{	0,	0,	0,	5,	0,	0,	0	},
	{	0,	5,	18,	32,	18,	5,	0	},
	{	0,	18,	64,	100,64,	18,	0	},
	{	5,	32,	100,100,100,32,	5	},
	{	0,	18,	64,	100,64,	18,	0	},
	{	0,	5,	18,	32,	18,	5,	0	},
	{	0,	0,	0,	5,	0,	0,	0	}
};
DWORD convert(float a)
{
	DWORD A = DWORD(a);
	clamp(A,0ul,255ul);
	return A;
}

//---------------------------------------------------------------------------
// Un-optimized code to perform general image filtering
// outputs to dst using a filter kernel in ker which must be a 2D float
// array of size [2*k+1][2*k+1]
//---------------------------------------------------------------------------
extern "C" void DLL_API ip_BuildKernel	(float* dest, float* src, int DIM=3, float norm=1.f)
{
	float	*I,*E;

	float	sum		= 0;
	int		size	= 2*DIM+1;
	E				= src + (size*size);
	for (I=src; I!=E; I++) sum += *I;
	float	scale	= norm/sum;
	for (I=src; I!=E; I++) *dest++ = *I * scale;
}
//---------------------------------------------------------------------------
extern "C" void DLL_API ip_ProcessKernel(DWORD* dest, DWORD* src, int w, int h, float* kern, int DIM=3)
{
	for (int y=0;y<h;y++)
	{
		for (int x=0;x<w;x++)
		{
			Fvector4 total; 
			total.set	(0,0,0,0);
			float *kp	= kern;
			for (int j=-DIM;j<=DIM;j++)
			for (int i=-DIM;i<=DIM;i++)
			{
				int x2=x+i,y2=y+j;

				// wrap pixels
				if (x2<0) x2+=w;
				else if (x2>=w) x2-=w;
				if (y2<0) y2+=h;
				else if (y2>=h) y2-=h;
				
				float blend = *kp;
				total.x += float(RGBA_GETRED(src[y2*w+x2])) * blend;
				total.y += float(RGBA_GETGREEN(src[y2*w+x2])) * blend;
				total.z += float(RGBA_GETBLUE(src[y2*w+x2])) * blend;
				total.w += float(RGBA_GETALPHA(src[y2*w+x2])) * blend;
				kp++;
			}
			*dest = RGBA_MAKE(convert(total.x),convert(total.y),convert(total.z),convert(total.w));
			dest++;
		}
	}
}
