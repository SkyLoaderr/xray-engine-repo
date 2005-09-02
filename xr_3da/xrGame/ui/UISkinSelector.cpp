#include "StdAfx.h"
#include <dinput.h>
#include "UISkinSelector.h"
#include "../level.h"
//#include"../hudmanager.h"
#include "UIXmlInit.h"
#include "UIStatic.h"
#include "UIStatix.h"
#include "../game_cl_deathmatch.h"

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
	for (int i = 0; i<4; i++)
	{
		m_pImage[i]->InitTexture(m_skins[i + m_fristSkin].c_str());
		m_pImage[i]->RescaleRelative2Rect(m_pImage[i]->GetStaticItem()->GetOriginalRect());
	}
}

void CUISkinSelectorWnd::Init(const char* strSectionName)
{
	R_ASSERT(0 != strSectionName[0]);
	m_strSection = strSectionName;
	
	CUIXml xml_doc;
	bool xml_result = xml_doc.Init(CONFIG_PATH, UI_PATH, "skin_selector.xml");
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit::InitWindow(xml_doc,"skin_selector",				0,	this);
	CUIXmlInit::InitStatic(xml_doc,"skin_selector:caption",		0,	m_pCaption);
	CUIXmlInit::InitStatic(xml_doc,"skin_selector:background",	0,	m_pBackground);
	CUIXmlInit::InitStatic(xml_doc,"skin_selector:image_frames",0,	m_pFrames);

	InitSkins();
	string64 buff;
	for (int i = 0; i<4; i++)
	{
		sprintf(buff,"skin_selector:image_%d",i);
		CUIXmlInit::InitStatic(xml_doc,buff,0,m_pImage[i]);
	}
	UpdateSkins();
}

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
}

void CUISkinSelectorWnd::OnBtnCancel(){
    Game().StartStopMenu(this,true);
}

void CUISkinSelectorWnd::OnBtnOK(){
	Game().StartStopMenu(this,true);
	game_cl_Deathmatch * dm = smart_cast<game_cl_Deathmatch *>(&(Game()));
	dm->OnSkinMenu_Ok();
}

bool CUISkinSelectorWnd::OnMouse(float x, float y, EUIMessages mouse_action)
{
	return CUIWindow::OnMouse(x,y,mouse_action);	
}

bool CUISkinSelectorWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if (WINDOW_KEY_PRESSED != keyboard_action)
		return false;

	if (dik >= DIK_1 && dik < DIK_4 + 2)
	{
		m_iActiveIndex = dik - DIK_1;
		OnBtnOK();
		return true;
	}
	if (DIK_ESCAPE == dik)
	{
		OnBtnCancel();
		return true;
	}

	if (DIK_LEFT == dik)
	{
		OnKeyLeft();
		return true;
	}
	if (DIK_RIGHT == dik)
	{
		OnKeyRight();
		return true;
	}

	return false;
}

void CUISkinSelectorWnd::OnKeyLeft()
{	
	if (m_fristSkin > 0)
	{
		m_fristSkin--;
		UpdateSkins();
	}
}

void CUISkinSelectorWnd::OnKeyRight()
{
	if (m_fristSkin + 4 < (int)m_skins.size())
	{
		m_fristSkin++;
		UpdateSkins();
	}	
}

