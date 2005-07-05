//-----------------------------------------------------------------------------/
//  ���� ������ ����� � ������� ����
//-----------------------------------------------------------------------------/

#include "StdAfx.h"
#include <dinput.h>
#include "UISkinSelector.h"
#include "../level.h"
#include"../hudmanager.h"

#include "../game_cl_deathmatch.h"

const char * const	SKIN_SELECTOR_XML		= "skin_selector.xml";

CUISkinSelectorWnd::CUISkinSelectorWnd(const char* strSectionName)
	: m_uActiveIndex(0)
{
	Init(strSectionName);

	Hide();
}

CUISkinSelectorWnd::CUISkinSelectorWnd()
{
	// For tests
//	Init("deathmatch_team0");
}

CUISkinSelectorWnd::~CUISkinSelectorWnd()
{
}

void CUISkinSelectorWnd::Init(const char *strSectionName)
{
	R_ASSERT(xr_strcmp(strSectionName, ""));
	m_strSection = strSectionName;

	CUIXml xmlDoc;
	bool xml_result = xmlDoc.Init(CONFIG_PATH, UI_PATH, SKIN_SELECTOR_XML);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit xml_init;

	CUIWindow::Init(CUIXmlInit::ApplyAlignX(0, alCenter),
					CUIXmlInit::ApplyAlignY(0, alCenter),
					UI_BASE_WIDTH, UI_BASE_HEIGHT);

	for (int i = 0; i < SKINS_COUNT; ++i)
	{
		AttachChild(&m_vSkinWindows[i]);
		m_vSkinWindows[i].Init(xmlDoc, i, m_strSection);
	}

	// Ok button
	Frect r = m_vSkinWindows[0].UIBackground.GetWndRect();

	AttachChild(&UIOkBtn);
	xml_init.InitButton(xmlDoc, "button", 0, &UIOkBtn);
	Frect rect = UIOkBtn.GetWndRect();

	UIOkBtn.SetWndPos(	UI_BASE_WIDTH/2.0f - ((rect.right - rect.left) * 1.5f),
						r.bottom + r.top/2.0f - UIOkBtn.GetHeight());

	// cancel button
	AttachChild(&UICancelBtn);
	xml_init.InitButton(xmlDoc, "button", 1, &UICancelBtn);
	rect	= UICancelBtn.GetWndRect();
	UICancelBtn.SetWndPos(UI_BASE_WIDTH/2.0f + ((rect.right - rect.left) * 0.5f),
		r.bottom+r.top/2.0f - UICancelBtn.GetHeight());

	SetFont(HUD().Font().pFontHeaderRussian);
}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	switch (msg){
		case BUTTON_CLICKED:
			if (pWnd == &UIOkBtn)
				OnBtnOK();
			else if (pWnd == &UICancelBtn)
				OnBtnCancel();
			break;
		case WINDOW_LBUTTON_DB_CLICK:
			for ( u8 i = 0; i < SKINS_COUNT; i++)
                if (pWnd == &m_vSkinWindows[i])
				{
					SwitchSkin(i);
					OnBtnOK();
				}
			break;
	}
    
	inherited::SendMessage(pWnd, msg, pData);
}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::OnBtnCancel(){
    Game().StartStopMenu(this,true);
}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::OnBtnOK(){
	Game().StartStopMenu(this,true);
	game_cl_Deathmatch * dm = smart_cast<game_cl_Deathmatch *>(&(Game()));
	dm->OnSkinMenu_Ok();
}

////////////////////////////////////////////////////////////////////////////////

int CUISkinSelectorWnd::SwitchSkin(const int idx)
{
	if (idx >= SKINS_COUNT) return m_uActiveIndex;

	int prevIdx = m_uActiveIndex;
	m_vSkinWindows[m_uActiveIndex].UIHighlight.SetColor(clInactive);
	m_vSkinWindows[idx].UIHighlight.SetColor(clActive);
	m_uActiveIndex = idx;
	return prevIdx;
}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::OnMouse(float x, float y, EUIMessages mouse_action)
{
	Frect rect;

	inherited::OnMouse(x, y, mouse_action); // need to generate DB_CLICK action
	
	switch (mouse_action){
		case WINDOW_LBUTTON_DOWN:
			for (int i = 0; i < SKINS_COUNT; ++i)
			{
				rect = m_vSkinWindows[i].UIHighlight.GetAbsoluteRect();
				if (rect.in(x,y) && i != m_uActiveIndex)
				{
					SwitchSkin(i);
				}
			}
			break;
		default:
			break;
	}	
}



////////////////////////////////////////////////////////////////////////////////

bool CUISkinSelectorWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if (dik > DIK_ESCAPE && dik < SKINS_COUNT + 2 && WINDOW_KEY_PRESSED == keyboard_action)
	{
		SwitchSkin(static_cast<u8>(dik - 2));
		return true;
	}
	if (DIK_RETURN == dik && WINDOW_KEY_PRESSED == keyboard_action)
	{
		SendMessage(&UIOkBtn, BUTTON_CLICKED, NULL);
		return true;
	}
	if (DIK_ESCAPE == dik && WINDOW_KEY_PRESSED == keyboard_action)
	{
		SendMessage(&UICancelBtn, BUTTON_CLICKED, NULL);
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::DrawKBAccelerators()
{
	for (u8 i = 0; i < SKINS_COUNT; ++i)
	{
		Frect rect = m_vSkinWindows[i].UIBackground.GetAbsoluteRect();

		Frect r;
		r.set(0, 0, UI_BASE_WIDTH, UI_BASE_WIDTH);

		UI()->OutText(GetFont(), r,
			(rect.right - rect.left) / 2.0f + rect.left, 
			float(rect.bottom - HUD().Font().pFontHeaderRussian->CurrentHeight()- 40),
			"%d", i + 1);
		GetFont()->OnRender();
	}
}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::Draw()
{
	inherited::Draw();
	DrawKBAccelerators();
}
