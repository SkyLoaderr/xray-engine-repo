#include "StdAfx.h"
#include "ServerList.h"
#include "UIXmlInit.h"
#include "xrXmlParser.h"


CServerList::CServerList(){
	AttachChild(&m_list);
}

void CServerList::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);
//	m_list.Init(x,y,width,height);
}

void CServerList::InitFromXml(CUIXml& xml_doc, const char* path){
	CUIXmlInit::InitWindow(xml_doc, path, 0, this);
	string256 buf;
	CUIXmlInit::InitListWnd(xml_doc, strconcat(buf,path,":list"), 0, &m_list);
	CUIXmlInit::InitFont(xml_doc, strconcat(buf,path,":list_item:text"), 0, m_itemInfo.color, m_itemInfo.font);

	m_itemInfo.size.icon	= xml_doc.ReadAttribFlt( strconcat(buf, path, ":sizes"), 0, "icon");
	m_itemInfo.size.server	= xml_doc.ReadAttribFlt( buf, 0, "server");
	m_itemInfo.size.map		= xml_doc.ReadAttribFlt( buf, 0, "map");
	m_itemInfo.size.game	= xml_doc.ReadAttribFlt( buf, 0, "game");
	m_itemInfo.size.players	= xml_doc.ReadAttribFlt( buf, 0, "players");
	m_itemInfo.size.ping	= xml_doc.ReadAttribFlt( buf, 0, "ping");
////////////////////////////////////////////////////////////////////////
	CUIListItemServer* item = xr_new<CUIListItemServer>();
	float w = m_list.GetItemWidth();
	float h = m_list.GetItemHeight();

	m_itemInfo.info.server	= "server name";
	m_itemInfo.info.map		= "map";
	m_itemInfo.info.game	= "game type";
	m_itemInfo.info.players	= "players";
	m_itemInfo.info.ping	= "ping";

	item = xr_new<CUIListItemServer>();
	item->Init(m_itemInfo, 0, 0, w, h);
	m_list.AddItem<CUIListItemServer>(item);

	m_itemInfo.info.server	= "prof";
	m_itemInfo.info.map		= "testers_mp_factory";
	m_itemInfo.info.game	= "team deathmatch";
	m_itemInfo.info.players	= "10\32";
	m_itemInfo.info.ping	= "666";

	item = xr_new<CUIListItemServer>();
	m_list.AddItem<CUIListItemServer>(item);
}

void	CServerList::RefreshGameSpyList	(bool Local)
{
	if (Local)
	{
		Msg("Refresh Local List");
	}
	else
	{
		Msg("Refresh MasterServer List");
	}
}