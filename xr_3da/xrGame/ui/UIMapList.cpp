
#include "StdAfx.h"
#include "UIMapList.h"
#include "UIListBox.h"
#include "UILabel.h"
#include "UIFrameWindow.h"
#include "UI3tButton.h"
#include "UISpinText.h"
#include "UIXmlInit.h"
#include "UIMapInfo.h"
#include "UIComboBox.h"
#include "../../xr_ioconsole.h"



extern ENGINE_API string512  g_sLaunchOnExit_app;
extern ENGINE_API string512  g_sLaunchOnExit_params;

xr_token	game_types		[];

CUIMapList::CUIMapList(){
	m_item2del = -1;

	m_pList1 = xr_new<CUIListBox>();
	m_pList2 = xr_new<CUIListBox>();
	m_pFrame1 = xr_new<CUIFrameWindow>();
	m_pFrame2 = xr_new<CUIFrameWindow>();
	m_pLbl1 = xr_new<CUILabel>();
	m_pLbl2 = xr_new<CUILabel>();
	m_pBtnLeft = xr_new<CUI3tButton>();
	m_pBtnRight = xr_new<CUI3tButton>();
	m_pBtnUp = xr_new<CUI3tButton>();
	m_pBtnDown = xr_new<CUI3tButton>();

//	m_pList1->ShowSelectedItem();
//	m_pList2->ShowSelectedItem();

	m_pList1->SetAutoDelete(true);
	m_pList2->SetAutoDelete(true);
	m_pFrame1->SetAutoDelete(true);
	m_pFrame2->SetAutoDelete(true);
	m_pLbl1->SetAutoDelete(true);
	m_pLbl2->SetAutoDelete(true);
	m_pBtnLeft->SetAutoDelete(true);
	m_pBtnRight->SetAutoDelete(true);
	m_pBtnUp->SetAutoDelete(true);
	m_pBtnDown->SetAutoDelete(true);

	AttachChild(m_pLbl1);
	AttachChild(m_pLbl2);
	AttachChild(m_pFrame1);
	AttachChild(m_pFrame2);
	AttachChild(m_pList1);
	AttachChild(m_pList2);
	AttachChild(m_pBtnLeft);
	AttachChild(m_pBtnRight);
	AttachChild(m_pBtnUp);
	AttachChild(m_pBtnDown);
}

CUIMapList::~CUIMapList(){
	
}

void CUIMapList::StartDedicatedServer(){
	strcpy					(g_sLaunchOnExit_app,"dedicated//xr_3da.exe");

	strcpy					(g_sLaunchOnExit_params,"-");
	strcat					(g_sLaunchOnExit_params,GetCommandLine(""));
	Msg						("%s","-- Going to quit before starting dedicated server");
	Msg						("%s %s",g_sLaunchOnExit_app, g_sLaunchOnExit_params);
	Console->Execute		("quit");
}

void CUIMapList::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);	
}

void CUIMapList::SendMessage(CUIWindow* pWnd, s16 msg, void* pData ){
	if (BUTTON_CLICKED == msg)
	{
		if (m_pBtnLeft == pWnd)
			OnBtnLeftClick();
		else if (m_pBtnRight == pWnd)
			OnBtnRightClick();
		else if (m_pBtnUp == pWnd)
			OnBtnUpClick();
		else if (m_pBtnDown == pWnd)
			OnBtnDownClick();
		else if (m_pModeSelector == pWnd)
			OnModeChange();
	}
	else if (WINDOW_LBUTTON_DB_CLICK == msg)
	{
		if (m_pList1 ==pWnd)
			OnBtnRightClick();
		else if (m_pList2 ==pWnd)
			OnBtnLeftClick();
	}
	else if (LIST_ITEM_CLICKED == msg)
	{
		if (pWnd == m_pList1)
            OnListItemClicked();
	}

		
}

