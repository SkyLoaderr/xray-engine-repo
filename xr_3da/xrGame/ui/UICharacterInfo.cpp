// UICharacterInfo.cpp:  окошко, с информацией о персонаже
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;

#include "uicharacterinfo.h"
#include "../entity.h"
#include "../character_info.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

CUICharacterInfo::CUICharacterInfo()
{
}

CUICharacterInfo::~CUICharacterInfo()
{
}

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
	if(uiXml.NavigateToNode("static", 0))
	{
		xml_init.InitStatic(uiXml, "static", 0, &UIName);
	}
	else
	{
		UIName.Show(false);
		UIName.Enable(false);
	}

	AttachChild(&UIRank);
	if(uiXml.NavigateToNode("static", 1))
		xml_init.InitStatic(uiXml, "static", 1, &UIRank);
	else
	{
		UIRank.Show(false);
		UIRank.Enable(false);
	}

	AttachChild(&UICommunity);
	if(uiXml.NavigateToNode("static", 2))
		xml_init.InitStatic(uiXml, "static", 2, &UICommunity);
	else
	{
		UICommunity.Show(false);
		UICommunity.Enable(false);
	}

	AttachChild(&UIText);
	if(uiXml.NavigateToNode("description", 0))
		xml_init.InitStatic(uiXml, "description", 0, &UIText);
	else
	{
		UIText.Show(false);
		UIText.Enable(false);
	}

	AttachChild(&UIRelation);
	if(uiXml.NavigateToNode("relation_static", 0))
		xml_init.InitStatic(uiXml, "relation_static", 0, &UIRelation);
	else
	{
		UIRelation.Show(false);
		UIRelation.Enable(false);
	}
}


void  CUICharacterInfo::InitCharacter(CCharacterInfo* pCharInfo, bool withPrefixes)
{
	VERIFY(pCharInfo);

	static const ref_str withPrefixesPatterns[3] =
	{
		"name: %s",
			"rank: %d",
			"community: %s"
	};

	static const ref_str withoutPrefixesPatterns[3] =
	{
		"%s",
			"%d",
			"%s"
	};

	const ref_str * patterns = withPrefixes ? withPrefixesPatterns : withoutPrefixesPatterns;

	string256 str;
	sprintf(str, *patterns[0], pCharInfo->Name());
	UIName.SetText(str);

	sprintf(str, *patterns[1], pCharInfo->Rank());
	UIRank.SetText(str);

	sprintf(str, *patterns[2], pCharInfo->Community());
	UICommunity.SetText(str);

	UIIcon.SetShader(GetCharIconsShader());
	UIIcon.GetUIStaticItem().SetOriginalRect(
		pCharInfo->TradeIconX()*ICON_GRID_WIDTH,
		pCharInfo->TradeIconY()*ICON_GRID_HEIGHT,
		pCharInfo->TradeIconX()+CHAR_ICON_WIDTH*ICON_GRID_WIDTH,
		pCharInfo->TradeIconY()+CHAR_ICON_HEIGHT*ICON_GRID_HEIGHT);
}

void CUICharacterInfo::InitCharacter(CInventoryOwner* pInvOwner, bool withPrefixes)
{
	VERIFY(pInvOwner);
	InitCharacter(&pInvOwner->CharacterInfo(), withPrefixes);
}

void  CUICharacterInfo::SetRelation(ALife::ERelationType relation, bool withPrefix)
{
	LPCSTR relation_str = NULL;

	switch(relation) {
	case ALife::eRelationTypeFriend:
		relation_str = "%c0,200,0friend";
		break;
	case ALife::eRelationTypeNeutral:
		relation_str = "%c192,192,192neutral";
		break;
	case ALife::eRelationTypeEnemy:
		relation_str = "%c200,0,0enemy";
		break;
	default:
		NODEFAULT;
	}


	string256 str;
	if (withPrefix)
		sprintf(str, "relation: %s", relation_str);
	else
		sprintf(str, "%s", relation_str);
	UIRelation.SetText(str);
}

void CUICharacterInfo::ResetAllStrings()
{
	UIName.SetText("");
	UIRank.SetText("");
	UICommunity.SetText("");
	UIText.SetText("");
	UIRelation.SetText("");
}