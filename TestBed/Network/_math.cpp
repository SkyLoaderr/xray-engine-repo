#include "stdafx.h"
#pragma hdrstop

#include <process.h>

// mmsystem.h
#define MMNOSOUND
#define MMNOMIDI
#define MMNOAUX
#define MMNOMIXER
#define MMNOJOY
#include <mmsystem.h>

// Initialized on startup
XRCORE_API	Fmatrix			Fidentity;
XRCORE_API	Dmatrix			Didentity;
XRCORE_API	CRandom			Random;

#ifdef _M_AMD64
u16			getFPUsw()		{ return 0;	}

namespace FPU 
{
	XRCORE_API void 	m24		(void)	{
		_control87	( _PC_24,   MCW_PC );
		_control87	( _RC_CHOP, MCW_RC );
	}
	XRCORE_API void 	m24r	(void)	{
		_control87	( _PC_24,   MCW_PC );
		_control87	( _RC_NEAR, MCW_RC );
	}
	XRCORE_API void 	m53		(void)	{
		_control87	( _PC_53,   MCW_PC );
		_control87	( _RC_CHOP, MCW_RC );
	}
	XRCORE_API void 	m53r	(void)	{
		_control87	( _PC_53,   MCW_PC );
		_control87	( _RC_NEAR, MCW_RC );
	}
	XRCORE_API void 	m64		(void)	{
		_control87	( _PC_64,   MCW_PC );
		_control87	( _RC_CHOP, MCW_RC );
	}
	XRCORE_API void 	m64r	(void)	{
		_control87	( _PC_64,   MCW_PC );
		_control87	( _RC_NEAR, MCW_RC );
	}

	void		initialize		()				{}
};
#else
u16 getFPUsw() 
{
	u16		SW;
	__asm	fstcw SW;
	return	SW;
}

namespace FPU 
{
	u16			_24	=0;
	u16			_24r=0;
	u16			_53	=0;
	u16			_53r=0;
	u16			_64	=0;
	u16			_64r=0;

	XRCORE_API void 	m24		()	{
		u16		p	= _24;
		__asm fldcw p;	
	}
	XRCORE_API void 	m24r	()	{
		u16		p	= _24r;
		__asm fldcw p;  
	}
	XRCORE_API void 	m53		()	{
		u16		p	= _53;
		__asm fldcw p;	
	}
	XRCORE_API void 	m53r	()	{
		u16		p	= _53r;
		__asm fldcw p;	
	}
	XRCORE_API void 	m64		()	{ 
		u16		p	= _64;
		__asm fldcw p;	
	}
	XRCORE_API void 	m64r	()	{
		u16		p	= _64r;
		__asm fldcw p;  
	}

	void		initialize		()
	{
		_clear87	();

		_control87	( _PC_24,   MCW_PC );
		_control87	( _RC_CHOP, MCW_RC );
		_24			= getFPUsw();	// 24, chop
		_control87	( _RC_NEAR, MCW_RC );
		_24r		= getFPUsw();	// 24, rounding

		_control87	( _PC_53,   MCW_PC );
		_control87	( _RC_CHOP, MCW_RC );
		_53			= getFPUsw();	// 53, chop
		_control87	( _RC_NEAR, MCW_RC );
		_53r		= getFPUsw();	// 53, rounding

		_control87	( _PC_64,   MCW_PC );
		_control87	( _RC_CHOP, MCW_RC );
		_64			= getFPUsw();	// 64, chop
		_control87	( _RC_NEAR, MCW_RC );
		_64r		= getFPUsw();	// 64, rounding

		m24r		();
	}
};
#endif

namespace CPU 
{
	XRCORE_API u64				cycles_per_second;
	XRCORE_API u64				cycles_per_milisec;
	XRCORE_API u64				cycles_per_microsec;
	XRCORE_API u64				cycles_overhead;
	XRCORE_API float			cycles2seconds;
	XRCORE_API float			cycles2milisec;
	XRCORE_API float			cycles2microsec;
	XRCORE_API _processor_info	ID;

#ifdef M_BORLAND
	u64	__fastcall GetCycleCount(void)
	{
		_asm    db 0x0F;
		_asm    db 0x31;
	}
#endif

