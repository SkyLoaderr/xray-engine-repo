//////////////////////////////////////////////////////////
//	Module			:	Math routines					//
//	File			:	_math.cpp						//
//	Creation date	:	31.08.2002						//
//	Author			:	Oles Shishkovtsov				//
//	Modifications	:	Dmitriy Iassenev				//
//////////////////////////////////////////////////////////

#include "stdafx.h"

#include "_math.h"
#include "_time.h"
#include "misc.h"

#define RDTSC_TRIES_NUMBER	64

u16 wfGetFPU_SW() 
{
	u16 SW;
	__asm fstcw SW;
	return SW;
}

namespace FPU {
	u16	_24	=0;
	u16	_24r=0;
	u16	_53	=0;
	u16	_53r=0;
	u16	_64	=0;
	u16	_64r=0;
}

namespace CPU 
{
	u64				qwCyclesPerSecond;
	u64				qwCyclesPerRDTSC;
	f32				fSecondsInCycles;
	f32				fMillisecondsInCycles;
#ifdef __PROFILE__	
	u64				qwCyclesPerTimer;
#endif
	SProcessorInfo	ID;

	void vfDetectCPU_FPU()
	{
		// General CPU identification
		if (!dwfGetCPU_ID(&ID)) {
			vfDualPrintF("Warning : can't detect CPU/FPU.");
			return;
		}

		// Detecting timers and frequency
		u64	qwStart, qwEnd;
		u32	dwStart, dwTest;

		// setting realtime priority
		SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);

		// Detecting frequency
		dwTest	= timeGetTime();
		do {
			dwStart = timeGetTime(); 
		}
		while (dwTest == dwStart);

		qwStart	= CPU::qwfGetCycleCount();

		for (; timeGetTime() - dwStart < 1000;);
		
		qwEnd				= CPU::qwfGetCycleCount();
		qwCyclesPerSecond	= qwEnd - qwStart;

		// Detect Overhead
		qwCyclesPerRDTSC	= 0;
		u64 qwDummy			= 0;
		for (s32 i=0; i<RDTSC_TRIES_NUMBER; i++) {
			qwStart				=	CPU::qwfGetCycleCount();
			qwCyclesPerRDTSC	+=	CPU::qwfGetCycleCount() - qwStart - qwDummy;
		}
		qwCyclesPerRDTSC	/= RDTSC_TRIES_NUMBER;
		qwCyclesPerSecond	-= qwCyclesPerRDTSC;
		
		// setting normal priority
		SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);

		_control87	( _PC_64,   MCW_PC );
		_control87	( _RC_CHOP, MCW_RC );
		
		f64 a, b;
		
		a = 1.0;		
		b = f64(s64(qwCyclesPerSecond));
		fSecondsInCycles = f32(f64(a/b));

		a = 1000.0;	
		b = f64(s64(qwCyclesPerSecond));
		fMillisecondsInCycles = f32(f64(a/b));
	}

	void vfInitHardware() 
	{
		char caFeatures[128] = "RDTSC";

		vfDualPrintF("Detecting hardware...");
		
		vfDetectCPU_FPU();
		
		vfDualPrintF("completed\n  Detected CPU: %s %s, F%d/M%d/S%d, %d mhz, %d-clk\n",
			CPU::ID.caVendorName,CPU::ID.caModelName,
			CPU::ID.dwFamily,CPU::ID.dwModel,CPU::ID.dwStepping,
			u32(CPU::qwCyclesPerSecond/s64(1000000)),
			u32(CPU::qwCyclesPerRDTSC)
			);
		if (CPU::ID.dwFeatures & _CPU_FEATURE_MMX)	
			strcat(caFeatures,", MMX");
		if (CPU::ID.dwFeatures & _CPU_FEATURE_3DNOW)	
			strcat(caFeatures,", 3DNow!");
		if (CPU::ID.dwFeatures & _CPU_FEATURE_SSE)	
			strcat(caFeatures,", SSE");
		if (CPU::ID.dwFeatures & _CPU_FEATURE_SSE2)	
			strcat(caFeatures,", SSE2");
		vfDualPrintF("  CPU Features: %s\n", caFeatures);

		_clear87();

		_control87( _PC_24,   MCW_PC );
		_control87( _RC_CHOP, MCW_RC );
		FPU::_24  = wfGetFPU_SW();	// 24, chop
		_control87( _RC_NEAR, MCW_RC );
		FPU::_24r = wfGetFPU_SW();	// 24, rounding

		_control87( _PC_53,   MCW_PC );
		_control87( _RC_CHOP, MCW_RC );
		FPU::_53  = wfGetFPU_SW();	// 53, chop
		_control87( _RC_NEAR, MCW_RC );
		FPU::_53r = wfGetFPU_SW();	// 53, rounding

		_control87( _PC_64,   MCW_PC );
		_control87( _RC_CHOP, MCW_RC );
		FPU::_64  = wfGetFPU_SW();	// 64, chop
		_control87( _RC_NEAR, MCW_RC );
		FPU::_64r = wfGetFPU_SW();	// 64, rounding

		FPU::m24r();

#ifdef __PROFILE__	
		SetPriorityClass	(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
		SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
		
		CPU::qwCyclesPerTimer = 0;
		for (s32 i=0; i<1048576; i++) {
			u64 qwStartTime = CPU::qwfGetCycleCount();
			TIMER_START(TimerOverhead);
			TIMER_STOP(TimerOverhead);
			u64 qwStopTime = CPU::qwfGetCycleCount();;
			CPU::qwCyclesPerTimer += qwStopTime - qwStartTime - CPU::qwCyclesPerRDTSC;
		}
		CPU::qwCyclesPerTimer >>= 20;
		TIMER_INIT(TimerOverhead);

		SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
		SetPriorityClass	(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
#endif
	}
};