////////////////////////////////////////////////////////////////////////////
//	Module 		: fpu.cpp
//	Created 	: 01.10.2004
//  Modified 	: 01.10.2004
//	Author		: Dmitriy Iassenev
//	Description : FPU namespace
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fpu.h"
#include <float.h>

u16	FPU::_24	=0;
u16	FPU::_24r	=0;
u16	FPU::_53	=0;
u16	FPU::_53r	=0;
u16	FPU::_64	=0;
u16	FPU::_64r	=0;

IC	u16 fpu_sw				()
{
	u16		SW;
	__asm	fstcw SW;
	return	SW;
}

void FPU::init()
{
	_clear87();

	_control87( _PC_24,   MCW_PC );
	_control87( _RC_CHOP, MCW_RC );
	FPU::_24  = fpu_sw();	// 24, chop
	_control87( _RC_NEAR, MCW_RC );
	FPU::_24r = fpu_sw();	// 24, rounding

	_control87( _PC_53,   MCW_PC );
	_control87( _RC_CHOP, MCW_RC );
	FPU::_53  = fpu_sw();	// 53, chop
	_control87( _RC_NEAR, MCW_RC );
	FPU::_53r = fpu_sw();	// 53, rounding

	_control87( _PC_64,   MCW_PC );
	_control87( _RC_CHOP, MCW_RC );
	FPU::_64  = fpu_sw();	// 64, chop
	_control87( _RC_NEAR, MCW_RC );
	FPU::_64r = fpu_sw();	// 64, rounding

	FPU::m24r();
}