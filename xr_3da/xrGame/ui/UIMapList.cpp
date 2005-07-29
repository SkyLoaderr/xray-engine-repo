
#include "StdAfx.h"
#include "UIMapList.h"
#include "UIListWnd.h"
#include "UILabel.h"
#include "UIFrameWindow.h"
#include "UI3tButton.h"
#include "UISpinText.h"
#include "UIXmlInit.h"
#include "../../xr_ioconsole.h"

#define	MAP_LIST	"map_list.ltx"
#define	MAP_ROTATION_LIST	"maprot_list.ltx"

extern ENGINE_API string512  g_sLaunchOnExit;

static LPSTR	g_GameTypeName	[]		= {
	"UNKNOWN"					,
		"Deathmatch"				,
		"TeamDeathmatch"			,
		"ArtefactHunt"				,

		"END_LIST"					,
};

CUIMapList::CUIMapList(){
	m_item2del = -1;

	m_pList1 = xr_new<CUIListWnd>();
	m_pList2 = xr_new<CUIListWnd>();
	m_pFrame1 = xr_new<CUIFrameWindow>();
	m_pFrame2 = xr_new<CUIFrameWindow>();
	m_pLbl1 = xr_new<CUILabel>();
	m_pLbl2 = xr_new<CUILabel>();
	m_pBtnLeft = xr_new<CUI3tButton>();
	m_pBtnRight = xr_new<CUI3tButton>();
	m_pBtnUp = xr_new<CUI3tButton>();
	m_pBtnDown = xr_new<CUI3tButton>();

	m_pList1->ShowSelectedItem();
	m_pList2->ShowSelectedItem();

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
	strcpy(g_sLaunchOnExit,"xr_3da.exe -dedicated -");
	strcat(g_sLaunchOnExit,GetCommandLine(""));
	Msg("%s","-- Going to quit before starting dedicated server");
	Msg("%s",g_sLaunchOnExit);
	Console->Execute("quit");
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
	int iItem = m_pList1->GetSelectedItem();
	if (-1 == iItem)
		return;

	CUIListItem* pItem = m_pList1->GetItem(iItem);
	xr_string map_name = "ui\\ui_map_pic_";
	map_name +=	pItem->GetText();
	xr_string full_name = map_name + ".dds";

	if (FS.exist("$game_textures$",full_name.c_str()))
		m_pMapPic->InitTexture(map_name.c_str());
	else
		m_pMapPic->InitTexture("ui\\ui_map_nopic");

	xr_string desc_txt = "text\\map_desc\\";
	desc_txt += pItem->GetText();
	desc_txt += ".txt";


	m_pMapName->SetText(pItem->GetText());

	if (FS.exist("$game_config$", desc_txt.c_str()))
	{
		IReader *F = FS.r_open("$game_config$", desc_txt.c_str());
		if(F==NULL) return;

		CMemoryWriter W;	
		
		m_pMapDesc->SetText(ParseFile(W, F));
		//	W.w(F->pointer(),F->length());
		W.w_stringZ("");
		FS.r_close(F);
	}
	else
		m_pMapDesc->SetText("");

}

LPCSTR ParseFile(CMemoryWriter& W, IReader *F )
{
	string4096	str;

	while( !F->eof() ){
		F->r_string		(str,sizeof(str));
		W.w_string		(str);
	}
	W.w_stringZ("");

	return (LPCSTR )W.pointer();
}

xr_token g_GameModes[ ];

void CUIMapList::OnModeChange(){
	UpdateMapList(GetCurGameType());
}

const char* CUIMapList::GetCLGameModeName(){
	LPCSTR text = m_pModeSelector->GetText();

	if (0 == xr_strcmp(text, g_GameModes[0].name))
		return	g_GameTypeName[GAME_DEATHMATCH];
	else if (0 == xr_strcmp(text, g_GameModes[1].name))
		return	g_GameTypeName[GAME_TEAMDEATHMATCH];
	else if (0 == xr_strcmp(text, g_GameModes[2].name))
		return	g_GameTypeName[GAME_ARTEFACTHUNT];
	else
		NODEFAULT;

	return NULL;
}

GAME_TYPE CUIMapList::GetCurGameType(){
	LPCSTR text = m_pModeSelector->GetText();

	if (0 == xr_strcmp(text, g_GameModes[0].name))
		return	GAME_DEATHMATCH;
	else if (0 == xr_strcmp(text, g_GameModes[1].name))
		return	GAME_TEAMDEATHMATCH;
	else if (0 == xr_strcmp(text, g_GameModes[2].name))
		return	GAME_ARTEFACTHUNT;
	else
		NODEFAULT;

	return GAME_UNKNOWN;
}

