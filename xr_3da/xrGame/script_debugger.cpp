#include "stdafx.h"
#include "script_debugger.h"
#include "script_lua_helper.h"
#include "mslotutils.h"

CScriptDebugger* CScriptDebugger::m_pDebugger = NULL;

LRESULT CScriptDebugger::_SendMessage(u32 message, WPARAM wParam, LPARAM lParam)
{
	if ( (m_pDebugger)&&(message >= _DMSG_FIRST_MSG && message <= _DMSG_LAST_MSG) )
		return m_pDebugger->DebugMessage(message, wParam, lParam);

	return 0;
}

LRESULT CScriptDebugger::DebugMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CMailSlotMsg msg;

	switch( nMsg )
	{
	case DMSG_WRITE_DEBUG:
		msg.w_int(DMSG_WRITE_DEBUG);
		msg.w_string((char*)wParam);
		SendMailslotMessage(IDE_MAIL_SLOT, msg);
		break;

	case DMSG_HAS_BREAKPOINT:
		msg.w_int(DMSG_HAS_BREAKPOINT);
		msg.w_string((char*)wParam);
		msg.w_int((int)lParam);
		SendMailslotMessage(IDE_MAIL_SLOT, msg);
		return WaitForReply(DMSG_HAS_BREAKPOINT);
		break;

	case DMSG_GOTO_FILELINE:
		msg.w_int(DMSG_GOTO_FILELINE);
		msg.w_string((char*)wParam);
		msg.w_int((int)lParam);
		SendMailslotMessage(IDE_MAIL_SLOT, msg);
		break;

	case DMSG_DEBUG_BREAK:{
		msg.w_int(DMSG_SHOW_IDE);
		SendMailslotMessage(IDE_MAIL_SLOT, msg);
		m_nMode = (int)WaitForReply(DMSG_SHOW_IDE);

		if(m_nMode == DMOD_SHOW_STACK_LEVEL){
			msg.Reset();
			msg.w_int(DMSG_GET_STACKTRACE_LEVEL);
			SendMailslotMessage(IDE_MAIL_SLOT, msg);
			int sl = WaitForReply(DMSG_GET_STACKTRACE_LEVEL);
			m_callStack.GotoStackTraceLevel(sl);
			}
		}break;

	case DMSG_CLEAR_STACKTRACE:
		m_callStack.Clear();
		msg.w_int(DMSG_CLEAR_STACKTRACE);
		SendMailslotMessage(IDE_MAIL_SLOT, msg);
		return 1;
		break;

	case DMSG_ADD_STACKTRACE:
		m_callStack.Add(((StackTrace*)wParam)->szDesc, 
			((StackTrace*)wParam)->szFile, 
			((StackTrace*)wParam)->nLine);

		msg.w_int(DMSG_ADD_STACKTRACE);
		msg.w_buff((StackTrace*)wParam, sizeof(StackTrace) );
		SendMailslotMessage(IDE_MAIL_SLOT, msg);
		return 1;
		break;

	case DMSG_GOTO_STACKTRACE_LEVEL:
		m_callStack.GotoStackTraceLevel((int)wParam);
		StackLevelChanged();
		return 1;
		break;
	
	case DMSG_CLEAR_LOCALVARIABLES:
		msg.w_int(DMSG_CLEAR_LOCALVARIABLES);
		SendMailslotMessage(IDE_MAIL_SLOT, msg);
		return 1;
		break;

	case DMSG_ADD_LOCALVARIABLE:
		msg.w_int(DMSG_ADD_LOCALVARIABLE);
		msg.w_buff((void*)wParam,sizeof(Variable));
		SendMailslotMessage(IDE_MAIL_SLOT, msg);
		return 1;
		break;

	case DMSG_REDRAW_WATCHES:
//		m_wndWatches.Redraw();
		return 0;
		break;

	}//case

	return 0;
}


BOOL CScriptDebugger::Active()
{
	return m_bIdePresent;
}

CScriptDebugger::CScriptDebugger()
{
	m_pDebugger = this;
	m_nLevel = 0;
	m_mailSlot = CreateMailSlotByName(DEBUGGER_MAIL_SLOT);

	m_bIdePresent = CheckExisting(IDE_MAIL_SLOT);
}

CScriptDebugger::~CScriptDebugger()
{
	CloseHandle(m_mailSlot);
}

void CScriptDebugger::UnPrepareLua(lua_State* l, int idx)
{
	if(idx == -1) return; // !Active()
	m_lua.UnPrepareLua (l, idx);
}

int CScriptDebugger::PrepareLua(lua_State* l)
{
	// call this function immediatly before calling lua_pcall. 
	//returns index in stack for errorFunc
	if(!Active())return -1;

	m_nMode = DMOD_NONE;
	return m_lua.PrepareLua(l);
}

BOOL CScriptDebugger::PrepareLuaBind()
{
	if(!Active())return FALSE;

	m_lua.PrepareLuaBind();
	m_nMode = DMOD_NONE;

	return TRUE;
}

void CScriptDebugger::Break()
{

}

void CScriptDebugger::Write(const char* szMsg)
{
	_SendMessage(DMSG_WRITE_DEBUG, (WPARAM)szMsg, 0);
}

void CScriptDebugger::LineHook(const char *szFile, int nLine)
{
	if ( m_nMode == DMOD_STOP )
		Break();

	if (
		_SendMessage(DMSG_HAS_BREAKPOINT, (WPARAM)szFile, (LPARAM)nLine) ||
			m_nMode==DMOD_STEP_INTO	|| 
			m_nMode==DMOD_BREAK ||
		(m_nMode==DMOD_STEP_OVER && m_nLevel<=0) || 
		(m_nMode==DMOD_STEP_OUT && m_nLevel<0) ||
		(m_nMode==DMOD_RUN_TO_CURSOR && 
		xr_strcmp(m_strPathName,szFile) &&
		m_nLine==nLine) )
	{
		DebugBreak(szFile, nLine);		
	}
}

