#include "StdAfx.h"
#include <dinput.h>
#include "UISkinSelector.h"
#include "../level.h"
//#include"../hudmanager.h"
#include "UIXmlInit.h"
#include "UIStatic.h"
#include "UIStatix.h"
#include "../game_cl_deathmatch.h"

const char * const	SKIN_SELECTOR_XML		= "skin_selector.xml";

CUISkinSelectorWnd::CUISkinSelectorWnd(const char* strSectionName)
{	
	m_pBackground	= xr_new<CUIStatic>();	AttachChild(m_pBackground);
	m_pCaption		= xr_new<CUIStatic>();	AttachChild(m_pCaption);
	m_pFrames		= xr_new<CUIStatic>();	AttachChild(m_pFrames);
	for (int i = 0; i<4; i++)
	{
		m_pImage[i] = xr_new<CUIStatix>();
		AttachChild(m_pImage[i]);
	}
	m_fristSkin = 0;
	Init(strSectionName);
}

CUISkinSelectorWnd::~CUISkinSelectorWnd()
{
	xr_delete(m_pCaption);
	xr_delete(m_pBackground);
	xr_delete(m_pFrames);
	for (int i = 0; i<4; i++)
		xr_delete(m_pImage[i]);
}

void CUISkinSelectorWnd::InitSkins(){
	R_ASSERT2(pSettings->section_exist(m_strSection), *m_strSection);
	R_ASSERT2(pSettings->line_exist(m_strSection, "skins"), *m_strSection);

	LPCSTR lst = pSettings->r_string(m_strSection,"skins");
	string256	singleItem;
	u32 count	= _GetItemCount(lst);
	R_ASSERT2(count,"no skins in this game");
	for (u32 j = 0; j < count; ++j)
	{
		_GetItem(lst, j, singleItem);
		m_skins.push_back(singleItem);
	}
}

void CUISkinSelectorWnd::UpdateSkins(){
	//string64 buff;
	//for (int i = 0; i<4; i++)
	//{		
	//	sprintf(buff,"skin_selector:image_%d",i);
	//	CUIXmlInit::InitStatic(xml_doc,buff,0,m_pImage[i]);
 //       m_pImage[i]->InitTexture(m_skins[i].c_str());
	//	m_pImage[i]->RescaleRelative2Rect();
	//}

}

void CUISkinSelectorWnd::Init(const char* strSectionName)
{
	R_ASSERT(0 != strSectionName[0]);
	m_strSection = strSectionName;
	InitSkins();		

	CUIXml xml_doc;
	bool xml_result = xml_doc.Init(CONFIG_PATH, UI_PATH, SKIN_SELECTOR_XML);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit::InitWindow(xml_doc,"skin_selector",0,this);
	CUIXmlInit::InitStatic(xml_doc,"skin_selector:caption",		0,	m_pCaption);
	CUIXmlInit::InitStatic(xml_doc,"skin_selector:background",	0,	m_pBackground);
	CUIXmlInit::InitStatic(xml_doc,"skin_selector:image_frames",0,	m_pFrames);

	string64 buff;
	for (int i = 0; i<4; i++)
	{
		sprintf(buff,"skin_selector:image_%d",i);
		CUIXmlInit::InitStatic(xml_doc,buff,0,m_pImage[i]);
		m_pImage[i]->SetStretchTexture(true);
		m_pImage[i]->InitTexture(m_skins[i].c_str());
		m_pImage[i]->RescaleRelative2Rect(m_pImage[i]->GetStaticItem()->GetOriginalRect());
	}
	UpdateSkins();

	
	//CUIWindow::Init(CUIXmlInit::ApplyAlignX(0, alCenter),
	//				CUIXmlInit::ApplyAlignY(0, alCenter),
	//				UI_BASE_WIDTH, UI_BASE_HEIGHT);

	//for (int i = 0; i < SKINS_COUNT; ++i)
	//{
	//	AttachChild(&m_vSkinWindows[i]);
	//	m_vSkinWindows[i].Init(xmlDoc, i, m_strSection);
	//}

	// Ok button
//	Frect r = m_vSkinWindows[0].UIBackground.GetWndRect();

	//AttachChild(&UIOkBtn);
	//xml_init.InitButton(xmlDoc, "button", 0, &UIOkBtn);
	//Frect rect = UIOkBtn.GetWndRect();

	//UIOkBtn.SetWndPos(	UI_BASE_WIDTH/2.0f - ((rect.right - rect.left) * 1.5f),
	//					r.bottom + r.top/2.0f - UIOkBtn.GetHeight());

	//// cancel button
	//AttachChild(&UICancelBtn);
	//xml_init.InitButton(xmlDoc, "button", 1, &UICancelBtn);
	//rect	= UICancelBtn.GetWndRect();
	//UICancelBtn.SetWndPos(UI_BASE_WIDTH/2.0f + ((rect.right - rect.left) * 0.5f),
	//	r.bottom+r.top/2.0f - UICancelBtn.GetHeight());

	//SetFont(HUD().Font().pFontHeaderRussian);
}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	switch (msg){
		case BUTTON_CLICKED:
			for (int i = 0; i<4; i++)
                if (pWnd == m_pImage[i])
				{
                    m_iActiveIndex = i;
					OnBtnOK();
					break;
				}
			
			break;
	}
	//	case WINDOW_LBUTTON_DB_CLICK:
	//		for ( u8 i = 0; i < SKINS_COUNT; i++)
 //               if (pWnd == &m_vSkinWindows[i])
	//			{
	//				SwitchSkin(i);
	//				OnBtnOK();
	//			}
	//		break;
	//}
 //   
	//inherited::SendMessage(pWnd, msg, pData);
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

