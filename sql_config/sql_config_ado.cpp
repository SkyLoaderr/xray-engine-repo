#include "MyADO.h"

bool CreateInitialString(CString& dest, char* sName, char* db_name)
{
	dest.Format("Provider=SQLOLEDB.1;Data Source=%s;Initial Catalog=%s;", sName, db_name);
	return true;
}

extern "C"{
	__declspec(dllexport) int __cdecl ado_testConnection(char* sName, char* user_name, char* pwd)
	{
		CString conn;
		CMyADO MyADOObject;
		CreateInitialString(conn,sName,"master");
		HRESULT res = MyADOObject.Open( conn.GetBuffer(), user_name, pwd );
				if(res!=S_OK)
					return (-1);
				res	= MyADOObject.Close();
				if(res!=S_OK)
					return (-1);

		return (0);

	}
	__declspec(dllexport) int __cdecl ado_attachDatabase(char* sName, char* user_name, char* pwd, char* db_name, char* file1, char* file2)
	{
		CString conn;
		CMyADO MyADOObject;
		CreateInitialString(conn,sName,"master");
		HRESULT res = MyADOObject.Open( conn.GetBuffer(), user_name, pwd );

		res = MyADOObject.Initialize( "dbo.sp_attach_db" );
		res = MyADOObject.AddParameterInputText( "dbname", db_name );
		res = MyADOObject.AddParameterInputText( "filename1", file1 );//mdf
		res = MyADOObject.AddParameterInputText( "filename2", file2 );//ldf
		res =  MyADOObject.Execute();
		MyADOObject.Close();

		return (0);
	}

};
/*
int main ()
{
	int r = ado_testConnection("localhost\\kas_sql_srv","sa","KAStorka40");
	if (r==0)
		printf ("All OK");
	else
		printf ("Failed");

}*/