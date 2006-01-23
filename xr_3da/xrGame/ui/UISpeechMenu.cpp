#include "StdAfx.h"
#include "UISpeechMenu.h"
#include "UIListWnd.h"
#include "xrXmlParser.h"
#include "UIXmlInit.h"
#include "../game_cl_mp.h"
#include <dinput.h>
#include "../level.h"
#include "../string_table.h"
#include "../MainUI.h"
#include "../HUDManager.h"

CUISpeechMenu::CUISpeechMenu(LPCSTR section_name){
	m_pList = xr_new<CUIListWnd>();AttachChild(m_pList);m_pList->SetAutoDelete(true);
	CUIXml xml_doc;
	xml_doc.Init(CONFIG_PATH, UI_PATH, "maingame.xml");
	CUIXmlInit::InitWindow(xml_doc, "speech_menu",0,this);
	float h = xml_doc.ReadAttribFlt("speech_menu",0,"item_height");
	if (h)
		m_pList->SetItemHeight(h);
	// default text settings
	m_text_color = 0xffffffff;
	SetFont(UI()->Font()->pFontLetterica18Russian);
	CUIXmlInit::InitFont(xml_doc,"speech_menu:text",0,m_text_color,m_pFont);	
    InitList(section_name);
}

CUISpeechMenu::~CUISpeechMenu()
{
	int x=0;
	x=x;
}

void CUISpeechMenu::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);
	m_pList->Init(0,0,width,height);
}

void CUISpeechMenu::InitList(LPCSTR section_name){
	R_ASSERT2(pSettings->section_exist(section_name), section_name);
	CUIListItem* pItem = NULL;

	string64 phrase;
	string256 str;
	for (int i = 0; true; i++)
	{
		CStringTable st;
		sprintf(phrase,"phrase_%i",i);		
		if (pSettings->line_exist(section_name, phrase))
		{
			
            LPCSTR s = pSettings->r_string(section_name, phrase);
			_GetItem(s,0,phrase);
			sprintf(str, "%d. %s",i+1, *st.translate(phrase));
            pItem = xr_new<CUIListItem>();
			pItem->SetFont(GetFont());
			pItem->SetTextColor(m_text_color);
			pItem->SetText(str);
			m_pList->AddItem(pItem);
		}
		else
			break;
	}
}

bool CUISpeechMenu::OnKeyboard(int dik, EUIMessages keyboard_action){
	if (m_pList->GetItemsCount() < dik - DIK_1 + 1)
		return CUIDialogWnd::OnKeyboard(dik, keyboard_action);
    if (dik < DIK_1 || dik > DIK_0)
		return CUIDialogWnd::OnKeyboard(dik, keyboard_action);

	game_cl_mp* game = smart_cast<game_cl_mp*>(&Game());

	Game().StartStopMenu(this,true);
	game->OnMessageSelected(this, static_cast<u8>(dik - DIK_1));

//	CUIDialogWnd::OnKeyboard(dik, keyboard_action);

	return true;
}