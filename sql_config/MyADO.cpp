//#include <stdio.h>
#include <comdef.h>
//#include <conio.h>

#include "MyADO.h"


void CMyADO::SetConnectionString(CString str)
{
	m_strConnection = str;
}
CMyADO::CMyADO()
{
	m_pCommandPtr = NULL;
	m_strUser = "";
	m_strPwd = "";

}

CMyADO::~CMyADO()
{
	if( m_pRecordsetPtr )
	{
		//if( m_pRecordsetPtr->State == adStateOpen )
		//	m_pRecordsetPtr->Close();

		//m_pRecordsetPtr = NULL;
	}
	//m_pRecordsetPtr->Close();

	//m_pCommandPtr = NULL;

}



void CMyADO::SetError(_com_error &e)
{
	m_error.sErrorMessage = (LPCTSTR)e.ErrorMessage();
	m_error.sErrorDescription = (LPCTSTR)e.Description();
	m_error.sErrorSourse = (LPCTSTR)e.Source();
}

CString CMyADO::GetError()
{
	return m_error.sErrorMessage+"\n\r"
		+  m_error.sErrorDescription+"\n\r"
		+  m_error.sErrorSourse+"\n\r";
}

//	Create a Parameter and Add it to the CommandPtr Object (Which will be used to Execute the Stored Procedure)
HRESULT CMyADO::AddParameter( CString btParameterName, DataTypeEnum enDataType, ParameterDirectionEnum enParameterDirection, long lSize, _variant_t vtValue )
{
	HRESULT hReturn = S_FALSE;

	if( IsConnected() && IsInitialized())
	{
		try
		{
			ADODB::_ParameterPtr pParameterPtr = m_pCommandPtr->CreateParameter( (LPCTSTR)btParameterName, enDataType, enParameterDirection, lSize, vtValue );
			m_pCommandPtr->Parameters->Append( pParameterPtr );

			hReturn = S_OK;
		}
		catch( _com_error& eComError )
		{
			SetError(eComError);
			//char szErrorMsg[256];
			//_snprintf( szErrorMsg, sizeof( szErrorMsg ), "ERROR in CMyADO::Execute() - %s\n", eComError.ErrorMessage());
			//OutputDebugString( szErrorMsg );
		}
		catch( ... )
		{
		}
	}

	return hReturn;
}

//	Add Parameter Heler Function for Long Type and Input Direction
HRESULT CMyADO::AddParameterInputLong( CString btParameterName, long lValue )
{
	return AddParameter( btParameterName, ADODB::adInteger, adParamInput, sizeof( long ), _variant_t( lValue ));
}

//	Add Parameter Helper Function for Text Type and Input Direction
HRESULT CMyADO::AddParameterInputText( CString btParameterName, CString btValue )
{
	return AddParameter( btParameterName, adVarChar, adParamInput, btValue.GetLength(), _variant_t( btValue ));
}

//	Add Parameter Helper Function for Long Type and Input/Output Direction
HRESULT CMyADO::AddParameterInputOutputLong( CString btParameterName, long lValue )
{
	return AddParameter( btParameterName, adInteger, adParamInputOutput, sizeof( long ), _variant_t( lValue ));
}

//	Add Parameter Helper Function for Text Type and Input/Output Direction
HRESULT CMyADO::AddParameterInputOutputText( CString btParameterName, CString btValue, DWORD dwMaxTextSize )
{
	return AddParameter( btParameterName, adVarChar, adParamInputOutput, dwMaxTextSize, _variant_t( btValue ));
}

//	Add Parameter Helper Function for Long Type and Output Direction
HRESULT CMyADO::AddParameterOutputLong( CString btParameterName )
{
	_variant_t vtNull;

	return AddParameter( btParameterName, adInteger, adParamOutput, 0, vtNull );
}

//	Add Parameter Helper Function for Text Type and Output Direction
HRESULT CMyADO::AddParameterOutputText( CString btParameterName, DWORD dwMaxTextSize )
{
	_variant_t vtNull;

	return AddParameter( btParameterName, adVarChar, adParamOutput, dwMaxTextSize, vtNull );
}

//	Add Parameter Helper Function for Return Value
HRESULT CMyADO::AddParameterReturnValue()
{
	_variant_t vtNull;

	return AddParameter( "RETURN_VALUE", adInteger, adParamReturnValue, 0, vtNull );
}

