//////////////////////////////////////////////////////////////////////
// UIPdaMsgListItem.cpp: ������� ���� ������ � �������� 
// ������ ��� ��������� PDA
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "UIPdaMsgListItem.h"

#include "../Entity.h"
#include "../character_info.h"

#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#define PDA_MSG_MAINGAME_CHAR "maingame_pda_msg.xml"

CUIPdaMsgListItem::CUIPdaMsgListItem(void)
{
}

CUIPdaMsgListItem::~CUIPdaMsgListItem(void)
{
}

void CUIPdaMsgListItem::Init(int x, int y, int width, int height)
{
	inherited::Init(x, y, width, height);

	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$",PDA_MSG_MAINGAME_CHAR);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit xml_init;
	AttachChild(&UIIcon);
	xml_init.InitStatic(uiXml, "icon_static", 0, &UIIcon);
	UIIcon.ClipperOn();

	AttachChild(&UIName);
	if(uiXml.NavigateToNode("name_static",0))
		xml_init.InitStatic(uiXml, "name_static", 0, &UIName);
	else
	{
		UIName.Show(false);
		UIName.Enable(false);
	}

	AttachChild(&UIMsgText);
	xml_init.InitStatic(uiXml, "text_static", 0, &UIMsgText);
}
void CUIPdaMsgListItem::Update()
{
	inherited::Update();
}

void CUIPdaMsgListItem::Draw()
{
	inherited::Draw();
}

void CUIPdaMsgListItem::InitCharacter(CInventoryOwner* pInvOwner)
{
	VERIFY(pInvOwner);

	string256 str;
	sprintf(str, "name: %s", pInvOwner->CharacterInfo().Name());
	UIName.SetText(str);

	UIIcon.SetShader(GetCharIconsShader());
	UIIcon.GetUIStaticItem().SetOriginalRect(
					pInvOwner->CharacterInfo().TradeIconX()*ICON_GRID_WIDTH,
					pInvOwner->CharacterInfo().TradeIconY()*ICON_GRID_HEIGHT,
					pInvOwner->CharacterInfo().TradeIconX()+CHAR_ICON_WIDTH*ICON_GRID_WIDTH,
					pInvOwner->CharacterInfo().TradeIconY()+CHAR_ICON_HEIGHT*ICON_GRID_HEIGHT);
}