// MyADO.h: interface for the CMyADO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYADO_H__E144D98C_2388_4800_BC00_F7E963816A73__INCLUDED_)
#define AFX_MYADO_H__E144D98C_2388_4800_BC00_F7E963816A73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#import <msado15.dll> no_namespace rename( "EOF", "adoEOF" )
#include "atlstr.h"

#import "i:\Program Files\Common Files\System\ADO\msado15.dll" \
              rename("EOF", "EndOfFile")


//#import "c:\Program Files\Common Files\System\ADO\msado15.dll" \
//              rename("EOF", "EndOfFile")

using namespace ADODB; 

struct ERROR_HANDLE_ADO
{
	CString sErrorMessage;
	CString sErrorSourse;
	CString sErrorDescription;
};

class CMyADO  
{
public:
	CMyADO();
	virtual ~CMyADO();

	void SetError(_com_error &e);
	CString GetError();
	ERROR_HANDLE_ADO m_error;


	HRESULT Open( CString btConnectionString, CString btUserID, CString btPassword );
	HRESULT Open();
	HRESULT Close();

	HRESULT AddParameterReturnValue();
	HRESULT AddParameterInputLong( CString btParameterName, long lValue );
	HRESULT AddParameterInputText( CString btParameterName, CString btValue );

	HRESULT AddParameterInputOutputLong( CString btParameterName, long lValue );
	HRESULT AddParameterInputOutputText( CString btParameterName, CString btValue, DWORD dwMaxTextSize );

	HRESULT AddParameterOutputLong( CString btParameterName );
	HRESULT AddParameterOutputText( CString btParameterName, DWORD dwMaxTextSize );

	HRESULT Execute();
	HRESULT Execute2();

	HRESULT GetFieldLong( CString btFieldName, long* plValue );
	HRESULT GetFieldText( CString btFieldName, CString* szText, DWORD dwMaxTextSize );

	HRESULT GetParameterReturnValue( long* plReturnValue );

	HRESULT GetParameterLong( CString btParameterName, long* plValue );
	HRESULT GetParameterText( CString btParameterName, CString* szText, DWORD dwMaxTextSize );

	void SetConnectionString(CString str);
	HRESULT Initialize ( CString btStoredProcedureName );
	HRESULT Initialize2( CString btStoredProcedureName );

	BOOL IsEOF();

	HRESULT MoveNext();


	HRESULT GetRecordCount( long* lRecordCount );


	HRESULT AddParameter( CString btParameterName, DataTypeEnum enDataType, ParameterDirectionEnum enParameterDirection, long lSize, _variant_t vtValue );
	HRESULT GetField( _variant_t vtFieldName, _variant_t& vtValue );
	HRESULT GetParameter( _variant_t vtParameterName, _variant_t& vtValue );

	BOOL IsConnected();
	BOOL IsInitialized();

	_ConnectionPtr m_pConnectionPtr;
	_CommandPtr m_pCommandPtr;
	_RecordsetPtr m_pRecordsetPtr;

	CString m_strConnection;
	CString m_strUser;
	CString m_strPwd;


};

#endif // !defined(AFX_MYADO_H__E144D98C_2388_4800_BC00_F7E963816A73__INCLUDED_)
