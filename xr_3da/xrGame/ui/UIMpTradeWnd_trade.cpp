#include "stdafx.h"
#include "UIMpTradeWnd.h"
#include "UIMpItemsStoreWnd.h"

#include "UICellItem.h"
#include "UIDragDropListEx.h"
#include "UICellCustomItems.h"
#include <dinput.h>

bool CUIMpTradeWnd::TryToSellItem(SBuyItemInfo* sell_itm)
{
	SellItemAddons						(sell_itm, at_scope);
	SellItemAddons						(sell_itm, at_silencer);
	SellItemAddons						(sell_itm, at_glauncher);

	u32		_item_cost					= m_item_mngr->GetItemCost(sell_itm->m_name_sect, GetRank() );
	
	SetMoneyAmount						(GetMoneyAmount() + _item_cost);

	CUICellItem* _itm					= NULL;

	if(sell_itm->m_cell_item->OwnerList())
		_itm = sell_itm->m_cell_item->OwnerList()->RemoveItem(sell_itm->m_cell_item, false );
	else
		_itm	= sell_itm->m_cell_item;

	SBuyItemInfo* iinfo					= FindItem(_itm); //just detached

	u32 cnt_in_shop						= GetItemCount(sell_itm->m_name_sect, SBuyItemInfo::e_shop);

	if(cnt_in_shop!=0)
	{
		DestroyItem						(iinfo);
	}else
	{//return to shop
		iinfo->SetState					(SBuyItemInfo::e_sold);

		if(m_store_hierarchy->CurrentLevel().HasItem(iinfo->m_name_sect) )
		{
			CUIDragDropListEx* _new_owner		= m_list[e_shop];
			_new_owner->SetItem					(iinfo->m_cell_item);
			int accel_idx						= m_store_hierarchy->CurrentLevel().GetItemIdx(iinfo->m_name_sect);
			VERIFY								(accel_idx!=-1);
			iinfo->m_cell_item->SetAccelerator	( (accel_idx>10) ? 0 : DIK_1+accel_idx );
			iinfo->m_cell_item->SetCustomDraw	(xr_new<CUICellItemAccelDraw>());

		}
	}
	if(_item_cost!=0)
	{
		string64							buff;
		sprintf								(buff,"+%d RU",_item_cost);
		SetInfoString						(buff);
	}
	return true;
}

bool CUIMpTradeWnd::TryToBuyItem(SBuyItemInfo* buy_itm, bool own_item)
{
	SBuyItemInfo* iinfo 			= buy_itm;
	const shared_str& buy_item_name = iinfo->m_name_sect;
	if(!own_item)
	{
		bool	b_can_buy		= CheckBuyPossibility(buy_item_name);
		if(!b_can_buy)
			return				false;
	}

	u32 _item_cost				= m_item_mngr->GetItemCost(buy_item_name, GetRank() );

	if(!own_item)
	{
		SetMoneyAmount				(GetMoneyAmount() - _item_cost);
		iinfo->SetState				(SBuyItemInfo::e_bought);
	}else
		iinfo->SetState				(SBuyItemInfo::e_own);


	CUICellItem* cell_itm				= NULL;
	if(iinfo->m_cell_item->OwnerList())// just from shop
		cell_itm					= iinfo->m_cell_item->OwnerList()->RemoveItem(iinfo->m_cell_item, false );
	else //new created
		cell_itm					= iinfo->m_cell_item;


	bool b_addon					= TryToAttachItemAsAddon(iinfo);
	if(!b_addon)
	{
		CUIDragDropListEx*_new_owner = NULL;
		_new_owner					= GetMatchedListForItem(buy_item_name);
		_new_owner->SetItem			(cell_itm);
		cell_itm->SetCustomDraw		(NULL);
		cell_itm->SetAccelerator	(0);
	}else
		DestroyItem					(iinfo);

	RenewShopItem					(buy_item_name, true);

	if(!own_item && _item_cost!=0)
	{
		string64					buff;
		sprintf						(buff,"-%d RU",_item_cost);
		SetInfoString				(buff);
	}
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
		pitem->m_cell_item->SetAccelerator	( (accel_idx>9) ? 0 : DIK_1+accel_idx );

		pitem->m_cell_item->SetCustomDraw	(xr_new<CUICellItemAccelDraw>());
		pList->SetItem						(pitem->m_cell_item);
	}
}

void CUIMpTradeWnd::ItemToBelt(const shared_str& sectionName)
{
	VERIFY(m_item_mngr->GetItemIdx(sectionName)!=u32(-1) );

	CUIDragDropListEx*	pList			= GetMatchedListForItem(sectionName);

	SBuyItemInfo* pitem					= CreateItem(sectionName, SBuyItemInfo::e_own, false);
	pList->SetItem						(pitem->m_cell_item);
}

void CUIMpTradeWnd::ItemToRuck(const shared_str& sectionName, u8 addons)
{
	VERIFY(m_item_mngr->GetItemIdx(sectionName)!=u32(-1) );

	CUIDragDropListEx*	pList			= GetMatchedListForItem(sectionName);

	SBuyItemInfo* pitem					= CreateItem(sectionName, SBuyItemInfo::e_own, false);
	pList->SetItem						(pitem->m_cell_item);

	SetItemAddonsState_ext				(pitem, addons);
}

void CUIMpTradeWnd::ItemToSlot(const shared_str& sectionName, u8 addons)
{
	VERIFY(m_item_mngr->GetItemIdx(sectionName)!=u32(-1) );

	CUIDragDropListEx*	pList			= GetMatchedListForItem(sectionName);

	SBuyItemInfo* pitem					= CreateItem(sectionName, SBuyItemInfo::e_own, false);
	pList->SetItem						(pitem->m_cell_item);
	SetItemAddonsState_ext				(pitem, addons);
}
