////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_processor.h
//	Created 	: 19.09.2003
//  Modified 	: 22.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script processor
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_space.h"

class CScript;

DEFINE_VECTOR(CScript*,SCRIPT_VECTOR,SCRIPT_IT);

class CScriptProcessor {
	SCRIPT_VECTOR			m_tpScripts;
	string4096				m_caOutput;
	xr_vector<LPSTR>		m_scripts_to_run;
public:
						CScriptProcessor			(LPCSTR					caCaption, LPCSTR caScriptString);
	virtual				~CScriptProcessor			();
			void		update						();
			void		add_script					(LPCSTR					script_name);
			void		run_scripts					();
};