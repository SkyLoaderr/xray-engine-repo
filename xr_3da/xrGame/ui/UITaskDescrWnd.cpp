#include "stdafx.h"
#include "UITaskDescrWnd.h"
#include "UIXmlInit.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIScrollView.h"
#include "UIEncyclopediaArticleWnd.h"


CUITaskDescrWnd::CUITaskDescrWnd	()
{}

CUITaskDescrWnd::~CUITaskDescrWnd	()
{}

void CUITaskDescrWnd::Init	(CUIXml* doc, LPCSTR start_from)
{
	CUIXmlInit xml_init;
	
	xml_init.InitWindow		(*doc,start_from,0,this);
	
	string512				str;

	m_UIMainFrame			= xr_new<CUIFrameWindow>(); m_UIMainFrame->SetAutoDelete(true);
	AttachChild				(m_UIMainFrame);
	strconcat				(str,start_from,":main_frame");
	xml_init.InitFrameWindow(*doc,str,0,m_UIMainFrame);

	m_UIMainHeader			= xr_new<CUIFrameLineWnd>();m_UIMainHeader->SetAutoDelete(true);
	AttachChild				(m_UIMainHeader);
	strconcat				(str,start_from,":main_frame:header_frame_line");
	xml_init.InitFrameLine	(*doc,str,0,m_UIMainHeader);

	m_UITaskInfoWnd			= xr_new<CUIScrollView>(); m_UITaskInfoWnd->SetAutoDelete(true);
	m_UIMainFrame->AttachChild(m_UITaskInfoWnd);
	strconcat				(str,start_from,":main_frame:scroll_view");
	xml_init.InitScrollView	(*doc,str,0,m_UITaskInfoWnd);

}

void CUITaskDescrWnd::ClearAll					()
{
	m_UITaskInfoWnd->Clear		();
}

void CUITaskDescrWnd::AddArticle				(LPCSTR article)
{
	CUIEncyclopediaArticleWnd*	article_info = xr_new<CUIEncyclopediaArticleWnd>();article_info->SetAutoDelete(true);
	article_info->Init			("pda_events.xml","main_wnd:right_frame:task_descr_view:main_frame:scroll_view:objective_item");
	article_info->SetArticle	(article);
	m_UITaskInfoWnd->AddWindow	(article_info);
}