#include "StdAfx.h"
#include <dinput.h>
#include "UISkinSelector.h"
#include "../level.h"
//#include"../hudmanager.h"
#include "UIXmlInit.h"
#include "UIStatic.h"
#include "UIAnimatedStatic.h"
#include "UI3tButton.h"
#include "UIStatix.h"
#include "../game_cl_deathmatch.h"

CUISkinSelectorWnd::CUISkinSelectorWnd(const char* strSectionName)
{	
	m_iActiveIndex = -1;
	m_pBackground	= xr_new<CUIStatic>();	AttachChild(m_pBackground);
	m_pCaption		= xr_new<CUIStatic>();	AttachChild(m_pCaption);

	for (int i = 0; i<4; i++)
	{
		m_pImage[i] = xr_new<CUIStatix>();
		AttachChild(m_pImage[i]);
	}

	m_pFrames		= xr_new<CUIStatic>();	AttachChild(m_pFrames);
	m_pAnims[0]		= xr_new<CUIAnimatedStatic>(); m_pFrames->AttachChild(m_pAnims[0]);
	m_pAnims[1]		= xr_new<CUIAnimatedStatic>(); m_pFrames->AttachChild(m_pAnims[1]);
	m_pButtons[0]	= xr_new<CUI3tButton>();	m_pFrames->AttachChild(m_pButtons[0]); m_pButtons[0]->SetMessageTarget(this);
	m_pButtons[1]	= xr_new<CUI3tButton>();	m_pFrames->AttachChild(m_pButtons[1]); m_pButtons[1]->SetMessageTarget(this);

	m_pBtnAutoSelect= xr_new<CUI3tButton>();	AttachChild(m_pBtnAutoSelect);
	m_pBtnSpectator	= xr_new<CUI3tButton>();	AttachChild(m_pBtnSpectator);
	m_pBtnBack		= xr_new<CUI3tButton>();	AttachChild(m_pBtnBack);

	m_fristSkin = 0;
	Init(strSectionName);	
}

CUISkinSelectorWnd::~CUISkinSelectorWnd()
{
	xr_delete(m_pCaption);
	xr_delete(m_pBackground);
	xr_delete(m_pFrames);
	xr_delete(m_pButtons[0]);
	xr_delete(m_pButtons[1]);
	xr_delete(m_pAnims[0]);
	xr_delete(m_pAnims[1]);
	xr_delete(m_pBtnAutoSelect);
	xr_delete(m_pBtnSpectator);
	xr_delete(m_pBtnBack);	
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
		if (!!m_shader)
            m_pImage[i]->InitTextureEx(m_skins[i + m_fristSkin].c_str(), *m_shader);
		else
			m_pImage[i]->InitTexture(m_skins[i + m_fristSkin].c_str());
		m_pImage[i]->RescaleRelative2Rect(m_pImage[i]->GetStaticItem()->GetOriginalRect());

		if (m_iActiveIndex - m_fristSkin == i)
			m_pImage[i]->SetSelectedState(true);
		else
			m_pImage[i]->SetSelectedState(false);
	}

	m_pButtons[0]->Enable(m_fristSkin > 0);
	m_pButtons[1]->Enable(m_fristSkin + 4 < (int)m_skins.size());
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

	CUIXmlInit::Init3tButton(xml_doc,"skin_selector:image_frames:btn_left",	0,	m_pButtons[0]);
	CUIXmlInit::Init3tButton(xml_doc,"skin_selector:image_frames:btn_right",0,	m_pButtons[1]);

	CUIXmlInit::InitAnimatedStatic(xml_doc,"skin_selector:image_frames:a_static_1",	0,	m_pAnims[0]);
	CUIXmlInit::InitAnimatedStatic(xml_doc,"skin_selector:image_frames:a_static_2",	0,	m_pAnims[1]);

	CUIXmlInit::Init3tButton(xml_doc,"skin_selector:btn_spectator",	0,m_pBtnSpectator);
	CUIXmlInit::Init3tButton(xml_doc,"skin_selector:btn_autoselect",0,m_pBtnAutoSelect);
	CUIXmlInit::Init3tButton(xml_doc,"skin_selector:btn_back",		0,m_pBtnBack);

	if (xml_doc.NavigateToNode("skin_selector:skin_shader",0))
		m_shader = xml_doc.Read("skin_selector:skin_shader",0);

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
	game_cl_Deathmatch * dm = NULL;
	switch (msg){
		case BUTTON_CLICKED:
			dm = smart_cast<game_cl_Deathmatch *>(&(Game()));

			if (pWnd == m_pButtons[0])
				OnKeyLeft();
			else if (pWnd == m_pButtons[1])
				OnKeyRight();
			else if (pWnd == m_pBtnAutoSelect)
			{
				m_iActiveIndex = -1;
				OnBtnOK();		
			}
			else if (pWnd == m_pBtnSpectator)
			{
				Game().StartStopMenu(this,true);
				dm->OnSpectatorSelect();
			}
			else if (pWnd == m_pBtnBack)
			{
				Game().StartStopMenu(this,true);
				dm->OnSkinMenuBack();				
			}
			else
                for (int i = 0; i<4; i++)
					if (pWnd == m_pImage[i])
					{
						m_iActiveIndex = i;
						OnBtnOK();					
					}
			break;
		case STATIC_FOCUS_RECEIVED:
			if (pWnd == m_pButtons[0])
			{
				m_pAnims[0]->Rewind(0);
				m_pAnims[0]->Play();
			}
			else if (pWnd == m_pButtons[1])
			{
				m_pAnims[1]->Rewind(0);
				m_pAnims[1]->Play();
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

int	CUISkinSelectorWnd::GetActiveIndex(){ 
	if (-1 == m_iActiveIndex)
		return -1;
	else
        return m_iActiveIndex + m_fristSkin; 
} 	

void CUISkinSelectorWnd::SetVisibleForBtn(ESKINMENU_BTN btn, bool state){
	switch (btn)
	{
	case 	SKIN_MENU_BACK:			this->m_pBtnBack->SetVisible(state);		break;
	case	SKIN_MENU_SPECTATOR:	this->m_pBtnSpectator->SetVisible(state);	break;		
	case	SKIN_MENU_AUTOSELECT:	this->m_pBtnAutoSelect->SetVisible(state);	break;
	default:
		R_ASSERT2(false,"invalid btn ID");	
	}
}

void CUISkinSelectorWnd::SetCurSkin(int skin){
	R_ASSERT2(skin>= -1 && skin + m_fristSkin <= (int)m_skins.size(), "invalid skin index");

	m_iActiveIndex = skin;

	UpdateSkins();
}
