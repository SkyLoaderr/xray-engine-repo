////////////////////////////////////////////////////////////////////////////
//	Module 		: script_process.cpp
//	Created 	: 19.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script process class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "script_engine.h"
#include "script_process.h"
#include "script_thread.h"
#include "ai_space.h"
#include "object_broker.h"

string4096			g_ca_stdout;

#ifdef USE_DEBUGGER
#	include "script_debugger.h"
#endif

CScriptProcess::CScriptProcess	(shared_str name, shared_str scripts) :
	m_name						(name)
{
	Msg				("* Initializing %s script process",*m_name);
	
	string256		I;
	for (u32 i=0, n = _GetItemCount(*scripts); i<n; ++i)
		add_script	(_GetItem(*scripts,i,I));

	m_iterator		= 0;
}

CScriptProcess::~CScriptProcess()
{
	delete_data		(m_scripts);
}

void CScriptProcess::run_scripts()
{
	LPSTR			S;
	for ( ; !m_scripts_to_run.empty(); ) {
		LPSTR		I = m_scripts_to_run.back();
		S			= xr_strdup(I);
		xr_free		(I);
		m_scripts_to_run.pop_back();

		ai().script_engine().reload_modules(true);
		CScriptThread		*l_tpScript = xr_new<CScriptThread>(S);
		xr_free		(S);

		if (l_tpScript->m_bActive)
			m_scripts.push_back(l_tpScript);
		else
			xr_delete(l_tpScript);
	}
}

void CScriptProcess::run_strings()
{
	for ( ; !m_strings_to_run.empty(); ) {
		LPSTR		I = m_strings_to_run.back();
		int			err_code = lua_dostring(ai().script_engine().lua(),I);

		if (err_code) {
			if (!ai().script_engine().print_output(ai().script_engine().lua(),"console_string",err_code))
				ai().script_engine().print_error(ai().script_engine().lua(),err_code);
		}
		xr_free		(I);
		m_strings_to_run.pop_back();
	}
}


// Oles: 
//		changed to process one script per-frame
//		changed log-output to stack-based buffer (avoid persistent 4K storage)
void CScriptProcess::update()
{
#ifdef DBG_DISABLE_SCRIPTS
	m_scripts_to_run.clear();
	m_strings_to_run.clear();
	return;
#endif

	run_scripts			();
	run_strings			();

	if (m_scripts.empty())
		return;

	// update script
	g_ca_stdout[0]		= 0;
	u32					_id	= (++m_iterator)%m_scripts.size();
	if (!m_scripts[_id]->Update()) {
		xr_delete			(m_scripts[_id]);
		m_scripts.erase	(m_scripts.begin() + _id);
		--m_iterator;		// try to avoid skipping
	}

	if (g_ca_stdout[0]) {
		fputc							(0,stderr);
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeInfo,"%s",g_ca_stdout);
		fflush							(stderr);
	}

#ifdef _DEBUG
	lua_setgcthreshold	(ai().script_engine().lua(),0);
#endif
}

void CScriptProcess::add_script	(LPCSTR	script_name)
{
	m_scripts_to_run.push_back(xr_strdup(script_name));
}

void CScriptProcess::add_string	(LPCSTR	string_to_run)
{
	m_strings_to_run.push_back(xr_strdup(string_to_run));
}