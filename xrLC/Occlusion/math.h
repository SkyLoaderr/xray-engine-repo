#ifndef __XR_MATH_H__
#define __XR_MATH_H__

#define VCPP_BUG
#define M_PI	3.1415926535897932384626433832795f

// float values defines
#define fdSGN	0x080000000		// mask for sign bit
#define fdMABS  0x07FFFFFFF		// mask for absolute value (~sgn)
#define fdMANT  0x0007FFFFF		// mask for mantissa
#define fdEXPO	0x07F800000		// mask for exponent
#define fdONE	0x03F800000     // 1.0f
#define fdHALF	0x03F000000     // 0.5f
#define fdTWO	0x040000000     // 2.0
#define fdOOB	0x000000000     // "out of bounds" value
#define fdNAN	0x07fffffff     // "Not a number" value
#define fdMAX	0x07F7FFFFF     // FLT_MAX
#define fdRLE10	0x03ede5bdb     // 1/ln10

#define fpuSetNoRound()	__asm fldcw FPUswNoRound
#define fpuRestore()	__asm fldcw FPUsw

extern void InitMath(void);

extern ENGINE_API	_matrix		precalc_identity;
extern ENGINE_API	WORD		FPUswNoRound;
extern ENGINE_API	WORD		FPUsw;

//------------------------------------------------------------------------------------
// Type conversions  *****************************************************************
//------------------------------------------------------------------------------------
int __forceinline iROUND(float a) {
int	res;

	__asm {
		fld			a
		fistp       res
	}
	return res;
}

int __forceinline iNOROUND(float a) {
int	res;

	_asm {
		fld			a;
		fldcw		FPUswNoRound;
		fistp       res;
		fldcw		FPUsw;
	}
	if (res & 0x80000000) res--;
	return res;
}
//------------------------------------------------------------------------------------
void spline1( float t, _vector *p, _vector *ret );
void spline2( float t, _vector *p, _vector *ret );
void spline3( float t, _vector *p, _vector *ret );
//------------------------------------------------------------------------------------


#endif //__XR_MATH_H__
