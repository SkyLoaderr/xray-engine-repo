#pragma once


#include "script_lua_helper.h"
#include "script_CallStack.h"
#include "script_debugger_messages.h"
#include "script_debugger_utils.h"

//#include "../../luaDbg/dbgIde/dbgIde/script_debug_ide.h"
//#pragma comment(lib, "x:/dbgIde.lib")

/*
typedef LRESULT (*TmsgFunc)(UINT Msg,	WPARAM wParam,	LPARAM lParam);
class CScriptDeduggerIDE 
{
public:
	LRESULT		OnMessageToIDE				(UINT Msg, WPARAM wParam,	LPARAM lParam){return 0;};
	void		SetDebuggerMsgFunc			(TmsgFunc F){};

	CScriptDeduggerIDE(){};
	~CScriptDeduggerIDE(){};
};*/
class CMailSlotMsg;
struct lua_State;

#define DMOD_NONE					0
#define DMOD_STEP_INTO				1
#define DMOD_STEP_OVER				2
#define DMOD_STEP_OUT				3
#define DMOD_RUN_TO_CURSOR			4
#define DMOD_SHOW_STACK_LEVEL		5

#define DMOD_BREAK					10
#define DMOD_STOP					11

struct SBreakPoint{
	string256	fileName;
	s32			nLine;
	SBreakPoint(){fileName[0]=0;nLine=0;};
	SBreakPoint(const SBreakPoint& other)
	{
		operator = (other);
	};
	SBreakPoint& operator = (const SBreakPoint& other){
		fileName[0]=0;
		strcat(fileName,other.fileName);
		nLine = other.nLine;
		return *this;
	}

};

class CScriptDebugger
{
public:
	void			Eval				(const char* strCode, char* res);
	void			AddLocalVariable	(const char* name, const char* type, const char* value);
	void			ClearLocalVariables	();
	void			AddGlobalVariable	(const char* name, const char* type, const char* value);
	void			ClearGlobalVariables();
	void			StackLevelChanged	();
	void			Break				();
	void			DebugBreak			();
	void			DebugBreak			(const char* szFile, int nLine);
	void			ErrorBreak			(const char* szFile = 0, int nLine = 0);
	void			LineHook			(const char* szFile, int nLine);
	void			FunctionHook		(const char* szFile, int nLine, BOOL bCall);
	void			Write				(const char* szMsg);

	int				PrepareLua			(lua_State* );
	void			UnPrepareLua		(lua_State* l, int idx);
	BOOL			PrepareLuaBind		();

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
	
	BOOL			Active				();
	static CScriptDebugger* GetDebugger	() { return m_pDebugger; };
	static LRESULT			_SendMessage(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	void			FillBreakPointsIn	(CMailSlotMsg* msg);
	bool			HasBreakPoint		(const char* fileName, s32 lineNum);
	void			CheckNewMessages	();
	LRESULT			DebugMessage		(UINT nMsg, WPARAM wParam, LPARAM lParam);
	LRESULT			WaitForReply		(UINT nMsg);
	int				TranslateIdeMessage (CMailSlotMsg*);

	UINT			StartDebugger		();	

	CDbgLuaHelper						m_lua;
	CScriptCallStack					m_callStack;
	static CScriptDebugger*				m_pDebugger;
	int									m_nMode;
	int									m_nLevel;  //for step into/over/out
	string_path							m_strPathName;	//for run_to_line_number
	int									m_nLine;		//for run_to_line_number

	HANDLE								m_mailSlot;
	BOOL								m_bIdePresent;

	xr_vector<SBreakPoint>				m_breakPoints;
};
