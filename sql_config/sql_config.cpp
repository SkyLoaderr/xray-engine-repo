// sql_config.cpp : Defines the entry point for the DLL application.
//

#include "windows.h"
#include <oleauto.h>
#include <atlconv.h>
#include <initguid.h>
#include <sqldmo.h>
#include <sqldmoid.h>

extern "C"{

	__declspec(dllexport) int __cdecl tuneServer(char* sName, char* user_name, char* pwd)
	{
		USES_CONVERSION;
		if FAILED(CoInitialize (NULL))
			return (-1);


		LPSQLDMOSERVER pSQLServer;

		if FAILED(CoCreateInstance(CLSID_SQLDMOServer, NULL, CLSCTX_INPROC_SERVER,
			IID_ISQLDMOServer, (LPVOID*)&pSQLServer))
		{
			return(-1);
		}

		HRESULT hr;
		if ( (hr=pSQLServer->Connect(A2W(sName), A2W(user_name), A2W(pwd) )) != S_OK)
		{	
			return(-1);
		}

		LPSQLDMOINTEGRATEDSECURITY pSec;
		pSQLServer->GetIntegratedSecurity(&pSec);
		pSec->SetSecurityMode(SQLDMOSecurity_Normal);


		LPSQLDMOJOBSERVER       pJobServer = NULL;
		pSQLServer->GetJobServer(&pJobServer);

		if(pJobServer){
			pJobServer->SetAutoStart(TRUE);
			pJobServer->Release();// ?????
		}


		pSQLServer->Release();

		CoUninitialize();
		return 0;
	}

	__declspec(dllexport) int __cdecl attachDatabase(char* sName, char* user_name, char* pwd, char* db_name, char* files)
	{
		USES_CONVERSION;
		if FAILED(CoInitialize (NULL))
			return (-1);


		LPSQLDMOSERVER pSQLServer;
		if FAILED(CoCreateInstance(CLSID_SQLDMOServer, NULL, CLSCTX_INPROC_SERVER,
			IID_ISQLDMOServer, (LPVOID*)&pSQLServer))
		{
			return(-1);
		}

		HRESULT hr;
		if ( (hr=pSQLServer->Connect(A2W(sName), A2W(user_name), A2W(pwd) )) != S_OK)
		{	
			return(-1);
		}
		BSTR sRes;
		if( FAILED(pSQLServer->AttachDB(A2W(db_name),A2W(files),&sRes) ) )
		{
			return -1;
		}


		pSQLServer->Release();

		CoUninitialize();
		return 0;
	}

	__declspec(dllexport) int __cdecl runSQLString(char* sName, char* user_name, char* pwd, char* db_name, char* sql_line)
	{
		USES_CONVERSION;
		if FAILED(CoInitialize (NULL))
			return (-1);

		LPSQLDMOSERVER pSQLServer;
		if FAILED(CoCreateInstance(CLSID_SQLDMOServer, NULL, CLSCTX_INPROC_SERVER,
			IID_ISQLDMOServer, (LPVOID*)&pSQLServer))
		{
			return(-1);
		}

		HRESULT hr;
		if ( (hr=pSQLServer->Connect(A2W(sName), A2W(user_name), A2W(pwd) )) != S_OK)
		{	
			return(-1);
		}

		LPSQLDMODATABASE ppDatabase;
		if( FAILED(pSQLServer->GetDatabaseByName(A2W(db_name),&ppDatabase) ) )
		{
			return -1;
		}
		
		if( FAILED(ppDatabase->ExecuteImmediate(A2W(sql_line)) ))
		{
			return -1;
		}

		ppDatabase->Release();
		pSQLServer->Release();
		CoUninitialize();
		return 0;

	};


};

int main ()
{
//	tuneServer("localhost","sa","tester");
//	attachDatabase("localhost","sa","tester","aaaaaa","c:\\client_repl_Data.mdf c:\\client_repl_Log.ldf");
	runSQLString("localhost","sa","tester","master", "EXEC sp_addextendedproc xp_check, 'xp_indexing.dll'" );
}
