//////////////////////////////////////////////////////////////////////
// UIPdaListItem.cpp: элемент окна списка в PDA
// для отображения информации о контакте PDA
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "UIPdaListItem.h"
#include "../actor.h"
#include "../level.h"
#include "UIInventoryUtilities.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "../character_info.h"

#define PDA_CONTACT_CHAR "pda_character_new.xml"

CUIPdaListItem::CUIPdaListItem(void)
{
}

CUIPdaListItem::~CUIPdaListItem(void)
{
}

void CUIPdaListItem::Init(int x, int y, int width, int height)
{
	inherited::Init(x, y, width, height);

//	AttachChild(&UICharacterInfo);
//	UICharacterInfo.Init(0,0,width, height, PDA_CONTACT_CHAR);

	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", PDA_CONTACT_CHAR);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit xml_init;

	AttachChild(&UIIcon);
	if(uiXml.NavigateToNode("icon_static",0))	
	{
		xml_init.InitStatic(uiXml, "icon_static", 0, &UIIcon);
		UIIcon.ClipperOn();
		UIIcon.Show(true);
		UIIcon.Enable(true);
	}
	else
	{
		UIIcon.Show(false);
		UIIcon.Enable(false);
	}

	if (uiXml.NavigateToNode("mask_frame_window", 0))
	{
		xml_init.InitFrameWindow(uiXml, "mask_frame_window", 0, &UIMask);
		UIIcon.SetMask(&UIMask);
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

	AttachChild(&UICharText);
	if (uiXml.NavigateToNode("charinfo_mt_static", 0))
	{
		xml_init.InitMultiTextStatic(uiXml, "charinfo_mt_static", 0, &UICharText);
	}
}
void CUIPdaListItem::Update()
{
	inherited::Update();
}

void CUIPdaListItem::Draw()
{
	inherited::Draw();
}

void CUIPdaListItem::InitCharacter(CInventoryOwner* pInvOwner)
{
	VERIFY(pInvOwner);

	string256 str;
	sprintf(str, "%s", pInvOwner->CharacterInfo().Name());
	UIName.SetText(str);

	sprintf(str, "%d", pInvOwner->CharacterInfo().Rank());
	UICharText.GetPhraseByIndex(eRank)->str = str;

	sprintf(str, "%s", *pInvOwner->CharacterInfo().Community());
	UICharText.GetPhraseByIndex(eCommunity)->str = str;

	UIIcon.SetShader(InventoryUtilities::GetCharIconsShader());
	UIIcon.GetUIStaticItem().SetOriginalRect(
		pInvOwner->CharacterInfo().TradeIconX()*ICON_GRID_WIDTH,
		pInvOwner->CharacterInfo().TradeIconY()*ICON_GRID_HEIGHT,
		pInvOwner->CharacterInfo().TradeIconX()+CHAR_ICON_WIDTH*ICON_GRID_WIDTH,
		pInvOwner->CharacterInfo().TradeIconY()+CHAR_ICON_HEIGHT*ICON_GRID_HEIGHT);
	
	CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if (pActor)
	{
		CEntityAlive* ContactEA = smart_cast<CEntityAlive*>(pInvOwner);
		LPCSTR	relation_str = NULL;
		u32		color		 = 0;

		switch (ContactEA->tfGetRelationType(pActor))
		{
		case ALife::eRelationTypeFriend:
			relation_str	= "friend";
			color			= 0xff00ff00;
			break;
		case ALife::eRelationTypeNeutral:
			relation_str	= "neutral";
			color			= 0xffc0c0c0;
			break;
		case ALife::eRelationTypeEnemy:
			relation_str	= "enemy";
			color			= 0xffff0000;
			break;
		default:
			NODEFAULT;
		}

		string256 str;
		sprintf(str, "%s", relation_str);
		UICharText.GetPhraseByIndex(eRelation)->str = str;
		UICharText.GetPhraseByIndex(eRelation)->effect.SetTextColor(color);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaListItem::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (pWnd == this && BUTTON_FOCUS_RECEIVED == msg)
	{
		UIName.SetHighlightText(true);
	}
	else if (pWnd == this && BUTTON_FOCUS_LOST == msg)
	{
		UIName.SetHighlightText(false);
	}
}