#include "StdAfx.h"
#include "ServerList.h"
#include "UIXmlInit.h"
#include "xrXmlParser.h"
#include "../string_table.h"
#include "../../xr_ioconsole.h"
#include "UIEditBox.h"


CServerList::CServerList(){
	m_list.ShowSelectedItem();
	AttachChild(&m_frame);
	for (int i = 0; i<6; i++)
		AttachChild(&m_header[i]);
	for (int i = 0; i<5; i++)
		AttachChild(&m_separator[i]);

	AttachChild(&m_list);

	m_serverBrowser = NULL;
	GameSpy_Browser_ClearServersList();
}

CServerList::~CServerList()
{
	GameSpy_Browser_ClearServersList();
	GameSpy_Browser_Destroy();	
};

void CServerList::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);

	GameSpy_Browser_Init();
}

void CServerList::SetFilters(SServerFilters& sf){
	m_sf = sf;
	GameSpy_Browser_OnUpdateCompleted();
}

void CServerList::SetPlayerName(const char* name){
	m_palyerName = name;
}

bool CServerList::IsValidItem(ServerInfo& item){
	bool result = true;

	result &= m_sf.empty ? (m_sf.empty == (item.m_ServerNumPlayers == 0))						: true;
	result &= m_sf.full ? (m_sf.full == (item.m_ServerNumPlayers == item.m_ServerMaxPlayers))	: true;
	result &= m_sf.with_pass ? (m_sf.with_pass == item.m_bPassword)								: true;
	result &= m_sf.without_pass ? (m_sf.without_pass != item.m_bPassword)						: true;
	result &= m_sf.without_ff ? (m_sf.without_ff != item.m_bFFire)								: true;
	//result &= m_sf.without_pb ? (m_sf.without_pb = item.m)
	result &= m_sf.listen_servers ? (m_sf.listen_servers != item.m_bDedicated)					: true;

	return result;
}

void CServerList::InitFromXml(CUIXml& xml_doc, const char* path){
	CUIXmlInit::InitWindow(xml_doc, path, 0, this);
	string256 buf;
	CUIXmlInit::InitListWnd(xml_doc, strconcat(buf,path,":list"), 0, &m_list);
	CUIXmlInit::InitFrameWindow(xml_doc, strconcat(buf,path,":frame"), 0, &m_frame);
	CUIXmlInit::InitFont(xml_doc, strconcat(buf,path,":list_item:text"), 0, m_itemInfo.color, m_itemInfo.font);
	
	m_itemInfo.size.icon	= xml_doc.ReadAttribFlt( strconcat(buf, path, ":sizes"), 0, "icon");
	m_itemInfo.size.server	= xml_doc.ReadAttribFlt( buf, 0, "server");
	m_itemInfo.size.map		= xml_doc.ReadAttribFlt( buf, 0, "map");
	m_itemInfo.size.game	= xml_doc.ReadAttribFlt( buf, 0, "game");
	m_itemInfo.size.players	= xml_doc.ReadAttribFlt( buf, 0, "players");
	m_itemInfo.size.ping	= xml_doc.ReadAttribFlt( buf, 0, "ping");

	// init header elements
	for (int i = 0; i<6; i++)
		CUIXmlInit::InitLabel(xml_doc,strconcat(buf,path,":header"), 0, &m_header[i]);
	for (int i = 0; i<5; i++)
	{
		CUIXmlInit::InitFrameLine(xml_doc, strconcat(buf,path,":separator"), 0, &m_separator[i]);
		m_separator[i].SetHeight(m_wndSize.y);
	}
	InitHeader();

////////////////////////////////////////////////////////////////////////
	/*
	CUIListItemServer* item = xr_new<CUIListItemServer>();
	float w = m_list.GetItemWidth();
	float h = m_list.GetItemHeight();

	m_itemInfo.info.server	= "rainbow";
	m_itemInfo.info.map		= "testers_mp_bath";
	m_itemInfo.info.game	= "artefacthunt";
	m_itemInfo.info.players	= "10\32";
	m_itemInfo.info.ping	= "36646";

	item = xr_new<CUIListItemServer>();
	item->Init(m_itemInfo, 0, 0, w, h);
	m_list.AddItem<CUIListItemServer>(item);

	m_itemInfo.info.server	= "prof";
	m_itemInfo.info.map		= "testers_mp_factory";
	m_itemInfo.info.game	= "team deathmatch";
	m_itemInfo.info.players	= "10\32";
	m_itemInfo.info.ping	= "666";

	item = xr_new<CUIListItemServer>();
	item->Init(m_itemInfo, 0, 0, w, h);
	m_list.AddItem<CUIListItemServer>(item);
	*/
}

