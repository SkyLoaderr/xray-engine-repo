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
		return false;
	}
	return true;
}

bool _Disconnect(LPSQLDMOSERVER pSQLServer)
{
	USES_CONVERSION;
	HRESULT hr;
	if ( (hr=pSQLServer->DisConnect() ) != S_OK)
	{	
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

		_Disconnect(pSQLServer);
		pSQLServer->Release();
		return (0);
	}

	__declspec(dllexport) int __cdecl tuneServer(char* sName, char* user_name, char* pwd)
	{
		USES_CONVERSION;


		LPSQLDMOSERVER pSQLServer;
		if ( !GetSQLServer(&pSQLServer) )
			return (-1);

		if ( !_Connect(pSQLServer,sName, user_name, pwd) ){
			pSQLServer->Release();
			return (-1);
		}

		LPSQLDMOINTEGRATEDSECURITY pSec;
		pSQLServer->GetIntegratedSecurity(&pSec);
		if(pSec){
			pSec->SetSecurityMode(SQLDMOSecurity_Normal);
			pSec->Release();
		}


		LPSQLDMOJOBSERVER       pJobServer = NULL;
		pSQLServer->GetJobServer(&pJobServer);

		if(pJobServer){
			pJobServer->SetAutoStart(TRUE);
			pJobServer->Release();
		}

		_Disconnect(pSQLServer);
		pSQLServer->Release();

		return (0);
	}

	__declspec(dllexport) int __cdecl attachDatabase(char* sName, char* user_name, char* pwd, char* db_name, char* files)
	{
		USES_CONVERSION;


		LPSQLDMOSERVER pSQLServer;
		if ( !GetSQLServer(&pSQLServer) )
			return (-1);
		

		if ( !_Connect(pSQLServer,sName, user_name, pwd) ){
			pSQLServer->Release();
			return (-1);
		}

		BSTR sRes;
		if( FAILED(pSQLServer->AttachDB(A2W(db_name),A2W(files),&sRes) ) )
		{
			_Disconnect(pSQLServer);
			pSQLServer->Release();
			return (-1);
		}

		_Disconnect(pSQLServer);
		pSQLServer->Release();
		return (0);
	}
	__declspec(dllexport) int __cdecl detachDatabase(char* sName, char* user_name, char* pwd, char* db_name)
	{
		USES_CONVERSION;


		LPSQLDMOSERVER pSQLServer;
		if (! GetSQLServer(&pSQLServer) )
			return (-1);

		if ( !_Connect(pSQLServer,sName, user_name, pwd) )
			pSQLServer->Release();
			return (-1);

		BSTR sRes;
		if( FAILED(pSQLServer->DetachDB(A2W(db_name),&sRes) ) )
		{
			_Disconnect(pSQLServer);
			pSQLServer->Release();
			return (-1);
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

		if ( !_Connect(pSQLServer, sName, user_name, pwd) ){
			pSQLServer->Release();
			return (-1);
		}

		LPSQLDMODATABASE ppDatabase;
		if( FAILED(pSQLServer->GetDatabaseByName(A2W(db_name),&ppDatabase) ) )
		{
			_Disconnect(pSQLServer);
			pSQLServer->Release();
			return (-1);
		}
		
		if( FAILED(ppDatabase->ExecuteImmediate(A2W(sql_line)) ))
		{
			_Disconnect(pSQLServer);
			pSQLServer->Release();
			return (-1);
		}

		ppDatabase->Release();
		pSQLServer->Release();
		return (0);

	};


};
/*
void test();
int main ()
{
	initLibrary();
	test();
	deInitLibrary();
}
#include "MyADO.h"
void test()
{

char conn[] =    "Provider=SQLOLEDB.1;Data Source=localhost\\kas_sql_srv;Initial Catalog=KAS;";

	CMyADO MyADOObject;
	HRESULT res = MyADOObject.Open( conn, "sa", "KAStorka40" );
	MyADOObject.Close();
}
*/