////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_processor.h
//	Created 	: 19.09.2003
//  Modified 	: 22.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script processor
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_script_space.h"

class CScript;
//struct CLuaVirtualMachine;

DEFINE_VECTOR(CScript*,SCRIPT_VECTOR,SCRIPT_IT);

class CScriptProcessor {
	CLuaVirtualMachine	*m_tpLuaVirtualMachine;
	SCRIPT_VECTOR		m_tpScripts;
public:
						CScriptProcessor			(LPCSTR					caCaption, LPCSTR caScriptString);
	virtual				~CScriptProcessor			();
			void		Update						();
};