#include "stdafx.h"
#pragma hdrstop

#ifdef _EDITOR
#include "skeletonX.h"
#include "skeletoncustom.h"
#else
#include "..\skeletonX.h"
#include "..\skeletoncustom.h"
#endif

// -- offsets -------------------------------------------------------
#define M11 0
#define M12 4
#define M13 8
#define M14 12
#define M21 16
#define M22 20
#define M23 24
#define M24 28
#define M31 32
#define M32 36
#define M33 40
#define M34 44
#define M41 48
#define M42 52
#define M43 56
#define M44 60
// ------------------------------------------------------------------

/*
struct vertBoned2W	// (1+3+3 + 1+3+3 + 2)*4 = 16*4 = 64 bytes
{
	u16	matrix0;
	u16	matrix1;
	Fvector	P0;
	Fvector	N0;
	Fvector	P1;
	Fvector	N1;
	float	w;
	float	u,v;
};

struct vertRender
{
	Fvector	P;
	Fvector	N;
	float	u,v;
};

	IC	void	transform_tiny		(Tvector &dest, const Tvector &v)	const // preferred to use
	{
		dest.x = v.x*_11 + v.y*_21 + v.z*_31 + _41;
		dest.y = v.x*_12 + v.y*_22 + v.z*_32 + _42;
		dest.z = v.x*_13 + v.y*_23 + v.z*_33 + _43;
	}
	IC	void	transform_dir		(Tvector &dest, const Tvector &v)	const 	// preferred to use
	{
		dest.x = v.x*_11 + v.y*_21 + v.z*_31;
		dest.y = v.x*_12 + v.y*_22 + v.z*_32;
		dest.z = v.x*_13 + v.y*_23 + v.z*_33;
	}
	IC	SelfRef	lerp(const Self &p1, const Self &p2, T t )
	{
		T invt = 1.f-t;
		x = p1.x*invt + p2.x*t;
		y = p1.y*invt + p2.y*t;
		z = p1.z*invt + p2.z*t;
		return *this;	
	}

*/


#define LINE1(base) XMMWORD PTR [base+M11]
#define LINE2(base) XMMWORD PTR [base+M21]
#define LINE3(base) XMMWORD PTR [base+M31]
#define LINE4(base) XMMWORD PTR [base+M41]

