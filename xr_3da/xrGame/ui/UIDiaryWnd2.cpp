#include "stdafx.h"
#include "UIDiaryWnd.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UINewsWnd.h"
#include "UIAnimatedStatic.h"
#include "UIXmlInit.h"
#include "../object_broker.h"

CUIDiaryWnd::CUIDiaryWnd()
{}

CUIDiaryWnd::~CUIDiaryWnd()
{
	delete_data(UINewsWnd);
}

void CUIDiaryWnd::Show(bool status)
{
	inherited::Show		(status);
	UINewsWnd->Show		(status);
	SetTitle			("Game news");
}

void CUIDiaryWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, "events_new.xml");
	R_ASSERT3(xml_result, "xml file not found", "events_new.xml");
	CUIXmlInit xml_init;

	xml_init.InitWindow				(uiXml, "main_wnd", 0, this);

	m_UILeftFrame					= xr_new<CUIFrameWindow>(); m_UILeftFrame->SetAutoDelete(true);
	xml_init.InitFrameWindow		(uiXml,"main_wnd:left_frame",0,m_UILeftFrame);
	AttachChild						(m_UILeftFrame);

	m_UILeftHeader					= xr_new<CUIFrameLineWnd>(); m_UILeftHeader->SetAutoDelete(true);
	xml_init.InitFrameLine			(uiXml, "main_wnd:left_frame:left_frame_header", 0, m_UILeftHeader);
	m_UILeftFrame->AttachChild		(m_UILeftHeader);

	m_UIAnimation					= xr_new<CUIAnimatedStatic>(); m_UIAnimation->SetAutoDelete(true);
	xml_init.InitAnimatedStatic		(uiXml, "main_wnd:left_frame:left_frame_header:anim_static", 0, m_UIAnimation);
	m_UILeftHeader->AttachChild		(m_UIAnimation);

	m_UITitle						= xr_new<CUIStatic>(); m_UITitle->SetAutoDelete(true);
	xml_init.InitStatic				(uiXml, "main_wnd:left_frame:left_frame_header:title_static", 0, m_UITitle);
	m_UILeftHeader->AttachChild		(m_UITitle);

	m_UIMainWnd						= xr_new<CUIWindow>(); m_UIMainWnd->SetAutoDelete(true);
	xml_init.InitWindow				(uiXml, "main_wnd:left_frame:work_area", 0, m_UIMainWnd);
	m_UILeftFrame->AttachChild		(m_UIMainWnd);

	UINewsWnd						= xr_new<CUINewsWnd>();UINewsWnd->SetAutoDelete(false);
	UINewsWnd->Init					("events_new.xml","main_wnd:left_frame:work_area:news_wnd:");
	
	m_UIMainWnd->AttachChild		(UINewsWnd);

}

void CUIDiaryWnd::AddNews			()
{
	UINewsWnd->AddNews		();
}

void CUIDiaryWnd::MarkNewsAsRead (bool status)
{

}

void CUIDiaryWnd::SetTitle		(LPCSTR str)
{
	m_UITitle->SetText		(str);
}
