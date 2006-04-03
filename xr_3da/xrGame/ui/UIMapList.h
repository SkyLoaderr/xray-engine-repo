
#pragma once

#include "UIWindow.h"

class CUIListBox;
class CUILabel;
class CUIStatic;
class CUIFrameWindow;
class CUI3tButton;
class CUISpinText;
class CUIMapInfo;
class CUIComboBox;

class CUIXml;

enum	GAME_TYPE
{
	GAME_UNKNOWN			= 0	,
	GAME_DEATHMATCH				,
	GAME_TEAMDEATHMATCH			,
	GAME_ARTEFACTHUNT			,

	GAME_END_LIST				,
};

#define	MAP_LIST	"map_list.ltx"
#define	MAP_ROTATION_LIST	"maprot_list.ltx"

bool GetToken(char** sx, char* e, char* token);
bool MP_map_cmp(shared_str map1, shared_str map2);

class CUIMapList : public CUIWindow {
public:
	CUIMapList();
	virtual ~CUIMapList();
	virtual void	Init(float x, float y, float width, float height);
	virtual void	Update();
	virtual void	SendMessage(CUIWindow* pWnd, s16 msg, void* pData  = NULL);
			void	InitFromXml(CUIXml& xml_doc, const char* path);	

			void	SetWeatherSelector(CUIComboBox* ws);
			void	SetModeSelector(CUISpinText* ms);
			void	SetMapPic(CUIStatic* map_pic);
			void	SetMapInfo(CUIMapInfo* map_info);
			void	SetServerParams(LPCSTR params);
			void	OnModeChange();
			void	OnListItemClicked();
			void	LoadMapList();
			void	SaveMapList();
	const char*		GetCommandLine(LPCSTR player_name);
		GAME_TYPE	GetCurGameType();
			void	StartDedicatedServer();

private:
	const char*		GetCLGameModeName(); // CL - command line
			void	UpdateMapList(GAME_TYPE GameType);						
			void	SaveRightList();

			void	OnBtnLeftClick();
			void	OnBtnRightClick();
			void	OnBtnUpClick();
			void	OnBtnDownClick();
			void	AddWeather(char* WeatherType, char* WeatherTime);
			void	ParseWeather(char** ps, char* e);

	CUIListBox*			m_pList1;
	CUIListBox*			m_pList2;
	CUIFrameWindow*		m_pFrame1;
	CUIFrameWindow*		m_pFrame2;
	CUILabel*			m_pLbl1;
	CUILabel*			m_pLbl2;
	CUI3tButton*		m_pBtnLeft;
	CUI3tButton*		m_pBtnRight;
	CUI3tButton*		m_pBtnUp;
	CUI3tButton*		m_pBtnDown;

	CUIComboBox*		m_pWeatherSelector;
	CUISpinText*		m_pModeSelector;
	CUIStatic*			m_pMapPic;
	CUIMapInfo*			m_pMapInfo;

	GAME_TYPE	m_GameType;
	
	xr_vector<shared_str> m_Maps[GAME_END_LIST];
//	int			m_MapsNum[GAME_END_LIST];	

	xr_map<xr_string,int> m_mapWeather;
	xr_string	m_command;
	xr_string	m_srv_params;

	int			m_item2del;
};