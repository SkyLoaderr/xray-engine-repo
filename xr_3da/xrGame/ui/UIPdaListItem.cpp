//////////////////////////////////////////////////////////////////////
// UIPdaListItem.cpp: элемент окна списка в PDA
// для отображения информации о контакте PDA
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "UIPdaListItem.h"
#include "../actor.h"
#include "../level.h"
#include "UIInventoryUtilities.h"
#include "../string_table.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "../character_info.h"

//////////////////////////////////////////////////////////////////////////

const char * const	PDA_CONTACT_CHAR		= "pda_character.xml";

//////////////////////////////////////////////////////////////////////////

CUIPdaListItem::CUIPdaListItem()
{
}

//////////////////////////////////////////////////////////////////////////

CUIPdaListItem::~CUIPdaListItem()
{
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaListItem::Init(int x, int y, int width, int height)
{
	inherited::Init(x, y, width, height);

//	AttachChild(&UICharacterInfo);
//	UICharacterInfo.Init(0,0,width, height, PDA_CONTACT_CHAR);

	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", PDA_CONTACT_CHAR);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit xml_init;

	AttachChild(&UIInfo);
	UIInfo.Init(0, 0, width, height, PDA_CONTACT_CHAR);

	if (uiXml.NavigateToNode("mask_frame_window", 0))
	{
		xml_init.InitFrameWindow(uiXml, "mask_frame_window", 0, &UIMask);
		UIInfo.UIIcon.SetMask(&UIMask);
	}

	AttachChild(&UIName);
	if(uiXml.NavigateToNode("static", 0))
	{
		xml_init.InitStatic(uiXml, "static", 0, &UIName);
		UIName.SetIcon("ui\\ui_pda_contacts_online", 16);
		UIName.SetIconOffset(0, 4);
	}
	else
	{
		UIName.Show(false);
		UIName.Enable(false);
	}
}

void CUIPdaListItem::InitCharacter(CInventoryOwner* pInvOwner)
{
	VERIFY(pInvOwner);
	UIInfo.InitCharacter(pInvOwner);
	UIName.SetText(UIInfo.UIName.GetText());
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaListItem::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (pWnd == this && STATIC_FOCUS_RECEIVED == msg)
	{
		UIName.SetHighlightText(true);
	}
	else if (pWnd == this && STATIC_FOCUS_LOST == msg)
	{
		UIName.SetHighlightText(false);
	}
}