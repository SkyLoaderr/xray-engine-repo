#pragma once

#include "UIWindow.h"
#include "UIListWnd.h"
#include "UIListItemServer.h"
#include "UIFrameWindow.h"
#include "UILabel.h"

#include "../GameSpy/GameSpy_Browser.h"

class CUIXml;
class CGameSpy_Browser;

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
			void InitFromXml(CUIXml& xml_doc, const char* path);
			void InitHeader();
			void InitSeparator();
			void AddServerToList	(ServerInfo* pServerInfo);
			void ConnectToSelected();
			void SetFilters(SServerFilters& sf);
			void SetPlayerName(const char* name);

	virtual void	RefreshGameSpyList	(bool Local);
			void	RefreshQuick();


protected:
			bool IsValidItem(ServerInfo& item);

	LIST_SRV_ITEM	m_itemInfo;
	SServerFilters	m_sf;
	CUIListWnd		m_list;
	CUIFrameWindow	m_frame;
	CUILabel		m_header[6];
	CUIFrameLineWnd	m_separator[5];
	xr_string		m_playerName;

	CGameSpy_Browser	m_GSBrowser;	

	virtual	void	RefreshList();
};