void CServerList::ConnectToSelected(){
	int sel = m_list.GetSelectedItem();
	if (-1 == sel)
		return;
	CUIListItemServer* item = smart_cast<CUIListItemServer*>(m_list.GetItem(sel));

	xr_string command;

	item->CreateConsoleCommand(command, m_palyerName.c_str());

	Console->Execute("main_menu off");
	Console->Execute(command.c_str());
}

void CServerList::InitHeader(){
	Fvector2 pos;
	pos.set(0,0);

	float y = m_header[0].GetHeight();

	CStringTable st;

//	pWnd->SetText(*st(*text));

	m_header[0].SetWidth(m_itemInfo.size.icon);
	m_header[0].SetText(*st("icon"));
	pos.x += m_itemInfo.size.icon;
	m_separator[0].SetWndPos(pos.x,y);
	m_header[1].SetWidth(m_itemInfo.size.server);
	m_header[1].SetWndPos(pos);
	m_header[1].SetText(*st("server name"));
	pos.x += m_itemInfo.size.server;
	m_separator[1].SetWndPos(pos.x,y);
	m_header[2].SetWidth(m_itemInfo.size.map);
	m_header[2].SetWndPos(pos);
	m_header[2].SetText(*st("map"));
	pos.x += m_itemInfo.size.map;
	m_separator[2].SetWndPos(pos.x,y);
	m_header[3].SetWidth(m_itemInfo.size.game);
	m_header[3].SetWndPos(pos);
	m_header[3].SetText(*st("game type"));
	pos.x += m_itemInfo.size.game;
	m_separator[3].SetWndPos(pos.x,y);
	m_header[4].SetWidth(m_itemInfo.size.players);
	m_header[4].SetWndPos(pos);
	m_header[4].SetText(*st("players"));
	pos.x += m_itemInfo.size.players;
	m_separator[4].SetWndPos(pos.x,y);
	m_header[5].SetWidth(m_itemInfo.size.ping);
	m_header[5].SetWndPos(pos);
	m_header[5].SetText(*st("ping"));
//	m_separator[5].SetWndPos(pos.x,y);
}

void	CServerList::RefreshGameSpyList	(bool Local){
	if (Local)
	{
		Msg("Refresh Local List");
	}
	else
	{
		Msg("Refresh MasterServer List");
	}
	GameSpy_Browser_RefreshList(Local);
}


void CServerList::AddServerToList	(ServerInfo* pServerInfo)
{
	CUIListItemServer* item = xr_new<CUIListItemServer>();
	float w = m_list.GetItemWidth();
	float h = m_list.GetItemHeight();

	m_itemInfo.info.server	= pServerInfo->m_ServerName;
	m_itemInfo.info.port.sprintf("%d", pServerInfo->m_Port);
	m_itemInfo.info.map		= pServerInfo->m_SessionName;
	m_itemInfo.info.game	= pServerInfo->m_ServerGameType;
	m_itemInfo.info.players.sprintf("%d/%d", pServerInfo->m_ServerNumPlayers, pServerInfo->m_ServerMaxPlayers);
	m_itemInfo.info.ping.sprintf("%d", pServerInfo->m_Ping);

	item->Init(m_itemInfo, 0, 0, w, h);
	m_list.AddItem<CUIListItemServer>(item);
};