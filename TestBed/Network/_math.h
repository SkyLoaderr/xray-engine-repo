#ifndef __XR_MATH_H__
#define __XR_MATH_H__

#include "cpuid.h"

#ifdef M_BORLAND
#define NAKED	__declspec(naked)
#define BCALL	__fastcall
#endif

namespace FPU {
	ENGINE_API extern	u16		_24;
	ENGINE_API extern	u16		_24r;
	ENGINE_API extern	u16		_53;
	ENGINE_API extern 	u16		_53r;
	ENGINE_API extern 	u16		_64;
	ENGINE_API extern 	u16		_64r;
	
#ifdef M_VISUAL
	IC void m24	(void)	{	__asm fldcw _24  };
	IC void m24r(void)	{	__asm fldcw _24r };
	IC void m53	(void)	{ 	__asm fldcw _53  };
	IC void m53r(void)	{ 	__asm fldcw _53r };
	IC void m64	(void)	{ 	__asm fldcw _64  };
	IC void m64r(void)	{ 	__asm fldcw _64r };
#endif
#ifdef M_BORLAND
	void	BCALL	m24		(u16 p=_24);
	void	BCALL	m24r	(u16 p=_24r);	
	void	BCALL	m53		(u16 p=_53);	
	void	BCALL	m53r	(u16 p=_53r);	
	void	BCALL	m64		(u16 p=_64);	
	void	BCALL	m64r	(u16 p=_64r);	
#endif
};
namespace CPU {
	ENGINE_API extern u64				cycles_per_second;
	ENGINE_API extern u64				cycles_overhead;
	ENGINE_API extern float				cycles2seconds;
	ENGINE_API extern float				cycles2milisec;
	ENGINE_API extern _processor_info	ID;

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
	u64	BCALL	GetCycleCount(void);
#endif
};

extern void InitMath(void);

#endif //__XR_MATH_H__
