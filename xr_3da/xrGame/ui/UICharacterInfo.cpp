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
	bool xml_result = uiXml.Init("$game_data$",xml_name);
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
}

void CUICharacterInfo::InitCharacter(CInventoryOwner* pInvOwner)
{
	VERIFY(pInvOwner);

	string256 str;
	sprintf(str, "name: %s", pInvOwner->CharacterInfo().Name());
	UIName.SetText(str);

	sprintf(str, "rank: %s", pInvOwner->CharacterInfo().Rank());
	UIRank.SetText(str);

	sprintf(str, "community: %s", pInvOwner->CharacterInfo().Community());
	UICommunity.SetText(str);

	CEntity* pInvOwnerEntity = dynamic_cast<CEntity*>(pInvOwner);

	UIIcon.SetShader(GetCharIconsShader());
	UIIcon.GetUIStaticItem().SetOriginalRect(
					pInvOwner->CharacterInfo().TradeIconX()*ICON_GRID_WIDTH,
					pInvOwner->CharacterInfo().TradeIconY()*ICON_GRID_HEIGHT,
					pInvOwner->CharacterInfo().TradeIconX()+CHAR_ICON_WIDTH*ICON_GRID_WIDTH,
					pInvOwner->CharacterInfo().TradeIconY()+CHAR_ICON_HEIGHT*ICON_GRID_HEIGHT);
}

void CUICharacterInfo::ResetAllStrings()
{
	UIName.SetText("");
	UIRank.SetText("");
	UICommunity.SetText("");
	UIText.SetText("");
}