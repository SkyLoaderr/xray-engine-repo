// XR_Quit.h: interface for the CXR_Quit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XR_QUIT_H__B0F60960_DB2A_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_XR_QUIT_H__B0F60960_DB2A_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "fcontroller.h"

class CXR_Quit :
	public CController,
	public pureRender2D
{
public:
	virtual void OnMousePress	(int btn);
	virtual void OnKeyboardPress(int btn);

	virtual void OnRender2D		();

	CXR_Quit();
	~CXR_Quit();
};

// extern CXR_Quit *xrQuit;

#endif // !defined(AFX_XR_QUIT_H__B0F60960_DB2A_11D3_B4E3_4854E82A090D__INCLUDED_)
