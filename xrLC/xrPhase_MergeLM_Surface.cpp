#include "stdafx.h"
#include "build.h"
#include "xrPhase_MergeLM_Rect.h"

static	BYTE	surface			[lmap_size*lmap_size];
const	u32	alpha_ref		= 254-BORDER;

// Initialization
void _InitSurface	()
{
	FillMemory		(surface,lmap_size*lmap_size,0);
}

// Rendering of rect
void _rect_register	(L_rect &R, CDeflector::Layer* D, BOOL bRotate)
{
	u32*	lm	= D->lm.pSurface;
	u32		s_x	= D->lm.dwWidth+2*BORDER;
	u32		s_y = D->lm.dwHeight+2*BORDER;
	
	if (!bRotate) {
		// Normal (and fastest way)
		for (u32 y=0; y<s_y; y++)
		{
			BYTE*	P = surface+(y+R.a.y)*lmap_size+R.a.x;	// destination scan-line
			u32*	S = lm + y*s_x;
			for (u32 x=0; x<s_x; x++,P++) 
			{
				u32 C = *S++;
				u32 A = color_get_A	(C);
				if (A>=alpha_ref)	*P	= 255;
			}
		}
	} else {
		// Rotated :(
		for (u32 y=0; y<s_x; y++)
		{
			BYTE*	P = surface+(y+R.a.y)*lmap_size+R.a.x;	// destination scan-line
			for (u32 x=0; x<s_y; x++,P++)
			{
				u32 C = lm[x*s_x+y];
				u32 A = color_get_A(C);
				if (A>=alpha_ref)	*P	= 255;
			}
		}
	}
}

// Test of per-pixel intersection (surface test)
bool Place_Perpixel	(L_rect& R, CDeflector::Layer* D, BOOL bRotate)
{
	u32*	lm			= D->lm.pSurface;
	u32	s_x			= D->lm.dwWidth	+2*BORDER;
	u32	s_y			= D->lm.dwHeight+2*BORDER;
	
	if (!bRotate) {
		// Normal (and fastest way)
		for (u32 y=0; y<s_y; y++)
		{
			BYTE*	P = surface+(y+R.a.y)*lmap_size+R.a.x;	// destination scan-line
			u32*	S = lm + y*s_x;
			for (u32 x=0; x<s_x; x++,P++) 
			{
				u32 C = *S++;
				u32 A = color_get_A(C);
				if ((*P)&&(A>=alpha_ref))	return false;
			}
		}
	} else {
		// Rotated :(
		for (u32 y=0; y<s_x; y++)
		{
			BYTE*	P = surface+(y+R.a.y)*lmap_size+R.a.x;	// destination scan-line
			for (u32 x=0; x<s_y; x++,P++)
			{
				u32 C = lm[x*s_x+y];
				u32 A = color_get_A(C);
				if ((*P)&&(A>=alpha_ref))	return false;
			}
		}
	}
	
	// It's OK to place it
	return true;
}

// Check for intersection
BOOL _rect_place(L_rect &r, CDeflector::Layer* D)
{
	L_rect R;

	// Normal
	{
		u32 x_max = lmap_size-r.b.x; 
		u32 y_max = lmap_size-r.b.y; 
		for (u32 _Y=0; _Y<y_max; _Y++)
		{
			for (u32 _X=0; _X<x_max; _X++)
			{
				if (surface[_Y*lmap_size+_X]) continue;
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
		u32 x_max = lmap_size-r.b.y; 
		u32 y_max = lmap_size-r.b.x; 
		for (u32 _Y=0; _Y<y_max; _Y++)
		{
			for (u32 _X=0; _X<x_max; _X++)
			{
				if (surface[_Y*lmap_size+_X]) continue;
				
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

