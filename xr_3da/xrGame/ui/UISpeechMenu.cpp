#include "StdAfx.h"
#include "UISpeechMenu.h"
#include "UIListWnd.h"
#include "xrXmlParser.h"
#include "UIXmlInit.h"
#include "../game_cl_mp.h"
#include <dinput.h>
#include "../level.h"

CUISpeechMenu::CUISpeechMenu(LPCSTR section_name){
	m_pList = xr_new<CUIListWnd>();AttachChild(m_pList);m_pList->SetAutoDelete(true);
	CUIXml xml_doc;
	xml_doc.Init(CONFIG_PATH, UI_PATH, "maingame.xml");
	CUIXmlInit::InitWindow(xml_doc, "speech_menu",0,this);
    InitList(section_name);
}

void CUISpeechMenu::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);
	m_pList->Init(0,0,width,height);
}

void CUISpeechMenu::InitList(LPCSTR section_name){
	R_ASSERT2(pSettings->section_exist(section_name), section_name);

	string256 phrase;
	for (int i = 0; true; i++)
	{
		sprintf(phrase,"phrase_%i",i);		
		if (pSettings->line_exist(section_name, phrase))
		{
            LPCSTR s = pSettings->r_string(section_name, phrase);
			sprintf(phrase, "%d. %s",i+1, s);
			m_pList->AddItem<CUIListItem>(phrase, 0, NULL, 0, -1);
		}
		else
			break;
	}
}

bool CUISpeechMenu::OnKeyboard(int dik, EUIMessages keyboard_action){
	if (m_pList->GetItemsCount() < dik - DIK_1 + 1)
		return false;
    if (dik < DIK_1 || dik > DIK_0)
		return false;

	game_cl_mp* game = smart_cast<game_cl_mp*>(&Game());

	game->OnMessageSelected(0, static_cast<u8>(dik - DIK_1));


	return true;
}