////////////////////////////////////////////////////////////////////////////
//	Module 		: stdafx.cpp
//	Created 	: 01.10.2004
//  Modified 	: 01.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Standard precompiled header
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ui.h"

#pragma comment(lib,"winmm.lib")

void boost::throw_exception(const std::exception &A)
{
	ui().log("Boost throwed exception %s and program is terminated\n",A.what());
	exit(-1);
}

