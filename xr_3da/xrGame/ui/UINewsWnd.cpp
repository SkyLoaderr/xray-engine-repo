//=============================================================================
//  Filename:   UINewsWnd.cpp
//  News subwindow in PDA dialog
//=============================================================================


#include "stdafx.h"
#include "UINewsWnd.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "UIString.h"
#include "UIPdaWnd.h"
#include "UIInteractiveListItem.h"

//=============================================================================
//  CUINewsWnd class
//=============================================================================
CUINewsWnd::CUINewsWnd()
{
	
}

CUINewsWnd::~CUINewsWnd()
{

}

void CUINewsWnd::Init(CUIXml &uiXml, CUIWindow *pNewParent)
{
	CUIXmlInit xml_init;

//	inherited::Init(0,0, Device.dwWidth, Device.dwHeight);

	// Заголовок 1
	AttachChild(&UIStaticCaptionMain);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticCaptionMain);

	// Заголовок 2
	AttachChild(&UIStaticCaptionCenter);
	xml_init.InitStatic(uiXml, "static", 1, &UIStaticCaptionCenter);

	// Список для вывода информации
	AttachChild(&UIListWnd);
	xml_init.InitListWnd(uiXml, "list", 0, &UIListWnd);
	UIListWnd.EnableScrollBar(true);
	//	UIListWnd.Show(true);

	//Элементы автоматического добавления
//	xml_init.InitAutoStatic(uiXml, "auto_static", this);
}

void CUINewsWnd::SendMessage(CUIWindow *pWnd, s16 msg, void* pData)
{
	if (dynamic_cast<CUIInteractiveListItem*>(pWnd) && 
		CUIInteractiveListItem::INTERACTIVE_ITEM_CLICK == msg)
	{
		GetMessageTarget()->SendMessage(this, CUIInteractiveListItem::INTERACTIVE_ITEM_CLICK, pData);
	}
}