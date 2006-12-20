#include "stdafx.h"
#include "UIMpTradeWnd.h"
#include "../inventory_item.h"
#include "../PhysicsShellHolder.h"
#include "../object_broker.h"
#include "UICellItem.h"
#include "UIDragDropListEx.h"
#include "../string_table.h"


extern "C"
DLL_Pure*	__cdecl xrFactory_Create		(CLASS_ID clsid);

extern "C"
void	__cdecl xrFactory_Destroy		(DLL_Pure* O);

CUICellItem*	create_cell_item(CInventoryItem* itm);



SBuyItemInfo::SBuyItemInfo()
{
	m_item_state = e_undefined;
}

SBuyItemInfo::~SBuyItemInfo()
{
	CInventoryItem*			iitem = (CInventoryItem*)m_cell_item->m_pData;
	xrFactory_Destroy		(&iitem->object());
	delete_data				(m_cell_item);
}

void SBuyItemInfo::SetState	(const EItmState& s)
{
	switch(m_item_state)
	{
	case e_undefined:
		{
			m_item_state	= s;
			break;
		};
	case e_bought:
		{
			VERIFY2			(s==e_shop||s==e_sold,"incorrect SBuyItemInfo::SetState sequence");
			
			m_item_state	= e_shop;
			break;
		};
	case e_sold:
		{
			VERIFY2			(s==e_own||s==e_bought,"incorrect SBuyItemInfo::SetState sequence");
			m_item_state	= e_own;
			break;
		};
	case e_own:
		{
			VERIFY2			(s==e_sold,"incorrect SBuyItemInfo::SetState sequence");
			m_item_state	= s;
			break;
		};
	case e_shop:
		{
			VERIFY2			(s==e_bought,"incorrect SBuyItemInfo::SetState sequence");
			m_item_state	= s;
			break;
		};
	};
		
}

LPCSTR _state_names []={
	"e_undefined",
	"e_bought",
	"e_sold",
	"e_own",
	"e_shop"
};

LPCSTR SBuyItemInfo::GetStateAsText() const
{
	EItmState st = GetState();
	return _state_names[st];
}


CInventoryItem* CUIMpTradeWnd::CreateItem_internal(const shared_str& name_sect)
{
	CLASS_ID	class_id		= pSettings->r_clsid(name_sect,"class");

	DLL_Pure					*dll_pure = xrFactory_Create(class_id);
	VERIFY						(dll_pure);
	CInventoryItem*				pIItem = smart_cast<CInventoryItem*>(dll_pure);
	pIItem->object().Load		(name_sect.c_str());
	VERIFY						(pIItem);
	return						(pIItem);
}

SBuyItemInfo* CUIMpTradeWnd::CreateItem(const shared_str& name_sect, SBuyItemInfo::EItmState type, bool find_if_exist)
{
	SBuyItemInfo* iinfo			= (find_if_exist)?FindItem(name_sect, type):NULL;
	if(iinfo)
		return					iinfo;

	iinfo						= xr_new<SBuyItemInfo>();
	m_all_items.push_back		( iinfo );
	iinfo->m_name_sect			= name_sect;
	iinfo->SetState				(type);
	iinfo->m_cell_item			= create_cell_item(CreateItem_internal(name_sect));
	iinfo->m_cell_item->m_b_destroy_childs = false;
	return						iinfo;
}

struct state_eq{
	SBuyItemInfo::EItmState m_state;

	state_eq(SBuyItemInfo::EItmState state):m_state(state){}

	bool operator () (SBuyItemInfo* itm){
		return ( m_state==itm->GetState() );
	}
};

SBuyItemInfo* CUIMpTradeWnd::FindItem(SBuyItemInfo::EItmState state)
{
	state_eq	eq		(state);

	ITEMS_vec_cit it = std::find_if(m_all_items.begin(), m_all_items.end(), eq );
	return (it==m_all_items.end())?NULL:(*it);
}

