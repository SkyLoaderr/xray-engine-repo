////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_processor.cpp
//	Created 	: 19.09.2003
//  Modified 	: 22.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script processor
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "script_engine.h"
#include "ai_script_processor.h"
#include "ai_script.h"

DEFINE_VECTOR(LPSTR,LPSTR_VECTOR,LPSTR_IT);

CScriptProcessor::CScriptProcessor(LPCSTR caCaption, LPCSTR caScriptString)
{
	Msg				("* Initializing %s script processor",caCaption);
	u32				N = _GetItemCount(caScriptString);
	string256		I;
	for (u32 i=0; i<N; ++i)
		add_script	(_GetItem(caScriptString,i,I));
}

CScriptProcessor::~CScriptProcessor()
{
	SCRIPT_IT		I = m_tpScripts.begin();
	SCRIPT_IT		E = m_tpScripts.end();
	for ( ; I != E; ++I)
		xr_delete	(*I);
}

void CScriptProcessor::run_scripts()
{
	string256		S,S1 = "";
	for ( ; !m_scripts_to_run.empty(); ) {
		LPSTR		I = m_scripts_to_run.back();
		FS.update_path(S,"$game_scripts$",strconcat(S1,I,".script"));
		R_ASSERT3	(FS.exist(S),"Script file not found!",S);
		xr_free		(I);
		m_scripts_to_run.pop_back();

		CScript		*l_tpScript = xr_new<CScript>(S);
		if (l_tpScript->m_bActive)
			m_tpScripts.push_back(l_tpScript);
		else
			xr_delete(l_tpScript);
	}
}

void CScriptProcessor::update()
{
	run_scripts			();
	LPSTR	S = g_ca_stdout;
	for (int i=0, n=(int)m_tpScripts.size(); i<n; ++i) {
		strcpy	(m_caOutput,"");
		g_ca_stdout = m_caOutput;
		if (!m_tpScripts[i]->Update()) {
			xr_delete(m_tpScripts[i]);
			m_tpScripts.erase(m_tpScripts.begin() + i);
			--i;
			--n;
			continue;
		}
		if (xr_strlen(m_caOutput))
			ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeInfo,"%s",m_caOutput);
	}
	g_ca_stdout		= S;
}

void CScriptProcessor::add_script(LPCSTR	script_name)
{
	m_scripts_to_run.push_back(xr_strdup(script_name));
}
