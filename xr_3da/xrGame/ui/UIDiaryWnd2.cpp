#include "stdafx.h"
#include "UIDiaryWnd.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UINewsWnd.h"
#include "UIAnimatedStatic.h"
#include "UIXmlInit.h"
#include "../object_broker.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "UITabControl.h"

CUIDiaryWnd::CUIDiaryWnd()
{
	m_currFilter	= eNone;
}

CUIDiaryWnd::~CUIDiaryWnd()
{
	delete_data(m_UINewsWnd);
}

void CUIDiaryWnd::Show(bool status)
{
	inherited::Show		(status);
	if(status)
		Reload( (EDiaryFilter)m_FilterTab->GetActiveIndex() );
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

	m_FilterTab						= xr_new<CUITabControl>();m_FilterTab->SetAutoDelete(true);
	m_UILeftHeader->AttachChild		(m_FilterTab);
	xml_init.InitTabControl			(uiXml, "main_wnd:left_frame:left_frame_header:filter_tab", 0, m_FilterTab);
	m_FilterTab->SetWindowName		("filter_tab");
	Register						(m_FilterTab);
    AddCallback						("filter_tab",TAB_CHANGED,boost::bind(&CUIDiaryWnd::OnFilterChanged,this,_1,_2));

	m_UIAnimation					= xr_new<CUIAnimatedStatic>(); m_UIAnimation->SetAutoDelete(true);
	xml_init.InitAnimatedStatic		(uiXml, "main_wnd:left_frame:left_frame_header:anim_static", 0, m_UIAnimation);
	m_UILeftHeader->AttachChild		(m_UIAnimation);


	m_UILeftWnd						= xr_new<CUIWindow>(); m_UILeftWnd->SetAutoDelete(true);
	xml_init.InitWindow				(uiXml, "main_wnd:left_frame:work_area", 0, m_UILeftWnd);
	m_UILeftFrame->AttachChild		(m_UILeftWnd);


	m_UIRightFrame					= xr_new<CUIFrameWindow>();		m_UIRightFrame->SetAutoDelete(true);
	xml_init.InitFrameWindow		(uiXml,"main_wnd:right_frame",0,m_UIRightFrame);
	AttachChild						(m_UIRightFrame);

	m_UIRightHeader					= xr_new<CUIFrameLineWnd>();	m_UIRightHeader->SetAutoDelete(true);
	xml_init.InitFrameLine			(uiXml, "main_wnd:right_frame:right_frame_header", 0, m_UIRightHeader);
	m_UIRightFrame->AttachChild		(m_UIRightHeader);

	m_UIRightWnd						= xr_new<CUIWindow>(); m_UIRightWnd->SetAutoDelete(true);
	xml_init.InitWindow				(uiXml, "main_wnd:right_frame:work_area", 0, m_UIRightWnd);
	m_UIRightFrame->AttachChild		(m_UIRightWnd);

	m_UINewsWnd						= xr_new<CUINewsWnd>();m_UINewsWnd->SetAutoDelete(false);
	m_UINewsWnd->Init					("events_new.xml","main_wnd:right_frame:work_area:news_wnd:");
	
}

void	CUIDiaryWnd::SendMessage			(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}

void CUIDiaryWnd::OnFilterChanged			(CUIWindow* w, void*)
{
	Reload( (EDiaryFilter)m_FilterTab->GetActiveIndex() );
}

void CUIDiaryWnd::Reload	(EDiaryFilter new_filter)
{
	if(m_currFilter==new_filter) return;

	switch (m_currFilter){
		case eGame:
			UnloadGameTab	();
			break;
		case eInfo:
			UnloadInfoTab	();
			break;
		case eNews:
			UnloadNewsTab	();
			break;
	};

	m_currFilter = new_filter;

	switch (m_currFilter){
		case eGame:
			LoadGameTab	();
			break;
		case eInfo:
			LoadInfoTab	();
			break;
		case eNews:
			LoadNewsTab	();
			break;
	};
}

void CUIDiaryWnd::AddNews	()
{
	m_UINewsWnd->AddNews		();
}

void CUIDiaryWnd::MarkNewsAsRead (bool status)
{

}

void CUIDiaryWnd::UnloadGameTab		()
{
}

void CUIDiaryWnd::LoadGameTab			()
{
}

void CUIDiaryWnd::UnloadInfoTab		()
{
}

void CUIDiaryWnd::LoadInfoTab			()
{
}

void CUIDiaryWnd::UnloadNewsTab		()
{
	m_UIRightWnd->DetachChild	(m_UINewsWnd);
	m_UINewsWnd->Show			(false);
}

void CUIDiaryWnd::LoadNewsTab			()
{
	m_UIRightWnd->AttachChild	(m_UINewsWnd);
	m_UINewsWnd->Show			(true);
}
