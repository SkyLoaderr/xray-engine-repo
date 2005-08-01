
#pragma once

#include "UIWindow.h"

class CUIListWnd;
class CUILabel;
class CUIStatic;
class CUIFrameWindow;
class CUI3tButton;
class CUISpinText;

class CUIXml;

enum	GAME_TYPE
{
	GAME_UNKNOWN			= 0	,
	GAME_DEATHMATCH				,
	GAME_TEAMDEATHMATCH			,
	GAME_ARTEFACTHUNT			,

	GAME_END_LIST				,
};

bool GetToken(char** sx, char* e, char* token);
LPCSTR ParseFile(CMemoryWriter& W, IReader *F );

class CUIMapList : public CUIWindow {
public:
	CUIMapList();
	virtual ~CUIMapList();
	virtual void	Init(float x, float y, float width, float height);
	virtual void	Update();
	virtual void	SendMessage(CUIWindow* pWnd, s16 msg, void* pData  = NULL);
			void	InitFromXml(CUIXml& xml_doc, const char* path);	

			void	SetWeatherSelector(CUISpinText* ws);
			void	SetModeSelector(CUISpinText* ms);
			void	SetMapPic(CUIStatic* map_pic);
			void	SetMapDesc(CUIStatic* map_desc, CUILabel* map_name);
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

	CUIListWnd*			m_pList1;
	CUIListWnd*			m_pList2;
	CUIFrameWindow*		m_pFrame1;
	CUIFrameWindow*		m_pFrame2;
	CUILabel*			m_pLbl1;
	CUILabel*			m_pLbl2;
	CUI3tButton*		m_pBtnLeft;
	CUI3tButton*		m_pBtnRight;
	CUI3tButton*		m_pBtnUp;
	CUI3tButton*		m_pBtnDown;

	CUISpinText*		m_pWeatherSelector;
	CUISpinText*		m_pModeSelector;
	CUIStatic*			m_pMapPic;
	CUIStatic*			m_pMapDesc;
	CUILabel*			m_pMapName;

	GAME_TYPE	m_GameType;
	char		m_Maps[GAME_END_LIST][20][1024];
	int			m_MapsNum[GAME_END_LIST];	

	xr_map<xr_string,int> m_mapWeather;
	xr_string	m_command;
	xr_string	m_srv_params;

	int			m_item2del;
};