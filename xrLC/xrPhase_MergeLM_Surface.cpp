#include "stdafx.h"
#include "build.h"
#include "xrPhase_MergeLM_Rect.h"

static	BYTE	surface			[512*512];
const	DWORD	alpha_ref		= 254-BORDER;

// Initialization
void _InitSurface	()
{
	FillMemory		(surface,512*512,0);
}

// Rendering of rect
void _rect_register	(_rect &R, CDeflector::Layer* D, BOOL bRotate)
{
	LPDWORD lm	= D->lm.pSurface;
	DWORD	s_x	= D->lm.dwWidth+2*BORDER;
	DWORD	s_y = D->lm.dwHeight+2*BORDER;
	
	if (!bRotate) {
		// Normal (and fastest way)
		for (DWORD y=0; y<s_y; y++)
		{
			BYTE*	P = surface+(y+R.a.y)*512+R.a.x;	// destination scan-line
			DWORD*	S = lm + y*s_x;
			for (DWORD x=0; x<s_x; x++,P++) 
			{
				DWORD C = *S++;
				DWORD A = RGBA_GETALPHA	(C);
				if (A>=alpha_ref)	*P	= 255;
			}
		}
	} else {
		// Rotated :(
		for (DWORD y=0; y<s_x; y++)
		{
			BYTE*	P = surface+(y+R.a.y)*512+R.a.x;	// destination scan-line
			for (DWORD x=0; x<s_y; x++,P++)
			{
				DWORD C = lm[x*s_x+y];
				DWORD A = RGBA_GETALPHA(C);
				if (A>=alpha_ref)	*P	= 255;
			}
		}
	}
}

// Test of per-pixel intersection (surface test)
bool Place_Perpixel	(_rect& R, CDeflector::Layer* D, BOOL bRotate)
{
	LPDWORD lm			= D->lm.pSurface;
	DWORD	s_x			= D->lm.dwWidth	+2*BORDER;
	DWORD	s_y			= D->lm.dwHeight+2*BORDER;
	
	if (!bRotate) {
		// Normal (and fastest way)
		for (DWORD y=0; y<s_y; y++)
		{
			BYTE*	P = surface+(y+R.a.y)*512+R.a.x;	// destination scan-line
			DWORD*	S = lm + y*s_x;
			for (DWORD x=0; x<s_x; x++,P++) 
			{
				DWORD C = *S++;
				DWORD A = RGBA_GETALPHA(C);
				if ((*P)&&(A>=alpha_ref))	return false;
			}
		}
	} else {
		// Rotated :(
		for (DWORD y=0; y<s_x; y++)
		{
			BYTE*	P = surface+(y+R.a.y)*512+R.a.x;	// destination scan-line
			for (DWORD x=0; x<s_y; x++,P++)
			{
				DWORD C = lm[x*s_x+y];
				DWORD A = RGBA_GETALPHA(C);
				if ((*P)&&(A>=alpha_ref))	return false;
			}
		}
	}
	
	// It's OK to place it
	return true;
}

// Check for intersection
BOOL _rect_place(_rect &r, CDeflector::Layer* D)
{
	_rect R;

	// Normal
	{
		DWORD x_max = 512-r.b.x; 
		DWORD y_max = 512-r.b.y; 
		for (DWORD _Y=0; _Y<y_max; _Y++)
		{
			for (DWORD _X=0; _X<x_max; _X++)
			{
				if (surface[_Y*512+_X]) continue;
				R.init(_X,_Y,_X+r.b.x,_Y+r.b.y);
				if (Place_Perpixel	(R,D,FALSE)) {
					_rect_register	(R,D,FALSE);
					r.set			(R);
					return TRUE;
				}
			}
		}
	}
	
	// Rotated
	{
		DWORD x_max = 512-r.b.y; 
		DWORD y_max = 512-r.b.x; 
		for (DWORD _Y=0; _Y<y_max; _Y++)
		{
			for (DWORD _X=0; _X<x_max; _X++)
			{
				if (surface[_Y*512+_X]) continue;
				
				R.init(_X,_Y,_X+r.b.y,_Y+r.b.x);
				if (Place_Perpixel	(R,D,TRUE)) {
					_rect_register	(R,D,TRUE);
					r.set			(R);
					return TRUE;
				}
			}
		}
	}
	
	return FALSE;
};

