#include "stdafx.h"
#include "script_debugger_threads.h"
#include "ai_space.h"
#include "ai_script_processor.h"
#include "script_engine.h"
#include "ai_script.h"
#include "script_debugger.h"


u32 CDbgScriptThreads::Fill()
{
	u32 res = 0;
	CScriptProcessor* sp = ai().script_engine().script_processor("game");
	VERIFY(sp);
	res += FillFrom(sp);
	VERIFY(sp);
	sp = ai().script_engine().script_processor("level");
	res += FillFrom(sp);
	return res;
}

u32 CDbgScriptThreads::FillFrom(CScriptProcessor* sp)
{
	m_threads.clear();
	SCRIPT_VECTOR vScripts = 	sp->scripts();
	SCRIPT_IT It = vScripts.begin();
	for(;It!=vScripts.end(); ++It){
		SScriptThread th;
		th.pScript		= (*It);
		th.scriptID		= (*It)->m_thread_reference;
		th.active		= (*It)->m_bActive;
		strcat(th.name, (*It)->m_script_name);
		strcat(th.processor, sp->name());

		m_threads.push_back(th);
	}
	return m_threads.size();
}

lua_State* CDbgScriptThreads::FindScript(int nThreadID)
{
	xr_vector<SScriptThread>::iterator It = m_threads.begin();
	for(;It!=m_threads.end();++It){
		if( (*It).scriptID == nThreadID )
			return ((CScript*)(*It).pScript)->lua();
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

