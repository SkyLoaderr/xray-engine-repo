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
#include "UIString.h"
#include "../UI.h"
#include "../HUDManager.h"
#include "../level.h"
#include "../game_news.h"
#include "../actor.h"

const char * const	NEWS_XML			= "news.xml";

//////////////////////////////////////////////////////////////////////////

CUINewsWnd::CUINewsWnd()
{
	// Remove this for enable interactive behaviour of list
	UIListWnd.ActivateList(false);
	SetWindowName("News");
}

//////////////////////////////////////////////////////////////////////////

CUINewsWnd::~CUINewsWnd()
{

}

//////////////////////////////////////////////////////////////////////////

void CUINewsWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", NEWS_XML);
	R_ASSERT3(xml_result, "xml file not found", NEWS_XML);
	CUIXmlInit xml_init;

	inherited::Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	AttachChild(&UIListWnd);
	xml_init.InitListWnd(uiXml, "list", 0, &UIListWnd);
	UIListWnd.ActivateList(false);
	UIListWnd.EnableScrollBar(true);
	UIListWnd.SetRightIndention(static_cast<int>(15 * HUD().GetScale()));
}

//////////////////////////////////////////////////////////////////////////

void CUINewsWnd::AddNews()
{
	UIListWnd.RemoveAll();
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());

	if (pActor)
	{
		GAME_NEWS_VECTOR& news_vector = pActor->game_news_registry.objects();
		for (GAME_NEWS_IT it = news_vector.begin(); it != news_vector.end(); ++it)
		{
			AddNewsItem(it->FullText());
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUINewsWnd::AddNewsItem(const shared_str &text)
{
	static CUIString	str;
	str.SetText(*text);
	UIListWnd.AddParsedItem<CUIListItem>(str, 0, UIListWnd.GetTextColor());
	UIListWnd.AddItem<CUIListItem>("");
}

//////////////////////////////////////////////////////////////////////////

void CUINewsWnd::Show(bool status)
{
	if (status)
		AddNews();
	inherited::Show(status);
}