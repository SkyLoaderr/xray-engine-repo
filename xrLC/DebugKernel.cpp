// DebugKernel.cpp: implementation of the CDebugKernel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Process.h>
#include "resource.h"
#include <imagehlp.h>
#include "xrSyncronize.h"
#include "log.h"

CDebugKernel Debug;

static const char * dlgExpr = NULL;
static const char * dlgFile = NULL;
static char			dlgLine[16];

static BOOL CALLBACK verifyProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		{
			SetWindowText(GetDlgItem(hw,IDC_DESC),dlgExpr);
			SetWindowText(GetDlgItem(hw,IDC_FILE),dlgFile);
			SetWindowText(GetDlgItem(hw,IDC_LINE),dlgLine);

			// Stack
			for (DWORD i=0; i<Debug.GetCount(); i++)
			{
				Log(Debug.GetName(i));
				SendMessage	(
					GetDlgItem(hw,IDC_STACK), LB_ADDSTRING,
					0, (LPARAM) Debug.GetName(i));
			}
		}
		break;
	case WM_DESTROY:
		break;
	case WM_CLOSE:
		EndDialog(hw, IDSTOP);
		break;
	case WM_COMMAND:
		if( LOWORD(wp)==IDSTOP ) {
			EndDialog(hw, IDSTOP);
		}
		if( LOWORD(wp)==IDCONTINUE) {
			EndDialog(hw, IDCONTINUE);
		}
		if( LOWORD(wp)==IDDEBUG) {
			EndDialog(hw, IDDEBUG);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

ENGINE_API void __fastcall _verify     (const char *expr, char *file, int line) 
{
	static	CCriticalSection	CS;

	CS.Enter			();
	// Log Description
	char _tmp_buf[1024];
	sprintf(_tmp_buf,"Assertion failed in file %s, line %d.\nExpression was '%s'",
		file, line, expr);
	Log(_tmp_buf);

	// Call the dialog
	dlgExpr = expr;	dlgFile = file;
	sprintf(dlgLine,"%d",line);
	Debug.Update();
	int res = DialogBox(
		GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDD_VERIFY),
		NULL,
		verifyProc );
	switch (res) {
	case IDSTOP:
		abort();
		break;
	case IDDEBUG:
		__asm { int 3 };
		break;
	case IDCONTINUE:
		break;
	}
	CS.Leave			();
}

