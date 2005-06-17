//////////////////////////////////////////////////////////////////////
// UIPdaListItem.cpp: элемент окна списка в PDA
// для отображения информации о контакте PDA
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "UIPdaListItem.h"
#include "../actor.h"
#include "UIInventoryUtilities.h"
#include "../string_table.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "../character_info.h"

#include "UIFrameWindow.h"
#include "..\InventoryOwner.h"
#include "UICharacterInfo.h"
#include "UIIconedListItem.h"

//////////////////////////////////////////////////////////////////////////

const char * const	PDA_CONTACT_CHAR		= "pda_character.xml";

//////////////////////////////////////////////////////////////////////////

CUIPdaListItem::CUIPdaListItem()
{
	UIMask = NULL;
	UIName = NULL;
	UIInfo = NULL;
}

//////////////////////////////////////////////////////////////////////////

CUIPdaListItem::~CUIPdaListItem()
{
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaListItem::Init(float x, float y, float width, float height)
{
	inherited::Init(x, y, width, height);

	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, PDA_CONTACT_CHAR);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit xml_init;
	UIInfo = xr_new<CUICharacterInfo>			();
	UIInfo->SetAutoDelete						(true);
	AttachChild(UIInfo);
	UIInfo->Init(0, 0, width, height, PDA_CONTACT_CHAR);

	if (uiXml.NavigateToNode("mask_frame_window", 0))
	{
		UIMask = xr_new<CUIFrameWindow>	();
		UIMask->SetAutoDelete			(true);
		xml_init.InitFrameWindow(uiXml, "mask_frame_window", 0, UIMask);
		UIInfo->UIIcon().SetMask(UIMask);
	}

	if(uiXml.NavigateToNode("name_static", 0))
	{
		UIName = xr_new<CUIIconedListItem>			();
		UIName->SetAutoDelete						(true);
		AttachChild(UIName);
		xml_init.InitStatic(uiXml, "name_static", 0, UIName);
		UIName->SetIcon("ui\\ui_pda_contacts_online", 16);
		UIName->SetIconOffset(0, 4);
	}
}

void CUIPdaListItem::InitCharacter(CInventoryOwner* pInvOwner)
{
	VERIFY(pInvOwner);
	UIInfo->InitCharacter(pInvOwner);
	UIName->SetText(pInvOwner->Name());
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaListItem::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (pWnd == this && STATIC_FOCUS_RECEIVED == msg)
	{
		if(UIName)UIName->SetHighlightText(true);
	}
	else if (pWnd == this && STATIC_FOCUS_LOST == msg)
	{
		if(UIName)UIName->SetHighlightText(false);
	}
}