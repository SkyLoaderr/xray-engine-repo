////////////////////////////////////////////////////////////////////////////
//	Module 		: hardware.cpp
//	Created 	: 12.11.2004
//  Modified 	: 12.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Hardware namespace
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hardware.h"
#include "cpu.h"
#include "fpu.h"
#include "gpu.h"

void Hardware::detect()
{
	CPU::detect	();
	FPU::detect	();
	GPU::detect	();
}
