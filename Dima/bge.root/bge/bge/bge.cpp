////////////////////////////////////////////////////////////////////////////
//	Module 		: bge.cpp
//	Created 	: 01.10.2004
//  Modified 	: 01.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Board game engine
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hardware.h"
#include "script.h"

void __cdecl main(char argc, char *argv[])
{
	Hardware::detect	();
	script().init		();
	ui().execute		(argc,argv);
}
