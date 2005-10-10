#include "stdafx.h"
#pragma hdrstop

// Initialized on startup
ENGINE_API Fmatrix			precalc_identity;
ENGINE_API CRandom			Random;

WORD getFPUsw() 
{
	WORD SW;
	__asm fstcw SW;
	return SW;
}

namespace FPU 
{
	ENGINE_API u16		_24	=0;
	ENGINE_API u16		_24r=0;
	ENGINE_API u16		_53	=0;
	ENGINE_API u16		_53r=0;
	ENGINE_API u16		_64	=0;
	ENGINE_API u16		_64r=0;

#ifdef M_BORLAND
	void BCALL	m24		(void)	{	__asm fldcw _24;	__asm ret; }
	void BCALL	m24r	(void)	{	__asm fldcw _24r;	__asm ret; }
	void BCALL	m53		(void)	{ 	__asm fldcw _53;	__asm ret; }
	void BCALL	m53r	(void)	{ 	__asm fldcw _53r;	__asm ret; }
	void BCALL	m64		(void)	{ 	__asm fldcw _64;	__asm ret; }
	void BCALL	m64r	(void)	{ 	__asm fldcw _64r;	__asm ret; }
#endif
};

namespace CPU 
{
	ENGINE_API u64				cycles_per_second;
	ENGINE_API u64				cycles_overhead;
	ENGINE_API float			cycles2seconds;
	ENGINE_API float			cycles2milisec;
	ENGINE_API _processor_info	ID;

#ifdef M_BORLAND
	u64	BCALL GetCycleCount(void)
	{
		_asm    _emit 0x0F;
		_asm    _emit 0x31;
	}
#endif

	void Detect()
	{
		// General CPU identification
		if (!_cpuid	(&ID))	
		{
			Log					("Fatal error: can't detect CPU/FPU.");
			abort				();
		}

		// Timers & frequency
		u64			start,end;
		u32			dwStart,dwTest;

		SetPriorityClass	(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);

		// Detect Freq
		dwTest	= timeGetTime();
		do { dwStart = timeGetTime(); } while (dwTest==dwStart);
		start	= GetCycleCount();
		while (timeGetTime()-dwStart<1000) ;
		end		= GetCycleCount();
		cycles_per_second = end-start;

		// Detect Overhead
		cycles_overhead = 0;
		u64 dummy		= 0;
		for (int i=0; i<64; i++)
		{
			start			=	GetCycleCount();
			cycles_overhead	+=	GetCycleCount()-start-dummy;
		}
		cycles_overhead	/= 64;

		cycles_per_second -= cycles_overhead;
		SetPriorityClass	(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);

		_control87	( _PC_64,   MCW_PC );
		_control87	( _RC_CHOP, MCW_RC );
		double a,b;
		a = 1;		b = double(cycles_per_second);
		cycles2seconds = float(double(a/b));
		a = 1000;	b = double(cycles_per_second);
		cycles2milisec = float(double(a/b));
	}
};

//------------------------------------------------------------------------------------
void InitMath(void) 
{
	char features[128]="RDTSC";

	Log("Initializing geometry pipeline and mathematic routines...");
	CPU::Detect();

	Msg("* Detected CPU: %s %s, F%d/M%d/S%d, %d mhz, %d clk overhead",
		CPU::ID.v_name,CPU::ID.model_name,
		CPU::ID.family,CPU::ID.model,CPU::ID.stepping,
		DWORD(CPU::cycles_per_second/__int64(1000000)),
		DWORD(CPU::cycles_overhead)
		);
    if (CPU::ID.feature&_CPU_FEATURE_MMX)	strcat(features,", MMX");
    if (CPU::ID.feature&_CPU_FEATURE_3DNOW)	strcat(features,", 3DNow!");
    if (CPU::ID.feature&_CPU_FEATURE_SSE)	strcat(features,", SSE");
    if (CPU::ID.feature&_CPU_FEATURE_SSE2)	strcat(features,", SSE2");
	Msg("* CPU Features: %s\n",features);

	
	precalc_identity.identity	();	// Identity matrix
	pvInitializeStatics			();	// Lookup table for compressed normals

	_clear87();

	_control87( _PC_24,   MCW_PC );
	_control87( _RC_CHOP, MCW_RC );
	FPU::_24  = getFPUsw();	// 24, chop
	_control87( _RC_NEAR, MCW_RC );
	FPU::_24r = getFPUsw();	// 24, rounding

	_control87( _PC_53,   MCW_PC );
	_control87( _RC_CHOP, MCW_RC );
	FPU::_53  = getFPUsw();	// 53, chop
	_control87( _RC_NEAR, MCW_RC );
	FPU::_53r = getFPUsw();	// 53, rounding

	_control87( _PC_64,   MCW_PC );
	_control87( _RC_CHOP, MCW_RC );
	FPU::_64  = getFPUsw();	// 64, chop
	_control87( _RC_NEAR, MCW_RC );
	FPU::_64r = getFPUsw();	// 64, rounding

	FPU::m24r();
}


