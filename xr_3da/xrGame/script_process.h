////////////////////////////////////////////////////////////////////////////
//	Module 		: script_process.h
//	Created 	: 19.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script process class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_space.h"

class CScriptThread;

DEFINE_VECTOR(CScriptThread*,SCRIPT_VECTOR,SCRIPT_IT);

class CScriptProcess {
protected:
	SCRIPT_VECTOR			m_tpScripts;
	string4096				m_caOutput;
	xr_vector<LPSTR>		m_scripts_to_run;
	xr_vector<LPSTR>		m_strings_to_run;
	string256				m_name;
protected:
			void		run_scripts					();
			void		run_strings					();

public:
						CScriptProcess			(LPCSTR					caCaption, LPCSTR caScriptString);
	virtual				~CScriptProcess			();
			void		update						();
			void		add_script					(LPCSTR					script_name);
			void		add_string					(LPCSTR					string_to_run);
			const SCRIPT_VECTOR&	scripts			() const {return m_tpScripts;};
			LPCSTR		name						() const {return m_name;};
};