const char* CUIMapList::GetCommandLine(LPCSTR player_name){
	char buf[16];
	if (0 == m_pList2->GetItemsCount())
		return NULL;
	if (0 == xr_strlen(m_pList2->GetItem(0)->GetText()))
		return NULL;
	m_command.clear();
	m_command = "start server(";
	m_command += m_pList2->GetItem(0)->GetText(); //map_name
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

void CUIMapList::LoadMapList(){
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

	int MapListType = GAME_UNKNOWN;

	while (1)
	{
		if (!GetToken(&s, e, token)) break;
		if (!xr_strcmp(token, "deathmatch")) MapListType = GAME_DEATHMATCH;
		else
		{
			if (!xr_strcmp(token, "teamdeathmatch")) MapListType = GAME_TEAMDEATHMATCH;
			else
			{
				if (!xr_strcmp(token, "artefacthunt")) MapListType = GAME_ARTEFACTHUNT;
				else 
				{
					if (!xr_strcmp(token, "weather"))
					{
						ParseWeather(&s, e);
						continue;
					}
					else break;
				};				
			}
		}
		if (!GetToken(&s, e, token)) break;
		if (xr_strcmp(token, "{")) break;

		m_MapsNum[MapListType] = 0;
		while (1)
		{
			if (!GetToken(&s, e, token)) break;
			if (!xr_strcmp(token, "}")) break;

			if (!xr_strcmp(token, "mapname"))
			{
				GetToken(&s, e, token);
				strcpy(m_Maps[MapListType][m_MapsNum[MapListType]++], token);
			};
		};
	};
}

void	CUIMapList::SaveMapList(){
	FILE* MapRotFile = fopen(MAP_ROTATION_LIST, "w");
	if (!MapRotFile)
		return;
	
	int count = m_pList2->GetItemsCount();

	for (int i = 0; i < count; i++)
        fprintf(MapRotFile, "sv_addmap %s\n", m_pList2->GetItem(i)->GetText());

	fclose(MapRotFile);
}

void	CUIMapList::ParseWeather(char** ps, char* e)
{
	//	char* s = *ps;
	char token[1024];

	while (1)
	{	
		if (!GetToken(ps, e, token)) break;
		if (xr_strcmp(token, "{")) break;
		while (1)
		{
			if (!GetToken(ps, e, token)) break;
			if (!xr_strcmp(token, "}")) return;
			if (!xr_strcmp(token, "startweather"))
			{
				char WeatherType[1024], WeatherTime[1024];
				GetToken(ps, e, WeatherType);
				GetToken(ps, e, WeatherTime);

				AddWeather(WeatherType, WeatherTime);
			};
		};
	};
};

void CUIMapList::SetWeatherSelector(CUISpinText* ws){
	m_pWeatherSelector = ws;
}

void CUIMapList::SetModeSelector(CUISpinText* ms){
	m_pModeSelector = ms;
}

void CUIMapList::SetMapPic(CUIStatic* map_pic){
	m_pMapPic = map_pic;
}

void CUIMapList::SetMapDesc(CUIStatic* map_desc, CUILabel* map_name){
	m_pMapDesc = map_desc;
	m_pMapName = map_name;
}

void CUIMapList::SetServerParams(LPCSTR params){
	m_srv_params = params;
}

void CUIMapList::AddWeather(char* WeatherType, char* WeatherTime){
	R_ASSERT2(m_pWeatherSelector, "m_pWeatherSelector == NULL");
	m_pWeatherSelector->AddItem(WeatherType);

	xr_string	w_type = WeatherType;
	int			w_time;
	int hour = 0, min = 0;

	sscanf(WeatherTime, "%d:%d", &hour, &min);
	w_time = hour*60+min;

	m_mapWeather.insert(mk_pair(w_type,w_time));
}

void CUIMapList::InitFromXml(CUIXml& xml_doc, const char* path){
	CUIXmlInit::InitWindow(xml_doc, path, 0, this);
	string256 buf;
	CUIXmlInit::InitLabel		(xml_doc, strconcat(buf, path, ":header_1"),	0, m_pLbl1);
	CUIXmlInit::InitLabel		(xml_doc, strconcat(buf, path, ":header_2"),	0, m_pLbl2);
	CUIXmlInit::InitFrameWindow	(xml_doc, strconcat(buf, path, ":frame_1"),		0, m_pFrame1);
	CUIXmlInit::InitFrameWindow	(xml_doc, strconcat(buf, path, ":frame_2"),		0, m_pFrame2);
	CUIXmlInit::InitListWnd		(xml_doc, strconcat(buf, path, ":list_1"),		0, m_pList1);
	m_pList1->EnableScrollBar(true);
	CUIXmlInit::InitListWnd		(xml_doc, strconcat(buf, path, ":list_2"),		0, m_pList2);
	m_pList2->EnableScrollBar(true);
	CUIXmlInit::Init3tButton	(xml_doc, strconcat(buf, path, ":btn_left"),	0, m_pBtnLeft);
	CUIXmlInit::Init3tButton	(xml_doc, strconcat(buf, path, ":btn_right"),	0, m_pBtnRight);
	CUIXmlInit::Init3tButton	(xml_doc, strconcat(buf, path, ":btn_up"),		0, m_pBtnUp);
	CUIXmlInit::Init3tButton	(xml_doc, strconcat(buf, path, ":btn_down"),	0, m_pBtnDown);
}

void CUIMapList::UpdateMapList(GAME_TYPE GameType){
	m_pList1->RemoveAll();
	m_pList2->RemoveAll();

	for (int i=0; i<m_MapsNum[GameType]; i++)
		m_pList1->AddItem<CUIListItem>(m_Maps[GameType][i]);
}

void CUIMapList::OnBtnLeftClick(){
	m_item2del = m_pList2->GetSelectedItem();
}

void CUIMapList::Update(){
	CUIWindow::Update();
	if (-1 != m_item2del)
	{
		m_pList2->RemoveItem(m_item2del);
		m_item2del = -1;
	}
}

void CUIMapList::OnBtnRightClick(){
	int isel = m_pList1->GetSelectedItem();

	if (-1 != isel)
	{
		CUIListItem* li = m_pList1->GetItem(isel);
		m_pList2->AddItem<CUIListItem>(li->GetText());
	}

}

void CUIMapList::OnBtnUpClick(){
	int isel = m_pList2->GetSelectedItem();

	if (isel > 0)
	{
		xr_string text = m_pList2->GetItem(isel)->GetText();
		m_pList2->RemoveItem(isel);
		m_pList2->AddItem<CUIListItem>(text.c_str(), 0.0f, NULL, 0, isel-1);
		m_pList2->SetSelectedItem(isel - 1);
	}

}

void CUIMapList::OnBtnDownClick(){
	int isel = m_pList2->GetSelectedItem();

	if (isel < m_pList2->GetItemsCount() - 1 )
	{
		xr_string text = m_pList2->GetItem(isel)->GetText();
		m_pList2->RemoveItem(isel);
		m_pList2->AddItem<CUIListItem>(text.c_str(), 0.0f, NULL, 0, isel+1);
		m_pList2->SetSelectedItem(isel+1);
	}


}


bool GetToken(char** sx, char* e, char* token)
{
	char* s = *sx;

	if (!s || !token) return false;	

skipspace:

	if (s == e) return false;
	while (*s <= 32 || s == e)
	{
		if (s == e) 
		{
			*sx = s;
			return false;
		}
		s++;
	};

	// comments	; # //
	if (*s == ';' || *s == '#' || (s[0] == '/' && s[1] == '/'))
	{
		while (*s++ != '\n')
		{
			if (s == e) 
			{
				*sx = s;
				return false;
			}
		};
		goto skipspace;
	}
	// comments /* */
	if (s[0] == '/' && s[1] == '*')
	{
		s += 2;
		while (s[0] != '*' || s[1] != '/')
		{
			if (s == e) 
			{
				*sx = s;
				return false;
			}
			s++;
		};
		s += 2;
		goto skipspace;
	};

	char *t = token;
	char control;
	if (*s == '"' || *s == '\'')
	{
		control = *s;
		s++;
		while (*s != control) 
		{
			*t++ = *s++;
			if (s == e) 
			{
				*sx = s;
				return false;
			}
		}
		*t = 0;
		s++;
		*sx = s;
	}
	else
	{
		while (*s > 32 && *s != ';')
		{
			*t++ = *s++;
			if (s == e) 
			{
				*sx = s;
				return false;
			}
		};
		*t = 0;
		*sx = s;
	};
	return true;
};