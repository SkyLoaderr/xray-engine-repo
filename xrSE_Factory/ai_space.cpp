////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_space.h
//	Created 	: 12.11.2003
//  Modified 	: 18.06.2004
//	Author		: Dmitriy Iassenev
//	Description : AI space class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_space.h"
#include "script_engine.h"

CAI_Space *g_ai_space = 0;

CAI_Space::CAI_Space				()
{
	m_script_engine			= xr_new<CScriptEngine>();
}

CAI_Space::~CAI_Space				()
{
	xr_delete				(m_script_engine);
}
