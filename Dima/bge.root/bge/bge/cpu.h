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
	extern CProcessorInfo	processor_info;
	extern u64				cycles_per_second;
	extern u64				cycles_per_rdtsc;
	extern float			cycles2seconds;
	extern float			cycles2milisec;

	#pragma warning(disable:4035)
	IC	u64				cycles()
	{
		_asm _emit 0x0F;
		_asm _emit 0x31;
	}
	#pragma warning(default:4035)

		void			init	();
}