void CUIMapList::OnListItemClicked(){
//	LPCSTR map = 
//	int iItem = m_pList1->GetSelectedItem();
//	if (-1 == iItem)
//		return;

//	CUIListItem* pItem = m_pList1->GetItem(iItem);
	xr_string map_name = "intro\\intro_map_pic_";
	map_name +=	m_pList1->GetSelectedText();
	xr_string full_name = map_name + ".dds";

	if (FS.exist("$game_textures$",full_name.c_str()))
		m_pMapPic->InitTexture(map_name.c_str());
	else
		m_pMapPic->InitTexture("ui\\ui_noise");

	xr_string desc_txt = "text\\map_desc\\";
	desc_txt += m_pList1->GetSelectedText();
	desc_txt += ".txt";


	m_pMapInfo->InitMap(m_pList1->GetSelectedText());
}
xr_token g_GameModes[ ];

void CUIMapList::OnModeChange(){
	UpdateMapList(GetCurGameType());
}

const char* CUIMapList::GetCLGameModeName(){
	return get_token_name(game_types, GetCurGameType() );
}

EGameTypes CUIMapList::GetCurGameType(){
	LPCSTR text = m_pModeSelector->GetText();

	if (0 == xr_strcmp(text, get_token_name(g_GameModes,GAME_DEATHMATCH)) )
		return	GAME_DEATHMATCH;
	else if (0 == xr_strcmp(text, get_token_name(g_GameModes,GAME_TEAMDEATHMATCH)) )
		return	GAME_TEAMDEATHMATCH;
	else if (0 == xr_strcmp(text, get_token_name(g_GameModes,GAME_ARTEFACTHUNT)) )
		return	GAME_ARTEFACTHUNT;
	else
		NODEFAULT;

#ifdef DEBUG
	return GAME_ANY;
#endif
}

const char* CUIMapList::GetCommandLine(LPCSTR player_name){
	char buf[16];
	LPCSTR txt = m_pList2->GetText(0);
	if (NULL == txt)
		return NULL;
	if (0 == txt[0])
		return NULL;
	m_command.clear();
	m_command = "start server(";
	m_command += txt; //map_name
	m_command += "/";
	m_command += GetCLGameModeName();
	m_command += m_srv_params;
	m_command += "/estime=";
	const char* weather = m_pWeatherSelector->GetText();
	int estime = (*m_mapWeather.find(weather)).second;	
	m_command += itoa(estime/60,buf,10);
	m_command += ":";
	m_command += itoa(estime%60,buf,10);
	m_command += ")";


	m_command +=" client(localhost/name=";
	if (player_name == NULL || 0 == xr_strlen(player_name))
		m_command += Core.UserName;
	else
		m_command +=player_name;
	m_command +=")";

    return m_command.c_str();
}

void CUIMapList::LoadMapList()
{
	string_path				fn;
	FS.update_path			(fn, "$game_config$", MAP_LIST);
	CInifile map_list_cfg	(fn);

	// maps
	for (int k=0; game_types[k].name; ++k)
	{
		EGameTypes _id			= (EGameTypes)game_types[k].id;
		LPCSTR _name			= game_types[k].name;

		if( !map_list_cfg.section_exist(_name) ) continue;

		CInifile::Sect& S		= map_list_cfg.r_section(_name);
		CInifile::SectIt it		= S.begin(), end = S.end();
		
		for (;it!=end; ++it){
			shared_str _map_name = it->first;
			m_maps[_id].push_back	(_map_name);
		}
	}

	//weather
	shared_str				weather_sect = "weather";
	CInifile::Sect& S		= map_list_cfg.r_section(weather_sect);
	CInifile::SectIt it		= S.begin(), end = S.end();
	
	shared_str				WeatherType;
	shared_str				WeatherTime;

	for (;it!=end; ++it){
		WeatherType			= it->first;
		WeatherTime			= map_list_cfg.r_string(weather_sect, *WeatherType);

		AddWeather			(WeatherType, WeatherTime);
	}

	if(S.size())
		m_pWeatherSelector->SetItem(0);
}

