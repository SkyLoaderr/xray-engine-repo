#ifndef __XR_MATH_H__
#define __XR_MATH_H__

#include "cpuid.h"

namespace FPU {
	XRCORE_API void	__stdcall	m24		(u16 p=_24);
	XRCORE_API void	__stdcall	m24r	(u16 p=_24r);	
	XRCORE_API void	__stdcall	m53		(u16 p=_53);	
	XRCORE_API void	__stdcall	m53r	(u16 p=_53r);	
	XRCORE_API void	__stdcall	m64		(u16 p=_64);	
	XRCORE_API void	__stdcall	m64r	(u16 p=_64r);	
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
	#pragma warning(disable:4035)
	IC u64	GetCycleCount(void)
	{
		_asm    _emit 0x0F;
		_asm    _emit 0x31;
	}
	#pragma warning(default:4035)
#endif
#ifdef M_BORLAND
	XRCORE_API u64 __fastcall	GetCycleCount		(void);
#endif
};

extern void InitMath(void);

#endif //__XR_MATH_H__
