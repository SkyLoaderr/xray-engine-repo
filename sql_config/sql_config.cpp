// sql_config.cpp : Defines the entry point for the DLL application.
//

#include "windows.h"
#include <oleauto.h>
#include <atlconv.h>
#include <initguid.h>
#include "sqldmo.h"
#include "sqldmoid.h"

bool GetSQLServer(LPSQLDMOSERVER* pSQLServer){
	if FAILED(CoCreateInstance(CLSID_SQLDMOServer, NULL, CLSCTX_INPROC_SERVER,
		IID_ISQLDMOServer, (LPVOID*)pSQLServer))
	{
		MessageBox( NULL, "Unable to create SQLDMOServer instance", "Error", MB_OK | MB_ICONINFORMATION );
		return false;
	}
	return true;
}


bool _Connect(LPSQLDMOSERVER pSQLServer, char* sName, char* user_name, char* pwd)
{
	USES_CONVERSION;
	HRESULT hr;
	if ( (hr=pSQLServer->Connect(A2W(sName), A2W(user_name), A2W(pwd) )) != S_OK)
	{	
//		MessageBox( NULL, "Connect to SQL Server failed", "Error", MB_OK | MB_ICONINFORMATION );
		return false;
	}
	return true;
}

extern "C"{
	__declspec(dllexport) bool __cdecl initLibrary(){
		if FAILED(CoInitialize (NULL))
			return (false);
		return true;
		}
	__declspec(dllexport) bool __cdecl deInitLibrary(){
		CoUninitialize();
		return true;
		}
	__declspec(dllexport) int __cdecl testConnection(char* sName, char* user_name, char* pwd)
	{
		USES_CONVERSION;

		LPSQLDMOSERVER pSQLServer;
		if ( !GetSQLServer(&pSQLServer) )
			return (-1);

		if ( !_Connect(pSQLServer,sName, user_name, pwd) ){
			pSQLServer->Release();
			return (-1);
		}

		pSQLServer->Release();
		return 0;
	}

	__declspec(dllexport) int __cdecl tuneServer(char* sName, char* user_name, char* pwd)
	{
		USES_CONVERSION;


		LPSQLDMOSERVER pSQLServer;
		if ( !GetSQLServer(&pSQLServer) )
			return (-1);

		if ( !_Connect(pSQLServer,sName, user_name, pwd) )
			return (-1);

		LPSQLDMOINTEGRATEDSECURITY pSec;
		pSQLServer->GetIntegratedSecurity(&pSec);
		pSec->SetSecurityMode(SQLDMOSecurity_Normal);


		LPSQLDMOJOBSERVER       pJobServer = NULL;
		pSQLServer->GetJobServer(&pJobServer);

		if(pJobServer){
			pJobServer->SetAutoStart(TRUE);
			pJobServer->Release();
		}


		pSQLServer->Release();

		return 0;
	}

	__declspec(dllexport) int __cdecl attachDatabase(char* sName, char* user_name, char* pwd, char* db_name, char* files)
	{
		USES_CONVERSION;


		LPSQLDMOSERVER pSQLServer;
		if ( !GetSQLServer(&pSQLServer) )
			return (-1);

		if ( !_Connect(pSQLServer,sName, user_name, pwd) )
			return (-1);

		BSTR sRes;
		if( FAILED(pSQLServer->AttachDB(A2W(db_name),A2W(files),&sRes) ) )
		{
			MessageBox( NULL, "AttachDB failed", "Error", MB_OK | MB_ICONINFORMATION );
			return (-1);
		}


		pSQLServer->Release();
		return 0;
	}
	__declspec(dllexport) int __cdecl detachDatabase(char* sName, char* user_name, char* pwd, char* db_name)
	{
		USES_CONVERSION;


		LPSQLDMOSERVER pSQLServer;
		if (! GetSQLServer(&pSQLServer) )
			return (-1);

		if ( !_Connect(pSQLServer,sName, user_name, pwd) )
			return (-1);

		BSTR sRes;
		if( FAILED(pSQLServer->DetachDB(A2W(db_name),&sRes) ) )
		{
			MessageBox( NULL, "DetachDB failed", "Error", MB_OK | MB_ICONINFORMATION );
			return -1;
		}


		pSQLServer->Release();
		return 0;
	}

	__declspec(dllexport) int __cdecl runSQLString(char* sName, char* user_name, char* pwd, char* db_name, char* sql_line)
	{
		USES_CONVERSION;

		LPSQLDMOSERVER pSQLServer;
		if ( !GetSQLServer(&pSQLServer) )
			return (-1);

		if ( !_Connect(pSQLServer, sName, user_name, pwd) )
			return (-1);

		LPSQLDMODATABASE ppDatabase;
		if( FAILED(pSQLServer->GetDatabaseByName(A2W(db_name),&ppDatabase) ) )
		{
			return (-1);
		}
		
		if( FAILED(ppDatabase->ExecuteImmediate(A2W(sql_line)) ))
		{
			return (-1);
		}

		ppDatabase->Release();
		pSQLServer->Release();
		return 0;

	};


};

int main ()
{
	initLibrary();

	deInitLibrary();
//	tuneServer("localhost","sa","tester");
//	attachDatabase("localhost","sa","tester","aaaaaa","c:\\client_repl_Data.mdf c:\\client_repl_Log.ldf");
//	runSQLString("localhost","sa","tester","master", "EXEC sp_addextendedproc xp_check, 'xp_indexing.dll'" );
/*
	typedef int	 __cdecl LauncherFunc	(int);

	LauncherFunc*	pLauncher		= NULL;

	HMODULE hm = LoadLibrary("x:\\xrLauncher.dll");
	if (0==hm)	{
		LPVOID lpMsgBuf;
		if (!FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL ))
		{
			// Handle the error.
			MessageBox( NULL, "Unknown error", "Error", MB_OK | MB_ICONINFORMATION );
			return 1;
		}

		// Process any inserts in lpMsgBuf.
		// ...

		// Display the string.
		MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );

		// Free the buffer.
		LocalFree( lpMsgBuf );
		return 1;
	}
//	if(!hm)
//		MessageBox( NULL, "xrLauncher DLL raised exception during loading or there is no xrLauncher.dll at all", "Error", MB_OK | MB_ICONINFORMATION );

	pLauncher = (LauncherFunc*)GetProcAddress(hm,"RunXRLauncher");
	if(!pLauncher)
		MessageBox( NULL, "Cannot obtain RunXRLauncher function from xrLauncher.dll", "Error", MB_OK | MB_ICONINFORMATION );

	MessageBox( NULL, "All OK", "Error", MB_OK | MB_ICONINFORMATION );
*/
}
