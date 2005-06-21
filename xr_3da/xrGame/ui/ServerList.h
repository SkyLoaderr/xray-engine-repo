#pragma once

#include "UIWindow.h"
#include "UIListWnd.h"
#include "UIListItemServer.h"
#include "UIFrameWindow.h"
#include "UILabel.h"
#include "../GameSpy/ServerBrowsing/sb_serverbrowsing.h"

class CUIXml;

struct ServerInfo{
	SBServer pSBServer;
	string128	m_Address;
	string128	m_HostName;
	string128	m_ServerName;
	string128	m_SessionName;
	string128	m_ServerGameType;

	s16						m_ServerNumPlayers;
	s16						m_ServerMaxPlayers;
	bool					m_bDedicated;
	bool					m_bFFire;
	bool					m_bPassword;
	s16						m_Ping;
	s16						m_Port;

	ServerInfo (string128 NewAddress) 
	{
		strcpy(m_Address, NewAddress);
	};

	bool			operator	==		(LPCSTR Address){int res = xr_strcmp(m_Address, Address);return	res	 == 0;};
};

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

DEF_VECTOR(SERVERINFO, ServerInfo);

class CServerList : public CUIWindow{
public:
	CServerList();
	virtual ~CServerList();

	virtual void Init(float x, float y, float width, float height);
			void InitFromXml(CUIXml& xml_doc, const char* path);
			void InitHeader();
			void InitSeparator();
			void AddServerToList	(ServerInfo* pServerInfo);
			void ConnectToSelected();
			void SetFilters(SServerFilters& sf);
			void SetPlayerName(const char* name);

	virtual void GameSpy_Browser_Init();
	virtual void GameSpy_Browser_Destroy();
	virtual	void GameSpy_Browser_RefreshList (bool Local);
	
	virtual void GameSpy_Browser_OnServerAdded		(SBServer server);
	virtual void GameSpy_Browser_OnServerUpdated	(SBServer server);
	virtual void GameSpy_Browser_OnServerRemoved	(SBServer server);
	virtual void GameSpy_Browser_OnUpdateCompleted();
	
	virtual void	RefreshGameSpyList	(bool Local);


protected:
			bool IsValidItem(ServerInfo& item);

	LIST_SRV_ITEM	m_itemInfo;
	SServerFilters	m_sf;
	CUIListWnd		m_list;
	CUIFrameWindow	m_frame;
	CUILabel		m_header[6];
	CUIFrameLineWnd	m_separator[5];
	xr_string		m_palyerName;

	ServerBrowser m_serverBrowser;

	SERVERINFO		m_aServers;

	virtual	void	GameSpy_Browser_ReadServerData	(SBServer server, ServerInfo* pServerInfo);
	virtual void	GameSpy_Browser_ClearServersList	() {m_aServers.clear();};
	virtual SERVERINFO_it	GameSpy_Browser_GetServerInfo	(SBServer server, bool Create = false);
};