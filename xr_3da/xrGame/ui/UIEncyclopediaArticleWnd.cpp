#include "stdafx.h"
#include "UIEncyclopediaArticleWnd.h"
#include "UIStatic.h"
#include "../encyclopedia_article.h"
#include "UIXmlInit.h"
#include "../string_table.h"

CUIStatic*			m_UIImage;
CUIStatic*			m_UItext;

CUIEncyclopediaArticleWnd::CUIEncyclopediaArticleWnd	()
:m_Article(NULL)
{
}

CUIEncyclopediaArticleWnd::~CUIEncyclopediaArticleWnd	()
{
}

void CUIEncyclopediaArticleWnd::Init(LPCSTR xml_name, LPCSTR start_from)
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, xml_name);
	R_ASSERT3(xml_result, "xml file not found", xml_name);

	CUIXmlInit xml_init;

	string512 str;

	strcpy(str,start_from);
	xml_init.InitWindow			(uiXml,str,0,this);

	strconcat(str,start_from,":image");
	m_UIImage				= xr_new<CUIStatic>();	m_UIImage->SetAutoDelete(true);
	xml_init.InitStatic			(uiXml,str,0,m_UIImage);
	AttachChild				(m_UIImage);

	strconcat(str,start_from,":text_cont");
	m_UIText				= xr_new<CUIStatic>();	m_UIText->SetAutoDelete(true);
	xml_init.InitStatic		(uiXml,str,0,m_UIText);
	AttachChild				(m_UIText);
}

void CUIEncyclopediaArticleWnd::SetArticle(CEncyclopediaArticle* article)
{
//	m_Article						= article;
	m_UIImage->SetShader			(article->data()->image.GetShader());
	m_UIImage->SetOriginalRect		(article->data()->image.GetStaticItem()->GetOriginalRect());
	m_UIText->SetText				(*CStringTable()(article->data()->text.c_str()));
	m_UIText->AdjustHeightToText	();
	SetHeight						(m_UIText->GetWndPos().y+m_UIText->GetHeight());
}

void CUIEncyclopediaArticleWnd::SetArticle	(LPCSTR article)
{
	CEncyclopediaArticle	A;
	A.Load		(article);
	SetArticle	(&A);
}
