#include "stdafx.h"
#include "UIMpItemsStoreWnd.h"
#include "UIXmlInit.h"
#include "UITabButtonMP.h"
#include "../object_broker.h"

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
		CUIXmlInit::Init3tButton	(xml, _item->m_btn_xml_name.c_str(), 0, btn);
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
		}
		Msg("group[%s]", _itm->m_name.c_str());
		Msg("items[%s]", v.c_str());
		Msg("");
	}else
		for(u32 i=0; i<cnt;++i)
			InitItemsInGroup					(sect,_itm->m_childs[i]);
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

void CItemCostMgr::Load(const shared_str& sect_cost)
{
	CInifile::Sect &sect = pSettings->r_section(sect_cost);

	u32 idx	=0;
	for (CInifile::SectIt it = sect.begin(); it != sect.end(); ++it,++idx)
	{
		_i&		val			= m_items[it->first]; 
		int c = sscanf		(it->second.c_str(),"%d,%d,%d,%d,%d",&val.foo[0],&val.foo[1],&val.foo[2],&val.foo[3],&val.foo[4]);
		VERIFY				(c>0);

		while(c<_RANK_COUNT)
		{
			val.foo[c]	= val.foo[c-1];
			++c;
		}
	}
}

u32	CItemCostMgr::GetItemCost	(const shared_str& sect_name, u32 rank)
{
	COST_MAP_IT it		= m_items.find(sect_name);
	VERIFY				(it!=m_items.end());
	return				it->second.foo[rank];
}

void CItemCostMgr::Dump() const
{
	COST_MAP_CIT it		= m_items.begin();
	COST_MAP_CIT it_e	= m_items.end();

	Msg("--CItemCostMgr::Dump");
	for(;it!=it_e;++it)
	{
		const _i&		val		= it->second; 
		Msg				("[%s] = %d,%d,%d,%d,%d",it->first.c_str(),val.foo[0],val.foo[1],val.foo[2],val.foo[3],val.foo[4]);
	}

}

