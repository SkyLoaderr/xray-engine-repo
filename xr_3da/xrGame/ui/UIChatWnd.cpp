//=============================================================================
//  Filename:   UIChatWnd.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Simple chat window for multiplayer purposes
//=============================================================================

#include <stdafx.h>
#include "UIChatWnd.h"
#include "UIListWnd.h"
#include "UIEditBox.h"
#include "UIXmlInit.h"
#include "xrXmlParser.h"
#include "../game_cl_base.h"
#include "../xr_level_controller.h"

//////////////////////////////////////////////////////////////////////////

const char * const		CHAT_MP_WND_XML		= "chat_mp.xml";

//////////////////////////////////////////////////////////////////////////

CUIChatWnd::CUIChatWnd(CUIListWnd *pList)
	:	pUILogList		(pList)
{
	R_ASSERT(pUILogList);
}

//////////////////////////////////////////////////////////////////////////

CUIChatWnd::~CUIChatWnd()
{
}

//////////////////////////////////////////////////////////////////////////

void CUIChatWnd::Init()
{
	CUIXml uiXml;
	uiXml.Init("$game_data$", CHAT_MP_WND_XML);

	CUIXmlInit xml_init;

	// Prefix static
	AttachChild(&UIPrefix);
	xml_init.InitStatic(uiXml, "prefix_static", 0, &UIPrefix);

	// Edit box
	AttachChild(&UIEditBox);
	xml_init.InitStatic(uiXml, "edit_box", 0, &UIEditBox);
}

//////////////////////////////////////////////////////////////////////////

void CUIChatWnd::SetEditBoxPrefix(const ref_str &prefix)
{
	UIPrefix.SetText(*prefix);
}

//////////////////////////////////////////////////////////////////////////

void CUIChatWnd::SetReplicaAuthor(const ref_str &authorName)
{
	m_AuthorName = authorName;
}

//////////////////////////////////////////////////////////////////////////

void CUIChatWnd::Say(const ref_str &phrase)
{
	string256 fullLine;
	::ZeroMemory(fullLine, 256);
	strconcat(fullLine, *m_AuthorName, ": ", *phrase);
	R_ASSERT(pUILogList);
	pUILogList->AddItem<CUIListItem>(fullLine, 0, NULL, 5000);
}

//////////////////////////////////////////////////////////////////////////

void CUIChatWnd::Show()
{
	UIEditBox.CaptureFocus(true);
	inherited::Show();
}

//////////////////////////////////////////////////////////////////////////

void CUIChatWnd::Hide()
{
	UIEditBox.CaptureFocus(false);
	inherited::Hide();
}

//////////////////////////////////////////////////////////////////////////

void CUIChatWnd::SetKeyboardCapture(CUIWindow* pChildWindow, bool capture_status)
{
	if (&UIEditBox == pChildWindow && false == capture_status)
	{
		if (pOwner)
			pOwner->OnKeyboardPress(kCHAT);
	}

	inherited::SetKeyboardCapture(pChildWindow, capture_status);
}