SBuyItemInfo* CUIMpTradeWnd::FindItem(const shared_str& name_sect, SBuyItemInfo::EItmState type)
{
	ITEMS_vec_cit it = m_all_items.begin();
	ITEMS_vec_cit it_e = m_all_items.end();
	for(;it!=it_e;++it)
	{
		SBuyItemInfo* pitm = *it;
		if(pitm->m_name_sect==name_sect && pitm->GetState()==type)
			return pitm;
	}
	return NULL;
}

SBuyItemInfo* CUIMpTradeWnd::FindItem(CUICellItem* item)
{
	ITEMS_vec_cit it = m_all_items.begin();
	ITEMS_vec_cit it_e = m_all_items.end();

	for(;it!=it_e;++it)
	{
		SBuyItemInfo* pitm = *it;
		if(pitm->m_cell_item==item)
			return pitm;
	}
	return	NULL;
}

void CUIMpTradeWnd::DestroyItem(SBuyItemInfo* item)
{
	ITEMS_vec_it it		= std::find(m_all_items.begin(), m_all_items.end(), item);
	VERIFY				(it!= m_all_items.end() );

#pragma todo("detach addons first")
	m_all_items.erase	(it);
	delete_data			(item);
}

u32 CUIMpTradeWnd::GetItemCount(SBuyItemInfo::EItmState state) const
{
	u32 res					= 0;
	ITEMS_vec_cit it		= m_all_items.begin();
	ITEMS_vec_cit it_e		= m_all_items.end();

	for(;it!=it_e;++it)
	{
		SBuyItemInfo* pitm = *it;
		if( pitm->GetState()==state )
			++res;
	}
	return	res;
}

u32 CUIMpTradeWnd::GetItemCount(const shared_str& name_sect, SBuyItemInfo::EItmState state) const
{
	u32 res					= 0;
	ITEMS_vec_cit it		= m_all_items.begin();
	ITEMS_vec_cit it_e		= m_all_items.end();

	for(;it!=it_e;++it)
	{
		SBuyItemInfo* pitm = *it;
		if( (pitm->m_name_sect == name_sect) && (pitm->GetState()==state) )
			++res;
	}
	return	res;
}

u32 CUIMpTradeWnd::GetGroupCount(const shared_str& name_group, SBuyItemInfo::EItmState state) const
{
	u32 res					= 0;
	ITEMS_vec_cit it		= m_all_items.begin();
	ITEMS_vec_cit it_e		= m_all_items.end();

	for(;it!=it_e;++it)
	{
		SBuyItemInfo* pitm				= *it;
		const shared_str& group_name	= g_mp_restrictions.GetItemGroup(pitm->m_name_sect);
		if( (group_name == name_group) && (pitm->GetState()==state) )
			++res;
	}
	return	res;
}
const preset_items& CUIMpTradeWnd::GetPreset(ETradePreset idx)
{
	VERIFY			(idx<_preset_count); 
	return			m_preset_storage[idx];
};

u32 _list_prio[]={
	6,	//	e_pistol	
	4,	//	e_pistol_ammo	
	7,  //	e_rifle
	5,  //	e_rifle_ammo
	10,  //	e_outfit
	9,  //	e_medkit
	8,  //	e_granade
	3,  //	e_others
	2,  //	e_player_bag
	0,	//	e_shop
	0,
	0,
	0,
	0,
};

struct preset_sorter {
	CItemMgr* m_mgr;
	preset_sorter(CItemMgr* mgr):m_mgr(mgr){};
	bool operator() (const _preset_item& i1, const _preset_item& i2)
	{
		u8 list_idx1	= m_mgr->GetItemSlotIdx(i1.sect_name);
		u8 list_idx2	= m_mgr->GetItemSlotIdx(i2.sect_name);
		
		return		(_list_prio[list_idx1] > _list_prio[list_idx2]);
	};
};

