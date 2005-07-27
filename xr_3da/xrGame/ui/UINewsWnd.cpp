//=============================================================================
//  Filename:   UINewsWnd.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  News subwindow in PDA dialog
//=============================================================================

#include "stdafx.h"
#include "UINewsWnd.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../UI.h"
#include "../HUDManager.h"
#include "../level.h"
#include "../game_news.h"
#include "../actor.h"
#include "../alife_registry_wrappers.h"
#include "UIInventoryUtilities.h"
#include "UINewsItemWnd.h"
#include "UIScrollView.h"

const char * const	NEWS_XML			= "news.xml";

#define NEWS_TO_SHOW 50

//////////////////////////////////////////////////////////////////////////

CUINewsWnd::CUINewsWnd()
{
	SetWindowName("News");
}

//////////////////////////////////////////////////////////////////////////

CUINewsWnd::~CUINewsWnd()
{

}

void CUINewsWnd::Init(LPCSTR xml_name, LPCSTR start_from)
{
	string512 pth;

	CUIXml uiXml;
	bool xml_result				= uiXml.Init(CONFIG_PATH, UI_PATH, xml_name);
	R_ASSERT3					(xml_result, "xml file not found", xml_name);
	CUIXmlInit xml_init;

	strconcat					(pth,start_from,"list");
	xml_init.InitWindow			(uiXml, pth, 0, this);
	UIScrollWnd					= xr_new<CUIScrollView>();UIScrollWnd->SetAutoDelete(true);
	AttachChild					(UIScrollWnd);
	xml_init.InitScrollView		(uiXml, pth, 0, UIScrollWnd);
}

void CUINewsWnd::Init()
{
	Init				(NEWS_XML,"");
}

//////////////////////////////////////////////////////////////////////////

void CUINewsWnd::AddNews()
{
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	UIScrollWnd->Clear();

	static u32 lastNewsCount = 0;

	if (pActor)
	{
		GAME_NEWS_VECTOR& news_vector = pActor->game_news_registry->registry().objects();
		if (lastNewsCount < news_vector.size())
		{
			lastNewsCount = news_vector.size();
			if(GetMessageTarget())
				GetMessageTarget()->SendMessage(this, DIARY_SET_NEWS_AS_UNREAD, NULL);
		}
		
		// Показать только NEWS_TO_SHOW последних ньюсов
		int currentNews = 0;

		for (GAME_NEWS_VECTOR::reverse_iterator it = news_vector.rbegin(); it != news_vector.rend() && currentNews < NEWS_TO_SHOW ; ++it)
		{
			AddNewsItem(it->FullText());
			++currentNews;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUINewsWnd::AddNewsItem(const shared_str &text)
{
	CUINewsItemWnd* itm	= xr_new<CUINewsItemWnd>(); itm->SetAutoDelete(true);
	itm->Init	(NEWS_XML,"news_item");
	itm->SetText(text);
	UIScrollWnd->AddWindow(itm);
//	UIListWnd.AddParsedItem<CUIListItem>(str, 0, UIListWnd.GetTextColor());
//	UIListWnd.AddItem<CUIListItem>("");
}

//////////////////////////////////////////////////////////////////////////

void CUINewsWnd::Show(bool status)
{
	if (status)
		AddNews();
	else
		InventoryUtilities::SendInfoToActor("ui_pda_news_hide");
	inherited::Show(status);
}