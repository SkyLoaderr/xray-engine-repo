//=============================================================================
//  Filename:   UINewsWnd.cpp
//  News subwindow in PDA dialog
//=============================================================================


#include "stdafx.h"
#include "UINewsWnd.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "UIString.h"

const char * const JOBS_XML = "jobs.xml";

//-----------------------------------------------------------------------------/
//	Default constructor
//-----------------------------------------------------------------------------/
CUINewsWnd::CUINewsWnd()
{
	Hide();
}

//-----------------------------------------------------------------------------/
//  Destructor
//-----------------------------------------------------------------------------/
CUINewsWnd::~CUINewsWnd()
{

}

void CUINewsWnd::Init()
{
	CUIXml uiXml;
	uiXml.Init("$game_data$", JOBS_XML);
	CUIXmlInit xml_init;

	inherited::Init(0,0, Device.dwWidth, Device.dwHeight);

	// Заголовок
	AttachChild(&UIStaticCaption);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticCaption);
	UIStaticCaption.SetText("NEWS");
	UIStaticCaption.SetTextX(10);
	UIStaticCaption.SetTextY(10);
	UIStaticCaption.SetTextColor(0xffffffff);
	UIStaticCaption.Show(true);

	// Табконтрол поддиалогов
	AttachChild(&UITabControl);
	xml_init.InitTabControl(uiXml, "tab", 0, &UITabControl);
//	UITabControl.Show(true);

	// Список для вывода информации
	AttachChild(&UIListWnd);
	xml_init.InitListWnd(uiXml, "list", 0, &UIListWnd);
	UIListWnd.EnableScrollBar(true);
//	UIListWnd.Show(true);

	//Элементы автоматического добавления
	xml_init.InitAutoStatic(uiXml, "auto_static", this);
}

void CUINewsWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (pWnd == &UITabControl)
	{
		CUIString str;
		str.SetText("Hello from Haiti!");
		UIListWnd.AddParsedItem(str, 0, 0xffffffff);
	}
	inherited::SendMessage(pWnd, msg, pData);
}

void CUINewsWnd::Draw()
{
	inherited::Draw();
}

void CUINewsWnd::Update()
{
	inherited::Update();
}