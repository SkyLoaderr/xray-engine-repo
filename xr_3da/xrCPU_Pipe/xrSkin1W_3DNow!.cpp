// -- cool defines --------------------------------------------------
#define _MANOWAR_SUPER_PROGRAMMER_
#define _TOP_SECRET_
#define _COOL_
#define _AMD_RULEZ_FOREVER_
#define _MS_BUGS_OFF_
// -- includes ------------------------------------------------------
#include "stdafx.h"
#include "..\skeletonX.h"
#include "..\bodyinstance.h"
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
void __stdcall xrSkin1W_SSE(	vertRender*		D,
								vertBoned1W*	S,
								u32				vCount,
								CBoneInstance*	Bones) 
{__asm
{
// ------------------------------------------------------------------
	mov			ecx,vCount		; ecx = vCount
// ------------------------------------------------------------------
	; esi		= source vector [S]
	; edi		= result vector [D]
	; eax		= transform matrix [m]
// ------------------------------------------------------------------
	mov			edi,D			; edi = D
	mov			esi,S			; esi = S
// ------------------------------------------------------------------
	ALIGN		16				;
	new_dot:					; _new cycle iteration
// ------------------------------------------------------------------
// data prefetching
// ------------------------------------------------------------------
	prefetcht0	[esi+(TYPE vertBoned1W)*3]S.P		;
//	prefetchnta	[edi+(TYPE vertRender)*6]D.P		;
// ------------------------------------------------------------------
// calculating transformation matrix address
// ------------------------------------------------------------------
	mov			eax,TYPE CBoneInstance			;
	mul			DWORD PTR [esi]S.matrix			;
	add			eax,Bones.mTransform			;
// ------------------------------------------------------------------
// loading 128-bit ALIGNED matrix
// ------------------------------------------------------------------
	movaps		xmm4,XMMWORD PTR [eax+M11]		; xmm4 = _14 | _13 | _12 | _11
	movaps		xmm5,XMMWORD PTR [eax+M21]		; xmm5 = _24 | _23 | _22 | _21
	movaps		xmm6,XMMWORD PTR [eax+M31]		; xmm6 = _34 | _33 | _32 | _31
	movaps		xmm7,XMMWORD PTR [eax+M41]		; xmm6 = _44 | _43 | _42 | _41
// ------------------------------------------------------------------
// => xmm4-xmm7		: Transform matrix
// ------------------------------------------------------------------
// transform tiny
// ------------------------------------------------------------------
	movups		xmm0,XMMWORD PTR [esi]S.P		; xmm0 = ?.? | v.z | v.y | v.x
	
	movaps		xmm1,xmm0						; xmm1 = ?.? | v.z | v.y | v.x
	movaps		xmm2,xmm0						; xmm2 = ?.? | v.z | v.y | v.x

	shufps		xmm1,xmm1,00000000b				; xmm1 = v.x | v.x | v.x | v.x
	shufps		xmm2,xmm2,01010101b				; xmm2 = v.y | v.y | v.y | v.y
	
	mulps		xmm1,xmm4						; xmm1 = v.x*_14 | v.x*_13 | v.x*_12 | v.x*_11
	mulps		xmm2,xmm5						; xmm2 = v.y*_24 | v.y*_23 | v.y*_22 | v.y*_21
	
	addps		xmm1,xmm2						; xmm1 = v.x*_14+v.y*_24 | v.x*_13+v.y*_23 |
												;		 v.x*_12+v.y*_22 | v.x*_11+v.y*_21
	movaps		xmm2,xmm0						; xmm2 = ?.? | v.z | v.y | v.x
	shufps		xmm2,xmm2,10101010b				; xmm2 = v.z | v.z | v.z | v.z
	mulps		xmm2,xmm6						; xmm2 = v.z*_34 | v.z*_33 | v.z*_32 | v.z*_31

	addps		xmm2,xmm7						; xmm2 = v.z*_34+_44 | v.z*_33+_43 | 
												;		 v.z*_32+_42 | v.z*_31+_41
	addps		xmm2,xmm1						; xmm2 = v.x*_14+v.y*_24+v.z*_34+_44 | v.x*_13+v.y*_23+v.z*_33+_43 |
												;		 v.x*_12+v.y*_22+v.z*_32+_42 | v.x*_11+v.y*_21+v.z*_31+_41
// ------------------------------------------------------------------
// => xmm2			: transform tiny result		: ?.? | td.z | td.y | td.x
// ------------------------------------------------------------------
// transform dir
// ------------------------------------------------------------------	
	movups		xmm0,XMMWORD PTR [esi]S.N		; xmm0 = ?.? | v.z | v.y | v.x
	
	movaps		xmm1,xmm0						; xmm1 = ?.? | v.z | v.y | v.x
	movaps		xmm3,xmm0						; xmm3 = ?.? | v.z | v.y | v.x

	shufps		xmm1,xmm1,00000000b				; xmm1 = v.x | v.x | v.x | v.x
	shufps		xmm3,xmm3,01010101b				; xmm3 = v.y | v.y | v.y | v.y

	mulps		xmm1,xmm4						; xmm1 = v.x*_14 | v.x*_13 | v.x*_12 | v.x*_11
	mulps		xmm3,xmm5						; xmm3 = v.y*_24 | v.y*_23 | v.y*_22 | v.y*_21
	
	addps		xmm1,xmm3						; xmm1 = v.x*_14+v.y*_24 | v.x*_13+v.y*_23 |
												;		 v.x*_12+v.y*_22 | v.x*_11+v.y*_21
	movaps		xmm3,xmm0						; xmm3 = ?.? | v.z | v.y | v.x
	shufps		xmm3,xmm3,10101010b				; xmm3 = v.z | v.z | v.z | v.z
	mulps		xmm3,xmm6						; xmm3 = v.z*_34 | v.z*_33 | v.z*_32 | v.z*_31

	addps		xmm3,xmm1						; xmm3 = v.x*_14+v.y*_24+v.z*_34 | v.x*_13+v.y*_23+v.z*_33 |
												;		 v.x*_12+v.y*_22+v.z*_32 | v.x*_11+v.y*_21+v.z*_31
// ------------------------------------------------------------------
// => xmm3			: transform dir result		: ?.? | dd.z | dd.y | dd.x
// ------------------------------------------------------------------	
// ------------------------------------------------------------------	
//	movlps		xmm0,[esi]S.u					;
	movntps		XMMWORD PTR [edi]D.P,xmm2		;
	movntps		XMMWORD PTR [edi+4]D.N,xmm3		;
//	movlps		[edi]D.u,xmm0					;
// ------------------------------------------------------------------	
	add			esi,TYPE vertBoned1W	;
	add			edi,TYPE vertRender		;
// ------------------------------------------------------------------
// ------------------------------------------------------------------	
	dec			ecx						; ecx = ecx - 1
	jnz			new_dot					; ecx==0 ? goto new_dot : exit
// ------------------------------------------------------------------
// ------------------------------------------------------------------
}
}
// -- cool defines --------------------------------------------------
#define _MANOWAR_SUPER_PROGRAMMER_
#define _TOP_SECRET_
#define _COOL_
#define _AMD_RULEZ_FOREVER_
#define _MS_BUGS_OFF_
// -- includes ------------------------------------------------------
#include "stdafx.h"
#include "..\skeletonX.h"
#include "..\bodyinstance.h"
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
void __stdcall xrSkin1W_3DNow(	vertRender*		D,
								vertBoned1W*	S,
								u32				vCount,
								CBoneInstance*	Bones) 
{__asm
{
// ------------------------------------------------------------------
	femms						; Clear MMX/3DNow! state
// ------------------------------------------------------------------
	mov			ecx,vCount		; ecx = vCount ~ 1100
// ------------------------------------------------------------------
	; esi		= source vector [S]
	; edi		= result vector [D]
	; eax		= transform matrix [m]
// ------------------------------------------------------------------
	mov			edi,D			; edi = D
	mov			esi,S			; esi = S
// ------------------------------------------------------------------
	ALIGN		16				;
	new_dot:					; _new cycle iteration
// ------------------------------------------------------------------
// data prefetching
// ------------------------------------------------------------------
	prefetchw	[edi+(TYPE vertRender)*5]		;
	prefetch	[esi+(TYPE vertBoned1W)*5]		;
// ------------------------------------------------------------------
// calculating transformation matrix address
// ------------------------------------------------------------------
	mov			eax,TYPE CBoneInstance			;
	mul			DWORD PTR [esi]S.matrix			;
	add			eax,Bones.mTransform			;
// ------------------------------------------------------------------
// transform tiny
// ------------------------------------------------------------------
			movq		mm0,QWORD PTR [esi]S.P	; mm0 = v.y | v.x
			movd		mm1,DWORD PTR [esi+8]S.P; mm1 = v.z | ?.?
			movq		mm2,mm0					; mm2 = v.y | v.x
			movq		mm3,QWORD PTR [eax+M11]	; mm3 = _12 | _11
			punpckldq	mm0,mm0					; mm0 = v.x | v.x
			movq		mm4,QWORD PTR [eax+M21]	; mm4 = _22 | _21
			pfmul		mm3,mm0					; mm3 = _12*v.x | _11*v.x
			punpckhdq	mm2,mm2					; mm2 = v.y | v.y
			pfmul		mm4,mm2					; mm4 = _22*v.y | _21*v.y
			movq		mm5,QWORD PTR [eax+M13]	; mm5 = _14 | _13
			movq		mm7,QWORD PTR [eax+M23]	; mm7 = _24 | _23
			movq		mm6,mm1					; mm6 = ?.? | v.z
			pfmul		mm5,mm0					; mm5 = _14*v.x | _13*v.x
			movq		mm0,QWORD PTR [eax+M31]	; mm0 = _32 | _31
			punpckldq	mm1,mm1					; mm1 = v.z | v.z
			pfmul		mm7,mm2					; mm7 = _24*v.y | _23*v.y
			movq		mm2,QWORD PTR [eax+M33]	; mm2 = _34 | _33
			pfmul		mm0,mm1					; mm0 = _32*v.z | _31*v.z
			pfadd		mm3,mm4					; mm3 = _12*v.x + _22*v.y |
												;		_11*v.x + _21*v.y
			movq		mm4,QWORD PTR [eax+M41]	; mm4 = _42 | _41
			pfmul		mm2,mm1					; mm2 = _34*v.z | _33*v.z
			pfadd		mm5,mm7					; mm5 = _14*v.x + _24*v.y |
												;		_13*v.x + _23*v.y
			movq		mm1,QWORD PTR [eax+M43]	; mm1 = _44 | _43
			pfadd		mm3,mm0					; mm3 = _12*v.x + _22*v.y + _32*v.z |
												;		_11*v.x + _21*v.y + _31*v.z
			pfadd		mm5,mm2					; _14*v.x + _24*v.y + _34*v.z |
												; _13*v.x + _23*v.y + _33*v.z
			pfadd		mm3,mm4					; _12*v.x + _22*v.y + _32*v.z + _42 |
												; _11*v.x + _21*v.y + _31*v.z + _41
			movq		QWORD PTR [edi]D.P,mm3	; dest.y = _12*v.x + _22*v.y + _32*v.z + _42 
												; dest.x = _11*v.x + _21*v.y + _31*v.z + _41
			pfadd		mm5,mm1					; mm5 = _14*v.x + _24*v.y + _34*v.z + _44 |
												;		_13*v.x + _23*v.y + _33*v.z + _43
			movd		DWORD PTR [edi+8]D.P,mm5; dest.z = _13*v.x + _23*v.y + _33*v.z + _43
// ------------------------------------------------------------------
// transform dir
// ------------------------------------------------------------------	
			movq		mm0,QWORD PTR [esi]S.N	; mm0 = v.y | v.x
			movd		mm1,DWORD PTR [esi+8]S.N; mm1 = ?.? | v.z
			movq		mm2,mm0					; mm2 = v.y | v.x
			movq		mm3,QWORD PTR [eax+M11]	; mm3 = _12 | _11
			punpckldq	mm0,mm0					; mm0 = v.x | v.x
			movq		mm4,QWORD PTR [eax+M21]	; mm4 = _22 | _21
			pfmul		mm3,mm0					; mm3 = _12*v.x | _11*v.x
			punpckhdq	mm2,mm2					; mm2 = v.y | v.y
			pfmul		mm4,mm2					; mm4 = _22*v.y | _21*v.y
			movq		mm5,QWORD PTR [eax+M13]	; mm5 = _14 | _13
			movq		mm7,QWORD PTR [eax+M23]	; mm7 = _24 | _23
			movq		mm6,mm1					; mm6 = ?.? | v.z
			pfmul		mm5,mm0					; mm5 = _14*v.x | _13*v.x
			movq		mm0,QWORD PTR [eax+M31]	; mm0 = _32 | _31
			punpckldq	mm1,mm1					; mm1 = v.z | v.z
			pfmul		mm7,mm2					; mm7 = _24*v.y | _23*v.y
			movq		mm2,QWORD PTR [eax+M33]	; mm2 = _34 | _33
			pfmul		mm0,mm1					; mm0 = _32*v.z | _31*v.z
			pfadd		mm3,mm4					; mm3 = _12*v.x + _22*v.y |
												;		_11*v.x + _21*v.y
			pfmul		mm2,mm1					; mm2 = _34*v.z | _33*v.z
			pfadd		mm5,mm7					; mm5 = _14*v.x + _24*v.y |
												;		_13*v.x + _23*v.y
			pfadd		mm3,mm0					; mm3 = _12*v.x + _22*v.y + _32*v.z |
												;		_11*v.x + _21*v.y + _31*v.z
			movq		QWORD PTR [edi]D.N,mm3	; dest.y = _12*v.x + _22*v.y + _32*v.z
												; dest.x = _11*v.x + _21*v.y + _31*v.z
			pfadd		mm5,mm2					; _14*v.x + _24*v.y + _34*v.z |
												; _13*v.x + _23*v.y + _33*v.z
			movd		DWORD PTR [edi+8]D.N,mm5; dest.z = _13*v.x + _23*v.y + _33*v.z
// ------------------------------------------------------------------
// ------------------------------------------------------------------	
	movq		mm0,[esi]S.u			;
	movq		[edi]D.u,mm0			;
// ------------------------------------------------------------------	
	add			esi,TYPE vertBoned1W	;
	add			edi,TYPE vertRender		;
// ------------------------------------------------------------------
// ------------------------------------------------------------------	
	dec			ecx						; ecx = ecx - 1
	jnz			new_dot					; ecx==0 ? goto new_dot : exit
// ------------------------------------------------------------------
	femms								; Clear MMX/3DNow! state
// ------------------------------------------------------------------
}}
