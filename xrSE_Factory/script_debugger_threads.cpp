#include "stdafx.h"
#include "script_debugger_threads.h"
#include "ai_space.h"
#include "script_process.h"
#include "script_engine.h"
#include "script_thread.h"
#include "script_debugger.h"


u32 CDbgScriptThreads::Fill()
{
	u32 res = 0;

#ifdef XRGAME_EXPORTS
	CScriptProcess* sp = ai().script_engine().script_process("game");

	if (!sp)
		return	res;

	res += FillFrom(sp);
	VERIFY(sp);
	sp = ai().script_engine().script_process("level");
	res += FillFrom(sp);
	return res;
#else
	return res;
#endif
}

u32 CDbgScriptThreads::FillFrom(CScriptProcess* sp)
{
	m_threads.clear();
	const CScriptProcess::SCRIPT_REGISTRY &vScripts = 	sp->scripts();
	CScriptProcess::SCRIPT_REGISTRY::const_iterator It = vScripts.begin();
	for(;It!=vScripts.end(); ++It){
		SScriptThread th;
//		th.pScript		= (*It);
		th.lua			= (*It)->lua();
		th.scriptID		= (*It)->m_thread_reference;
		th.active		= (*It)->m_bActive;
		strcat(th.name, (*It)->m_script_name);
		strcat(th.process, *sp->name());

		m_threads.push_back(th);
	}
	return m_threads.size();
}

lua_State* CDbgScriptThreads::FindScript(int nThreadID)
{
	xr_vector<SScriptThread>::iterator It = m_threads.begin();
	for(;It!=m_threads.end();++It){
		if( (*It).scriptID == nThreadID )
			return (*It).lua;
	}
	return 0;
}

void  CDbgScriptThreads::DrawThreads()
{
	CScriptDebugger::GetDebugger()->ClearThreads();
	xr_vector<SScriptThread>::iterator It = m_threads.begin();
	for(;It!=m_threads.end();++It){
		SScriptThread th;
		th = *It;
		CScriptDebugger::GetDebugger()->AddThread(th);
	}
}

