////////////////////////////////////////////////////////////////////////////
//	Module 		: script.h
//	Created 	: 03.12.2004
//  Modified 	: 03.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Scripting engine singleton
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <singleton.h>
#include "script_engine.h"

IC	CScriptEngine &script()
{
	return	(Loki::SingletonHolder<CScriptEngine>::Instance());
}