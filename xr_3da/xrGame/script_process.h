////////////////////////////////////////////////////////////////////////////
//	Module 		: script_process.h
//	Created 	: 19.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script process class
////////////////////////////////////////////////////////////////////////////

#pragma once

class CScriptThread;

class CScriptProcess {
public:
	typedef xr_vector<CScriptThread*> SCRIPT_REGISTRY;

protected:
	SCRIPT_REGISTRY					m_scripts;
	xr_vector<LPSTR>				m_scripts_to_run;
	xr_vector<LPSTR>				m_strings_to_run;
	ref_str							m_name;

protected:
	u32								m_iterator;				// Oles: iterative update

protected:
			void					run_scripts		();
			void					run_strings		();

public:
									CScriptProcess	(ref_str anme, ref_str scripts);
	virtual							~CScriptProcess	();
			void					update			();
			void					add_script		(LPCSTR	script_name);
			void					add_string		(LPCSTR	string_to_run);
	IC		const SCRIPT_REGISTRY	&scripts		() const;
	IC		ref_str					name			() const;
};

#include "script_process_inline.h"