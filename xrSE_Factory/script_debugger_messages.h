#pragma once

struct StackTrace{
	char szDesc[255];
	char szFile[255];
	int nLine;
	StackTrace(){szDesc[0]=0;szFile[0]=0;nLine=0;};
} ;

struct Variable
{
	char szName[255];
	char szType[50];
	char szValue[255];
	Variable(){szName[0]=0;szType[0]=0;szValue[0]=0;};
};

struct lua_State;

struct SScriptThread{
//	void*			pScript;
	lua_State		*lua;
	int				scriptID;
	bool			active;
	char			name[255];
	char			process[255];
	SScriptThread():/**pScript(0),/**/lua(0), scriptID(-1),active(false){name[0]=0;process[0]=0;};
	SScriptThread(const SScriptThread& other)
	{
		operator = (other);
	};
	SScriptThread& operator = (const SScriptThread& other){
//		pScript			= other.pScript;
		lua				= other.lua;
		scriptID		= other.scriptID;
		active			= other.active;
		name[0]			=0;
		process[0]	=0;
		strcat(name,other.name);
		strcat(process,other.process);

		return *this;
	}
};


#define DEBUGGER_MAIL_SLOT		"\\\\.\\mailslot\\script_debugger_mailslot"
#define IDE_MAIL_SLOT			"\\\\.\\mailslot\\script_ide_mailslot"

#define _DMSG_FIRST_MSG			DMSG_WRITE_DEBUG

#define DMSG_WRITE_DEBUG			WM_USER+1
#define DMSG_HAS_BREAKPOINT			WM_USER+2
#define DMSG_GOTO_FILELINE			WM_USER+3
#define DMSG_DEBUG_START			WM_USER+4
#define DMSG_DEBUG_BREAK			WM_USER+5
#define DMSG_DEBUG_END				WM_USER+6
#define DMSG_CLEAR_STACKTRACE		WM_USER+7
#define DMSG_ADD_STACKTRACE			WM_USER+8
#define DMSG_GOTO_STACKTRACE_LEVEL	WM_USER+9
#define DMSG_GET_STACKTRACE_LEVEL	WM_USER+10
#define DMSG_CLEAR_LOCALVARIABLES	WM_USER+11
#define DMSG_ADD_LOCALVARIABLE		WM_USER+12
#define DMSG_CLEAR_GLOBALVARIABLES	WM_USER+13
#define DMSG_ADD_GLOBALVARIABLE		WM_USER+14
#define DMSG_EVAL_WATCH				WM_USER+15

//#define DMSG_CREATE_IDE				WM_USER+16
//#define DMSG_DESTROY_IDE			WM_USER+17
#define DMSG_ACTIVATE_IDE			WM_USER+18
//#define DMSG_NEED_ANSWER			WM_USER+19

#define DMSG_DEBUG_STEP_INTO		WM_USER+20
#define DMSG_DEBUG_STEP_OVER		WM_USER+21
#define DMSG_DEBUG_STEP_OUT			WM_USER+22
#define DMSG_DEBUG_RUN_TO_CURSOR	WM_USER+23
#define DMSG_STOP_DEBUGGING			WM_USER+24
#define DMSG_GOTO_IDE_STACKTRACE_LEVEL	WM_USER+25
#define DMSG_NEW_CONNECTION			WM_USER+26
#define DMSG_DEBUG_GO				WM_USER+27
#define DMSG_GET_BREAKPOINTS		WM_USER+28
#define DMSG_CLEAR_THREADS			WM_USER+29
#define DMSG_ADD_THREAD				WM_USER+30
#define DMSG_THREAD_CHANGED			WM_USER+31
#define DMSG_GET_VAR_TABLE			WM_USER+32

#define _DMSG_LAST_MSG			DMSG_GET_VAR_TABLE