//int CUISkinSelectorWnd::SwitchSkin(const int idx)
//{
//	if (idx >= SKINS_COUNT) return m_uActiveIndex;

	//int prevIdx = m_uActiveIndex;
	//m_vSkinWindows[m_uActiveIndex].UIHighlight.SetColor(clInactive);
	//m_vSkinWindows[idx].UIHighlight.SetColor(clActive);
	//m_uActiveIndex = idx;
	//return prevIdx;
//}

////////////////////////////////////////////////////////////////////////////////

bool CUISkinSelectorWnd::OnMouse(float x, float y, EUIMessages mouse_action)
{
	CUIWindow::OnMouse(x,y,mouse_action);
	//Frect rect;

	//if(inherited::OnMouse(x, y, mouse_action)) return true; // need to generate DB_CLICK action
	//
	//switch (mouse_action){
	//	case WINDOW_LBUTTON_DOWN:
	//		for (int i = 0; i < SKINS_COUNT; ++i)
	//		{
	//			rect = m_vSkinWindows[i].UIHighlight.GetAbsoluteRect();
	//			if (rect.in(x,y) && i != m_uActiveIndex)
	//			{
	//				SwitchSkin(i);
	//				return true;
	//			}
	//		}
	//		break;
	//	default:
	//		break;
	//}	
	return false;
}



////////////////////////////////////////////////////////////////////////////////

bool CUISkinSelectorWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if (dik >= DIK_1 && dik < DIK_4 + 2 && WINDOW_KEY_PRESSED == keyboard_action)
	{
		m_iActiveIndex = dik - DIK_1;
		OnBtnOK();
		return true;
	}
	//if (DIK_RETURN == dik && WINDOW_KEY_PRESSED == keyboard_action)
	//{
	//	SendMessage(&UIOkBtn, BUTTON_CLICKED, NULL);
	//	return true;
	//}
	if (DIK_ESCAPE == dik && WINDOW_KEY_PRESSED == keyboard_action)
	{
		OnBtnCancel();
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

//void CUISkinSelectorWnd::DrawKBAccelerators()
//{
//	for (u8 i = 0; i < SKINS_COUNT; ++i)
//	{
//		Frect rect = m_vSkinWindows[i].UIBackground.GetAbsoluteRect();
//
//		Frect r;
//		r.set(0, 0, UI_BASE_WIDTH, UI_BASE_WIDTH);
//
//		UI()->OutText(GetFont(), r,
//			(rect.right - rect.left) / 2.0f + rect.left, 
//			float(rect.bottom - HUD().Font().pFontHeaderRussian->CurrentHeight()- 40),
//			"%d", i + 1);
//		GetFont()->OnRender();
//	}
//}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::Draw()
{
	inherited::Draw();
	//DrawKBAccelerators();
}