void CUIMpTradeWnd::StorePreset(ETradePreset idx)
{
	string512						buff;
	sprintf							(buff,	"%s [%d]",
											CStringTable().translate("ui_st_preset_stored_to").c_str(), idx);
	SetInfoString					(buff);
	
	ITEMS_vec_cit it				= m_all_items.begin();
	ITEMS_vec_cit it_e				= m_all_items.end();

	preset_items&	v				= m_preset_storage[idx];
	v.clear_not_free				();
	for(;it!=it_e; ++it)
	{
		SBuyItemInfo* iinfo			= *it;
		preset_items::iterator fit	= std::find(v.begin(), v.end(), iinfo->m_name_sect);
		if(fit!=v.end())
			continue;

		u32 cnt						= GetItemCount(iinfo->m_name_sect, SBuyItemInfo::e_bought);
		cnt							+=GetItemCount(iinfo->m_name_sect, SBuyItemInfo::e_own);
		if(0==cnt)				
			continue;

		v.resize					(v.size()+1);
		_preset_item& _one			= v.back();
		_one.sect_name				= iinfo->m_name_sect;
		_one.count					= cnt;
//.		addons_sect
	}

	std::sort						(v.begin(), v.end(), preset_sorter(m_item_mngr));
}

void CUIMpTradeWnd::ApplyPreset(ETradePreset idx)
{
	ResetToOrigin						();

	const preset_items&		v			=  GetPreset(idx);
	preset_items::const_iterator it		= v.begin();
	preset_items::const_iterator it_e	= v.end();

	for(;it!=it_e;++it)
	{
		const _preset_item& _one		= *it;
		u32 _cnt						= GetItemCount(_one.sect_name, SBuyItemInfo::e_own);
		for(u32 i=_cnt; i<_one.count; ++i)
		{
			SBuyItemInfo* pitem				= CreateItem(_one.sect_name, SBuyItemInfo::e_undefined, false);
			// dont forget about addons		!!!
			bool b_res						= TryToBuyItem(pitem, (idx==_preset_idx_origin) );
			if(!b_res)
				DestroyItem					(pitem);
		}
	}
}

void CUIMpTradeWnd::ResetToOrigin()
{
	// 1-sell all bought items
	// 2-buy all sold items
	
	ITEMS_vec_cit it;

	SBuyItemInfo*	iinfo	= NULL;
	bool			b_ok	= true;

	do{
		iinfo			= FindItem(SBuyItemInfo::e_bought);
		if(iinfo)
			b_ok		= TryToSellItem(iinfo);

		R_ASSERT		(b_ok);
	}while(iinfo);
	
	do{
		iinfo			= FindItem(SBuyItemInfo::e_sold);
		if(iinfo)
			b_ok		= TryToBuyItem(iinfo, false);

		R_ASSERT		(b_ok);
	}while(iinfo);

	do{
		iinfo						= FindItem(SBuyItemInfo::e_own);
		if(iinfo)
		{
			VERIFY					(iinfo->m_cell_item->OwnerList());
			CUICellItem* citem		= iinfo->m_cell_item->OwnerList()->RemoveItem(iinfo->m_cell_item, false );
			SBuyItemInfo* iinfo_int = FindItem(citem);
			DestroyItem				(iinfo_int);
		}
	}while(iinfo);
}

void CUIMpTradeWnd::SetupPlayerItemsBegin()
{
	if(
		(0!=GetItemCount(SBuyItemInfo::e_own))		||
		(0!=GetItemCount(SBuyItemInfo::e_sold))		||
		(0!=GetItemCount(SBuyItemInfo::e_bought))
		){
			DumpAllItems("");
			VERIFY2(0, "0!=GetItemCount");
		}
}

void CUIMpTradeWnd::SetupPlayerItemsEnd()
{
	StorePreset			(_preset_idx_origin);
}

void CUIMpTradeWnd::DumpAllItems(LPCSTR s)
{
	Msg("CUIMpTradeWnd::DumpAllItems.total[%d] reason [%s]", m_all_items.size(), s);
	ITEMS_vec_cit it = m_all_items.begin();
	ITEMS_vec_cit it_e = m_all_items.end();
	for(;it!=it_e;++it)
	{
		SBuyItemInfo* iinfo = *it;
		Msg("[%s] state[%s]", iinfo->m_name_sect.c_str(), iinfo->GetStateAsText());
	}
	Msg("------");
}

