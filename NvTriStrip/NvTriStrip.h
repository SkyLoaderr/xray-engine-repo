// NvTriStrip.h : main header file for the NVTRISTRIP application
//

#if !defined(AFX_NVTRISTRIP_H__1FFA50E4_7532_11D4_9C70_005004C20C3D__INCLUDED_)
#define AFX_NVTRISTRIP_H__1FFA50E4_7532_11D4_9C70_005004C20C3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNvTriStripApp:
// See NvTriStrip.cpp for the implementation of this class
//

class CNvTriStripApp : public CWinApp
{
public:
	CNvTriStripApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNvTriStripApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNvTriStripApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NVTRISTRIP_H__1FFA50E4_7532_11D4_9C70_005004C20C3D__INCLUDED_)
