#include "stdafx.h"
#include "LuaDirectWnd.h"
#include "afxrich.h"



BOOL CLuaRuntimeBar::Create(CWnd *pParentWnd, UINT nID, LPCTSTR lpszWindowName, CSize sizeDefault, DWORD dwStyle)
{
	BOOL bRet = CCJTabCtrlBar::Create(pParentWnd, nID, lpszWindowName, sizeDefault, dwStyle);
	if ( !bRet )
		return FALSE;

	AddView(_T("Lua runtime"),    RUNTIME_CLASS(CRichEditView));

	return TRUE;
}

