#include "stdafx.h"
#pragma hdrstop

#ifdef _EDITOR
#include "skeletonX.h"
#include "bodyinstance.h"
#else
#include "..\skeletonX.h"
#include "..\bodyinstance.h"
#endif

/*
struct vertBoned2W	// (1+3+3 + 1+3+3 + 2)*4 = 16*4 = 64 bytes
{
	WORD	matrix0;
	WORD	matrix1;
	Fvector	P0;
	Fvector	N0;
	Fvector	P1;
	Fvector	N1;
	float	w;
	float	u,v;
};
*/

void __stdcall xrSkin2W_x86(vertRender* D,
							vertBoned2W* S,
							DWORD vCount,
							CBoneInstance* Bones) 
{
	// Prepare
	int U_Count			= vCount;
	vertBoned2W*	V	= S;
	vertBoned2W*	E	= V+U_Count;
	Fvector			P0,N0,P1,N1;

	// NON-Unrolled loop
	for (; S!=E; )
	{
		Fmatrix& M0		= Bones[S->matrix0].mTransform;
		Fmatrix& M1		= Bones[S->matrix1].mTransform;
		M0.transform_tiny(P0,S->P0);
		M0.transform_dir (N0,S->N0);
		M1.transform_tiny(P1,S->P1);
		M1.transform_dir (N1,S->N1);
		D->P.lerp		(P0,P1,S->w);
		D->N.lerp		(N0,N1,S->w);
		D->u			= S->u;
		D->v			= S->v;
		S++; D++;
	}
}