//	Close the Current ADO Connection
HRESULT CMyADO::Close()
{
	HRESULT hReturn = S_FALSE;

	if( m_pConnectionPtr )
	{
		if( m_pConnectionPtr->State == adStateOpen )
		{
			try
			{
				//m_pRecordsetPtr->Close();
				//m_pConnectionPtr->Close();
				//m_pConnectionPtr = NULL;
				hReturn = S_OK;
			}
			catch( _com_error& eComError )
			{
				SetError(eComError);
				//char szErrorMsg[256];
				//_snprintf( szErrorMsg, sizeof( szErrorMsg ), "ERROR in CMyADO::Close() - %s\n", eComError.ErrorMessage());
				//OutputDebugString( szErrorMsg );
			}
			catch( ... )
			{
			}
		}
		else
			hReturn = S_OK;
	}
	else
		hReturn = S_OK;

	return hReturn;
}
//	Execute the Stored Procedure using the CommandPtr Object
HRESULT CMyADO::Execute()
{
	HRESULT hReturn = S_FALSE;

	if( IsConnected() && IsInitialized())
	{
		try
		{
			m_pRecordsetPtr = m_pCommandPtr->Execute( NULL, NULL, adCmdStoredProc );

			hReturn = S_OK;
		}
		catch( _com_error& eComError )
		{
			MessageBox(NULL,eComError.Description(),_T("Error ADO EXEC"),MB_ICONERROR);
			SetError(eComError);
			//char szErrorMsg[256];
			//_snprintf( szErrorMsg, sizeof( szErrorMsg ), "ERROR in CMyADO::Execute() - %s\n", eComError.ErrorMessage());
			//OutputDebugString( szErrorMsg );
		}
		catch( ... )
		{
		}
	}

	return hReturn;
}

HRESULT CMyADO::Execute2()
{
	HRESULT hReturn = S_FALSE;

	if( IsConnected() && IsInitialized())
	{
		try
		{
			m_pRecordsetPtr = m_pCommandPtr->Execute( NULL, NULL, adCmdText );

			hReturn = S_OK;
		}
		catch( _com_error& eComError )
		{
			SetError(eComError);
			//char szErrorMsg[256];
			//_snprintf( szErrorMsg, sizeof( szErrorMsg ), "ERROR in CMyADO::Execute() - %s\n", eComError.ErrorMessage());
			//OutputDebugString( szErrorMsg );
		}
		catch( ... )
		{
		}
	}

	return hReturn;
}

//	Retrieve a Value from the Recordset (which was created during Stored Procedure Execution)
HRESULT CMyADO::GetField( _variant_t vtFieldName, _variant_t& vtValue )
{
	HRESULT hReturn = S_FALSE;

	if( IsConnected() && IsInitialized())
	{
		try
		{
			vtValue = m_pRecordsetPtr->Fields->GetItem( vtFieldName )->Value;

			hReturn = S_OK;
		}
		catch( _com_error& eComError )
		{
			SetError(eComError);
			//char szErrorMsg[256];
			//_snprintf( szErrorMsg, sizeof( szErrorMsg ), "ERROR in CMyADO::GetField() - %s\n", eComError.ErrorMessage());
			//OutputDebugString( szErrorMsg );
		}
		catch( ... )
		{
			CString a;
		}
	}
	else
	{
		CString a;
	}

	return hReturn;
}

//	Get Field Helper Function for Long Type
HRESULT CMyADO::GetFieldLong( CString btFieldName, long* plValue )
{
	_variant_t vtValue;

	HRESULT hReturn = GetField( (LPCTSTR)btFieldName, vtValue );

	if( hReturn == S_OK )
		*plValue = ( long )vtValue;

	return hReturn;
}

//	Get Field Helper Function for Text Type
HRESULT CMyADO::GetFieldText( CString btFieldName, CString* szText, DWORD dwMaxTextSize )
{
	_variant_t vtValue;

	HRESULT hReturn = GetField( (LPCTSTR)btFieldName, vtValue );

	if( hReturn == S_OK )
	{
		CString btValue = (LPCTSTR)((_bstr_t)vtValue);

		//if( dwMaxTextSize < btValue.GetLength())
		//	hReturn = S_FALSE;
		//else
			//strcpy( szText, btValue );
			*szText = btValue;
	}

	return hReturn;
}

