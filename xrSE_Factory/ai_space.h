////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_space.h
//	Created 	: 12.11.2003
//  Modified 	: 18.06.2004
//	Author		: Dmitriy Iassenev
//	Description : AI space class
////////////////////////////////////////////////////////////////////////////

#pragma once

class	CScriptEngine;

class CAI_Space {
private:
	CScriptEngine			*m_script_engine;

public:
							CAI_Space		();
	virtual					~CAI_Space		();
	IC		CScriptEngine	&script_engine	() const;
};

IC	CAI_Space	&ai	();

#include "ai_space_inline.h"