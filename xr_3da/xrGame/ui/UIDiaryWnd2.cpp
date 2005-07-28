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
#include "UIScrollView.h"
#include "UIListWnd.h"
#include "UITreeViewItem.h"
#include "UIEncyclopediaArticleWnd.h"
#include "../level.h"
#include "../actor.h"
#include "../alife_registry_wrappers.h"
#include "../encyclopedia_article.h"

CUIDiaryWnd::CUIDiaryWnd()
{
	m_currFilter	= eNone;
}

CUIDiaryWnd::~CUIDiaryWnd()
{
	delete_data(m_UINewsWnd);
	delete_data(m_SrcListWnd);
	delete_data(m_DescrView);
	delete_data					(m_ArticlesDB);
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
	xml_init.InitAutoStaticGroup	(uiXml,"main_wnd:left_frame",m_UILeftFrame);
	
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

	m_SrcListWnd					= xr_new<CUIListWnd>(); m_SrcListWnd->SetAutoDelete(false);
	xml_init.InitListWnd			(uiXml, "main_wnd:left_frame:work_area:src_list", 0, m_SrcListWnd);
	m_SrcListWnd->SetWindowName		("src_list");
	Register						(m_SrcListWnd);
    AddCallback						("src_list",LIST_ITEM_CLICKED,boost::bind(&CUIDiaryWnd::OnSrcListItemClicked,this,_1,_2));

	xml_init.InitFont				(uiXml, "main_wnd:left_frame:work_area:src_list:tree_item_font", 0, m_uTreeItemColor, m_pTreeItemFont);
	R_ASSERT						(m_pTreeItemFont);
	xml_init.InitFont				(uiXml, "main_wnd:left_frame:work_area:src_list:tree_root_font", 0, m_uTreeRootColor, m_pTreeRootFont);
	R_ASSERT						(m_pTreeRootFont);

	m_UIRightFrame					= xr_new<CUIFrameWindow>();		m_UIRightFrame->SetAutoDelete(true);
	xml_init.InitFrameWindow		(uiXml,"main_wnd:right_frame",0,m_UIRightFrame);
	AttachChild						(m_UIRightFrame);
	xml_init.InitAutoStaticGroup	(uiXml,"main_wnd:right_frame",m_UIRightFrame);

	m_UIRightHeader					= xr_new<CUIFrameLineWnd>();	m_UIRightHeader->SetAutoDelete(true);
	xml_init.InitFrameLine			(uiXml, "main_wnd:right_frame:right_frame_header", 0, m_UIRightHeader);
	m_UIRightFrame->AttachChild		(m_UIRightHeader);

	m_UIRightWnd						= xr_new<CUIWindow>(); m_UIRightWnd->SetAutoDelete(true);
	xml_init.InitWindow				(uiXml, "main_wnd:right_frame:work_area", 0, m_UIRightWnd);
	m_UIRightFrame->AttachChild		(m_UIRightWnd);

	m_UINewsWnd						= xr_new<CUINewsWnd>();m_UINewsWnd->SetAutoDelete(false);
	m_UINewsWnd->Init				();

	m_DescrView						= xr_new<CUIScrollView>(); m_DescrView->SetAutoDelete(false);
	xml_init.InitScrollView			(uiXml, "main_wnd:right_frame:work_area:scroll_view", 0, m_DescrView);
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
		case eJournal:
			UnloadJournalTab	();
			break;
		case eInfo:
			UnloadJournalTab	();
			break;
		case eNews:
			UnloadNewsTab	();
			break;
	};

	m_currFilter = new_filter;

	switch (m_currFilter){
		case eJournal:
			LoadJournalTab	(ARTICLE_DATA::eJournalArticle);
			break;
		case eInfo:
			LoadJournalTab	(ARTICLE_DATA::eInfoArticle);
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

void CUIDiaryWnd::UnloadJournalTab		()
{
	m_UILeftWnd->DetachChild	(m_SrcListWnd);
	m_SrcListWnd->Show			(false);

	m_UIRightWnd->DetachChild	(m_DescrView);
	m_DescrView->Show			(false);
	delete_data					(m_ArticlesDB);
	m_DescrView->Clear			();
}

void CUIDiaryWnd::LoadJournalTab			(ARTICLE_DATA::EArticleType _type)
{
	delete_data			(m_ArticlesDB);

	m_UILeftWnd->AttachChild	(m_SrcListWnd);
	m_SrcListWnd->Show			(true);

	m_UIRightWnd->AttachChild	(m_DescrView);
	m_DescrView->Show			(true);

	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor && pActor->encyclopedia_registry->registry().objects_ptr())
	{
		ARTICLE_VECTOR::const_iterator it = pActor->encyclopedia_registry->registry().objects_ptr()->begin();
		for(; it != pActor->encyclopedia_registry->registry().objects_ptr()->end(); it++)
		{
//.			if (ARTICLE_DATA::eJournalArticle == it->article_type)

			if (_type == it->article_type)
				
			{
				m_ArticlesDB.resize(m_ArticlesDB.size() + 1);
				CEncyclopediaArticle*& a = m_ArticlesDB.back();
				a = xr_new<CEncyclopediaArticle>();
				a->Load(it->article_id);

				bool bReaded = false;
				CreateTreeBranch(a->data()->group, a->data()->name, m_SrcListWnd, m_ArticlesDB.size()-1, 
					m_pTreeRootFont, m_uTreeRootColor, m_pTreeItemFont, m_uTreeItemColor, bReaded);
			}
		}
	}

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

void CUIDiaryWnd::OnSrcListItemClicked	(CUIWindow* w,void* p)
{
	CUITreeViewItem*	pSelItem	= (CUITreeViewItem*)p;
	m_DescrView->Clear	();
	if (!pSelItem->IsRoot())
	{
		CUIEncyclopediaArticleWnd*	article_info = xr_new<CUIEncyclopediaArticleWnd>();article_info->SetAutoDelete(true);
		article_info->Init			("encyclopedia_item.xml","encyclopedia_wnd:objective_item");
		article_info->SetArticle	(m_ArticlesDB[pSelItem->GetValue()]);
		m_DescrView->AddWindow		(article_info);
	}
}