//	Retrieve a Parameter (which was previously set up as either an Output or InputOutput Direction and is set during Stored Procedure Execution)
HRESULT CMyADO::GetParameter( _variant_t vtParameterName, _variant_t& vtValue )
{
	HRESULT hReturn = S_FALSE;

	if( IsConnected() && IsInitialized())
	{
		try
		{
			vtValue = m_pCommandPtr->Parameters->GetItem( vtParameterName )->Value;

			hReturn = S_OK;
		}
		catch( _com_error& eComError )
		{
			SetError(eComError);
			//char szErrorMsg[256];
			//_snprintf( szErrorMsg, sizeof( szErrorMsg ), "ERROR in CMyADO::GetParameter() - %s\n", eComError.ErrorMessage());
			//OutputDebugString( szErrorMsg );
		}
		catch( ... )
		{
		}
	}

	return hReturn;
}

//	Retrieve Parameter Helper Function for Long Type
HRESULT CMyADO::GetParameterLong( CString btParameterName, long* plValue )
{
	_variant_t vtValue;

	HRESULT hReturn = GetParameter( (LPCTSTR)btParameterName, vtValue );

	if( hReturn == S_OK )
		*plValue = ( long )vtValue;

	return hReturn;
}

//	Retrieve Parameter Helper Function for Return Value
HRESULT CMyADO::GetParameterReturnValue( long* plReturnValue )
{
	return GetParameterLong( "RETURN_VALUE", plReturnValue );
}

//	Retrieve Parameter Helper Function for Text Type
HRESULT CMyADO::GetParameterText( CString btParameterName, CString* szText, DWORD dwMaxTextSize )
{
	_variant_t vtValue;

	HRESULT hReturn = GetParameter( (LPCTSTR)btParameterName, vtValue );

	if( hReturn == S_OK )
	{
		CString btValue = (LPCTSTR)(( _bstr_t )vtValue);

		//if( dwMaxTextSize < btValue.GetLength())
		//	hReturn = S_FALSE;
		//else
			//strcpy( szText, btValue );
			*szText = btValue;
	}

	return hReturn;
}

//	Retrieve the Record Count for the Recordset (which was created during Stored Procedure Execution)
HRESULT CMyADO::GetRecordCount( long* lRecordCount )
{
	HRESULT hReturn = S_FALSE;

	if( m_pRecordsetPtr )
	{
		try
		{
			*lRecordCount = m_pRecordsetPtr->RecordCount;
		}
		catch( _com_error& eComError )
		{
			SetError(eComError);
			//char szErrorMsg[256];
			//_snprintf( szErrorMsg, sizeof( szErrorMsg ), "ERROR in CMyADO::GetParameter() - %s\n", eComError.ErrorMessage());
			//OutputDebugString( szErrorMsg );
		}
		catch( ... )
		{
		}
	}

	return hReturn;
}

//	Close the Recordset and Initialize the CommandPtr Object
HRESULT CMyADO::Initialize( CString btStoredProcedureName )
{
	HRESULT hReturn = S_FALSE;

	if( IsConnected())
	{
		m_pCommandPtr = NULL;

		if( m_pRecordsetPtr )
		{
			if( m_pRecordsetPtr->State == adStateOpen )
				m_pRecordsetPtr->Close();

			m_pRecordsetPtr = NULL;
		}

		m_pCommandPtr.CreateInstance( __uuidof( Command ));

		m_pCommandPtr->ActiveConnection = m_pConnectionPtr;
		m_pCommandPtr->CommandText = (LPCTSTR)btStoredProcedureName;
		m_pCommandPtr->CommandType = adCmdStoredProc;
		m_pCommandPtr->CommandTimeout = 500000000;

		hReturn = S_OK;
	}

	return hReturn;
}

HRESULT CMyADO::Initialize2( CString btStoredProcedureName )
{
	HRESULT hReturn = S_FALSE;

	if( IsConnected())
	{
		m_pCommandPtr = NULL;

		if( m_pRecordsetPtr )
		{
			if( m_pRecordsetPtr->State == adStateOpen )
				m_pRecordsetPtr->Close();

			m_pRecordsetPtr = NULL;
		}

		m_pCommandPtr.CreateInstance( __uuidof( Command ));

		m_pCommandPtr->ActiveConnection = m_pConnectionPtr;
		m_pCommandPtr->CommandText = (LPCTSTR)btStoredProcedureName;
		m_pCommandPtr->CommandType = adCmdText;
		m_pCommandPtr->CommandTimeout = 50000;


		hReturn = S_OK;
	}

	return hReturn;
}

