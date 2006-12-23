#include "stdafx.h"
#include "UIMpItemsStoreWnd.h"
#include "UIXmlInit.h"
#include "UITabButtonMP.h"
#include "../object_broker.h"
#include "restrictions.h"

void CStoreHierarchy::item::destroy()
{
	delete_data(m_childs);
	delete_data(m_button);
}

CStoreHierarchy::CStoreHierarchy()
{
	m_root					= NULL;
	m_current_level			= NULL;
}

CStoreHierarchy::~CStoreHierarchy()
{
	delete_data(m_root);
}


void CStoreHierarchy::LoadLevel(CUIXml& xml, int index, item* _item, int depth_level)
{
	XML_NODE* stored_root = xml.GetLocalRoot();

	XML_NODE* node					= xml.NavigateToNode	("level", index);
	_item->m_name					= xml.ReadAttrib		("level", index, "name",	NULL);
	_item->m_btn_xml_name			= xml.ReadAttrib		("level", index, "btn_ref", NULL);

	if( depth_level>0 && _item->m_btn_xml_name.size() )
	{
		CUITabButtonMP* btn			= xr_new<CUITabButtonMP>();
		_item->m_button				= btn;
		btn->SetAutoDelete			(false);
	
		XML_NODE* stored_root2		= xml.GetLocalRoot();
		xml.SetLocalRoot			(xml.GetRoot());
		CUIXmlInit::InitTabButtonMP	(xml, _item->m_btn_xml_name.c_str(), 0, btn);
		int horz					= xml.ReadAttribInt	(_item->m_btn_xml_name.c_str(), 0, "horz_al", 0);
		btn->SetOrientation			(horz==1);
		xml.SetLocalRoot			(stored_root2);
	}
	
	string1024			buff;
	buff[0] = 0;
	for(int c = 0; c<depth_level; ++c) strcat(buff,"-");
	Msg("%s%s",buff,_item->m_name.c_str());

	int cnt				= xml.GetNodesNum("level", index, "level");
	for(int i=0; i<cnt; ++i)
	{
		xml.SetLocalRoot			(node);
		item*	it					= xr_new<CStoreHierarchy::item>();
		it->m_parent				= _item;
		_item->m_childs.push_back	(it);
		LoadLevel					(xml, i ,it, depth_level+1);
	}
	xml.SetLocalRoot				(stored_root);
}

void CStoreHierarchy::Init(CUIXml& xml, LPCSTR path)
{
 	XML_NODE* p_stored_root = xml.GetLocalRoot();

	XML_NODE* node			= xml.NavigateToNode	(path, 0);
	xml.SetLocalRoot		(node);

	m_root					= xr_new<CStoreHierarchy::item>();
	LoadLevel				(xml, 0 ,m_root, 0);
	xml.SetLocalRoot		(p_stored_root);
	m_current_level			= m_root;
}

void CStoreHierarchy::InitItemsInGroup(const shared_str& sect, item* _itm)
{
	if(!_itm)	_itm	= m_root;
	u32 cnt				= _itm->ChildCount();
	
	if( !_itm->HasSubLevels() )
	{
		shared_str v	= pSettings->r_string(sect, _itm->m_name.c_str());
		u32 n			= _GetItemCount	(v.c_str());
		string512		buff;

		for(u32 i=0; i<n;++i)
		{
			_GetItem							(v.c_str(),i,buff);
			_itm->m_items_in_group.push_back	(buff);
			VERIFY3(g_mp_restrictions.GetItemGroup(buff).size(),"item has no group in restrictions", buff);
		}
		Msg("group[%s]", _itm->m_name.c_str());
		Msg("items[%s]", v.c_str());
		Msg("");
	}else
		for(u32 i=0; i<cnt;++i)
			InitItemsInGroup					(sect,_itm->m_childs[i]);
}

bool CStoreHierarchy::item::HasItem(const shared_str& name_sect) const
{
	xr_vector<shared_str>::const_iterator it = m_items_in_group.begin();
	xr_vector<shared_str>::const_iterator it_e = m_items_in_group.end();
	for(;it!=it_e;++it)
	{
		if(*it==name_sect)	return true;
	}
	return false;
}

int CStoreHierarchy::item::GetItemIdx(const shared_str& name_sect)	const
{
	xr_vector<shared_str>::const_iterator it = m_items_in_group.begin();
	xr_vector<shared_str>::const_iterator it_e = m_items_in_group.end();
	
	for(int idx=0; it!=it_e; ++it, ++idx)
	{
		if(*it==name_sect)	return idx;
	}
	return -1;
}

bool CStoreHierarchy::MoveUp()
{
	VERIFY							(m_current_level); 
	if(m_current_level==m_root)		return false;

	m_current_level					= m_current_level->m_parent;
	return							true;
};

bool CStoreHierarchy::MoveDown(u32 idx)
{
	VERIFY							(m_current_level); 
	VERIFY							(m_current_level->ChildCount() && idx<m_current_level->ChildCount());
	m_current_level					= &m_current_level->ChildAt(idx);
	return							true;
}