void __stdcall xrSkin2W_SSE(vertRender*		D,
							vertBoned2W*	S,
							u32				vCount,
							CBoneInstance*	Bones) 
{__asm{
// ------------------------------------------------------------------
	mov			ecx,vCount		; ecx = vCount
// ------------------------------------------------------------------
//	esi		= source _vector_ [S]
//	edi		= result _vector_ [D]
//	eax		= transform matrix 0 [m0]
//	ebx		= transform matrix 1 [m1]
// ------------------------------------------------------------------
	mov			edi,D			; edi = D
	mov			esi,S			; esi = S
// ------------------------------------------------------------------
	ALIGN		16				;
	new_dot:					; _new cycle iteration
// ------------------------------------------------------------------
// checking whether the matrixes are equal
// ------------------------------------------------------------------
	movzx		eax,WORD PTR [esi]S.matrix0		;	// eax = m0
	movzx		ebx,WORD PTR [esi]S.matrix1		;	// ebx = m1
	cmp			eax,ebx							;
	jz			private_case					;
// ------------------------------------------------------------------
// calculating transformation matrix(es) addresses
// ------------------------------------------------------------------
	mul			eax,TYPE CBoneInstance			;
	add			eax,Bones						;
	mul			ebx,TYPE CBoneInstance			;
	add			ebx,Bones						;
// ------------------------------------------------------------------
// preparing data for lerps
// ------------------------------------------------------------------
	movss		xmm5,DWORD PTR [esi]S.w			; xmm5 = ?.? | ?.? | ?.? | w
	shufps		xmm5,xmm5,00000000b				; xmm5 = w | w | w | w
// ------------------------------------------------------------------
// transform tiny m 0
// ------------------------------------------------------------------
	movups		xmm0,XMMWORD PTR [esi]S.P0		; xmm0 = ?.? | v.z | v.y | v.x
	
	movaps		xmm1,xmm0						; xmm1 = ?.? | v.z | v.y | v.x
	movaps		xmm2,xmm0						; xmm2 = ?.? | v.z | v.y | v.x

	shufps		xmm1,xmm1,00000000b				; xmm1 = v.x | v.x | v.x | v.x
	shufps		xmm2,xmm2,01010101b				; xmm2 = v.y | v.y | v.y | v.y
	
	mulps		xmm1,XMMWORD PTR [eax]			; xmm1 = v.x*_14 | v.x*_13 | v.x*_12 | v.x*_11
	mulps		xmm2,LINE2(eax)					; xmm2 = v.y*_24 | v.y*_23 | v.y*_22 | v.y*_21
	
	addps		xmm1,xmm2						; xmm1 = v.x*_14+v.y*_24 | v.x*_13+v.y*_23 |
												;		 v.x*_12+v.y*_22 | v.x*_11+v.y*_21
	movaps		xmm2,xmm0						; xmm2 = ?.? | v.z | v.y | v.x
	shufps		xmm2,xmm2,10101010b				; xmm2 = v.z | v.z | v.z | v.z
	mulps		xmm2,LINE3(eax)					; xmm2 = v.z*_34 | v.z*_33 | v.z*_32 | v.z*_31

	addps		xmm2,LINE4(eax)					; xmm2 = v.z*_34+_44 | v.z*_33+_43 | 
												;		 v.z*_32+_42 | v.z*_31+_41
	addps		xmm2,xmm1						; xmm2 = v.x*_14+v.y*_24+v.z*_34+_44 | v.x*_13+v.y*_23+v.z*_33+_43 |
												;		 v.x*_12+v.y*_22+v.z*_32+_42 | v.x*_11+v.y*_21+v.z*_31+_41
// ------------------------------------------------------------------
// => xmm2			: transform tiny result	0	: ?.? | p0.z | p0.y | p0.x
// ------------------------------------------------------------------
// transform tiny m 1
// ------------------------------------------------------------------
	movups		xmm0,XMMWORD PTR [esi]S.P1		; xmm0 = ?.? | v.z | v.y | v.x
	
	movaps		xmm1,xmm0						; xmm1 = ?.? | v.z | v.y | v.x
	movaps		xmm3,xmm0						; xmm3 = ?.? | v.z | v.y | v.x

	shufps		xmm1,xmm1,00000000b				; xmm1 = v.x | v.x | v.x | v.x
	shufps		xmm3,xmm3,01010101b				; xmm3 = v.y | v.y | v.y | v.y
	
	mulps		xmm1,LINE1(ebx)					; xmm1 = v.x*_14 | v.x*_13 | v.x*_12 | v.x*_11
	mulps		xmm3,LINE2(ebx)					; xmm3 = v.y*_24 | v.y*_23 | v.y*_22 | v.y*_21
	
	addps		xmm1,xmm3						; xmm1 = v.x*_14+v.y*_24 | v.x*_13+v.y*_23 |
												;		 v.x*_12+v.y*_22 | v.x*_11+v.y*_21
	movaps		xmm3,xmm0						; xmm3 = ?.? | v.z | v.y | v.x
	shufps		xmm3,xmm3,10101010b				; xmm3 = v.z | v.z | v.z | v.z
	mulps		xmm3,LINE3(ebx)					; xmm3 = v.z*_34 | v.z*_33 | v.z*_32 | v.z*_31

	addps		xmm3,LINE4(ebx)					; xmm3 = v.z*_34+_44 | v.z*_33+_43 | 
												;		 v.z*_32+_42 | v.z*_31+_41
	addps		xmm3,xmm1						; xmm3 = v.x*_14+v.y*_24+v.z*_34+_44 | v.x*_13+v.y*_23+v.z*_33+_43 |
												;		 v.x*_12+v.y*_22+v.z*_32+_42 | v.x*_11+v.y*_21+v.z*_31+_41
// ------------------------------------------------------------------
// => xmm3			: transform tiny result	1	: ?.? | p1.z | p1.y | p1.x
// ------------------------------------------------------------------
// lerp P0 & P1
// ------------------------------------------------------------------
	subps		xmm3,xmm2			; xmm3 = ?.? | p1.z-p0.z | p1.y-p1.y | p1.x-p0.x
	mulps		xmm3,xmm5			; xmm3 = ?.? | (p1.z-p0.z)*w | (p1.y-p0.y)*w | (p1.x-p0.x)*w
	addps		xmm3,xmm2			; xmm3 = ?.? | (p1.z-p0.z)*w+p0.z | (p1.y-p0.y)*w+p0.y | (p1.x-p0.x)*w+p0.x
// ------------------------------------------------------------------
// => xmm3			: lerp(p0,p1) result	: ?.? | lerp(z) | lerp(y) | lerp(x)
// ------------------------------------------------------------------
	movups		XMMWORD PTR [edi]D.P,xmm3		; !
// ------------------------------------------------------------------
// transform dir m 0
// ------------------------------------------------------------------	
	movups		xmm0,XMMWORD PTR [esi]S.N0		; xmm0 = ?.? | v.z | v.y | v.x
	
	movaps		xmm1,xmm0						; xmm1 = ?.? | v.z | v.y | v.x
	movaps		xmm2,xmm0						; xmm2 = ?.? | v.z | v.y | v.x

	shufps		xmm1,xmm1,00000000b				; xmm1 = v.x | v.x | v.x | v.x
	shufps		xmm2,xmm2,01010101b				; xmm2 = v.y | v.y | v.y | v.y

	mulps		xmm1,LINE1(eax)					; xmm1 = v.x*_14 | v.x*_13 | v.x*_12 | v.x*_11
	mulps		xmm2,LINE2(eax)					; xmm2 = v.y*_24 | v.y*_23 | v.y*_22 | v.y*_21
	
	addps		xmm1,xmm2						; xmm1 = v.x*_14+v.y*_24 | v.x*_13+v.y*_23 |
												;		 v.x*_12+v.y*_22 | v.x*_11+v.y*_21
	movaps		xmm2,xmm0						; xmm2 = ?.? | v.z | v.y | v.x
	shufps		xmm2,xmm2,10101010b				; xmm2 = v.z | v.z | v.z | v.z
	mulps		xmm2,LINE3(eax)					; xmm2 = v.z*_34 | v.z*_33 | v.z*_32 | v.z*_31

	addps		xmm2,xmm1						; xmm2 = v.x*_14+v.y*_24+v.z*_34 | v.x*_13+v.y*_23+v.z*_33 |
												;		 v.x*_12+v.y*_22+v.z*_32 | v.x*_11+v.y*_21+v.z*_31
// ------------------------------------------------------------------
// => xmm2			: transform dir result	0	: ?.? | n0.z | n0.y | n0.x
// ------------------------------------------------------------------
// transform dir m 1
// ------------------------------------------------------------------	
	movups		xmm0,XMMWORD PTR [esi]S.N1		; xmm0 = ?.? | v.z | v.y | v.x
	
	movaps		xmm1,xmm0						; xmm1 = ?.? | v.z | v.y | v.x
	movaps		xmm3,xmm0						; xmm3 = ?.? | v.z | v.y | v.x

	shufps		xmm1,xmm1,00000000b				; xmm1 = v.x | v.x | v.x | v.x
	shufps		xmm3,xmm3,01010101b				; xmm3 = v.y | v.y | v.y | v.y

	mulps		xmm1,LINE1(ebx)					; xmm1 = v.x*_14 | v.x*_13 | v.x*_12 | v.x*_11
	mulps		xmm3,LINE2(ebx)					; xmm3 = v.y*_24 | v.y*_23 | v.y*_22 | v.y*_21
	
	addps		xmm1,xmm2						; xmm1 = v.x*_14+v.y*_24 | v.x*_13+v.y*_23 |
												;		 v.x*_12+v.y*_22 | v.x*_11+v.y*_21
	movaps		xmm3,xmm0						; xmm3 = ?.? | v.z | v.y | v.x
	shufps		xmm3,xmm3,10101010b				; xmm3 = v.z | v.z | v.z | v.z
	mulps		xmm3,LINE3(ebx)					; xmm3 = v.z*_34 | v.z*_33 | v.z*_32 | v.z*_31

	addps		xmm3,xmm1						; xmm3 = v.x*_14+v.y*_24+v.z*_34 | v.x*_13+v.y*_23+v.z*_33 |
												;		 v.x*_12+v.y*_22+v.z*_32 | v.x*_11+v.y*_21+v.z*_31
// ------------------------------------------------------------------
// => xmm3			: transform dir result	0	: ?.? | n1.z | n1.y | n1.x
// ------------------------------------------------------------------
// lerp N0 & N1
// ------------------------------------------------------------------
	subps		xmm3,xmm2			; xmm3 = ?.? | p1.z-p0.z | p1.y-p1.y | p1.x-p0.x
	mulps		xmm3,xmm5			; xmm3 = ?.? | (p1.z-p0.z)*w | (p1.y-p0.y)*w | (p1.x-p0.x)*w
	addps		xmm3,xmm2			; xmm3 = ?.? | (p1.z-p0.z)*w+p0.z | (p1.y-p0.y)*w+p0.y | (p1.x-p0.x)*w+p0.x
// ------------------------------------------------------------------
// => xmm3			: lerp(p0,p1) result	: ?.? | lerp(z) | lerp(y) | lerp(x)
// ------------------------------------------------------------------
	movups		XMMWORD PTR [edi]D.N,xmm3		; !
// ------------------------------------------------------------------
	mov			eax,DWORD PTR [esi]S.u			;	// u
	mov			ebx,DWORD PTR [esi]S.v			;	// v
	mov			DWORD PTR [edi]D.u,eax			;
	mov			DWORD PTR [edi]D.v,ebx			;
	add			esi,TYPE vertBoned2W			;	// advance source
	add			edi,TYPE vertRender				;	// advance dest
	dec			ecx								;	// ecx = ecx - 1
	jnz			new_dot							;	// ecx==0 ? exit : goto new_dot
	jmp short	exit							;

// ------------------------------------------------------------------
	ALIGN		16								;
	private_case:								;
// ------------------------------------------------------------------
// calculating transformation matrix 0 addresses
// ------------------------------------------------------------------
	mul			eax,TYPE CBoneInstance			;
	add			eax,Bones						;
// ------------------------------------------------------------------
// transform tiny m 0
// ------------------------------------------------------------------
	movups		xmm0,XMMWORD PTR [esi]S.P0		; xmm0 = ?.? | v.z | v.y | v.x
	
	movaps		xmm1,xmm0						; xmm1 = ?.? | v.z | v.y | v.x
	movaps		xmm2,xmm0						; xmm2 = ?.? | v.z | v.y | v.x

	shufps		xmm1,xmm1,00000000b				; xmm1 = v.x | v.x | v.x | v.x
	shufps		xmm2,xmm2,01010101b				; xmm2 = v.y | v.y | v.y | v.y
	
	mulps		xmm1,XMMWORD PTR [eax]			; xmm1 = v.x*_14 | v.x*_13 | v.x*_12 | v.x*_11
	mulps		xmm2,LINE2(eax)					; xmm2 = v.y*_24 | v.y*_23 | v.y*_22 | v.y*_21
	
	addps		xmm1,xmm2						; xmm1 = v.x*_14+v.y*_24 | v.x*_13+v.y*_23 |
												;		 v.x*_12+v.y*_22 | v.x*_11+v.y*_21
	movaps		xmm2,xmm0						; xmm2 = ?.? | v.z | v.y | v.x
	shufps		xmm2,xmm2,10101010b				; xmm2 = v.z | v.z | v.z | v.z
	mulps		xmm2,LINE3(eax)					; xmm2 = v.z*_34 | v.z*_33 | v.z*_32 | v.z*_31

	addps		xmm2,LINE4(eax)					; xmm2 = v.z*_34+_44 | v.z*_33+_43 | 
												;		 v.z*_32+_42 | v.z*_31+_41
	addps		xmm2,xmm1						; xmm2 = v.x*_14+v.y*_24+v.z*_34+_44 | v.x*_13+v.y*_23+v.z*_33+_43 |
												;		 v.x*_12+v.y*_22+v.z*_32+_42 | v.x*_11+v.y*_21+v.z*_31+_41
// ------------------------------------------------------------------
// => xmm2			: transform tiny result	0	: ?.? | p0.z | p0.y | p0.x
// ------------------------------------------------------------------
	movups		XMMWORD PTR [edi]D.P,xmm2		; !
// ------------------------------------------------------------------
// transform dir m 0
// ------------------------------------------------------------------	
	movups		xmm0,XMMWORD PTR [esi]S.N0		; xmm0 = ?.? | v.z | v.y | v.x
	
	movaps		xmm1,xmm0						; xmm1 = ?.? | v.z | v.y | v.x
	movaps		xmm3,xmm0						; xmm3 = ?.? | v.z | v.y | v.x

	shufps		xmm1,xmm1,00000000b				; xmm1 = v.x | v.x | v.x | v.x
	shufps		xmm3,xmm3,01010101b				; xmm3 = v.y | v.y | v.y | v.y

	mulps		xmm1,LINE1(eax)					; xmm1 = v.x*_14 | v.x*_13 | v.x*_12 | v.x*_11
	mulps		xmm3,LINE2(eax)					; xmm3 = v.y*_24 | v.y*_23 | v.y*_22 | v.y*_21
	
	addps		xmm1,xmm3						; xmm1 = v.x*_14+v.y*_24 | v.x*_13+v.y*_23 |
												;		 v.x*_12+v.y*_22 | v.x*_11+v.y*_21
	movaps		xmm3,xmm0						; xmm3 = ?.? | v.z | v.y | v.x
	shufps		xmm3,xmm3,10101010b				; xmm3 = v.z | v.z | v.z | v.z
	mulps		xmm3,LINE3(eax)					; xmm3 = v.z*_34 | v.z*_33 | v.z*_32 | v.z*_31

	addps		xmm3,xmm1						; xmm3 = v.x*_14+v.y*_24+v.z*_34 | v.x*_13+v.y*_23+v.z*_33 |
												;		 v.x*_12+v.y*_22+v.z*_32 | v.x*_11+v.y*_21+v.z*_31
// ------------------------------------------------------------------
// => xmm3			: transform dir result	0	: ?.? | n0.z | n0.y | n0.x
// ------------------------------------------------------------------
	movups		XMMWORD PTR [edi]D.N,xmm3		; !	
// ------------------------------------------------------------------
	mov			eax,DWORD PTR [esi]S.u			;	// u
	mov			ebx,DWORD PTR [esi]S.v			;	// v
	mov			DWORD PTR [edi]D.u,eax			;
	mov			DWORD PTR [edi]D.v,ebx			;
	add			esi,TYPE vertBoned2W			;	// advance source
	add			edi,TYPE vertRender				;	// advance dest
	dec			ecx								;	// ecx = ecx - 1
	jnz			new_dot							;	// ecx==0 ? exit : goto new_dot
// ------------------------------------------------------------------
	exit:
}}