//	Check for Connection Status
BOOL CMyADO::IsConnected()
{
	return ( m_pConnectionPtr );
}

//	Check for EOF on the Recordset (which was created during Stored Procedure Execution)
BOOL CMyADO::IsEOF()
{
	BOOL bReturn = TRUE;

	if( m_pRecordsetPtr )
		if( m_pRecordsetPtr->State == adStateOpen && !m_pRecordsetPtr->EndOfFile )
		{
			bReturn = FALSE;
			return bReturn;
		}

	return bReturn;
}

//	Check for Initialization Status (CommandPtr Object is valid)
BOOL CMyADO::IsInitialized()
{
	return ( m_pCommandPtr );
}

HRESULT CMyADO::Open()
{
	HRESULT hReturn = S_FALSE;

	if( m_pConnectionPtr == NULL )
	{
		m_pConnectionPtr.CreateInstance( __uuidof( Connection ));


		try
		{
			//m_pConnectionPtr->CursorLocation = adUseClient;
			m_pConnectionPtr->CommandTimeout = 50000000;
			m_pConnectionPtr->ConnectionTimeout = 5000000;

			m_pConnectionPtr->Open( (LPCTSTR)m_strConnection, (LPCTSTR)m_strUser, (LPCTSTR)m_strPwd, NULL );

/*
			if( hReturn == S_OK )
			{
				m_pConnectionPtr->CursorLocation = adUseClient;
				m_pConnectionPtr->CommandTimeout = 1200;
				m_pConnectionPtr->ConnectionTimeout = 1200;
			}
*/
			hReturn = S_OK;
		}
		catch( _com_error& eComError )
		{
			SetError(eComError);
			//char szErrorMsg[256];
			//_snprintf( szErrorMsg, sizeof( szErrorMsg ), "ERROR in CMyADO::Open( '%s', '%s', '%s' ) - %s\n", ( CString )btConnectionString, ( CString )btUserID, ( CString )btPassword, eComError.ErrorMessage());
			//OutputDebugString( szErrorMsg );
		}
		catch( ... )
		{
		}
	}
	else
		hReturn = S_FALSE;

	return hReturn;
}

//	Open a new ADO Connection
HRESULT CMyADO::Open( CString btConnectionString, CString btUserID, CString btPassword )
{
	HRESULT hReturn = S_FALSE;

	CoInitialize( NULL );
	
	if( m_pConnectionPtr == NULL )
	{
		m_pConnectionPtr.CreateInstance( __uuidof( Connection ));

		try
		{

			//m_pConnectionPtr->CursorLocation = adUseClient;
			m_pConnectionPtr->CommandTimeout = 5000000;
			m_pConnectionPtr->ConnectionTimeout = 5000000;

			m_pConnectionPtr->Open( (LPCTSTR)btConnectionString, (LPCTSTR)btUserID, (LPCTSTR)btPassword, NULL );
			//hReturn = m_pConnectionPtr->Open( (LPCTSTR)btUserID, (LPCTSTR)btPassword, (LPCTSTR)btConnectionString );
/*
			if( hReturn == S_OK )
			{
				m_pConnectionPtr->CursorLocation = adUseClient;
			}
*/
			hReturn = S_OK;
		}
		catch( _com_error& eComError )
		{
			SetError(eComError);
			//char szErrorMsg[256];
			//_snprintf( szErrorMsg, sizeof( szErrorMsg ), "ERROR in CMyADO::Open( '%s', '%s', '%s' ) - %s\n", ( CString )btConnectionString, ( CString )btUserID, ( CString )btPassword, eComError.ErrorMessage());
			//OutputDebugString( szErrorMsg );
		}
		catch( ... )
		{
		}
	}
	else
		hReturn = S_OK;

	::CoUninitialize();

	return hReturn;
}

//	Move to the Next Record in the Recordset (which was created during Stored Procedure Execution)
HRESULT CMyADO::MoveNext()
{
	HRESULT hResult = S_FALSE;

	if( !IsEOF())
	{
		try
		{
			m_pRecordsetPtr->MoveNext();
			hResult = S_OK;
		}
		catch( _com_error& eComError )
		{
			SetError(eComError);
			//char szErrorMsg[256];
			//_snprintf( szErrorMsg, sizeof( szErrorMsg ), "ERROR in CMyADO::MoveNext() - %s\n", eComError.ErrorMessage());
			//OutputDebugString( szErrorMsg );
		}
		catch( ... )
		{
		}
	}

	return hResult;
}
