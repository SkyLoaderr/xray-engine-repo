////////////////////////////////////////////////////////////////////////////
//	Module 		: cpu.h
//	Created 	: 01.10.2004
//  Modified 	: 01.10.2004
//	Author		: Dmitriy Iassenev
//	Description : CPU namespace
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "processor_info.h"

namespace CPU {
	u64					cycles_per_second;
	u64					cycles_overhead;
	u64					timer_overhead;
	float				cycles2seconds;
	float				cycles2milisec;
	CProcessorInfo		ID;

	#pragma warning(disable:4035)
	IC	u64				cycles()
	{
		_asm _emit 0x0F;
		_asm _emit 0x31;
	}
	#pragma warning(default:4035)
}