void CUIMpTradeWnd::DumpPreset(ETradePreset idx)
{
	const preset_items&		v			=  GetPreset(idx);
	preset_items::const_iterator it		= v.begin();
	preset_items::const_iterator it_e	= v.end();

	Msg("dump preset [%d]", idx);
	for(;it!=it_e;++it)
	{
		const _preset_item& _one		= *it;

		Msg("[%s]-[%d]", _one.sect_name.c_str(), _one.count);
	}
}

extern LPCSTR _list_names[];

void CItemMgr::Load(const shared_str& sect_cost)
{
	CInifile::Sect &sect = pSettings->r_section(sect_cost);

	u32 idx	=0;
	for (CInifile::SectIt it = sect.begin(); it != sect.end(); ++it,++idx)
	{
		_i&		val			= m_items[it->first]; 
		val.slot_idx		= 0xff;
		int c = sscanf		(it->second.c_str(),"%d,%d,%d,%d,%d",&val.cost[0],&val.cost[1],&val.cost[2],&val.cost[3],&val.cost[4]);
		VERIFY				(c>0);

		while(c<_RANK_COUNT)
		{
			val.cost[c]	= val.cost[c-1];
			++c;
		}
	}
	
	for(u8 i=CUIMpTradeWnd::e_first; i<CUIMpTradeWnd::e_total_lists; ++i)
	{
		const shared_str& buff		= pSettings->r_string("buy_menu_items_place", _list_names[i]);
		
		u32 cnt						= _GetItemCount(buff.c_str());
		string1024					_one;

		for(u32 c=0; c<cnt; ++c)
		{
			_GetItem				(buff.c_str(), c, _one);
			shared_str _one_str		= _one;
			COST_MAP_IT it			= m_items.find(_one_str);
			R_ASSERT				(it!=m_items.end());
			R_ASSERT3				(it->second.slot_idx==0xff,"item has duplicate record in [buy_menu_items_place] section ",_one);
			it->second.slot_idx		= i;
		}
	}
}

const u32	CItemMgr::GetItemCost(const shared_str& sect_name, u32 rank) const
{
	COST_MAP_CIT it		= m_items.find(sect_name);
	VERIFY				(it!=m_items.end());
	return				it->second.cost[rank];
}

const u8 CItemMgr::GetItemSlotIdx	(const shared_str& sect_name) const
{
	COST_MAP_CIT it		= m_items.find(sect_name);
	VERIFY				(it!=m_items.end());
	return				it->second.slot_idx;
}

const u32 CItemMgr::GetItemIdx(const shared_str& sect_name) const
{
	COST_MAP_CIT it		= m_items.find(sect_name);
	
	if		(it==m_items.end())
	{
		Msg("item not found in registry [%s]", sect_name.c_str());
		return u32(-1);
	}

	return				u32( std::distance(m_items.begin(), it) );
}

void CItemMgr::Dump() const
{
	COST_MAP_CIT it		= m_items.begin();
	COST_MAP_CIT it_e	= m_items.end();

	Msg("--CItemMgr::Dump");
	for(;it!=it_e;++it)
	{
		const _i&		val		= it->second; 
		R_ASSERT3		(it->second.slot_idx!=0xff,"item has no record in [buy_menu_items_place] section ",it->first.c_str());
		Msg				("[%s] slot=[%d] cost= %d,%d,%d,%d,%d",it->first.c_str(),val.slot_idx,val.cost[0],val.cost[1],val.cost[2],val.cost[3],val.cost[4]);
	}

}

const u32	CItemMgr::GetItemsCount	() const
{
	return m_items.size();
}

const shared_str&	CItemMgr::GetItemName		(u32 Idx) const
{
	R_ASSERT(Idx<m_items.size());
	return (m_items.begin()+Idx)->first;
}