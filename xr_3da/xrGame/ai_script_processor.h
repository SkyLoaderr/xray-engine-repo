////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_processor.h
//	Created 	: 19.09.2003
//  Modified 	: 22.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script processor
////////////////////////////////////////////////////////////////////////////

#pragma once

class CScript;

DEFINE_VECTOR(CScript*,SCRIPT_VECTOR,SCRIPT_IT);

class CScriptProcessor {
public:
	SCRIPT_VECTOR		m_tpScripts;

						CScriptProcessor			(LPCSTR					caCaption, LPCSTR caScriptString);
	virtual				~CScriptProcessor			();
			void		Update						();
};