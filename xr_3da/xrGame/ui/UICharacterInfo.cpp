// UICharacterInfo.cpp:  окошко, с информацией о персонаже
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;

#include "uicharacterinfo.h"
#include "../actor.h"
#include "../level.h"
#include "../character_info.h"
#include "../string_table.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

//////////////////////////////////////////////////////////////////////////

CUICharacterInfo::CUICharacterInfo()
{
}

//////////////////////////////////////////////////////////////////////////

CUICharacterInfo::~CUICharacterInfo()
{
}

//////////////////////////////////////////////////////////////////////////

void CUICharacterInfo::Init(int x, int y, int width, int height, const char* xml_name)
{
	inherited::Init(x, y, width, height);

	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", xml_name);
	R_ASSERT3(xml_result, "xml file not found", xml_name);

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

	AttachChild(&UIName);
	if(uiXml.NavigateToNode("name_static", 0))
	{
		xml_init.InitStatic(uiXml, "name_static", 0, &UIName);
		UIName.SetElipsis(CUIStatic::eepEnd, 0);
	}
	else
	{
		UIName.Show(false);
		UIName.Enable(false);
	}

	AttachChild(&UIRank);
	if(uiXml.NavigateToNode("rank_static", 0))
	{
		xml_init.InitStatic(uiXml, "rank_static", 0, &UIRank);
		UIRank.SetElipsis(CUIStatic::eepEnd, 0);
	}
	else
	{
		UIRank.Show(false);
		UIRank.Enable(false);
	}

	AttachChild(&UIRankCaption);
	if(uiXml.NavigateToNode("rank_caption", 0))
	{
		xml_init.InitStatic(uiXml, "rank_caption", 0, &UIRankCaption);
	}
	else
	{
		UIRankCaption.Show(false);
		UIRankCaption.Enable(false);
	}

	AttachChild(&UICommunity);
	if(uiXml.NavigateToNode("community_static", 0))
	{
		xml_init.InitStatic(uiXml, "community_static", 0, &UICommunity);
		UICommunity.SetElipsis(CUIStatic::eepEnd, 0);
	}
	else
	{
		UICommunity.Show(false);
		UICommunity.Enable(false);
	}

	AttachChild(&UICommunityCaption);
	if(uiXml.NavigateToNode("community_caption", 0))
	{
		xml_init.InitStatic(uiXml, "community_caption", 0, &UICommunityCaption);
	}
	else
	{
		UICommunityCaption.Show(false);
		UICommunityCaption.Enable(false);
	}

	AttachChild(&UIRelation);
	if(uiXml.NavigateToNode("relation_static", 0))
	{
		xml_init.InitStatic(uiXml, "relation_static", 0, &UIRelation);
		UIRelation.SetElipsis(CUIStatic::eepEnd, 0);
	}
	else
	{
		UIRelation.Enable(false);
		UIRelation.Show(false);
	}

	AttachChild(&UIRelationCaption);
	if(uiXml.NavigateToNode("relation_caption", 0))
	{
		xml_init.InitStatic(uiXml, "relation_caption", 0, &UIRelationCaption);
	}
	else
	{
		UIRelationCaption.Enable(false);
	}
	UIRelationCaption.Show(false);
}

//////////////////////////////////////////////////////////////////////////

void  CUICharacterInfo::InitCharacter(CCharacterInfo* pCharInfo, bool withPrefixes)
{
	VERIFY(pCharInfo);

	string256 str;
	sprintf(str, "%s", pCharInfo->Name());
	UIName.SetText(str);

	int offset;

	sprintf(str, "%d", pCharInfo->Rank());
	offset = static_cast<int>(UIRankCaption.GetFont()->SizeOf(UIRankCaption.GetText()) + UIRankCaption.GetWndRect().left + 5);
	UIRank.SetWndRect(offset, UIRank.GetWndRect().top, GetWndRect().right - offset - 10, UIRank.GetWndRect().bottom);
	UIRank.SetText(str);

	sprintf(str, "%s", pCharInfo->Community());
	offset = static_cast<int>(UICommunityCaption.GetFont()->SizeOf(UICommunityCaption.GetText()) + UICommunityCaption.GetWndRect().left + 5);
	UICommunity.SetWndRect(offset, UICommunity.GetWndRect().top, GetWndRect().right - offset - 10, UICommunity.GetWndRect().bottom - UICommunity.GetWndRect().top);
	UICommunity.SetText(str);

	UIIcon.SetShader(GetCharIconsShader());
	UIIcon.GetUIStaticItem().SetOriginalRect(
		pCharInfo->TradeIconX()*ICON_GRID_WIDTH,
		pCharInfo->TradeIconY()*ICON_GRID_HEIGHT,
		pCharInfo->TradeIconX()+CHAR_ICON_WIDTH*ICON_GRID_WIDTH,
		pCharInfo->TradeIconY()+CHAR_ICON_HEIGHT*ICON_GRID_HEIGHT);
}

//////////////////////////////////////////////////////////////////////////

void CUICharacterInfo::InitCharacter(CInventoryOwner* pInvOwner, bool withPrefixes)
{
	VERIFY(pInvOwner);
	InitCharacter(&pInvOwner->CharacterInfo(), withPrefixes);

	CActor *m_pActor = dynamic_cast<CActor *>(Level().CurrentEntity());

	CEntityAlive* ContactEA = dynamic_cast<CEntityAlive*>(pInvOwner);
	SetRelation(ContactEA->tfGetRelationType(m_pActor));
}

//////////////////////////////////////////////////////////////////////////

void  CUICharacterInfo::SetRelation(ALife::ERelationType relation, bool withPrefix)
{
	ref_str relation_str;

	CStringTable stbl;

	switch(relation) {
	case ALife::eRelationTypeFriend:
		UIRelation.SetTextColor(0xff00ff00);
		relation_str = stbl("friend");
		break;
	case ALife::eRelationTypeNeutral:
		UIRelation.SetTextColor(0xffc0c0c0);
		relation_str = stbl("neutral");
		break;
	case ALife::eRelationTypeEnemy:
		UIRelation.SetTextColor(0xffff0000);
		relation_str = stbl("enemy");
		break;
	default:
		NODEFAULT;
	}

	UIRelationCaption.Show(true);
	UIRelation.SetText(*relation_str);
	int offset = static_cast<int>(UIRelationCaption.GetFont()->SizeOf(UIRelationCaption.GetText()) + UIRelationCaption.GetWndRect().left + 5);
	UIRelation.SetWndRect(offset, UIRelation.GetWndRect().top, GetWndRect().right - offset - 10, UICommunity.GetWndRect().bottom - UIRelation.GetWndRect().top);
}

//////////////////////////////////////////////////////////////////////////

void CUICharacterInfo::ResetAllStrings()
{
	UIName.SetText("");
	UIRank.SetText("");
	UICommunity.SetText("");
	UIRelation.SetText("");
}