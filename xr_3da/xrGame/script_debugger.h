#pragma once


#include "script_lua_helper.h"
#include "script_CallStack.h"
#include "script_debugger_messages.h"
#include "script_debugger_utils.h"

#include "../../luaDbg/dbgIde/dbgIde/script_debug_ide.h"

struct lua_State;

#define DMOD_NONE					0
#define DMOD_STEP_INTO				1
#define DMOD_STEP_OVER				2
#define DMOD_STEP_OUT				3
#define DMOD_RUN_TO_CURSOR			4

#define DMOD_BREAK					10
#define DMOD_STOP					11




class CScriptDebugger :public xr_waitableThread 
{
public:
	virtual void	run_w				();//xr_waitableThread
	void			Execute				();
	void			Eval				(const char* strCode, char* res);
	/*static*/ void	EndThread			();
	BOOL			GetCalltip			(const char* szWord, char* szCalltip);
	void			AddLocalVariable	(const char* name, const char* type, const char* value);
	void			ClearLocalVariables	();
	void			AddGlobalVariable	(const char* name, const char* type, const char* value);
	void			ClearGlobalVariables();
	void			StackLevelChanged	();
	void			Break				();
	void			Stop				();
	void			DebugBreak			(const char* szFile, int nLine);
	void			LineHook			(const char* szFile, int nLine);
	void			FunctionHook		(const char* szFile, int nLine, BOOL bCall);
	void			Write				(const char* szMsg);
	BOOL			Start				();

	BOOL			Prepare				(lua_State* l);
	CScriptDebugger						();
	virtual ~CScriptDebugger			();

	void			Go					();
	void			StepInto			();
	void			StepOver			();
	void			StepOut				();
	void			RunToCursor			();

	void			ClearStackTrace		();
	void			AddStackTrace		(const char* strDesc, const char* strFile, int nLine);
	int				GetStackTraceLevel	();

	static CScriptDebugger* GetDebugger	() { return m_pDebugger; };
	lua_State*		GetLuaState			();
//	HWND GetMainWnd() { return m_hWndMainFrame; };
protected:
	static LRESULT	_SendMessage		(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT			DebugMessage		(UINT nMsg, WPARAM wParam, LPARAM lParam);


//	static UINT		StartDebugger		(LPVOID pParam );	
	UINT			StartDebugger		();	

	CScriptDeduggerIDE					m_ide_wrapper;
	CDbgLuaHelper						m_lua;
	CScriptCallStack
//	HWND m_hWndMainFrame;
//	CEvent m_event;
	xr_event							m_mutex;

	int									m_nMode;
//	CString m_strPathName;
	string_path							m_strPathName;
	int									m_nLine;
	int									m_nLevel;
//	CWinThread* m_pThread;

	static CScriptDebugger*				m_pDebugger;
};