void	CUIMapList::SaveMapList(){
	FILE* MapRotFile = fopen(MAP_ROTATION_LIST, "w");
	if (!MapRotFile)
		return;
	
	
	for(u32 idx=0; idx<m_pList2->GetSize(); ++idx)
	{
		LPCSTR txt = m_pList2->GetText(idx);
		fprintf(MapRotFile, "sv_addmap %s\n", txt);
	}

	fclose(MapRotFile);
}

void CUIMapList::SetWeatherSelector(CUIComboBox* ws){
	m_pWeatherSelector = ws;
}

void CUIMapList::SetModeSelector(CUISpinText* ms){
	m_pModeSelector = ms;
}

void CUIMapList::SetMapPic(CUIStatic* map_pic){
	m_pMapPic = map_pic;
}

void CUIMapList::SetMapInfo(CUIMapInfo* map_info){
	m_pMapInfo = map_info;	
}

void CUIMapList::SetServerParams(LPCSTR params){
	m_srv_params = params;
}

void CUIMapList::AddWeather(const shared_str& WeatherType, const shared_str& WeatherTime){
	R_ASSERT2					(m_pWeatherSelector, "m_pWeatherSelector == NULL");
	m_pWeatherSelector->AddItem	(*WeatherType);

	int	w_time;
	int hour = 0, min = 0;

	sscanf(*WeatherTime, "%d:%d", &hour, &min);
	w_time = hour*60+min;

	m_mapWeather.insert(mk_pair(WeatherType, w_time));
}

void CUIMapList::InitFromXml(CUIXml& xml_doc, const char* path){
	CUIXmlInit::InitWindow(xml_doc, path, 0, this);
	string256 buf;
	CUIXmlInit::InitLabel		(xml_doc, strconcat(buf, path, ":header_1"),	0, m_pLbl1);
	CUIXmlInit::InitLabel		(xml_doc, strconcat(buf, path, ":header_2"),	0, m_pLbl2);
	CUIXmlInit::InitFrameWindow	(xml_doc, strconcat(buf, path, ":frame_1"),		0, m_pFrame1);
	CUIXmlInit::InitFrameWindow	(xml_doc, strconcat(buf, path, ":frame_2"),		0, m_pFrame2);
	CUIXmlInit::InitListBox		(xml_doc, strconcat(buf, path, ":list_1"),		0, m_pList1);
	CUIXmlInit::InitListBox		(xml_doc, strconcat(buf, path, ":list_2"),		0, m_pList2);
	CUIXmlInit::Init3tButton	(xml_doc, strconcat(buf, path, ":btn_left"),	0, m_pBtnLeft);
	CUIXmlInit::Init3tButton	(xml_doc, strconcat(buf, path, ":btn_right"),	0, m_pBtnRight);
	CUIXmlInit::Init3tButton	(xml_doc, strconcat(buf, path, ":btn_up"),		0, m_pBtnUp);
	CUIXmlInit::Init3tButton	(xml_doc, strconcat(buf, path, ":btn_down"),	0, m_pBtnDown);
}

void CUIMapList::UpdateMapList(EGameTypes GameType){
	m_pList1->Clear				();
	m_pList2->Clear				();

	for (u32 i=0; i<m_maps[GameType].size(); ++i)
		m_pList1->AddItem(*m_maps[GameType][i]);
}

void CUIMapList::OnBtnLeftClick(){
	m_pList2->RemoveWindow(m_pList2->GetSelected());
}

void CUIMapList::Update(){
	CUIWindow::Update();
}

void CUIMapList::OnBtnRightClick(){
	m_pList2->AddItem(m_pList1->GetSelectedText());
/*
	{
		LPCSTR next = m_pList1->GetNextSelectedText();
		while(next)
		{
			m_pList2->AddItem(next);
			next = m_pList1->GetNextSelectedText();
		}
		m_pList1->SetSelected( (CUIWindow*)NULL );
	}
*/
}

void CUIMapList::OnBtnUpClick(){
	m_pList2->MoveSelectedUp();
}

void CUIMapList::OnBtnDownClick(){
	m_pList2->MoveSelectedDown();
}

bool CUIMapList::IsEmpty(){
	return 0 == m_pList2->GetSize();
}
