#pragma once

#include "UIListItem.h"


struct LIST_SRV_SIZES {
	float icon;
	float server;
	float map;
	float game; 
	float players;
	float ping;
};

struct LIST_SRV_INFO{
	shared_str server;
	shared_str port;
	shared_str map;
	shared_str game;
	shared_str players;
	shared_str ping;
};

struct LIST_SRV_ITEM {
	u32				color;
	CGameFont*		font;
	LIST_SRV_SIZES	size;
	LIST_SRV_INFO	info;
};

class CUIListItemServer : public CUIListItem {
public:
	CUIListItemServer();


	// CUISimpleWindow
	using CUIWindow::Init;
	virtual void Draw();

	// own
	virtual void Init(LIST_SRV_ITEM& params, float x, float y, float width, float height);
			void CreateConsoleCommand(xr_string& command, LPCSTR player_name);
	

protected:
			void SetTextColor(u32 color);
			void SetFont(CGameFont* pFont);
	LIST_SRV_SIZES m_sizes;

	LIST_SRV_ITEM m_srv_info;

	CUIStatic m_icon;
	CUIStatic m_server;
	CUIStatic m_map;
	CUIStatic m_game;
	CUIStatic m_players;
	CUIStatic m_ping;

};