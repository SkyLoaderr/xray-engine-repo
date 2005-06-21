#include "StdAfx.h"

#include "UIListItemServer.h"

CUIListItemServer::CUIListItemServer(){
	AttachChild(&m_icon);
	AttachChild(&m_server);
	AttachChild(&m_map);
	AttachChild(&m_game);
	AttachChild(&m_players);
	AttachChild(&m_ping);
}

void CUIListItemServer::Init(LIST_SRV_ITEM& params, float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);

	SetTextColor(params.color);
	SetFont(params.font);

	float offset = 0.0f;

	m_icon.Init(offset, 0, params.size.icon, height);
	offset += params.size.icon;

	m_server.Init(offset, 0, params.size.server, height);
	m_server.m_lines.SetText(*params.info.server);
	offset += params.size.server;

	m_map.Init(offset, 0, params.size.map, height);
	m_map.m_lines.SetText(*params.info.map);
	offset += params.size.map;

	m_game.Init(offset, 0, params.size.game, height);
	m_game.m_lines.SetText(*params.info.game);
	offset += params.size.game;

	m_players.Init(offset, 0, params.size.game, height);
	m_players.m_lines.SetText(*params.info.players);
	offset += params.size.players;

	m_ping.Init(offset, 0, params.size.game, height);
	m_ping.m_lines.SetText(*params.info.ping);

	m_srv_info = params;
}

void CUIListItemServer::Draw(){
	CUIStatic::Draw();
	CUIWindow::Draw();
}


void CUIListItemServer::SetTextColor(u32 color){
	m_icon.SetTextColor(color);
	m_server.SetTextColor(color);
	m_map.SetTextColor(color);
	m_game.SetTextColor(color);
	m_players.SetTextColor(color);
	m_ping.SetTextColor(color);
}

void CUIListItemServer::SetFont(CGameFont* pFont){
	m_icon.SetFont(pFont);
	m_server.SetFont(pFont);
	m_map.SetFont(pFont);
	m_game.SetFont(pFont);
	m_players.SetFont(pFont);
	m_ping.SetFont(pFont);
}

void CUIListItemServer::CreateConsoleCommand(xr_string& command, LPCSTR player_name){
	command = "start server(";
	command+= *m_srv_info.info.map;
	command+= "/";
	command+= *m_srv_info.info.game;
	command+= ") client(";
	command+= *m_srv_info.info.server;
	command+= "/port=";
	command+= *m_srv_info.info.port;
	command+= "/name=";
	command+= player_name;
	command+= ")";
}