/*

#include "UIBuyWndShared.h"
#include "Restrictions.h"
#include "UICellCustomItems.h"
#include "UICellItemFactory.h"
#include <dinput.h>
#include "../HUDManager.h"
#include "../weapon.h"
#include "../xrServer_Objects_ALife_Items.h"
#include "../game_cl_Deathmatch.h"

CUIMpItemsStoreWnd::CUIMpItemsStoreWnd()
{
	m_mlCurrLevel					= mlRoot;

	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
	{
		AttachChild					(&m_groups[i]);
	}

	AttachChild						(&m_btnBack);

	m_boxesDefs[0].xmlTag			= "btn_bag_shotgun";
	m_boxesDefs[0].filterString		= "shotgun";
	m_boxesDefs[0].gridHeight		= 2;
	m_boxesDefs[0].gridWidth		= 6;

	m_boxesDefs[1].xmlTag			= "btn_bag_assault";
	m_boxesDefs[1].filterString		= "assault_rifle";
	m_boxesDefs[1].gridHeight		= 2;
	m_boxesDefs[1].gridWidth		= 6;

	m_boxesDefs[2].xmlTag			= "btn_bag_sniper";
	m_boxesDefs[2].filterString		= "sniper_rifle";
	m_boxesDefs[2].gridHeight		= 2;
	m_boxesDefs[2].gridWidth		= 6;

	m_boxesDefs[3].xmlTag			= "btn_bag_heavy";
	m_boxesDefs[3].filterString		= "heavy_weapon";
	m_boxesDefs[3].gridHeight		= 2;
	m_boxesDefs[3].gridWidth		= 6;

	m_bIgnoreRank					= false;
	m_bIgnoreMoney					= false;
	subSection_group3[0] = subSection_group3[1] = subSection_group3[2] = subSection_group3[3] = 0;

	m_money							= 400;
}

CUIMpItemsStoreWnd::~CUIMpItemsStoreWnd()
{
	DestroyAllItems();
}

void CUIMpItemsStoreWnd::DestroyAllItems()
{
    u32 sz = m_allItems.size();
	for (u32 i = 0; i < sz; i++)
	{
		DestroyItem				(m_allItems[i]);
	}
	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
	{
		m_groups[i].ClearAll	(true);
	}
	m_allItems.clear			();
	m_info.clear				();

	for (int i = 0; i<4; i++)
        subSection_group3[i] = 0;
}

void CUIMpItemsStoreWnd::Init(CUIXml& xml, LPCSTR path, const shared_str& sectionName, const shared_str& sectionPrice)
{
	m_sectionName				= sectionName;
	m_sectionPrice				= sectionPrice;
	
	CUIXmlInit::InitStatic		(xml, path, 0, this);

	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
	{
		CUIXmlInit::InitDragDropListEx		(xml, "dragdrop_list_bag", 0, &m_groups[i]);
		m_groups[i].SetMessageTarget		(GetParent());
		BUY_WND*	w = smart_cast<BUY_WND*>(GetParent());
		w->BindDragDropListEvents			(&m_groups[i], true);
	}

	CUIXmlInit::Init3tButton				(xml, "bag_back_btn", 0, &m_btnBack);

	InitBoxes								(xml);
	InitWpnSectStorage						();
	InitItems								();	
}

void CUIMpItemsStoreWnd::InitItems()
{
    FillUpGroups			();
	HideAll					();
	SetMenuLevel			(mlRoot);
}

void CUIMpItemsStoreWnd::UpdateBuyPossibility()
{
	u32 sz		= m_allItems.size();

	for (u32 i = 0; i<sz; i++)
	{
		if (IsInBag(m_allItems[i]))
		{
			if (m_info[m_allItems[i]->m_index].bought)
			{
				m_allItems[i]->SetColor(0x00ffffff);
			}
			else if (UpdateRank(m_allItems[i]))		// update price if there no restriction for rank
			{
				if (UpdatePrice(m_allItems[i], i))
				{
					if (m_info[i].external)
                        SET_EXTERNAL_COLOR(m_allItems[i]);
				}
			}
		}
	}
}

bool CUIMpItemsStoreWnd::UpdateRank(CUICellItem* pItem)
{
	CInventoryItem*						pIItem;
	pIItem								= (CInventoryItem*)pItem->m_pData;
	bool av								= g_mp_restrictions.IsAvailable(*pIItem->object().cNameSect());

	if (av || m_bIgnoreRank)
	{
		SET_NO_RESTR_COLOR				(pItem);
		m_info[pItem->m_index].active	= true;
	}
	else
	{
		SET_RANK_RESTR_COLOR			(pItem);
		m_info[pItem->m_index].active	= false;
	}

    return av;
}



bool CUIMpItemsStoreWnd::IsBlueTeamItem(CUICellItem* itm)
{
	if (GameID() == GAME_DEATHMATCH)
		return true;					//in deathmath always blue

	CInventoryItem*	iitm				= (CInventoryItem*)itm->m_pData;
	LPCSTR			item				= *iitm->object().cNameSect();

	bool blue = !strstr(m_sectionName.c_str(),"team1");	//is our team blue

	//
	string64			wpnSection;
	string1024			wpnNames, wpnSingleName;

	for (int i = 1; i < 20; ++i)
	{
		// Имя поля
		sprintf			(wpnSection, "slot%i", i);

		if (!pSettings->line_exist(m_sectionName, wpnSection)) 
			continue;

		
		strcpy			(wpnNames, pSettings->r_string(m_sectionName, wpnSection));
		u32 count		= _GetItemCount(wpnNames);
		
		for (u32 j = 0; j < count; ++j)
		{
			_GetItem	(wpnNames, j, wpnSingleName);

			if (0 == xr_strcmp(item,wpnSingleName))
				return blue;
		}
	}	
    
	return !blue;
}

int CUIMpItemsStoreWnd::GetItemRank(CUICellItem* itm)
{
	CInventoryItem* iitm		= (CInventoryItem*)itm->m_pData;
	return	get_rank			(iitm->object().cNameSect());
//.    return g_mp_restrictions.GetItemRank(iitm->object().cNameSect());
}

bool CUIMpItemsStoreWnd::UpdatePrice(CUICellItem* pItem, int index)
{
	if (m_info[index].price > m_money && !m_bIgnoreMoney)
	{
		SET_PRICE_RESTR_COLOR			(pItem);
		m_info[pItem->m_index].active	= false;
		return false;
	}
	else
	{
		SET_NO_RESTR_COLOR				(pItem);
		m_info[pItem->m_index].active	= true;
		return							true;
	}
}

void CUIMpItemsStoreWnd::Update()
{
	UpdateBuyPossibility				();
	inherited::Update					();
}

// Init Boxes SLOT
void CUIMpItemsStoreWnd::InitBoxes(CUIXml& xml)
{

	for (u32 i = 0; i < 4; ++i)
	{
		CUITabButtonMP* pNewBtn			= xr_new<CUITabButtonMP>();
		pNewBtn->SetAutoDelete			(true);
		pNewBtn->SetOrientation			(false);
		pNewBtn->SetMessageTarget		(this);
		CUIXmlInit::Init3tButton		(xml, *m_boxesDefs[i].xmlTag, 0, pNewBtn);
		m_boxesDefs[i].pButton			= pNewBtn;
		m_groups[GROUP_BOXES].AttachChild(pNewBtn);
	}
}

void CUIMpItemsStoreWnd::InitWpnSectStorage()
{
	WPN_SECT_NAMES		wpnOneType;
	string64			wpnSection;
	string1024			wpnNames, wpnSingleName;


	// Поле strSectionName должно содержать имя секции
	R_ASSERT			(m_sectionName != "");
	R_ASSERT3			(pSettings->section_exist(m_sectionName), "Section doesn't exist", m_sectionName.c_str());

	for (int i = 1; i < 20; ++i)
	{
		// Очищаем буфер
		wpnOneType.clear();

		// Имя поля
		sprintf			(wpnSection, "slot%i", i);
		if (!pSettings->line_exist(m_sectionName, wpnSection)) 
		{
			m_wpnSectStorage.push_back(wpnOneType);
			continue;
		}

		strcpy		(wpnNames, pSettings->r_string(m_sectionName, wpnSection));
		u32 count	= _GetItemCount(wpnNames);

		for (u32 j = 0; j < count; ++j)
		{
			_GetItem				(wpnNames, j, wpnSingleName);
			wpnOneType.push_back	(wpnSingleName);
		}

		if (!wpnOneType.empty())
			m_wpnSectStorage.push_back(wpnOneType);
	}

	wpnOneType.clear();

	CInifile::Sect &sect = pSettings->r_section(m_sectionPrice.c_str());

	for (CInifile::SectIt it = sect.begin(); it != sect.end(); it++)
	{
		u8 group_id, index;
		GetWeaponIndexByName((*it).first.c_str(), group_id, index);

		if ((u8)(-1) == group_id || (u8)(-1) == index) // item not found
		{
			wpnOneType.push_back((*it).first.c_str());			//
		}	
	}
	if (!wpnOneType.empty())
			m_wpnSectStorage.push_back(wpnOneType);
}

void CUIMpItemsStoreWnd::FillUpGroups()
{	
	for (u32 i = 0; i < m_wpnSectStorage.size(); ++i)
		FillUpGroup(i);
}

extern "C"
DLL_Pure*	__cdecl xrFactory_Create		(CLASS_ID clsid);

void CUIMpItemsStoreWnd::FillUpGroup(const u32 group)
{
	string64 tmp_str;

	for (u32 j = 0; j < m_wpnSectStorage[group].size(); ++j)
	{
		const shared_str&	sect	= m_wpnSectStorage[group][j];		
		int			count			= g_mp_restrictions.GetItemCount( m_wpnSectStorage[group][j] );

		for (int i = 0; i < count; i++)
		{
			// Create item
			CUICellItem* itm			= CreateItem(sect);			

            // Set custom draw
			itoa						(j+1, tmp_str ,10);
			CBuyItemCustomDrawCell* p	= xr_new<CBuyItemCustomDrawCell>(tmp_str,UI()->Font()->pFontLetterica16Russian);
			itm->SetCustomDraw			(p);
            
			// Set Number
			itm->m_index				= m_allItems.size();

            // item info			
			m_allItems.push_back		(itm);
			ItmInfo ii;

			ii.price					= pSettings->r_s32(m_sectionPrice, m_wpnSectStorage[group][j].c_str());
			m_info.push_back			(ii);

			// 
			m_info[itm->m_index].short_cut		= u32(j + 1 % 10);
			m_info[itm->m_index].active			= true;
			m_info[itm->m_index].bought			= false;
			m_info[itm->m_index].section		= group;
			m_info[itm->m_index].pos_in_section = j;
			m_info[itm->m_index].external		= false;
			PutItemToGroup						(itm, group);
		}
	}
}

void CUIMpItemsStoreWnd::ReloadItemsPrices	()
{
	string256 ItemCostStr	= "";
	string256 RankStr		= "";

	u32 sz = m_allItems.size();

	for (u32 i = 0; i<sz; i++)
	{
		CUICellItem* itm				= m_allItems[i];
		CInventoryItem* iitm			= (CInventoryItem*)itm->m_pData;
		const shared_str itm_name		= iitm->object().cNameSect();

		m_info[itm->m_index].price		= pSettings->r_u32(m_sectionPrice, *itm_name);

		strconcat						(ItemCostStr, itm_name.c_str(), "_cost");
		if (pSettings->line_exist		(m_sectionPrice, ItemCostStr))
			m_info[itm->m_index].price	= pSettings->r_u32(m_sectionPrice, ItemCostStr);


		for (u32 i=1; i<=g_mp_restrictions.GetRank(); i++)
		{
			sprintf						(RankStr, "rank_%d", i);
			if (!pSettings->line_exist	(RankStr, ItemCostStr))	continue;
			m_info[itm->m_index].price	= pSettings->r_u32(RankStr, ItemCostStr);
		}
	}
}

static int counter = 0;

CUICellItem* CUIMpItemsStoreWnd::CreateItem(const shared_str& name_sect)
{
	counter++;
	CLASS_ID	class_id		= pSettings->r_clsid(name_sect,"class");

	DLL_Pure					*dll_pure = xrFactory_Create(class_id);
	VERIFY						(dll_pure);
	CInventoryItem*				pIItem = smart_cast<CInventoryItem*>(dll_pure);
	pIItem->object().Load		(name_sect.c_str());
	VERIFY						(pIItem);
	return create_cell_item		(pIItem);
}

void CUIMpItemsStoreWnd::DestroyItem(CUICellItem* itm)
{
	R_ASSERT					(itm);
	if (itm->m_pData)
	{
		CInventoryItem* iitem	= (CInventoryItem*)itm->m_pData;
		xr_delete				(iitem);
	}else
	{
		R_ASSERT2			(0, "!!!!!!! empty cell item");
	}
}

void CUIMpItemsStoreWnd::PutItemToGroup(CUICellItem* pItem, int iGroup)
{
	int iActiveSection			= -1;
	shared_str					weapon_class;
	CInventoryItem*				pIItem;
	string64					tmp_str;

	pIItem						= (CInventoryItem*)pItem->m_pData;		

	switch (iGroup)
	{
	case 1:
		iActiveSection			= GROUP_2;	break;

	case 2:		
		weapon_class = pSettings->r_string(pIItem->object().cNameSect(), "weapon_class");		

		if		(  m_boxesDefs[0].filterString == weapon_class )
			iActiveSection = GROUP_31;

		else if ( m_boxesDefs[1].filterString == weapon_class )
			iActiveSection = GROUP_32;

		else if ( m_boxesDefs[2].filterString == weapon_class )
			iActiveSection = GROUP_33;

		else if ( m_boxesDefs[3].filterString == weapon_class )
			iActiveSection = GROUP_34;
		break;

	case 3:
		iActiveSection = GROUP_4;	break;

	case 4:
		iActiveSection = GROUP_5;	break;

	case 5:
		iActiveSection = GROUP_6;	break;

	default:
		iActiveSection = GROUP_DEFAULT;
	}

	R_ASSERT2( (-1 != iActiveSection), "CUIBagWnd::PutItemToGroup - invalid section");

	m_groups[iActiveSection].SetItem		(pItem);	
	pItem->SetAutoDelete					(false);
	m_info[pItem->m_index].group_index		= iActiveSection;

	if ( 2 == iGroup)
	{
		++subSection_group3[iActiveSection - GROUP_31];
		
		sprintf						(tmp_str, "%d", subSection_group3[iActiveSection - GROUP_31]);
		CBuyItemCustomDrawCell* p	= xr_new<CBuyItemCustomDrawCell>(tmp_str, UI()->Font()->pFontLetterica16Russian);
		pItem->SetCustomDraw		(p);

		m_info[pItem->m_index].short_cut = subSection_group3[iActiveSection - GROUP_31] % 10;
	}

	if (3 == iGroup)
	{
		if (m_info[pItem->m_index].short_cut >= 6)
		{
            m_info[pItem->m_index].short_cut	= NULL; // no shortcut
			pItem->SetCustomDraw				(NULL);
		}
	}
}

CUIDragDropListEx*	CUIMpItemsStoreWnd::GetItemList(CUICellItem* pItem)
{
	return &m_groups[m_info[pItem->m_index].group_index];
}

void CUIMpItemsStoreWnd::GetWeaponIndexByName(const shared_str& sectionName, u8 &grpNum, u8 &idx)
{
	grpNum	= (u8)(-1);
	idx		= (u8)(-1);

	for (u8 i = 0; i < m_wpnSectStorage.size(); ++i)
	{
		for (u8 j = 0; j < m_wpnSectStorage[i].size(); ++j)
		{
			if (sectionName == m_wpnSectStorage[i][j])
			{
				grpNum	= i;
				idx		= j;
				return;
			}
		}
	}
}

void CUIMpItemsStoreWnd::HideAll()
{
	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
		m_groups[i].Show	(false);
}

bool CUIMpItemsStoreWnd::SetMenuLevel(MENU_LEVELS level)
{
	// check range
	if (level < mlRoot || level > mlWpnSubType) 
		return false;

	m_btnBack.Enable(mlRoot != level);

	// check we really change state
	if (m_mlCurrLevel == level)
		return false;

	m_mlCurrLevel = level;	

	GetMessageTarget()->SendMessage(this, XR_MENU_LEVEL_CHANGED, NULL);

	return true;
}

void CUIMpItemsStoreWnd::ShowSection(int iSection)
{
	R_ASSERT2(  (GROUP_2 <= iSection) && (iSection <= GROUP_6 + 1), "CUIBagWnd::ShowSection() - invalid section number");

	HideAll();

	if (GROUP_6 + 1 != iSection)
	{
        m_groups[iSection].Show(true);

		if ( GROUP_BOXES == iSection )
			SetMenuLevel(mlBoxes);
		else
			SetMenuLevel(mlWpnSubType);
	}
	else
		SetMenuLevel(mlRoot);
}

MENU_LEVELS CUIMpItemsStoreWnd::GetMenuLevel()
{
	return m_mlCurrLevel;
}

bool CUIMpItemsStoreWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if(keyboard_action!=WINDOW_KEY_PRESSED)		return false;

	int iGroup;

	if (DIK_ESCAPE == dik)
	{
		m_btnBack.OnClick();
		return true;
	}

	switch (GetMenuLevel())
	{
	case mlRoot:
		R_ASSERT2(false,"error: CUIBagWnd on level <mlRoot> can't handle keyboard");
		break;
	case mlBoxes:

		if (DIK_ESCAPE == dik)
		{
			ShowSectionEx(-1);			return true;
		}

		switch (dik)
		{
		case DIK_1:
			OnBtnShotgunsClicked();					return true;

		case DIK_2:
			OnBtnMachinegunsClicked();				return true;

		case DIK_3:
			OnBtnSniperClicked();					return true;

		case DIK_4:
			OnBtnHeavyClicked();					return true;
		}
		break;

	case mlWpnSubType:		
		iGroup = GetCurrentGroupIndex();
		if (DIK_ESCAPE == dik)
		{
			if (iGroup >= GROUP_31 && iGroup <= GROUP_34 )
				ShowSectionEx(GROUP_BOXES);
			else
				ShowSectionEx(-1);

			return true;
		}

		if (dik <= DIK_0 && dik >= DIK_1)
		{
			CUICellItem* itm = GetItemByKey(dik,GetCurrentGroupIndex());
			if (itm && IsInBag(itm))
                itm->GetMessageTarget()->SendMessage(itm, DRAG_DROP_ITEM_DB_CLICK, NULL);
		}
		break;
	default:
		NODEFAULT;
	}

	return false;
}

bool CUIMpItemsStoreWnd::IsInBag(CUICellItem* pItem)
{
	VERIFY				(pItem);
	return GetItemList	(pItem)->IsOwner(pItem);
}

bool CUIMpItemsStoreWnd::IsActive(CUICellItem* itm)
{
	VERIFY				(itm);
	return m_info[itm->m_index].active;
}

void CUIMpItemsStoreWnd::BuyItem(CUICellItem* itm)
{
	VERIFY(itm);

	m_info[itm->m_index].bought = true;

	if (!this->m_bIgnoreMoney)
	{
		if (m_info[itm->m_index].external)
			m_money -= m_info[itm->m_index].price/2;
		else
			m_money -= m_info[itm->m_index].price;

		GetParent()->SendMessage(this, MP_MONEY_CHANGE);
	}

	if (GetExternal(itm))
	{
		SET_EXTERNAL_COLOR(itm);
	}
}

void CUIMpItemsStoreWnd::ClearAmmoHighlight()
{
	u32 sz = m_groups[GROUP_4].ItemsCount();

	for (u32 i = 0; i<sz; i++)
	{
		CUICellItem* itm	= m_groups[GROUP_4].GetItemIdx(i);
		UNHIGHTLIGHT_ITEM	(itm);
	}
}

void CUIMpItemsStoreWnd::HightlightAmmo(LPCSTR ammo)
{
	CUICellItem* itm		= GetItemBySectoin(ammo);
	if (itm)
        HIGHTLIGHT_ITEM		(itm);
}

void CUIMpItemsStoreWnd::SellItem(CUICellItem* itm)
{
	VERIFY					(itm);

	m_info[itm->m_index].bought = false;

	if (itm->GetColor() == PRICE_RESTR_COLOR)		// Fuck... loose it
		return;

	if (!this->m_bIgnoreMoney)
	{
		if (m_info[itm->m_index].external)
			m_money += m_info[itm->m_index].price/2;
		else
			m_money += m_info[itm->m_index].price;

		GetParent()->SendMessage(this, MP_MONEY_CHANGE);
	}
}

bool CUIMpItemsStoreWnd::CanBuy(CUICellItem* itm)
{
	VERIFY					(itm);
	if (!IsInBag(itm))
		return false;

	CInventoryItem* iitm	= (CInventoryItem*)itm->m_pData;

	if (m_bIgnoreMoney)
	{
		if (m_bIgnoreRank)
            return true;
		else if (g_mp_restrictions.IsAvailable(iitm->object().cNameSect()))
			return true;
	}
	else if (m_bIgnoreRank)
	{
		if (m_info[itm->m_index].price < m_money)
			return true;
	}

	return m_info[itm->m_index].active && (!m_info[itm->m_index].bought);
}

bool CUIMpItemsStoreWnd::CanBuy(LPCSTR item)
{
	return CanBuy		(GetItemBySectoin(item));
}

CUICellItem* CUIMpItemsStoreWnd::GetItemByKey(int dik, int section){
	R_ASSERT2((GROUP_2 <= section) && (section <= GROUP_34), "CUIBagWnd::GetItemByKey() - invalid section number");
	u32 index = static_cast<u32>(dik - 1);

	u32 sz = m_allItems.size();
	for (u32 i = 0; i<sz; i++){
		if (m_info[m_allItems[i]->m_index].group_index == section 
			&& m_info[m_allItems[i]->m_index].short_cut == index)
		{
			return m_allItems[i];
		}		
	}
	return NULL;
}

void CUIMpItemsStoreWnd::ShowSectionEx(int iSection){
	HideAll();

	if (-1 == iSection)
	{
		SetMenuLevel(mlRoot);	return;
	}

	R_ASSERT2((GROUP_2 <= iSection) && (iSection <= GROUP_34), "CUIBagWnd::ShowSectionEx() - invalid section number");

    m_groups[iSection].Show(true);

	if ( GROUP_BOXES == iSection )
		SetMenuLevel(mlBoxes);
	else
		SetMenuLevel(mlWpnSubType);
}

void CUIMpItemsStoreWnd::OnBtnShotgunsClicked(){
	m_boxesDefs[0].pButton->OnClick();
	m_boxesDefs[0].pButton->OnMouse(-1, -1, WINDOW_MOUSE_MOVE);
	ShowSectionEx(GROUP_31);
}
void CUIMpItemsStoreWnd::OnBtnMachinegunsClicked(){
	m_boxesDefs[1].pButton->OnClick();
	m_boxesDefs[1].pButton->OnMouse(-1, -1, WINDOW_MOUSE_MOVE);
	ShowSectionEx(GROUP_32);
}
void CUIMpItemsStoreWnd::OnBtnSniperClicked(){
	m_boxesDefs[2].pButton->OnClick();
	m_boxesDefs[2].pButton->OnMouse(-1, -1, WINDOW_MOUSE_MOVE);
	ShowSectionEx(GROUP_33);
}
void CUIMpItemsStoreWnd::OnBtnHeavyClicked(){
	m_boxesDefs[3].pButton->OnClick();
	m_boxesDefs[3].pButton->OnMouse(-1, -1, WINDOW_MOUSE_MOVE);
	ShowSectionEx(GROUP_34);
}

void CUIMpItemsStoreWnd::OnBackClick(){
	int iGroup;

	switch (GetMenuLevel())
	{
	case mlRoot:
		R_ASSERT2(false,"error: CUIBagWnd on level <mlRoot> can't handle OnBackClick");
		break;
	case mlBoxes:
		ShowSectionEx(-1);
		break;
	case mlWpnSubType:		
		iGroup = GetCurrentGroupIndex();
		if (iGroup >= GROUP_31 && iGroup <= GROUP_34 )
			ShowSectionEx(GROUP_BOXES);
		else
			ShowSectionEx(-1);
		break;
	default:
		NODEFAULT;
	}
}

int CUIMpItemsStoreWnd::GetCurrentGroupIndex(){
	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
		if(m_groups[i].IsShown())
			return i;

	return GROUP_DEFAULT;
}

void CUIMpItemsStoreWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	switch (msg)
	{
		// we using our super-puper tab buttons enstead of DragDropItems
		// those buttons uses TAB_CHANGED message for Click event
	case TAB_CHANGED:
		if		(pWnd == m_boxesDefs[0].pButton)
			OnBtnShotgunsClicked();
		else if (pWnd == m_boxesDefs[1].pButton)
			OnBtnMachinegunsClicked();
		else if (pWnd == m_boxesDefs[2].pButton)
			OnBtnSniperClicked();
		else if (pWnd == m_boxesDefs[3].pButton)
			OnBtnHeavyClicked();
		break;

	case BUTTON_CLICKED:
		if (&m_btnBack == pWnd)
			OnBackClick();
		break;
	}
	inherited::SendMessage(pWnd, msg, pData);
}

u8 CUIMpItemsStoreWnd::GetItemIndex(CUICellItem* pItem, u8 &sectionNum){
	sectionNum = 0;
	u8 ret = static_cast<u8>(-1);

	if (!pItem)
		return ret;

	ret = static_cast<u8>(m_info[pItem->m_index].pos_in_section);
	sectionNum = static_cast<u8>(m_info[pItem->m_index].section);

	CInventoryItem* iitem = (CInventoryItem*)pItem->m_pData;

	if (iitem->GetSlot() == PISTOL_SLOT || iitem->GetSlot() == RIFLE_SLOT)
	{
		CWeapon* pWeapon = (CWeapon*)pItem->m_pData;

		u8 addon = pWeapon->GetAddonsState();

		if (addon & CSE_ALifeItemWeapon::eWeaponAddonScope)
			ret |= 1<<5;
		if (addon & CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher)
			ret |= 1<<6;
		if (addon & CSE_ALifeItemWeapon::eWeaponAddonSilencer)
			ret |= 1<<7;
	}
	
	return ret;
}

void CUIMpItemsStoreWnd::SetRank(int r)
{
	g_mp_restrictions.SetRank(r);
}

const shared_str& CUIMpItemsStoreWnd::GetWeaponNameByIndex(u8 grpNum, u8 idx)
{
	R_ASSERT (grpNum < m_wpnSectStorage.size() );

	R_ASSERT (idx < m_wpnSectStorage[grpNum].size() );
	return m_wpnSectStorage[grpNum][idx];
}

void CUIMpItemsStoreWnd::SetExternal(CUICellItem* itm, bool status){
	m_info[itm->m_index].external = status;
}

bool CUIMpItemsStoreWnd::GetExternal(CUICellItem* itm){
	return m_info[itm->m_index].external;
}

bool CUIMpItemsStoreWnd::HasEnoughtMoney(CUICellItem* itm){
	if (m_bIgnoreMoney) 
		return true;
	R_ASSERT(itm);
	return m_info[itm->m_index].price <= m_money;
}

int CUIMpItemsStoreWnd::GetItemPrice(CUICellItem* itm){
	return m_info[itm->m_index].price;
}

void CUIMpItemsStoreWnd::ClearExternalStatus(){
	u32 sz = m_info.size();

	for (u32 i = 0; i<sz; i++)
	{
		m_info[i].external = false;
	}
}

void CUIMpItemsStoreWnd::AttachAddon(CUICellItem* itm, CSE_ALifeItemWeapon::EWeaponAddonState add_on, bool external){
	R_ASSERT(itm);
	CWeapon* wpn = (CWeapon*)itm->m_pData;	
	CUICellItem* add_itm = NULL;
	CUIWeaponCellItem* wpn_itm = smart_cast<CUIWeaponCellItem*>(itm);
	R_ASSERT(wpn_itm);

	switch (add_on){
		case CSE_ALifeItemWeapon::eWeaponAddonScope:
			add_itm = GetItemBySectoin(*wpn->GetScopeName());
			break;
		case CSE_ALifeItemWeapon::eWeaponAddonSilencer:			
			add_itm = GetItemBySectoin(*wpn->GetSilencerName());
			break;
		case CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher:	
			add_itm = GetItemBySectoin(*wpn->GetGrenadeLauncherName());		
			break;
		default:	NODEFAULT;
	}

	if (external)
        m_info[add_itm->m_index].external = external;
	BuyItem(add_itm);
	wpn->Attach((CInventoryItem*)add_itm->m_pData);

	wpn_itm->Update();

	if (external){
		switch (add_on){
			case CSE_ALifeItemWeapon::eWeaponAddonScope:
				SET_EXTERNAL_COLOR(wpn_itm->get_addon_static(CUIWeaponCellItem::eScope));
				break;
			case CSE_ALifeItemWeapon::eWeaponAddonSilencer:			
				SET_EXTERNAL_COLOR(wpn_itm->get_addon_static(CUIWeaponCellItem::eSilencer));
				break;
			case CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher:	
				SET_EXTERNAL_COLOR(wpn_itm->get_addon_static(CUIWeaponCellItem::eLauncher));
				break;
			default:	
				NODEFAULT;
		}
	}
}


CUICellItem* CUIMpItemsStoreWnd::GetItemBySectoin(const shared_str& sectionName, bool bCreateOnFail){

	u32 sz			= m_allItems.size();

	for (u32 i = 0; i < sz; i++)
	{
		CInventoryItem* iitem		= (CInventoryItem*)m_allItems[i]->m_pData;

		if( iitem->object().cNameSect() == sectionName )
		{
			if (IsInBag(m_allItems[i]))
                return m_allItems[i];
		}
	}
	return NULL;
}

CUICellItem* CUIMpItemsStoreWnd::GetItemBySectoin(const u8 grpNum, u8 uIndexInSlot)
{
	u32 sz				= m_allItems.size();
	CUICellItem*		item;

	for (u32 i = 0; i < sz; i++)
	{
		item			= m_allItems[i];

		if (m_info[item->m_index].pos_in_section == uIndexInSlot &&	m_info[item->m_index].section == grpNum	)
		{
			if (IsInBag(item))
                return item;
		}
	}

	return NULL;
}

CUICellItem* CUIMpItemsStoreWnd::CreateNewItem(const u8 grpNum, u8 uIndexInSlot)
{
	u32			 sz = m_allItems.size();
	VERIFY		 (sz);
	CUICellItem* item;
	PIItem		 iitem = NULL;
	CUICellItem* new_item;

	for (u32 i = 0; i < sz; i++){
		item = m_allItems[i];

		if (m_info[item->m_index].pos_in_section == uIndexInSlot &&	m_info[item->m_index].section == grpNum	)
		{
			iitem = (PIItem)item->m_pData;
            break;             
		}
	}

	R_ASSERT				(iitem);

	new_item				= CreateItem(iitem->object().cNameSect());

	new_item->m_index		= m_allItems.size();
	m_allItems.push_back	(new_item);
	ItmInfo					ii;
	ii.price				= pSettings->r_s32(*m_sectionPrice, m_wpnSectStorage[grpNum][uIndexInSlot].c_str());
	m_info.push_back		(ii);

	m_info[new_item->m_index].short_cut = NULL;
	m_info[new_item->m_index].active = true;
	m_info[new_item->m_index].bought = false;
	m_info[new_item->m_index].section = grpNum;
	m_info[new_item->m_index].pos_in_section = uIndexInSlot;
	m_info[new_item->m_index].external = false;

    PutItemToGroup			(new_item, grpNum);

    
	return					new_item;
}
*/