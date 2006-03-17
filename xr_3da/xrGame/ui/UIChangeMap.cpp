#include "StdAfx.h"

#include "UIChangeMap.h"
#include "UIVotingCategory.h"
#include "UIXmlInit.h"
#include "xrXmlParser.h"
#include "UI3tButton.h"
#include "UIListBox.h"
#include "UIListBoxItem.h"
//#include "UIMapList.h"
#include "../level.h"
//#include "../game_cl_base.h"
#include "../game_cl_teamdeathmatch.h"
#include "../../xr_ioconsole.h"

#define	MAP_LIST	"map_list.ltx"

CUIChangeMap::CUIChangeMap(){
	m_prev_upd_time = 0;

	bkgrnd = xr_new<CUIStatic>(); bkgrnd->SetAutoDelete(true);
	AttachChild(bkgrnd);

	header = xr_new<CUIStatic>(); header->SetAutoDelete(true);
	AttachChild(header);

	lst_back = xr_new<CUIFrameWindow>(); lst_back->SetAutoDelete(true);
	AttachChild(lst_back);

	lst = xr_new<CUIListBox>(); lst->SetAutoDelete(true);
	AttachChild(lst);

	btn_ok = xr_new<CUI3tButton>(); btn_ok->SetAutoDelete(true);
	AttachChild(btn_ok);

	btn_cancel = xr_new<CUI3tButton>(); btn_cancel->SetAutoDelete(true);
	AttachChild(btn_cancel);

	selected_item = u32(-1);
}

void CUIChangeMap::Init(CUIXml& xml_doc){
	CUIXmlInit::InitWindow(xml_doc,			"change_map", 0, this);
	CUIXmlInit::InitStatic(xml_doc,			"change_map:header", 0, header);
	CUIXmlInit::InitStatic(xml_doc,			"change_map:background", 0, bkgrnd);
	CUIXmlInit::InitFrameWindow(xml_doc,	"change_map:list_back", 0, lst_back);
	CUIXmlInit::InitListBox(xml_doc,		"change_map:list", 0, lst);
	CUIXmlInit::Init3tButton(xml_doc,		"change_map:btn_ok", 0, btn_ok);
	CUIXmlInit::Init3tButton(xml_doc,		"change_map:btn_cancel", 0, btn_cancel);
	FillUpList();
}

#include <dinput.h>
bool CUIChangeMap::OnKeyboard(int dik, EUIMessages keyboard_action){
	if (dik == DIK_ESCAPE)
	{
		OnBtnCancel();
		return true;
	}
	return CUIDialogWnd::OnKeyboard(dik, keyboard_action);
}

void CUIChangeMap::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	if (LIST_ITEM_SELECT == msg && pWnd == lst)
	{		
		selected_item = *static_cast<u32*>(pData);
	}
	else if (BUTTON_CLICKED == msg)
	{
		if (pWnd == btn_ok)
			OnBtnOk();
		else if (pWnd == btn_cancel)
			OnBtnCancel();
	}
}

void CUIChangeMap::OnBtnOk(){
	LPCSTR name = lst->GetSelectedText();
	if (name && name[0])
	{
		string512 command;
        sprintf(command, "cl_votestart changemap %s", name);		
		Console->Execute(command);
		game_cl_mp* game = smart_cast<game_cl_mp*>(&Game());
		game->StartStopMenu(this, true);
	}
	else
		return;
}

extern bool GetToken(char** sx, char* e, char* token);

void CUIChangeMap::FillUpList(){
	char Buffer[1024];
	ZeroMemory(Buffer, sizeof(Buffer));
//	memfil(Buffer, 0, sizeof(Buffer));
	FILE* f = fopen(MAP_LIST, "rb");
	if (!f) return;

	size_t NumRead = fread(Buffer, 1, 1024, f);
	if (!NumRead) return;
	fclose(f);


	char token[1024];
	char* s = Buffer;
	char* e = Buffer + xr_strlen(Buffer) + 1;

//	int MapListType = GAME_ANY;

	while (1)
	{
//		if ()
		if (!GetToken(&s, e, token)) break;
		if (!xr_strcmp(token, "weather"))
		{
			do {
				GetToken(&s, e, token);
			} while(0!=xr_strcmp(token, "}"));
			continue;
		}
		if (!xr_strcmp(token, "deathmatch"))
		{
			if (GameID() != GAME_DEATHMATCH){
				do {
					GetToken(&s, e, token);
				} while(0!=xr_strcmp(token, "}"));
				continue;
			}
		}
		else if (!xr_strcmp(token, "teamdeathmatch")) 
		{
			if (GameID() != GAME_TEAMDEATHMATCH){
				do {
					GetToken(&s, e, token);
				} while(0!=xr_strcmp(token, "}"));
                continue;
			}
		}
		else if (!xr_strcmp(token, "artefacthunt")) 
		{
			if (GameID() != GAME_ARTEFACTHUNT){
				do {
					GetToken(&s, e, token);
				} while(0!=xr_strcmp(token, "}"));
                continue;
			}
		}
		else break;

		if (!GetToken(&s, e, token)) break;
		if (xr_strcmp(token, "{")) break;

		while (1)
		{
			if (!GetToken(&s, e, token)) break;
			if (!xr_strcmp(token, "}")) break;

			if (!xr_strcmp(token, "mapname"))
			{
				GetToken(&s, e, token);
				lst->AddItem(token);
			};
		};
	};
}

void CUIChangeMap::OnBtnCancel(){
    game_cl_mp* game = smart_cast<game_cl_mp*>(&Game());
	game->StartStopMenu(this, false);
}

IC bool	DM_Compare_Players		(LPVOID v1, LPVOID v2);