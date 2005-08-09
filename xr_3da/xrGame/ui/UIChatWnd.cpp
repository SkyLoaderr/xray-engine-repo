//=============================================================================
//  Filename:   UIChatWnd.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Simple chat window for multiplayer purposes
//=============================================================================

#include <stdafx.h>
#include "UIChatWnd.h"
#include "UIChatLog.h"
#include "UIEditBox.h"
#include "UIXmlInit.h"
#include "../game_cl_base.h"
#include "../xr_level_controller.h"
#include "../Level.h"
#include "../../xr_object.h"

//////////////////////////////////////////////////////////////////////////

const char * const	CHAT_MP_WND_XML	= "chat_mp.xml";
const int			fadeDelay		= 5000;

//////////////////////////////////////////////////////////////////////////

CUIChatWnd::CUIChatWnd(CUIChatLog *pList)
	:	pUILogList				(pList),
		sendNextMessageToTeam	(false),
		pOwner					(NULL)
{
	R_ASSERT(pUILogList);
}

//////////////////////////////////////////////////////////////////////////

CUIChatWnd::~CUIChatWnd()
{
}

//////////////////////////////////////////////////////////////////////////

void CUIChatWnd::Init(CUIXml& uiXml)
{
	AttachChild(&UIPrefix);
	CUIXmlInit::InitStatic(uiXml, "chat_prefix", 0, &UIPrefix);
	//UIPrefix.InitSharedTexture("ui_texture.xml","debug");
	//UIPrefix.SetStretchTexture(true);

	AttachChild(&UIEditBox);
	CUIXmlInit::InitEditBox(uiXml, "chat_edit_box", 0, &UIEditBox);
	//UIEditBox.InitSharedTexture("ui_texture.xml","debug");
	//UIEditBox.SetStretchTexture(true);

	m_AuthorName = Level().CurrentEntity()->cName();
}

//////////////////////////////////////////////////////////////////////////

void CUIChatWnd::SetEditBoxPrefix(const shared_str &prefix)
{
	UIPrefix.SetText(*prefix);
}

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
			shared_str phrase = UIEditBox.GetText();
			if (pOwner)
				pOwner->OnKeyboardPress(kCHAT);
			UIEditBox.SetText("");
	}

	inherited::SetKeyboardCapture(pChildWindow, capture_status);
}