int __cdecl _out_of_memory(unsigned size)
{
	// Log Description
	char _tmp_buf[1024];
	sprintf(_tmp_buf,"Fatal Error: Out of memory.\nSize queried: %d bytes",size);
	Log(_tmp_buf);

	// Call the dialog
	dlgExpr = _tmp_buf;	dlgFile = "MemoryManager";	
	strcpy(dlgLine,"unknown");
	Debug.Update();
	int res = DialogBox(
		GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDD_VERIFY),
		NULL,
		verifyProc );
	switch (res) {
	case IDSTOP:
		abort();
		break;
	case IDDEBUG:
		__asm { int 3 };
		break;
	case IDCONTINUE:
		return 0;
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------------------------
// Sets up the symbols for functions in the debug file.
//------------------------------------------------------------------------------------------------------------------------
typedef int (__cdecl * _PNH)( size_t );

_CRTIMP int		__cdecl _set_new_mode( int );
_CRTIMP _PNH	__cdecl _set_new_handler( _PNH );

BOOL	CDebugKernel::Start()
{
	char    pathname	[_MAX_PATH+1];
	char    directory	[_MAX_PATH+1];
	char    drive		[10];
	BOOL    ret;
	HANDLE  process;

	_set_new_mode		(1);				// gen exception if can't allocate memory
	_set_new_handler	(_out_of_memory);	// exception-handler for 'out of memory' condition

	::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME | SYMOPT_LOAD_LINES | SYMOPT_OMAP_FIND_NEAREST);
	process=GetCurrentProcess();
	//Get the apps name
	::GetModuleFileName(NULL, pathname, _MAX_PATH);
	//Turn it into a search path
	_splitpath(pathname, drive, directory, NULL, NULL);
	sprintf(pathname, "%s%s", drive, directory);
	//Append the current directory to build a search path for SymInit
	::lstrcat(pathname, ";.;.\\DLLs\\;");
	ret=::SymInitialize(process, pathname, FALSE);
	if( ret )
	{
		//Regenerate the name of the app
		::GetModuleFileName(NULL, pathname, _MAX_PATH);
		if(::SymLoadModule(process, NULL, pathname, NULL, 0, 0))
		{
			//You could load dll/lib information if you wish here...
			//            if(::SymLoadModule(process, NULL, GLibDLLName, NULL, 0, 0))
			{
				return TRUE;
			}
		}
		::SymCleanup(process);
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL	CDebugKernel::Stop()
{
	::SymCleanup(GetCurrentProcess());
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void    CDebugKernel::Update()
{
	__try
	{
		RaiseException(0, 0, 0, NULL);
	}
	__except(UpdateStack(GetExceptionInformation()))
	{
	}
}
int	CDebugKernel::LogStack(EXCEPTION_POINTERS *pex)
{
	UpdateStack(pex,1);
	Log("*****\nStack dump:\n*****");
	for (vector<string>::iterator I=Stack.begin(); I!=Stack.end(); I++)
		Log(I->c_str());
	return EXCEPTION_EXECUTE_HANDLER;
}
//------------------------------------------------------------------------------------------------------------------------
int		CDebugKernel::UpdateStack	(EXCEPTION_POINTERS *pex, int iSkip)
{
	int			count;
	STACKFRAME  stack_frame;
	BOOL        b_ret=TRUE;

	// Cleanup stack
	Stack.clear	();

	//Setup stack frame
	ZeroMemory(&stack_frame, sizeof(stack_frame));
	stack_frame.AddrPC.Mode		= AddrModeFlat;
	stack_frame.AddrPC.Offset	= pex->ContextRecord->Eip;
	stack_frame.AddrStack.Mode	= AddrModeFlat;
	stack_frame.AddrStack.Offset= pex->ContextRecord->Esp;
    stack_frame.AddrFrame.Mode	= AddrModeFlat;
	stack_frame.AddrFrame.Offset= pex->ContextRecord->Ebp;

	//Skip past some irrelevant functions
	for(count=0; count < iSkip && b_ret==TRUE; count++)
	{
		b_ret = StackWalk(
			IMAGE_FILE_MACHINE_I386,
			GetCurrentProcess(),
			GetCurrentThread(),
			&stack_frame,
			pex->ContextRecord,
			NULL,
			SymFunctionTableAccess,
			SymGetModuleBase,
			NULL);
	}

	//While there are still functions on the stack..
	while (b_ret)
	{
		char FNbuf[1024];

        if(!GetFunctionName(NULL, (void *) stack_frame.AddrPC.Offset, FNbuf))
		{
			sprintf(FNbuf, "%s", "<Unknown Function>");
		}
		Stack.push_back(FNbuf);
        b_ret = StackWalk(
			IMAGE_FILE_MACHINE_I386,
			GetCurrentProcess(),
			GetCurrentThread(),
			&stack_frame,
			pex->ContextRecord,
			NULL,
			SymFunctionTableAccess,
			SymGetModuleBase,
			NULL);
	}
	return EXCEPTION_CONTINUE_EXECUTION;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL	CDebugKernel::GetFunctionName(HINSTANCE instance, void *pointer, char *text){
	char				symbol_buffer[sizeof(IMAGEHLP_SYMBOL)+1024];
	DWORD				displacement=0;
	HANDLE              process=GetCurrentProcess();
	PIMAGEHLP_SYMBOL    psymbol=(PIMAGEHLP_SYMBOL)symbol_buffer;

	ZeroMemory(symbol_buffer, sizeof(symbol_buffer));
	psymbol->SizeOfStruct  = sizeof(symbol_buffer);
	psymbol->MaxNameLength = 1024;
	if(SymGetSymFromAddr(process, (DWORD) pointer, &displacement, psymbol))    {
        strcpy(text, psymbol->Name);
        strcat(text, "()");
        return TRUE;
	}
	return FALSE;
}