void spline1( float t, Fvector *p, Fvector *ret )
{
	float     t2  = t * t;
	float     t3  = t2 * t;
	float     m[4];

	ret->x=0.0f;
	ret->y=0.0f;
	ret->z=0.0f;
	m[0] = ( 0.5f * ( (-1.0f * t3) + ( 2.0f * t2) + (-1.0f * t) ) );
	m[1] = ( 0.5f * ( ( 3.0f * t3) + (-5.0f * t2) + ( 0.0f * t) + 2.0f ) );
	m[2] = ( 0.5f * ( (-3.0f * t3) + ( 4.0f * t2) + ( 1.0f * t) ) );
	m[3] = ( 0.5f * ( ( 1.0f * t3) + (-1.0f * t2) + ( 0.0f * t) ) );

	for( int i=0; i<4; i++ )
	{
		ret->x += p[i].x * m[i];
		ret->y += p[i].y * m[i];
		ret->z += p[i].z * m[i];
	}
}

void spline2( float t, Fvector *p, Fvector *ret )
{
	float	s= 1.0f - t;
	float   t2 = t * t;
	float   t3 = t2 * t;
	float   m[4];

	m[0] = s*s*s;
	m[1] = 3.0f*t3 - 6.0f*t2 + 4.0f;
	m[2] = -3.0f*t3 + 3.0f*t2 + 3.0f*t +1;
	m[3] = t3;

	ret->x = (p[0].x*m[0]+p[1].x*m[1]+p[2].x*m[2]+p[3].x*m[3])/6.0f;
	ret->y = (p[0].y*m[0]+p[1].y*m[1]+p[2].y*m[2]+p[3].y*m[3])/6.0f;
	ret->z = (p[0].z*m[0]+p[1].z*m[1]+p[2].z*m[2]+p[3].z*m[3])/6.0f;
}

#define beta1 1.0f
#define beta2 0.8f

void spline3( float t, Fvector *p, Fvector *ret )
{
	float	s= 1.0f - t;
	float   t2 = t * t;
	float   t3 = t2 * t;
	float	b12=beta1*beta2;
	float	b13=b12*beta1;
	float	delta=2.0f-b13+4.0f*b12+4.0f*beta1+beta2+2.0f;
	float	d=1.0f/delta;
	float	b0=2.0f*b13*d*s*s*s;
	float	b3=2.0f*t3*d;
	float	b1=d*(2*b13*t*(t2-3*t+3)+2*b12*(t3-3*t2+2)+2*beta1*(t3-3*t+2)+beta2*(2*t3-3*t2+1));
	float	b2=d*(2*b12*t2*(-t+3)+2*beta1*t*(-t2+3)+beta2*t2*(-2*t+3)+2*(-t3+1));

	ret->x = p[0].x*b0+p[1].x*b1+p[2].x*b2+p[3].x*b3;
	ret->y = p[0].y*b0+p[1].y*b1+p[2].y*b2+p[3].y*b3;
	ret->z = p[0].z*b0+p[1].z*b1+p[2].z*b2+p[3].z*b3;
}
