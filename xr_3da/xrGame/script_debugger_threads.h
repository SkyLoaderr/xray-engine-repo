#pragma once
#include "script_debugger_messages.h"

class CScriptProcessor;
struct lua_State;

class CDbgScriptThreads
{
	xr_vector<SScriptThread>			m_threads;
public:
				CDbgScriptThreads			(){};
				~CDbgScriptThreads			(){};
	u32			FillFrom					(CScriptProcessor*);
	u32			Fill						();
	lua_State*	FindScript					(int nthreadID);
	void		DrawThreads					();
};