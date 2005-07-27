#include "stdafx.h"
#include "UINewsItemWnd.h"
#include "UIXmlInit.h"
#include "UIStatic.h"

CUINewsItemWnd::CUINewsItemWnd()
{}

CUINewsItemWnd::~CUINewsItemWnd()
{}

void CUINewsItemWnd::Init				(LPCSTR xml_name, LPCSTR start_from)
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, xml_name);
	R_ASSERT3(xml_result, "xml file not found", xml_name);

	CUIXmlInit xml_init;

	string512 str;

	strcpy(str,start_from);
	xml_init.InitWindow			(uiXml,str,0,this);

//.	strconcat(str,start_from,":image");
//.	m_UIImage				= xr_new<CUIStatic>();	m_UIImage->SetAutoDelete(true);
//.	xml_init.InitStatic			(uiXml,str,0,m_UIImage);
//.	AttachChild				(m_UIImage);

	strconcat(str,start_from,":text_cont");
	m_UIText				= xr_new<CUIStatic>();	m_UIText->SetAutoDelete(true);
	xml_init.InitStatic		(uiXml,str,0,m_UIText);
	AttachChild				(m_UIText);
}

void CUINewsItemWnd::SetText			(const shared_str &text)
{
	m_UIText->SetText				(*text);
	m_UIText->AdjustHeightToText	();

	SetHeight						(m_UIText->GetWndPos().y + m_UIText->GetHeight() );
}
