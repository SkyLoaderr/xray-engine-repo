#include "stdafx.h"
#pragma hdrstop

#include "xrdebug.h"

#include "dxerr9.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#pragma warning(pop)

#ifdef __BORLANDC__
#	include "d3d9.h"
#	include "d3dx9.h"
#	include "D3DX_Wrapper.h"
#	pragma comment(lib,"EToolsB.lib")
#	define DEBUG_INVOKE	DebugBreak()
	static BOOL			bException	= TRUE;
#else
#	define DEBUG_INVOKE	__asm int 3
	static BOOL			bException	= FALSE;
#endif

#ifndef _M_AMD64
#	ifndef __BORLANDC__
#		pragma comment(lib,"dxerr9.lib")
#	endif
#endif

#include <dbghelp.h>					// MiniDump flags
#include "BugTrap.h"					// for BugTrap functionality
#pragma comment(lib,"BugTrap.lib")		// Link to ANSI DLL

XRCORE_API	xrDebug		Debug;

extern void copy_to_clipboard	(const char *string);

void copy_to_clipboard	(const char *string)
{
	if (!OpenClipboard(0))
		return;

	HGLOBAL				handle = GlobalAlloc(GHND | GMEM_DDESHARE,(strlen(string) + 1)*sizeof(char));
	if (!handle) {
		CloseClipboard	();
		return;
	}

	char				*memory = (char*)GlobalLock(handle);
	strcpy				(memory,string);
	GlobalUnlock		(handle);
	EmptyClipboard		();
	SetClipboardData	(CF_TEXT,handle);
	CloseClipboard		();
}

void update_clipboard	(const char *string)
{
	if (!OpenClipboard(0))
		return;

	HGLOBAL				handle = GetClipboardData(CF_TEXT);
	if (!handle) {
		CloseClipboard		();
		copy_to_clipboard	(string);
		return;
	}

	LPSTR				memory = (char*)GlobalLock(handle);
	u32					memory_length = xr_strlen(memory);
	u32					string_length = xr_strlen(string);
	LPSTR				buffer = (LPSTR)_alloca((memory_length + string_length + 1)*sizeof(char));
	strcpy				(buffer,memory);
	GlobalUnlock		(handle);

	strcat				(buffer,string);
	CloseClipboard		();
	copy_to_clipboard	(buffer);
}

