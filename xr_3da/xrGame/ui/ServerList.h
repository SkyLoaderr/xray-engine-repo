#pragma once

#include "UIWindow.h"
#include "UIListWnd.h"
#include "UIListItemServer.h"
#include "UIFrameWindow.h"
#include "UIEditBox.h"
//#include "UILabel.h"


#include "../GameSpy/GameSpy_Browser.h"

class CUIXml;
class CGameSpy_Browser;
class CUIColorAnimatorWrapper;

#define LST_SERVER		0 
#define LST_SRV_PROP	1
#define LST_PLAYERS		2
#define DIF_HEIGHT		180

class SServerFilters{
public:
	bool	empty;
	bool	full;
	bool	with_pass;
	bool	without_pass;
	bool	without_ff;
	bool	without_pb;
	bool	listen_servers;
};

class CServerList : public CUIWindow{
public:
	CServerList();
	virtual ~CServerList();

	virtual void Init(float x, float y, float width, float height);
	virtual void Update();
			void InitFromXml(CUIXml& xml_doc, const char* path);
			void InitHeader();
			void InitSeparator();
			void AddServerToList	(ServerInfo* pServerInfo);
			void UpdateServerInList	(ServerInfo* pServerInfo, int index);
			void UpdateServerInList	(ServerInfo* pServerInfo, CUIListItemServer* pItem);
			void ConnectToSelected();
			void SetFilters(SServerFilters& sf);
			void SetPlayerName(const char* name);

	virtual void	RefreshGameSpyList	(bool Local);
			void	RefreshQuick();
			void	ShowServerInfo();


protected:
			bool IsValidItem(ServerInfo& item);
			void SrvInfo2LstSrvInfo(const ServerInfo* pSrvInfo);
			void UpdateSizes();
			void BeforeAppear();
			void AfterAppear();
			void BeforeDisapear();
			void AfterDisappear();
			void FillUpDetailedServerInfo();
			void ClearDetailedServerInfo();

	CUIColorAnimatorWrapper* m_pAnimation;

	LIST_SRV_ITEM	m_itemInfo;
	SServerFilters	m_sf;
	CUIListWnd		m_list[3];
	CUIFrameWindow	m_frame[3];
	CUILabel		m_header[6];
	CUILabel		m_header2[4];
	CUIFrameLineWnd	m_separator[5];
	CUIEditBox		m_edit_gs_filter;
	xr_string		m_playerName;
	bool			m_bShowServerInfo;
	bool			m_bAnimation;
	float			m_fListH[2];
	float			m_fEditPos[2];



	CGameSpy_Browser	m_GSBrowser;	

	virtual	void	RefreshList();
};