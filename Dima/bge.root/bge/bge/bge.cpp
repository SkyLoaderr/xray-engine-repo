////////////////////////////////////////////////////////////////////////////
//	Module 		: bge.cpp
//	Created 	: 01.10.2004
//  Modified 	: 01.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Board game engine
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cpu.h"
#include "fpu.h"

void main(char *argc, char *argv[])
{
	CPU::init	();
	FPU::init	();
}