void CScriptDebugger::FunctionHook(const char *szFile, int nLine, BOOL bCall)
{
	if ( m_nMode == DMOD_STOP )
		Break();

	m_nLevel += (bCall?1:-1);
}

void CScriptDebugger::DebugBreak(const char *szFile, int nLine)
{
	m_lua.DrawStackTrace();
	m_callStack.SetStackTraceLevel(0);
	do{
		m_lua.DrawGlobalVariables();
		_SendMessage(DMSG_GOTO_STACKTRACE_LEVEL, GetStackTraceLevel(), 0);

		_SendMessage(DMSG_DEBUG_BREAK, 0, 0);

	}while(m_nMode==DMOD_SHOW_STACK_LEVEL);
}

void CScriptDebugger::ErrorBreak(const char* szFile, int nLine)
{
	DebugBreak(szFile, nLine);
/*	m_lua.DrawStackTrace();
	m_lua.DrawGlobalVariables();

	StackLevelChanged();

	_SendMessage(DMSG_DEBUG_BREAK, 0, 0);

	m_nMode = DMOD_NONE;
*/
}

void CScriptDebugger::ClearStackTrace()
{
	_SendMessage(DMSG_CLEAR_STACKTRACE, 0, 0);
}

void CScriptDebugger::AddStackTrace(const char* szDesc, const char* szFile, int nLine)
{
	StackTrace st;
	strcat(st.szDesc, szDesc);
	strcat(st.szFile, szFile);
	st.nLine = nLine;
	_SendMessage(DMSG_ADD_STACKTRACE, (WPARAM)&st, 0);
}

int CScriptDebugger::GetStackTraceLevel()
{
	return m_callStack.GetLevel();
}

void CScriptDebugger::StackLevelChanged()
{
	m_lua.DrawLocalVariables();
}



void CScriptDebugger::ClearLocalVariables()
{
	_SendMessage(DMSG_CLEAR_LOCALVARIABLES, 0, 0);
}

void CScriptDebugger::AddLocalVariable(const char *name, const char *type, const char *value)
{
	Variable var;
	strcat(var.szName, name );
	strcat(var.szType, type );
	strcat(var.szValue, value );
	_SendMessage(DMSG_ADD_LOCALVARIABLE, (WPARAM)&var, 0);
}

void CScriptDebugger::ClearGlobalVariables()
{
	_SendMessage(DMSG_CLEAR_GLOBALVARIABLES, 0, 0);
}

void CScriptDebugger::AddGlobalVariable(const char *name, const char *type, const char *value)
{
	Variable var;
	strcat(var.szName, name );
	strcat(var.szType, type );
	strcat(var.szValue, value );
	_SendMessage(DMSG_ADD_GLOBALVARIABLE, (WPARAM)&var, 0);
}


void CScriptDebugger::Eval(const char* strCode, char* res)
{
	string1024 strCodeFull;
	strCodeFull[0] = 0;
	const char * r = "return  ";
	strconcat(strCodeFull,r,strCode);
	m_lua.Eval(strCodeFull, res);
}


LRESULT CScriptDebugger::WaitForReply(UINT nMsg)
{
	CMailSlotMsg msg;
	while (true){
		if(CheckMailslotMessage(m_mailSlot,msg)) break;
		Sleep(100);
	};
	R_ASSERT(msg.GetLen());
	
	int msgType;
	msg.r_int(msgType);
	VERIFY(msgType==(int)nMsg);
	switch(msgType) {
	case DMSG_HAS_BREAKPOINT:{
			int res;
			msg.r_int(res);
			return res;
		}break;
	case DMSG_SHOW_IDE:{
			int res;
			msg.r_int(res);
			return TranslateIdeMessage(res);
	}break;
	case DMSG_GET_STACKTRACE_LEVEL:{
			int res;
			msg.r_int(res);
			return res;
	}break;
	default:
		break;
	}

}

int CScriptDebugger::TranslateIdeMessage (UINT msg)
{
	switch(msg) {
	case DMSG_DEBUG_STEP_INTO:
		return DMOD_STEP_INTO;
		break;

	case DMSG_DEBUG_STEP_OVER:
		m_nLevel=0;
		return DMOD_STEP_OVER;
		break;

	case DMSG_DEBUG_STEP_OUT:
		m_nLevel=0;
		return DMOD_STEP_OUT;
		break;

	case DMSG_DEBUG_RUN_TO_CURSOR:
		return DMOD_RUN_TO_CURSOR;
		break;

	case DMSG_STOP_DEBUGGING:
		return DMOD_STOP;
		break;
	
	case DMSG_GOTO_STACKTRACE_LEVEL:
		return DMOD_SHOW_STACK_LEVEL;
		break;

	default:
		return DMOD_NONE;
	}
}
/*
void CScriptDebugger::CreateIde()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    // Start the child process. 
    BOOL r = CreateProcess( NULL, // No module name (use command line). 
        "luaIDE.exe", // Command line. 
        NULL,             // Process handle not inheritable. 
        NULL,             // Thread handle not inheritable. 
        FALSE,            // Set handle inheritance to FALSE. 
        0,                // No creation flags. 
        NULL,             // Use parent's environment block. 
        NULL,             // Use parent's starting directory. 
        &si,              // Pointer to STARTUPINFO structure.
        &pi ) ;            // Pointer to PROCESS_INFORMATION structure.
}
*/
