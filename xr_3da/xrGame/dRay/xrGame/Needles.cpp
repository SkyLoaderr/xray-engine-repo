///////////////////////////////////////////////////////////////
// Needles.cpp
// Needles - артефакт иголки
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// BlackDrops.cpp
// BlackDrops - черные капли
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Needles.h"
#include "PhysicsShell.h"


CNeedles::CNeedles(void) 
{
}

CNeedles::~CNeedles(void) 
{
}

void CNeedles::Load(LPCSTR section) 
{
	// verify class
	LPCSTR Class = pSettings->r_string(section,"class");
	CLASS_ID load_cls = TEXT2CLSID(Class);
	R_ASSERT(load_cls==CLS_ID);

	inherited::Load(section);
}

