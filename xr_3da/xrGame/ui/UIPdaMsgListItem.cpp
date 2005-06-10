//////////////////////////////////////////////////////////////////////
// UIPdaMsgListItem.cpp: ������� ���� ������ � �������� 
// ������ ��� ��������� PDA
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIPdaMsgListItem.h"
#include "../Entity.h"
#include "../character_info.h"
#include "UIInventoryUtilities.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "uicoloranimatorwrapper.h"

#define PDA_MSG_MAINGAME_CHAR "maingame_pda_msg.xml"

using namespace InventoryUtilities;

CUIPdaMsgListItem::CUIPdaMsgListItem(void)
{
	m_iDelay = 0;
	m_timeBegin = 0;
}

CUIPdaMsgListItem::CUIPdaMsgListItem(int iDelay){
	m_iDelay = iDelay;
	m_timeBegin = 0;
}

CUIPdaMsgListItem::~CUIPdaMsgListItem(void)
{
}

void CUIPdaMsgListItem::SetFont(CGameFont* pFont){
	UIMsgText.SetFont(pFont);
}

void CUIPdaMsgListItem::Init(float x, float y, float width, float height)
{
	inherited::Init(x, y, width, height);

	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH,PDA_MSG_MAINGAME_CHAR);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit xml_init;
	AttachChild(&UIIcon);
	xml_init.InitStatic(uiXml, "icon_static", 0, &UIIcon);
	y = (height - UIIcon.GetHeight())/2;
	UIIcon.SetWndPos(UIIcon.GetWndPos().x , y);
	UIIcon.ClipperOn();

	AttachChild(&UIName);
	if(uiXml.NavigateToNode("name_static",0))
		xml_init.InitStatic(uiXml, "name_static", 0, &UIName);
	else
	{
		UIName.Show(false);
		UIName.Enable(false);
	}
//	y = (height - (int)UIName.GetFont()->CurrentHeight())/2;
//	UIName.SetTextPos(UIName.GetTextX(), y);		

	AttachChild(&UIMsgText);
	xml_init.InitStatic(uiXml, "text_static", 0, &UIMsgText);	
	UIMsgText.SetWidth(width - UIIcon.GetWidth());
	y = (height - (int)UIMsgText.GetFont()->CurrentHeight())/2;
	UIMsgText.SetTextPos(UIMsgText.GetTextX(), y);
}

void CUIPdaMsgListItem::SetDelay(int iDelay){
	m_iDelay = iDelay;
}

void CUIPdaMsgListItem::SetTextColor(u32 color){
	UIMsgText.SetTextColor(color);
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
					float(pInvOwner->CharacterInfo().TradeIconX()*ICON_GRID_WIDTH),
					float(pInvOwner->CharacterInfo().TradeIconY()*ICON_GRID_HEIGHT),
					float(pInvOwner->CharacterInfo().TradeIconX()+CHAR_ICON_WIDTH*ICON_GRID_WIDTH),
					float(pInvOwner->CharacterInfo().TradeIconY()+CHAR_ICON_HEIGHT*ICON_GRID_HEIGHT));
}

bool CUIPdaMsgListItem::IsTimeToDestroy(){
	if (m_iDelay < 0 )  // if delay less than ZERO then we never returns true
		return false;

	if (0 == m_iDelay)
		return true;

	if (0 == m_timeBegin)	
		m_timeBegin = Device.dwTimeGlobal;

	if (m_iDelay < Device.dwTimeGlobal - m_timeBegin)
	{		
		CUIColorAnimatorWrapper *p = reinterpret_cast<CUIColorAnimatorWrapper*>(GetData());
		p->Reset();
		m_iDelay = 0;
		return true;
	}

	return false;
}