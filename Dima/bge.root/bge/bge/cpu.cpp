////////////////////////////////////////////////////////////////////////////
//	Module 		: cpu.cpp
//	Created 	: 01.10.2004
//  Modified 	: 01.10.2004
//	Author		: Dmitriy Iassenev
//	Description : CPU namespace
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cpu.h"
#include "ui.h"
#include <float.h>

CProcessorInfo	CPU::processor_info;
u64				CPU::cycles_per_second;
u64				CPU::cycles_per_rdtsc;
float			CPU::cycles2seconds;
float			CPU::cycles2milisec;

const u32 RDTSC_TRY_COUNT = 64;

IC	void detect				()
{
	// General CPU processor_infoentification
	if (!CPU::processor_info.detected()) {
		ui().log			("Warning : can't detect CPU/FPU.");
		return;
	}

	// Detecting timers and frequency
	u64					qwStart, qwEnd;
	u32					dwStart, dwTest;

	// setting realtime priority
	SetPriorityClass	(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	Sleep(1);

	// Detecting frequency
	dwTest				= timeGetTime();
	do {
		dwStart			= timeGetTime(); 
	}
	while (dwTest == dwStart);

	qwStart				= CPU::cycles();

	for (; timeGetTime() - dwStart < 1000;);
	
	qwEnd				= CPU::cycles();
	CPU::cycles_per_second	= qwEnd - qwStart;

	// Detect Overhead
	CPU::cycles_per_rdtsc	= 0;
	u64 qwDummy			= 0;
	for (s32 i=0; i<RDTSC_TRY_COUNT; i++) {
		qwStart			=	CPU::cycles();
		CPU::cycles_per_rdtsc	+=	CPU::cycles() - qwStart - qwDummy;
	}
	CPU::cycles_per_rdtsc	/= RDTSC_TRY_COUNT;
	CPU::cycles_per_second	-= CPU::cycles_per_rdtsc;
	
	// setting normal priority
	SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass	(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);

	_control87			(_PC_64,MCW_PC);
	_control87			(_RC_CHOP,MCW_RC);
	
	f64					a, b;
	
	a					= 1.0;		
	b					= f64(s64(CPU::cycles_per_second));
	CPU::cycles2seconds	= f32(f64(a/b));

	a					= 1000.0;	
	b					= f64(s64(CPU::cycles_per_second));
	CPU::cycles2milisec	= f32(f64(a/b));
}

void CPU::detect	() 
{
	string128			features = "RDTSC";

	ui().log			("Detecting hardware...");
	
	::detect			();
	
	ui().log			("completed\n  Detected CPU: %s %s, F%d/M%d/S%d, %d mhz, %d-clk\n",
		CPU::processor_info.vendor_name(),
		CPU::processor_info.model_name(),
		CPU::processor_info.family(),
		CPU::processor_info.model(),
		CPU::processor_info.stepping(),
		u32(CPU::cycles_per_second/s64(1000000)),
		u32(CPU::cycles_per_rdtsc)
	);

	if (CPU::processor_info.features() & CProcessorInfo::CPU_FEATURE_MMX)	
		strcat			(features,", MMX");

	if (CPU::processor_info.features() & CProcessorInfo::CPU_FEATURE_3DNOW)	
		strcat			(features,", 3DNow!");

	if (CPU::processor_info.features() & CProcessorInfo::CPU_FEATURE_SSE)	
		strcat			(features,", SSE");

	if (CPU::processor_info.features() & CProcessorInfo::CPU_FEATURE_SSE2)	
		strcat			(features,", SSE2");

	ui().log			("  CPU Features: %s\n\n", features);
}
