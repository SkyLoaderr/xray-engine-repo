#pragma once
#include "script_debugger_messages.h"

class CScriptProcess;
struct lua_State;

class CDbgScriptThreads
{
	xr_vector<SScriptThread>			m_threads;
public:
				CDbgScriptThreads			(){};
				~CDbgScriptThreads			(){};
	u32			FillFrom					(CScriptProcess*);
	u32			Fill						();
	lua_State*	FindScript					(int nthreadID);
	void		DrawThreads					();
};