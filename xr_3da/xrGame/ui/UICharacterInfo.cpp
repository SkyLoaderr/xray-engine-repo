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
#include "../relation_registry.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

//////////////////////////////////////////////////////////////////////////

CUICharacterInfo::CUICharacterInfo()
	:	m_bInfoAutoAdjust		(true)
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
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, xml_name);
	R_ASSERT3(xml_result, "xml file not found", xml_name);

	CUIXmlInit xml_init;

	AttachChild(&UIIcon);
	if(uiXml.NavigateToNode("icon_static",0))	
	{
		xml_init.InitStatic(uiXml, "icon_static", 0, &UIIcon);
		UIIcon.ClipperOn();
//.		UIIcon.SetStretchTexture(true);
		UIIcon.Show(true);
		UIIcon.Enable(true);
	}
	else
	{
		UIIcon.Show(false);
		UIIcon.Enable(false);
	}

	AttachChild(&UIName);
	if(uiXml.NavigateToNode("name_static", 0)){
		xml_init.InitStatic(uiXml, "name_static", 0, &UIName);
		UIName.SetElipsis(CUIStatic::eepEnd, 0);
	}else{
		UIName.Show(false);
		UIName.Enable(false);
	}


	///////////////
	// rank
	AttachChild(&UIRank);
	if(uiXml.NavigateToNode("rank_static", 0))
	{
		xml_init.InitStatic(uiXml, "rank_static", 0, &UIRank);
		UIRank.SetElipsis(CUIStatic::eepEnd, 1);
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


	/////////////////////
	//community
	AttachChild(&UICommunity);
	if(uiXml.NavigateToNode("community_static", 0))
	{
		xml_init.InitStatic(uiXml, "community_static", 0, &UICommunity);
		UICommunity.SetElipsis(CUIStatic::eepEnd, 1);
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

	/////////////////////
	//reputation
	AttachChild(&UIReputation);
	if(uiXml.NavigateToNode("reputation_static", 0))
	{
		xml_init.InitStatic(uiXml, "reputation_static", 0, &UIReputation);
		UIReputation.SetElipsis(CUIStatic::eepEnd, 1);
	}
	else
	{
		UIReputation.Enable(false);
		UIReputation.Show(false);
	}
	AttachChild(&UIReputationCaption);
	if(uiXml.NavigateToNode("reputation_caption", 0))
	{
		xml_init.InitStatic(uiXml, "reputation_caption", 0, &UIReputationCaption);
	}
	else
	{
		UIReputationCaption.Enable(false);
		UIReputationCaption.Show(false);
	}

	///////////////////
	// relation
	AttachChild(&UIRelation);
	if(uiXml.NavigateToNode("relation_static", 0))
	{
		xml_init.InitStatic(uiXml, "relation_static", 0, &UIRelation);
		UIRelation.SetElipsis(CUIStatic::eepEnd, 1);
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
		UIRelationCaption.Show(false);
	}

	AttachChild(&UIBio);
	if (uiXml.NavigateToNode("biography_list", 0))
	{
		xml_init.InitListWnd(uiXml, "biography_list", 0, &UIBio);
	}
	else
	{
		UIBio.Enable(false);
		UIBio.Show(false);
	}
}

//////////////////////////////////////////////////////////////////////////

void  CUICharacterInfo::InitCharacter(CCharacterInfo* pCharInfo)
{
	VERIFY(pCharInfo);

	string256		str;
	sprintf			(str, "%s", pCharInfo->Name());
	UIName.SetText	(str);
	CStringTable	stbl;

	int offset;

#ifdef _DEBUG
	sprintf(str, "%s,%d", *stbl(GetRankAsText(pCharInfo->Rank().value())), pCharInfo->Rank().value());
#else
	sprintf(str, "%s", *stbl(GetRankAsText(pCharInfo->Rank().value())));
#endif
	if (m_bInfoAutoAdjust)
	{
		if (UIRankCaption.IsEnabled() && UIRankCaption.GetFont())
		{
			offset = static_cast<int>(UIRankCaption.GetFont()->SizeOf(UIRankCaption.GetText()) + UIRankCaption.GetWndRect().left + 5);
			UIRank.SetWndRect(offset, UIRank.GetWndRect().top, GetWndRect().right - offset - 10, UIRank.GetWndRect().bottom);
		}
	}
	UIRank.SetText(str);


#ifdef _DEBUG
	sprintf(str, "%s,%d", *stbl(GetReputationAsText(pCharInfo->Reputation().value())), pCharInfo->Reputation().value());
#else
	sprintf(str, "%s", *stbl(GetReputationAsText(pCharInfo->Reputation().value())));
#endif
	if (m_bInfoAutoAdjust)
	{
		if (UIReputationCaption.IsEnabled() && UIReputationCaption.GetFont())
		{
			offset = static_cast<int>(UIReputationCaption.GetFont()->SizeOf(UIRelationCaption.GetText()) + UIReputationCaption.GetWndRect().left + 5);
			UIReputation.SetWndRect(offset, UIReputation.GetWndRect().top, GetWndRect().right - offset - 10, UIReputation.GetWndRect().bottom);
		}
	}
	UIReputation.SetText(str);

	sprintf(str, "%s", *CStringTable()(pCharInfo->Community().id()));
	if (m_bInfoAutoAdjust)
	{
		if (UICommunityCaption.IsEnabled() && UICommunityCaption.GetFont())
		{
			offset = static_cast<int>(UICommunityCaption.GetFont()->SizeOf(UICommunityCaption.GetText()) + UICommunityCaption.GetWndRect().left + 5);
			UICommunity.SetWndRect(offset, UICommunity.GetWndRect().top, GetWndRect().right - offset - 10, UICommunity.GetWndRect().bottom - UICommunity.GetWndRect().top);
		}
	}
	UICommunity.SetText(str);

	UIIcon.SetShader(GetCharIconsShader());
	UIIcon.GetUIStaticItem().SetOriginalRect(
		pCharInfo->TradeIconX()*ICON_GRID_WIDTH,
		pCharInfo->TradeIconY()*ICON_GRID_HEIGHT,
		pCharInfo->TradeIconX()+CHAR_ICON_WIDTH*ICON_GRID_WIDTH,
		pCharInfo->TradeIconY()+CHAR_ICON_HEIGHT*ICON_GRID_HEIGHT);

	// Bio
	if (UIBio.IsEnabled())
	{
		UIBio.RemoveAll();
		static CUIString str;
		if (pCharInfo->Bio())
		{
			str.SetText(pCharInfo->Bio());
			UIBio.AddParsedItem<CUIListItem>(str, 0, UIBio.GetTextColor(), UIBio.GetFont());
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUICharacterInfo::InitCharacter(CInventoryOwner* pInvOwner)
{
	VERIFY(pInvOwner);
	InitCharacter(&pInvOwner->CharacterInfo());

	CActor *m_pActor = smart_cast<CActor *>(pInvOwner);
	if (m_pActor)
	{
		UIRelationCaption.Show(false);
		UIRelation.Show(false);
	}
	else
	{
		CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(pActor)
			SetRelation(RELATION_REGISTRY().GetRelationType(pInvOwner, static_cast<CInventoryOwner*>(pActor)),
						RELATION_REGISTRY().GetAttitude(pInvOwner, static_cast<CInventoryOwner*>(pActor)));
	}
}

//////////////////////////////////////////////////////////////////////////


void  CUICharacterInfo::SetRelation(ALife::ERelationType relation, CHARACTER_GOODWILL goodwill)
{
	shared_str relation_str;

	CStringTable stbl;

	switch(relation) {
	case ALife::eRelationTypeFriend:
		UIRelation.SetTextColor(0xff00ff00);
		break;
	case ALife::eRelationTypeNeutral:
		UIRelation.SetTextColor(0xffc0c0c0);
		break;
	case ALife::eRelationTypeEnemy:
		UIRelation.SetTextColor(0xffff0000);
		break;
	default:
		NODEFAULT;
	}

	string256		str;
#ifdef _DEBUG
	sprintf(str, "%s,%d", *stbl(GetGoodwillAsText(goodwill)), goodwill);
#else
	sprintf(str, "%s", *stbl(GetGoodwillAsText(goodwill)));
#endif

	UIRelation.SetText(str);
	if (m_bInfoAutoAdjust)
	{
		int offset = static_cast<int>(UIRelationCaption.GetFont()->SizeOf(UIRelationCaption.GetText()) + UIRelationCaption.GetWndRect().left + 5);
		UIRelation.SetWndRect(offset, UIRelation.GetWndRect().top, GetWndRect().right - offset - 10, UICommunity.GetWndRect().bottom - UIRelation.GetWndRect().top);
	}
}


//////////////////////////////////////////////////////////////////////////

void CUICharacterInfo::ResetAllStrings()
{
	UIName.SetText("");
	UIRank.SetText("");
	UICommunity.SetText("");
	UIRelation.SetText("");
	UIReputation.SetText("");
}