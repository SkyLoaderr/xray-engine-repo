////////////////////////////////////////////////////////////////////////////
//	Module 		: bge.cpp
//	Created 	: 01.10.2004
//  Modified 	: 01.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Board game engine
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hardware.h"
#include "ui.h"

void __cdecl main(char argc, char *argv[])
{
	Hardware::detect	();
	ui().execute		(argc,argv);
}
