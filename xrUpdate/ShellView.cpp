#include "stdafx.h"
#include "xrUpdate.h"

HANDLE hChildStdoutRd, hChildStdoutWr, hChildStdoutRdDup, hSaveStdout;  
void AddTexts(LPCTSTR string);
void AddTexts(TCHAR ch);


BOOL CxrUpdateApp::CreateShellRedirect()
{
	SECURITY_ATTRIBUTES saAttr;
	BOOL fSuccess;

	// Set the bInheritHandle flag so pipe handles are inherited.
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	// The steps for redirecting child process's STDOUT: 
	//     1. Save current STDOUT, to be restored later. 
	//     2. Create anonymous pipe to be STDOUT for child process. 
	//     3. Set STDOUT of the parent process to be write handle to 
	//        the pipe, so it is inherited by the child process. 
	//     4. Create a noninheritable duplicate of the read handle and
	//        close the inheritable read handle.  
	
	// Save the handle to the current STDOUT.  
	hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE);  
	
	// Create a pipe for the child process's STDOUT.  
	if( !CreatePipe( &hChildStdoutRd, &hChildStdoutWr, &saAttr, 0) )
	{
		TRACE0( _T("Stdout pipe creation failed\n") );
		return FALSE;
	}

	// Set a write handle to the pipe to be STDOUT.  
	if( !SetStdHandle(STD_OUTPUT_HANDLE, hChildStdoutWr)/*Wr*/ )
	{
		TRACE0( _T("Redirecting STDOUT failed\n") );
		return FALSE;
	}
   
/*
	// Create noninheritable read handle and close the inheritable read handle. 
    fSuccess = DuplicateHandle( GetCurrentProcess(), hChildStdoutRd,
        GetCurrentProcess(),  &hChildStdoutRdDup , 
		0,  FALSE,
		DUPLICATE_SAME_ACCESS );
	if( !fSuccess )
	{
		TRACE0( _T("DuplicateHandle failed\n") );
        return FALSE;
	}

	CloseHandle( hChildStdoutRd );
*/	

	m_pReadThread = AfxBeginThread( (AFX_THREADPROC)ReadPipeThreadProc,(LPVOID)this );
	if( !m_pReadThread )
	{
		TRACE0( _T("Cannot start read-redirect thread!\n") );
		return FALSE;
	}
	return TRUE;
}


#define BUFSIZE 2048

UINT CxrUpdateApp::ReadPipeThreadProc( LPVOID pParam )
{
	DWORD dwRead;
	TCHAR chBuf[BUFSIZE]; 

	TRACE0( _T("ReadPipe Thread begin run\n") );
	for( ;; )    
	{ 
		if( !ReadFile( hChildStdoutRd, chBuf, 
			BUFSIZE, &dwRead, NULL) || dwRead == 0) 
			break; 
		chBuf[dwRead/sizeof(TCHAR)] = _T('\0');
		AddTexts( chBuf );
	} 
	CloseHandle( hChildStdoutWr);
	CloseHandle( hChildStdoutRdDup );
	return 1;
}

void AddTexts(LPCTSTR string)
{
	SendMessage(g_app_wnd,ADD_LOG_MSG,(WPARAM)string,0);
}

void AddTexts(TCHAR ch)
{
/*
	TCHAR string[2];
	string[0] = ch;
	string[1] = _T('\0');
	AddTexts( (LPCTSTR)string );
*/
}