	void Detect()
	{
		// General CPU identification
		if (!_cpuid	(&ID))	
		{
			// Core.Fatal		("Fatal error: can't detect CPU/FPU.");
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
		cycles_overhead		/=	64;
		SetPriorityClass	(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);

		cycles_per_second	-=	cycles_overhead;
		cycles_per_milisec	=	cycles_per_second/1000;
		cycles_per_microsec	=	cycles_per_milisec/1000;

		_control87	( _PC_64,   MCW_PC );
		_control87	( _RC_CHOP, MCW_RC );
		double a,b;
		a = 1;		b = double(cycles_per_second);
		cycles2seconds = float(double(a/b));
		a = 1000;	b = double(cycles_per_second);
		cycles2milisec = float(double(a/b));
		a = 1000000;b = double(cycles_per_second);
		cycles2microsec = float(double(a/b));
	}
};

//------------------------------------------------------------------------------------
void _initialize_cpu	(void) 
{
	Msg("* Detected CPU: %s %s, F%d/M%d/S%d, %.2f mhz, %d-clk 'rdtsc'",
		CPU::ID.v_name,CPU::ID.model_name,
		CPU::ID.family,CPU::ID.model,CPU::ID.stepping,
		float(CPU::cycles_per_second/u64(1000000)),
		u32(CPU::cycles_overhead)
		);
	string128	features;	strcpy(features,"RDTSC");
    if (CPU::ID.feature&_CPU_FEATURE_MMX)	strcat(features,", MMX");
    if (CPU::ID.feature&_CPU_FEATURE_3DNOW)	strcat(features,", 3DNow!");
    if (CPU::ID.feature&_CPU_FEATURE_SSE)	strcat(features,", SSE");
    if (CPU::ID.feature&_CPU_FEATURE_SSE2)	strcat(features,", SSE2");
	Msg("* CPU Features: %s\n",features);

	Fidentity.identity		();	// Identity matrix
	Didentity.identity		();	// Identity matrix
	pvInitializeStatics		();	// Lookup table for compressed normals
	FPU::initialize			();
	_initialize_cpu_thread	();
}

// per-thread initialization
#include <xmmintrin.h>
#define _MM_DENORMALS_ZERO_MASK 0x0040
#define _MM_DENORMALS_ZERO_ON 0x0040
#define _MM_FLUSH_ZERO_MASK 0x8000
#define _MM_FLUSH_ZERO_ON 0x8000
#define _MM_SET_FLUSH_ZERO_MODE(mode) _mm_setcsr((_mm_getcsr() & ~_MM_FLUSH_ZERO_MASK) | (mode))
#define _MM_SET_DENORMALS_ZERO_MODE(mode) _mm_setcsr((_mm_getcsr() & ~_MM_DENORMALS_ZERO_MASK) | (mode))
void _initialize_cpu_thread	()
{
	// fpu & sse
	FPU::m24r	();
	if (CPU::ID.feature&_CPU_FEATURE_SSE)	{
		//_mm_setcsr ( _mm_getcsr() | (_MM_FLUSH_ZERO_ON+_MM_DENORMALS_ZERO_ON) );
		_MM_SET_FLUSH_ZERO_MODE			(_MM_FLUSH_ZERO_ON);
		__try
		{
			_MM_SET_DENORMALS_ZERO_MODE	(_MM_DENORMALS_ZERO_ON);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}

// threading API 
#pragma pack(push,8)
struct THREAD_NAME	{
	DWORD	dwType;
	LPCSTR	szName;
	DWORD	dwThreadID;
	DWORD	dwFlags;
};
void	thread_name	(const char* name)
{
	THREAD_NAME		tn;
	tn.dwType		= 0x1000;
	tn.szName		= name;
	tn.dwThreadID	= DWORD(-1);
	tn.dwFlags		= 0;
	__try
	{
		RaiseException(0x406D1388,0,sizeof(tn)/sizeof(DWORD),(DWORD*)&tn);
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}
#pragma pack(pop)

struct	THREAD_STARTUP
{
	thread_t*	entry	;
	char*		name	;
	void*		args	;
};
void	__cdecl			thread_entry	(void*	_params )	{
	// initialize
	THREAD_STARTUP*		startup	= (THREAD_STARTUP*)_params	;
	thread_name			(startup->name);
	thread_t*			entry	= startup->entry;
	void*				arglist	= startup->args;
	xr_delete			(startup);
	_initialize_cpu_thread		();

	// call
	entry				(arglist);
}

void	thread_spawn	(thread_t*	entry, const char*	name, unsigned	stack, void* arglist )
{
	THREAD_STARTUP*		startup	= xr_new<THREAD_STARTUP>	();
	startup->entry		= entry;
	startup->name		= (char*)name;
	startup->args		= arglist;
	_beginthread		(thread_entry,stack,startup);
}

void spline1	( float t, Fvector *p, Fvector *ret )
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
