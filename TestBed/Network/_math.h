#ifndef __XR_MATH_H__
#define __XR_MATH_H__

#include "cpuid.h"

namespace FPU {
	XRCORE_API void	__stdcall m24	(void);
	XRCORE_API void	__stdcall m24r	(void);	
	XRCORE_API void	__stdcall m53	(void);	
	XRCORE_API void	__stdcall m53r	(void);	
	XRCORE_API void	__stdcall m64	(void);	
	XRCORE_API void	__stdcall m64r	(void);	
};
namespace CPU {
	XRCORE_API extern u64				cycles_per_second;
	XRCORE_API extern u64				cycles_per_milisec;
	XRCORE_API extern u64				cycles_per_microsec;
	XRCORE_API extern u64				cycles_overhead;
	XRCORE_API extern float				cycles2seconds;
	XRCORE_API extern float				cycles2milisec;
	XRCORE_API extern float				cycles2microsec;
	XRCORE_API extern _processor_info	ID;

#ifdef M_VISUAL
	#ifndef _M_AMD64
		#pragma warning(disable:4035)
		IC u64	GetCycleCount(void)	{
			_asm    _emit 0x0F;
			_asm    _emit 0x31;
		}
		#pragma warning(default:4035)
	#else
		IC u64	GetCycleCount(void)	{
			return __rdtsc();
		}
	#endif
#endif

#ifdef M_BORLAND
	XRCORE_API u64 __fastcall	GetCycleCount		(void);
#endif
};

extern XRCORE_API	void	_initialize_cpu			();
extern XRCORE_API	void	_initialize_cpu_thread	();

// threading
typedef				void	thread_t				( void * );
extern XRCORE_API	void	thread_name				( const char* name);
extern XRCORE_API	void	thread_spawn			(
	thread_t*	entry,
	const char*	name,
	unsigned	stack,
	void*		arglist 
	);

#endif //__XR_MATH_H__
