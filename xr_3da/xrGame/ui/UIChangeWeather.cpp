#include "stdafx.h"
#include "UIChangeWeather.h"
#include "UIXmlInit.h"
#include "xrXmlParser.h"
#include "UI3tButton.h"
#include "../game_cl_teamdeathmatch.h"
#include "UIKickPlayer.h"
#include "UIChangeMap.h"
//#include "UIMapList.h"

CUIChangeWeather::CUIChangeWeather(){
	bkgrnd = xr_new<CUIStatic>(); 
	bkgrnd->SetAutoDelete(true);
	AttachChild(bkgrnd);

	header = xr_new<CUIStatic>();
	header->SetAutoDelete(true);
	AttachChild(header);

	btn_cancel = xr_new<CUI3tButton>();
	btn_cancel->SetAutoDelete(true);
	AttachChild(btn_cancel);

	for (int i = 0; i<4; i++)
	{
		btn[i] = xr_new<CUI3tButton>();
		btn[i]->SetAutoDelete(true);
		AttachChild(btn[i]);

		txt[i] = xr_new<CUIStatic>();
		txt[i]->SetAutoDelete(true);
		AttachChild(txt[i]);
	}

	weather_counter = 0;
}

void CUIChangeWeather::Init(CUIXml& xml_doc){
	CUIXmlInit::InitWindow(xml_doc, "change_weather", 0, this);

	CUIXmlInit::InitStatic(xml_doc, "change_weather:header", 0, header);
	CUIXmlInit::InitStatic(xml_doc, "change_weather:background", 0, bkgrnd);

	string256 _path;
	for (int i = 0; i<4; i++){
		sprintf(_path, "change_weather:btn_%d", i + 1);
		CUIXmlInit::Init3tButton(xml_doc, _path, 0, btn[i]);
		sprintf(_path, "change_weather:txt_%d", i + 1);
		CUIXmlInit::InitStatic(xml_doc, _path, 0, txt[i]);
	}

	CUIXmlInit::Init3tButton(xml_doc, "change_weather:btn_cancel", 0, btn_cancel);

	ParseWeather();
}

void CUIChangeWeather::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	if (BUTTON_CLICKED == msg)
	{
		if (btn_cancel == pWnd)
			OnBtnCancel();
		for (int i=0; i<4; i++){
			if (btn[i] == pWnd){
				OnBtn(i);
				return;
			}
		}
	}

}

#include <dinput.h>

bool CUIChangeWeather::OnKeyboard(int dik, EUIMessages keyboard_action){
	CUIDialogWnd::OnKeyboard(dik, keyboard_action);
	if (WINDOW_KEY_PRESSED == keyboard_action){
		if (DIK_ESCAPE == dik){
			OnBtnCancel();
			return true;
		}
		if (dik >= DIK_1 && dik <= DIK_4)
			OnBtn(dik - DIK_1);
	}

	return false;
}

#include "../../xr_ioconsole.h"

void CUIChangeWeather::OnBtn(int i){
	game_cl_mp* game = smart_cast<game_cl_mp*>(&Game());
//	LPCSTR weather_time = *txt[i]->WindowName();
	string1024 command;
	sprintf(command, "cl_votestart changeweather %s %s", txt[i]->GetText(), *txt[i]->WindowName());
	Console->Execute(command);
	game->StartStopMenu(this, true);
}

void CUIChangeWeather::OnBtnCancel(){
	game_cl_mp* game = smart_cast<game_cl_mp*>(&Game());
	game->StartStopMenu(this, true);
}

extern bool GetToken(char** sx, char* e, char* token);

void CUIChangeWeather::ParseWeather()
{
	weather_counter = 0;

	char Buffer[1024];
	ZeroMemory(Buffer, sizeof(Buffer));
//	memfil(Buffer, 0, sizeof(Buffer));
	FILE* f = fopen("map_list.ltx", "rb");
	R_ASSERT(f);

	size_t NumRead = fread(Buffer, 1, 1024, f);
	if (!NumRead) return;
	fclose(f);


	char token[1024];
	char* s = Buffer;
	char* e = Buffer + xr_strlen(Buffer) + 1;

	while (true)
	{
		if (!GetToken(&s, e, token))
			return;
		if (0 == xr_strcmp(token,"weather"))
			break;		
	}

	while (1)
	{	
		if (!GetToken(&s, e, token)) break;
		if (xr_strcmp(token, "{")) break;
		while (1)
		{
			if (!GetToken(&s, e, token)) break;
			if (!xr_strcmp(token, "}")) return;
			if (!xr_strcmp(token, "startweather"))
			{
				char WeatherType[1024], WeatherTime[1024];
				GetToken(&s, e, WeatherType);
				GetToken(&s, e, WeatherTime);

				AddWeather(WeatherType, WeatherTime);
			};
		};
	};
};

void CUIChangeWeather::AddWeather(LPCSTR weather, LPCSTR time){
	txt[weather_counter]->SetText(weather);
	txt[weather_counter]->SetWindowName(time);
	weather_counter++;
}

