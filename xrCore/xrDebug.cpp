#include "StdAfx.h"
#include "xrdebug.h"
#include "resource.h"
#include "dbghelp.h"

#ifdef __BORLANDC__
	#include "dxerr8.h"
	#pragma comment(lib,"dxerr8.lib")
	static BOOL			bException	= TRUE;
#else
	#include "dxerr9.h"
	#pragma comment(lib,"dxerr9.lib")
	static BOOL			bException	= FALSE;
#endif

XRCORE_API	xrDebug		Debug;

// Dialog support
static const char * dlgExpr		= NULL;
static const char * dlgFile		= NULL;
static char			dlgLine		[16];

static BOOL CALLBACK DialogProc	( HWND hw, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		{
			if (dlgFile)
			{
				SetWindowText(GetDlgItem(hw,IDC_DESC),dlgExpr);
				SetWindowText(GetDlgItem(hw,IDC_FILE),dlgFile);
				SetWindowText(GetDlgItem(hw,IDC_LINE),dlgLine);
			} else {
				SetWindowText(GetDlgItem(hw,IDC_DESC),dlgExpr);
				SetWindowText(GetDlgItem(hw,IDC_FILE),"");
				SetWindowText(GetDlgItem(hw,IDC_LINE),"");
			}
		}
		break;
	case WM_DESTROY:
		break;
	case WM_CLOSE:
		EndDialog	(hw, IDC_STOP);
		break;
	case WM_COMMAND:
		if( LOWORD(wp)==IDC_STOP ) {
			EndDialog(hw, IDC_STOP);
		}
		if( LOWORD(wp)==IDC_DEBUG) {
			EndDialog(hw, IDC_DEBUG);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void xrDebug::backend(const char* reason, const char *file, int line) 
{
	static	xrCriticalSection	CS;

	CS.Enter			();

	// Log
#pragma todo("must add logging-line in xrDebug::backend")
	// Msg					("***STOP*** file '%s', line %d.\n***Reason***: %s",file,line,reason);

	// Call the dialog
	dlgExpr		= reason;	
	dlgFile		= file;
	sprintf		(dlgLine,"%d",line);
	INT_PTR res	= DialogBox
		(
		GetModuleHandle(MODULE_NAME),
		MAKEINTRESOURCE(IDD_STOP),
		NULL,
		DialogProc 
		);

	switch (res) 
	{
	case IDC_STOP:
		if (bException)		TerminateProcess(GetCurrentProcess(),3);
		else				RaiseException	(0, 0, 0, NULL);
		break;
	case IDC_DEBUG:
		__asm { int 3 };
		break;
	}

	CS.Leave			();
}


void xrDebug::error		(HRESULT hr, const char* expr, const char *file, int line)
{
	string1024	buffer;
	string1024	reason;

#ifdef __BORLANDC__    
	const char *desc	= DXGetErrorString8A		(hr);
#else
	const char *desc	= DXGetErrorDescription9	(hr);
#endif
	if (desc==0) 
	{
		FormatMessage	(FORMAT_MESSAGE_FROM_SYSTEM,0,hr,0,buffer,1024,0);
		desc			= buffer;
	}

	sprintf		(reason,"*** API-failure ***\n%s\nExpression: %s",desc,expr);
	backend		(reason,file,line);
}

void xrDebug::fail		(const char *e1, const char *file, int line)
{
	string1024	reason;
	sprintf		(reason,"*** Assertion failed ***\nExpression: %s\n",e1);
	backend		(reason,file,line);
}
void xrDebug::fail		(const char *e1, const char *e2, const char *file, int line)
{
	string1024	reason;
	sprintf		(reason,"*** Assertion failed ***\nExpression: %s\n%s",e1,e2);
	backend		(reason,file,line);
}
void xrDebug::fail		(const char *e1, const char *e2, const char *e3, const char *file, int line)
{
	string1024	reason;
	sprintf		(reason,"*** Assertion failed ***\nExpression: %s\n%s\n%s",e1,e2,e3);
	backend		(reason,file,line);
}

void __cdecl xrDebug::fatal(const char* F,...)
{
	string1024	buffer;
	string1024	reason;

	va_list		p;
	va_start	(p,F);
	vsprintf	(buffer,F,p);
	va_end		(p);

	sprintf		(reason,"*** Fatal Error ***\n%s",buffer);
	backend		(reason,0,0);
}

int __cdecl _out_of_memory(unsigned size)
{
	Debug.fatal	("Out of memory. Memory request: %d K",size/1024);
	return 1;
}

// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
										 CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
										 CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
										 CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
										 );


void	timestamp	(string64& dest)
{
	string64	temp;

	/* Set time zone from TZ environment variable. If TZ is not set,
	* the operating system is queried to obtain the default value 
	* for the variable. 
	*/
	_tzset		();
	u32			it;

	// date
	_strdate	( temp );
	for (it=0; it<strlen(temp); it++)
		if ('/'==temp[it]) temp[it]='-';
	strconcat	( dest, temp, "_" );

	// time
	_strtime	( temp );
	for (it=0; it<strlen(temp); it++)
		if (':'==temp[it]) temp[it]='-';
	strcat		( dest, temp);
}

LONG UnhandledFilter	( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
	LONG retval		= EXCEPTION_CONTINUE_SEARCH;
	bException		= TRUE;

	// firstly see if dbghelp.dll is around and has the function we need
	// look next to the EXE first, as the one in System32 might be old 
	// (e.g. Windows 2000)
	HMODULE hDll	= NULL;
	char szDbgHelpPath[_MAX_PATH];

	if (GetModuleFileName( NULL, szDbgHelpPath, _MAX_PATH ))
	{
		char *pSlash = strchr( szDbgHelpPath, '\\' );
		if (pSlash)
		{
			strcpy	(pSlash+1, "DBGHELP.DLL" );
			hDll = ::LoadLibrary( szDbgHelpPath );
		}
	}

	if (hDll==NULL)
	{
		// load any version we can
		hDll = ::LoadLibrary( "DBGHELP.DLL" );
	}

	LPCTSTR szResult = NULL;

	if (hDll)
	{
		MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
		if (pDump)
		{
			char		szDumpPath	[_MAX_PATH];
			char		szScratch	[_MAX_PATH];
			string64	t_stemp;

			// work out a good place for the dump file
			timestamp	(t_stemp);
			strcpy		( szDumpPath, "logs\\"				);
			strcat		( szDumpPath, Core.ApplicationName	);
			strcat		( szDumpPath, "_"					);
			strcat		( szDumpPath, Core.UserName			);
			strcat		( szDumpPath, "_"					);
			strcat		( szDumpPath, t_stemp				);
			strcat		( szDumpPath, ".mdmp"				);

			// create the file
			HANDLE hFile = ::CreateFile( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

			if (hFile!=INVALID_HANDLE_VALUE)
			{
				_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

				ExInfo.ThreadId				= ::GetCurrentThreadId();
				ExInfo.ExceptionPointers	= pExceptionInfo;
				ExInfo.ClientPointers		= NULL;

				// write the dump
				MINIDUMP_TYPE	dump_flags	= MINIDUMP_TYPE(MiniDumpNormal | MiniDumpFilterMemory | MiniDumpScanMemory );

				BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, dump_flags, &ExInfo, NULL, NULL );
				if (bOK)
				{
					sprintf( szScratch, "Saved dump file to '%s'", szDumpPath );
					szResult = szScratch;
					retval = EXCEPTION_EXECUTE_HANDLER;
				}
				else
				{
					sprintf( szScratch, "Failed to save dump file to '%s' (error %d)", szDumpPath, GetLastError() );
					szResult = szScratch;
				}
				::CloseHandle(hFile);
			}
			else
			{
				sprintf( szScratch, "Failed to create dump file '%s' (error %d)", szDumpPath, GetLastError() );
				szResult = szScratch;
			}
		}
		else
		{
			szResult = "DBGHELP.DLL too old";
		}
	}
	else
	{
		szResult = "DBGHELP.DLL not found";
	}

	string1024		reason;
	sprintf			(reason,"*** Internal Error ***\n%s",szResult);
	Debug.backend	(reason,0,0);

	return retval;
}

//////////////////////////////////////////////////////////////////////
#ifdef M_BORLAND
//	typedef void ( _RTLENTRY *___new_handler) ();
	extern new_handler _RTLENTRY _EXPFUNC set_new_handler( new_handler new_p );

//    typedef int	(__stdcall * _PNH)( size_t );
//    _CRTIMP int	__cdecl _set_new_mode( int );
//    _PNH	__cdecl set_new_handler( _PNH );
//	typedef void (new * new_handler)();
//	new_handler set_new_handler(new_handler my_handler);
	static void __cdecl def_new_handler() 
    {
		Debug.fatal	("Out of memory.");
    }

    void	xrDebug::_initialize		()
    {
//        _set_new_mode					(1);					// gen exception if can't allocate memory
        std::set_new_handler			(def_new_handler  );	// exception-handler for 'out of memory' condition
        ::SetUnhandledExceptionFilter	( UnhandledFilter );	// exception handler to all "unhandled" exceptions
    }
#else
    typedef int		(__cdecl * _PNH)( size_t );
    _CRTIMP int		__cdecl _set_new_mode( int );
    _CRTIMP _PNH	__cdecl _set_new_handler( _PNH );

    void	xrDebug::_initialize		()
    {
        _set_new_mode					(1);					// gen exception if can't allocate memory
        _set_new_handler				(_out_of_memory	);		// exception-handler for 'out of memory' condition
        ::SetUnhandledExceptionFilter	( UnhandledFilter );	// exception handler to all "unhandled" exceptions
    }
#endif

