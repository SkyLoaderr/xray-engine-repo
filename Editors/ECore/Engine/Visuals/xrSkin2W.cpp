#ifdef	__VECTORC__
	#include <windows.h>
	#include <math.h>
	#include <float.h>
	
	#undef		min
	#undef		max

	#define		XRCORE_API
	#define		IC inline
	#define		VERIFY(a)

	template <class T> struct _quaternion;

	#include	"_types.h"
	#include	"_std_extensions.h"
	#include	"_random.h"
	#include	"_vector3d.h"
	#include	"_vector2.h"
	#include	"_vector4.h"
	#include	"_matrix.h"

	#pragma pack(push,4)
	struct vertBoned1W	// (3+3+2+1)*4 = 9*4 = 36 bytes
	{
		Fvector	P;
		Fvector	N;
		float	u,v;
		s32		matrix;
	};
	struct vertBoned2W	// (1+3+3 + 1+3+3 + 2)*4 = 16*4 = 64 bytes
	{
		s16		matrix0;
		s16		matrix1;
		Fvector	P0;
		Fvector	N0;
		Fvector	P1;
		Fvector	N1;
		float	w;
		float	u,v;
	};
	struct	vertRender		// 32
	{
		Fvector	P;
		Fvector	N;
		float	u,v;
	};
	#pragma pack(pop)

	#pragma pack(push,8)
	struct	CBoneInstance	// 80-4 + 4*4 = 92 (96??)
	{
		// data
		Fmatrix				mTransform;							// final x-form matrix
		void*				Callback;
		void*				Callback_Param;
		BOOL				Callback_overwrite;					// performance hint - don't calc anims
		float				param			[4];				// 
	};
	#pragma pack(pop)

#else
	#include "stdafx.h"
	#pragma hdrstop

	#ifdef _EDITOR
		#include "skeletonX.h"
		#include "skeletoncustom.h"
	#else
		#include "..\skeletonX.h"
		#include "..\skeletoncustom.h"
	#endif
#endif

#define	HINT_ALIGNED(a) __declspec (alignedvalue (a))

struct	v34
{
	float	x,y,z;
	float	w;
};
IC	void	transform_pos		(v34 &dest, const Fvector3 &v, const Fmatrix& M)	
{
	dest.x	= v.x*M._11 + v.y*M._21 + v.z*M._31 + M._41;
	dest.y	= v.x*M._12 + v.y*M._22 + v.z*M._32 + M._42;
	dest.z	= v.x*M._13 + v.y*M._23 + v.z*M._33 + M._43;
	//__hint__((okmodify(dest.w)));
	dest.w	= v.x*M._14 + v.y*M._24 + v.z*M._34 + M._44;
}
IC	void	transform_dir		(v34 &dest, const Fvector3 &v, const Fmatrix& M)	
{
	dest.x	= v.x*M._11 + v.y*M._21 + v.z*M._31;
	dest.y	= v.x*M._12 + v.y*M._22 + v.z*M._32;
	dest.z	= v.x*M._13 + v.y*M._23 + v.z*M._33;
	//__hint__((okmodify(dest.w)));
	dest.w	= v.x*M._14 + v.y*M._24 + v.z*M._34;
}
IC	void	lerp				(v34 &dest, const v34 &p1, const v34 &p2, float t)
{
	float invt	= 1.f-t;
	dest.x		= p1.x*invt + p2.x*t;
	dest.y		= p1.y*invt + p2.y*t;
	dest.z		= p1.z*invt + p2.z*t;
	//__hint__((okmodify(dest.w)));
	dest.w		= p1.w*invt + p2.w*t;
}

void __stdcall xrSkin2W_x86(HINT_ALIGNED(16)	vertRender*		restrict D,
							HINT_ALIGNED(16)	vertBoned2W*	restrict S,
							u32					vCount,
							HINT_ALIGNED(16)	CBoneInstance*	restrict Bones) 
{
	// Prepare
	int U_Count			= vCount;
	vertBoned2W*	V	= S;
	vertBoned2W*	E	= V+U_Count;
	v34				P0,N0,P1,N1,P,N;

	// NON-Unrolled loop
	for (; S!=E; ){
    	if (S->matrix1!=S->matrix0){
            Fmatrix& M0		= Bones[S->matrix0].mTransform;
            Fmatrix& M1		= Bones[S->matrix1].mTransform;
			transform_pos	(P0,S->P0,M0);
			transform_dir	(N0,S->N0,M0);
			transform_pos	(P1,S->P1,M1);
			transform_dir	(N1,S->N1,M1);
			lerp			(P,P0,P1,S->w);
			lerp			(N,N0,N1,S->w);

            D->P.set		(P.x,P.y,P.z);
            D->N.set		(N.x,N.y,N.z);
            D->u			= S->u;
            D->v			= S->v;
        }else{
			// Fmatrix& M0		= Bones[S->matrix0].mTransform;
			// M0.transform_tiny(D->P,S->P0);
			// M0.transform_dir (D->N,S->N0);
			// D->u			= S->u;
			// D->v			= S->v;
        }
		S++; D++;
	}
}
