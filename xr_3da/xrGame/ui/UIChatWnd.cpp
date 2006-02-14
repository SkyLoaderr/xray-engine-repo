#include <stdafx.h>
#include "UIChatWnd.h"
#include "UIGameLog.h"
#include "UIEditBox.h"
#include "UIXmlInit.h"
#include "../game_cl_base.h"
#include "../xr_level_controller.h"
#include "../Level.h"
#include "../../xr_object.h"

CUIChatWnd::CUIChatWnd(CUIGameLog *pList)
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

	AttachChild(&UIEditBox);
	CUIXmlInit::InitEditBox(uiXml, "chat_edit_box", 0, &UIEditBox);
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