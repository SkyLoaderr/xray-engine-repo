#include "StdAfx.h"
#include "ServerList.h"
#include "UIXmlInit.h"
#include "xrXmlParser.h"
#include "../string_table.h"


CServerList::CServerList(){
	AttachChild(&m_list);
	AttachChild(&m_frame);
	for (int i = 0; i<6; i++)
		AttachChild(&m_header[i]);
	for (int i = 0; i<5; i++)
		AttachChild(&m_separator[i]);
}

void CServerList::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);
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
}

void	CServerList::InitHeader(){
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
}