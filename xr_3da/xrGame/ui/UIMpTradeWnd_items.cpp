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

struct eq_name_state_comparer
{
	shared_str				m_name;
	SBuyItemInfo::EItmState m_state;
	eq_name_state_comparer	(const shared_str& _name, SBuyItemInfo::EItmState _state):m_name(_name),m_state(_state){}
	bool	operator	() (SBuyItemInfo* info)
	{
		return (info->m_name_sect==m_name)&&(info->GetState()==m_state);
	}
};

struct eq_name_state_addon_comparer
{
	shared_str				m_name;
	SBuyItemInfo::EItmState m_state;
	u8						m_addons;

	eq_name_state_addon_comparer	(const shared_str& _name, SBuyItemInfo::EItmState _state, u8 ad):m_name(_name),m_state(_state),m_addons(ad){}
	bool	operator	() (SBuyItemInfo* info)
	{
		if( (info->m_name_sect==m_name)&&(info->GetState()==m_state) )
		{
			return GetItemAddonsState_ext(info) == m_addons;
		}else
			return	false;
	}
};

struct eq_group_state_comparer
{
	shared_str				m_group;
	SBuyItemInfo::EItmState m_state;

	eq_group_state_comparer	(const shared_str& _group, SBuyItemInfo::EItmState _state):m_group(_group),m_state(_state){}
	bool	operator	() (SBuyItemInfo* info)
	{
		if((info->GetState()==m_state))
		{
			const shared_str& _grp = g_mp_restrictions.GetItemGroup(info->m_name_sect);
			return			(_grp==m_group);
		}else
			return			false;
	}
};

struct eq_state_comparer
{
	SBuyItemInfo::EItmState m_state;
	eq_state_comparer	(SBuyItemInfo::EItmState _state):m_state(_state){}
	bool	operator	() (SBuyItemInfo* info)
	{
		return (info->GetState()==m_state);
	}
};

u32 CUIMpTradeWnd::GetItemCount(SBuyItemInfo::EItmState state) const
{
	return	(u32)count_if(m_all_items.begin(), m_all_items.end(),eq_state_comparer(state));
/*
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
*/
}

u32 CUIMpTradeWnd::GetItemCount(const shared_str& name_sect, SBuyItemInfo::EItmState state) const
{
	return	(u32)count_if(m_all_items.begin(), m_all_items.end(),eq_name_state_comparer(name_sect,state));
/*
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
*/
}
u32 CUIMpTradeWnd::GetItemCount(const shared_str& name_sect, SBuyItemInfo::EItmState state, u8 addon) const
{
	return	(u32)count_if(m_all_items.begin(), m_all_items.end(),eq_name_state_addon_comparer(name_sect,state,addon));
}

u32 CUIMpTradeWnd::GetGroupCount(const shared_str& name_group, SBuyItemInfo::EItmState state) const
{
	return	(u32)count_if(m_all_items.begin(), m_all_items.end(),eq_group_state_comparer(name_group,state));
/*
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
*/
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
struct preset_eq {
	shared_str		m_name;
	u8				m_addon;
	preset_eq(const shared_str& _name, u8 ad):m_name(_name),m_addon(ad){};
	bool operator() (const _preset_item& pitem)
	{
		return (pitem.sect_name==m_name)&&(pitem.addon_state==m_addon);
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
		if(	!(iinfo->GetState()==SBuyItemInfo::e_bought || iinfo->GetState()==SBuyItemInfo::e_own)	)
		continue;

		u8 addon_state				= GetItemAddonsState_ext(iinfo);

		preset_items::iterator fit	= std::find_if(v.begin(), v.end(), preset_eq(iinfo->m_name_sect, addon_state) );
		if(fit!=v.end())
			continue;

		u32 cnt						= GetItemCount(iinfo->m_name_sect, SBuyItemInfo::e_bought, addon_state);
		cnt							+=GetItemCount(iinfo->m_name_sect, SBuyItemInfo::e_own, addon_state);
		if(0==cnt)				
			continue;

		v.resize					(v.size()+1);
		_preset_item& _one			= v.back();
		_one.sect_name				= iinfo->m_name_sect;
		_one.count					= cnt;
		_one.addon_state			= addon_state;
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
			SetItemAddonsState_ext			(pitem, _one.addon_state);

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

struct items_sorter {
	items_sorter(){};
	bool operator() (SBuyItemInfo* i1, SBuyItemInfo* i2)
	{
		if(i1->m_name_sect == i2->m_name_sect)
			return i1->GetState()<i2->GetState();

		return		i1->m_name_sect < i2->m_name_sect;
	};
};

void CUIMpTradeWnd::DumpAllItems(LPCSTR s)
{
	std::sort		(m_all_items.begin(), m_all_items.end(), items_sorter());

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