void xrDebug::backend(const char *expression, const char *description, const char *arguments, const char *file, int line, const char *function) 
{
	static xrCriticalSection CS;

	CS.Enter			();

	string4096			assertion_info;
	LPCSTR				endline = "\n";
	for (int i=0; i<2; ++i) {
		LPSTR			buffer = assertion_info;
		buffer			+= sprintf(buffer,"%s",endline);
		buffer			+= sprintf(buffer,"%sFatal Error%s%s",endline,endline,endline);
		buffer			+= sprintf(buffer,"[error] Expression    : %s%s",expression,endline);
		buffer			+= sprintf(buffer,"[error] Description   : %s%s",description,endline);
		if (arguments)
			buffer		+= sprintf(buffer,"[error] Arguments     : %s%s",arguments,endline);
		buffer			+= sprintf(buffer,"[error] Function      : %s%s",function,endline);
		buffer			+= sprintf(buffer,"[error] File          : %s%s",file,endline);
		buffer			+= sprintf(buffer,"[error] Line          : %d%s",line,endline);
		buffer			+= sprintf(buffer,"%s",endline);
		if (!i) {
			Msg			("%s",assertion_info);
			FlushLog	();
			endline		= "\r\n";
		}
		else
			copy_to_clipboard	(assertion_info);
	}

	if (handler)		handler();

#ifdef XRCORE_STATIC
	MessageBox			(NULL,tmp,"X-Ray error",MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
#else
	int					result = 
		MessageBox(
			NULL,
			assertion_info,
			"Fatal Error",
			MB_ABORTRETRYIGNORE|MB_ICONERROR|MB_SYSTEMMODAL
		);

	switch (result) {
		case IDABORT : {
			DEBUG_INVOKE;
			break;
		}
		case IDRETRY : {
			break;
		}
		case IDIGNORE : {
			break;
		}
		default : NODEFAULT;
	}
#endif

	CS.Leave			();
}

LPCSTR xrDebug::error2string	(long code)
{
	LPCSTR				result	= 0;
	static	string1024	desc_storage;

#ifdef _M_AMD64
#else
	result				= DXGetErrorDescription9	(code);
#endif
	if (0==result) 
	{
		FormatMessage	(FORMAT_MESSAGE_FROM_SYSTEM,0,code,0,desc_storage,sizeof(desc_storage)-1,0);
		result			= desc_storage;
	}
	return		result	;
}

void xrDebug::error		(long hr, const char* expr, const char *file, int line, const char *function)
{
	backend		(error2string(hr),expr,0,file,line,function);
}

void xrDebug::fail		(const char *e1, const char *file, int line, const char *function)
{
	backend		("assertion failed",e1,0,file,line,function);
}
void xrDebug::fail		(const char *e1, const char *e2, const char *file, int line, const char *function)
{
	backend		(e1,e2,0,file,line,function);
}
void xrDebug::fail		(const char *e1, const char *e2, const char *e3, const char *file, int line, const char *function)
{
	backend		(e1,e2,e3,file,line,function);
}
void __cdecl xrDebug::fatal(const char *file, int line, const char *function, const char* F,...)
{
	string1024	buffer;

	va_list		p;
	va_start	(p,F);
	vsprintf	(buffer,F,p);
	va_end		(p);

	backend		("fatal error","<no expression>",buffer,file,line,function);
}
int __cdecl _out_of_memory	(size_t size)
{
	Debug.fatal				(DEBUG_INFO,"Out of memory. Memory request: %d K",size/1024);
	return					1;
}
void __cdecl _terminate		()
{
	FATAL					("Unexpected application termination");
}

extern LPCSTR log_name();

void CALLBACK PreErrorHandler	(INT_PTR)
{
	string256				log_folder;
	FS.update_path			(log_folder,"$logs$","");
	BT_SetReportFilePath	(log_folder);
	Msg						("BT_SetReportFilePath(%s)",log_folder);
	BT_AddLogFile			(log_name());
	Msg						("BT_AddLogFile(%s)",log_name());
}

void SetupExceptionHandler	()
{
	BT_SetPreErrHandler		(PreErrorHandler,0);
	BT_SetAppName			("XRay Engine");
	BT_SetActivityType		(BTA_SAVEREPORT);
	BT_SetReportFormat		(BTRF_TEXT);
	BT_SetFlags				(BTF_DETAILEDMODE | /**BTF_EDIETMAIL | /**/BTF_ATTACHREPORT /**| BTF_LISTPROCESSES /**/| BTF_SHOWADVANCEDUI | BTF_SCREENCAPTURE);
	BT_SetDumpType			(
		MiniDumpNormal |
//		MiniDumpWithDataSegs |
//		MiniDumpWithFullMemory |
//		MiniDumpWithHandleData |
//		MiniDumpFilterMemory |
//		MiniDumpScanMemory |
//		MiniDumpWithUnloadedModules |
//.		MiniDumpWithIndirectlyReferenced |
//		MiniDumpFilterModulePaths |
//		MiniDumpWithProcessThreadData |
//		MiniDumpWithPrivateReadWriteMemory |
//		MiniDumpWithoutOptionalData |
//.		MiniDumpWithFullMemoryInfo |
//.		MiniDumpWithThreadInfo |
//.		MiniDumpWithCodeSegs |
		0
	);
//	BT_SetSupportEMail		("iassenev@gmail.com");
//	BT_SetSupportServer		("localhost", 9999);
//	BT_SetSupportURL		("http://www.your-web-site.com");
}

#if 1
extern void BuildStackTrace(struct _EXCEPTION_POINTERS *pExceptionInfo);
typedef LONG WINAPI UnhandledExceptionFilterType(struct _EXCEPTION_POINTERS *pExceptionInfo);
static UnhandledExceptionFilterType	*previous_filter = 0;
extern char g_stackTrace[100][256];
extern int g_stackTraceCount;

LONG WINAPI UnhandledFilter	(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
	CONTEXT					save = *pExceptionInfo->ContextRecord;
	BuildStackTrace			(pExceptionInfo);
	*pExceptionInfo->ContextRecord = save;

	string4096				buffer;

	Msg						("Stack trace:");
	update_clipboard		("Stack trace:\r\n\r\n");

	for (int i=0; i<g_stackTraceCount; ++i) {
		Msg					("%s",g_stackTrace[i]);
		sprintf				(buffer,"%s\r\n",g_stackTrace[i]);
		update_clipboard	(buffer);
	}
	FlushLog				();

	if (!previous_filter)
		return				(EXCEPTION_CONTINUE_SEARCH) ;

	previous_filter			(pExceptionInfo);

	return					(EXCEPTION_CONTINUE_SEARCH) ;
}
#endif

//////////////////////////////////////////////////////////////////////
#ifdef M_BORLAND
	namespace std{
		extern new_handler _RTLENTRY _EXPFUNC set_new_handler( new_handler new_p );
	};

	static void __cdecl def_new_handler() 
    {
		FATAL		("Out of memory.");
    }

    void	xrDebug::_initialize		()
    {
        std::set_new_handler			(def_new_handler);	// exception-handler for 'out of memory' condition
//		::SetUnhandledExceptionFilter	(UnhandledFilter);	// exception handler to all "unhandled" exceptions
    }
#else
    typedef int		(__cdecl * _PNH)( size_t );
    _CRTIMP int		__cdecl _set_new_mode( int );
    _CRTIMP _PNH	__cdecl _set_new_handler( _PNH );

    void	xrDebug::_initialize		()
    {
		handler							= 0;
        _set_new_mode					(1);					// gen exception if can't allocate memory
        _set_new_handler				(_out_of_memory	);		// exception-handler for 'out of memory' condition
		std::set_terminate				(_terminate);
		std::set_unexpected				(_terminate);
		SetupExceptionHandler			();
		previous_filter					= ::SetUnhandledExceptionFilter(UnhandledFilter);	// exception handler to all "unhandled" exceptions
	}
#endif