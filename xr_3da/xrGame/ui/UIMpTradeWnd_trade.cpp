#include "stdafx.h"
#include "UIMpTradeWnd.h"
#include "UIMpItemsStoreWnd.h"

#include "UICellItem.h"
#include "UIDragDropListEx.h"
#include <dinput.h>

bool CUIMpTradeWnd::TryToSellItem(SBuyItemInfo* buy_itm)
{
	SBuyItemInfo* iinfo 				= buy_itm;
	u32		_item_cost					= m_item_mngr->GetItemCost(iinfo->m_name_sect, GetRank() );
	
	SetMoneyAmount						(GetMoneyAmount() + _item_cost);

	CUICellItem* _itm					= iinfo->m_cell_item->OwnerList()->RemoveItem(iinfo->m_cell_item, false );

	SBuyItemInfo* _in_shop				= FindItem(iinfo->m_name_sect, SBuyItemInfo::e_shop);
	if(_in_shop)
	{
		SBuyItemInfo* _to_del			= FindItem(_itm);
		DestroyItem						(_to_del);
	}else
	{//return to shop
		iinfo->SetState					(SBuyItemInfo::e_sold);

		if(m_store_hierarchy->CurrentLevel().HasItem(iinfo->m_name_sect) )
		{
			CUIDragDropListEx* _new_owner	= m_list[e_shop];
			_new_owner->SetItem				(_itm);
			int accel_idx					= m_store_hierarchy->CurrentLevel().GetItemIdx(iinfo->m_name_sect);
			VERIFY							(accel_idx!=-1);
			_itm->SetAccelerator			(DIK_1+accel_idx);
		}
	}
	string64							buff;
	sprintf								(buff,"+%d RU",_item_cost);
	SetInfoString						(buff);

	return true;
}

bool CUIMpTradeWnd::TryToBuyItem(SBuyItemInfo* buy_itm)
{
	SBuyItemInfo* iinfo 		= buy_itm;
	
	bool	b_can_buy			= CheckBuyPossibility(iinfo->m_name_sect);
	
	if(!b_can_buy)
		return					false;
	
	u32 _item_cost				= m_item_mngr->GetItemCost(iinfo->m_name_sect, GetRank() );
	SetMoneyAmount				(GetMoneyAmount() - _item_cost);
	iinfo->SetState				(SBuyItemInfo::e_bought);

	CUICellItem* cell_itm				= NULL;
	if(iinfo->m_cell_item->OwnerList())// just from shop
		cell_itm				= iinfo->m_cell_item->OwnerList()->RemoveItem(iinfo->m_cell_item, false );
	else //new created
		cell_itm				= iinfo->m_cell_item;


	CUIDragDropListEx*_new_owner = NULL;
	_new_owner					= GetMatchedListForItem(iinfo->m_name_sect);
	_new_owner->SetItem			(cell_itm);
	cell_itm->SetAccelerator	(0);

	RenewShopItem				(iinfo->m_name_sect, true);

	string64					buff;
	sprintf						(buff,"-%d RU",_item_cost);
	SetInfoString				(buff);

	return						true;
}

bool CUIMpTradeWnd::CheckBuyPossibility(const shared_str& sect_name)
{
	string256					info_buffer;
	bool b_can_buy				= true;

	u32 _item_cost				= m_item_mngr->GetItemCost(sect_name, GetRank() );

	if(GetMoneyAmount() < _item_cost)
	{
		sprintf					(	info_buffer,"Cant buy item. Not enought money. has[%d] need[%d]", 
									GetMoneyAmount(), 
									_item_cost);
		b_can_buy				= false;
	}else
	if(!g_mp_restrictions.IsAvailable(sect_name))
	{
		sprintf					(	info_buffer,"Cant buy item. Rank restrictions. has[%s] need[%s] ", 
									g_mp_restrictions.GetRankName(GetRank()).c_str(), 
									g_mp_restrictions.GetRankName(get_rank(sect_name)).c_str());
		b_can_buy				= false;
	}else
	{
		const shared_str& group = g_mp_restrictions.GetItemGroup(sect_name);
		u32 cnt_restr			= g_mp_restrictions.GetGroupCount(group);
		
		u32 cnt_have			=  GetGroupCount					(group, SBuyItemInfo::e_bought);
			cnt_have			+= GetGroupCount					(group, SBuyItemInfo::e_own);

		if(cnt_have>=cnt_restr)
		{
		sprintf					(	info_buffer,"Cant buy item. Count restrictions. You already have [%d] item of this type", 
									cnt_have);
		b_can_buy				= false;
		}
	}


	if(!b_can_buy)
	{
		//Msg						("[%s] message [%s]",sect_name.c_str(), info_buffer);
		SetInfoString			(info_buffer);
	};
	
	return						b_can_buy;
}

void CUIMpTradeWnd::RenewShopItem(const shared_str& sect_name, bool b_just_bought)
{
/*
	if(b_just_bought)
	{
		VERIFY					(0==GetItemCount(sect_name, SBuyItemInfo::e_shop));

		const shared_str& group = g_mp_restrictions.GetItemGroup(sect_name);
		u32 cnt_can_have		= g_mp_restrictions.GetGroupCount(group);

		u32 cnt_have			=  GetGroupCount					(group, SBuyItemInfo::e_bought);
			cnt_have			+= GetGroupCount					(group, SBuyItemInfo::e_own);

		if(cnt_can_have<=cnt_have)
		{
			Msg("RenewShopItem: there is no need to create item [%s]", sect_name.c_str());
			return;
		}
	}else
	{
	
	}
*/
	if(m_store_hierarchy->CurrentLevel().HasItem(sect_name) )
	{
		CUIDragDropListEx*	pList			= m_list[e_shop];

		SBuyItemInfo* pitem					= CreateItem(sect_name, SBuyItemInfo::e_shop, true);
		int accel_idx						= m_store_hierarchy->CurrentLevel().GetItemIdx(sect_name);
		pitem->m_cell_item->SetAccelerator	( DIK_1 + accel_idx );
		pList->SetItem						(pitem->m_cell_item);
	}
}

