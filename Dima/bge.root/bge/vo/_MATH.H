//////////////////////////////////////////////////////////
//	Module			:	Math routines					//
//	File			:	_math.h							//
//	Creation date	:	31.08.2002						//
//	Author			:	Oles Shishkovtsov				//
//	Modifications	:	Dmitriy Iassenev				//
//////////////////////////////////////////////////////////

#pragma once

#include "cpuid.h"

namespace FPU {
	extern u16	_24;
	extern u16	_24r;
	extern u16	_53;
	extern u16	_53r;
	extern u16	_64;
	extern u16	_64r;
	
	IC void m24	()	{	__asm fldcw _24  };
	IC void m24r()	{	__asm fldcw _24r };
	IC void m53	()	{ 	__asm fldcw _53  };
	IC void m53r()	{ 	__asm fldcw _53r };
	IC void m64	()	{ 	__asm fldcw _64  };
	IC void m64r()	{ 	__asm fldcw _64r };
};

namespace CPU {
	
	extern  u64				qwCyclesPerSecond;
	extern  u64				qwCyclesPerRDTSC;
	extern  f32				fSecondsInCycles;
	extern  f32				fMillisecondsInCycles;
	extern  SProcessorInfo	ID;
#ifdef __PROFILE__	
	extern  u64				qwCyclesPerTimer;
#endif

	#pragma warning(disable:4035)
	IC u64 qwfGetCycleCount()	
	{
		__asm _emit 0x0F; 
		__asm _emit 0x31;
	}
	#pragma warning(default:4035)
	
	extern void vfInitHardware();
};
