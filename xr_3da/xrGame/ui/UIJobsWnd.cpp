//=============================================================================
//  Filename:   UIJobsWnd.cpp
//	---------------------------------------------------------------------------
//  Jobs dialog subwindow in Diary window
//=============================================================================

#include "StdAfx.h"
#include "UIJobsWnd.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"

void CUIJobsWnd::Init(CUIXml &uiXml, CUIWindow *pNewParent)
{
	CUIXmlInit xml_init;

	//	inherited::Init(0,0, Device.dwWidth, Device.dwHeight);

	// Заголовок
	AttachChild(&UIStaticCaption);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticCaption);
}
