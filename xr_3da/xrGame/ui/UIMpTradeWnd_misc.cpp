#include "stdafx.h"
#include "UIMpTradeWnd.h"
#include "UIMpItemsStoreWnd.h"

#include "UICellItem.h"
#include "UITabControl.h"
#include "UIDragDropListEx.h"
#include "UIItemInfo.h"

#include "../inventory_item.h"
#include "../PhysicsShellHolder.h"
#include "../object_broker.h"


bool CUIMpTradeWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if(!m_store_hierarchy->CurrentIsRoot())
	{
		if (m_shop_wnd->OnKeyboard(dik, keyboard_action) )
			return true;

		m_root_tab_control->SetAcceleratorsMode		(false);
	}

	bool res =  inherited::OnKeyboard(dik, keyboard_action);

	m_root_tab_control->SetAcceleratorsMode		(true);

	return			res;
}

void CUIMpTradeWnd::Update()
{
	inherited::Update			();
	UpdateMomeyIndicator		();
}

void CUIMpTradeWnd::UpdateMomeyIndicator()
{
	string128					buff;
	sprintf						(buff, "%d", m_money);
	m_static_money->SetText		(buff);
}

void CUIMpTradeWnd::SetInfoString(LPCSTR str)
{
	m_static_information->SetText			(str);
	m_static_information->ResetClrAnimation	();
	Msg("Buy menu message:%s", str);
}

void CUIMpTradeWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent		(pWnd, msg, pData);
}

CUICellItem* CUIMpTradeWnd::CurrentItem()
{
	return		m_pCurrentCellItem;
}

CInventoryItem* CUIMpTradeWnd::CurrentIItem()
{
	return	(m_pCurrentCellItem)?(CInventoryItem*)m_pCurrentCellItem->m_pData : NULL;
}

void CUIMpTradeWnd::SetCurrentItem(CUICellItem* itm)
{
	if(m_pCurrentCellItem == itm)		return;
	m_pCurrentCellItem					= itm;
	m_item_info->InitItem				(CurrentIItem());
	if (m_pCurrentCellItem)
	{
		const shared_str& current_sect_name = CurrentIItem()->object().cNameSect();
		/*
		string256						str;
		sprintf							(str, "%s [%d RU]", current_sect_name.c_str(), GetItemPrice(CurrentIItem()));
		m_static_current_item->SetText	(str);
		*/
		string256						str;
		sprintf							(str, "%d", GetItemPrice(CurrentIItem()));
		m_item_info->UICost->SetText	(str);

		m_item_info->UIName->SetText	(CurrentIItem()->NameShort());

		string64						tex_name;
		string64						team;

		if (true /*m_bag.IsBlueTeamItem(itm)*/ )
			strcpy						(team, "blue");
		else 
			strcpy						(team, "green");

		sprintf							(tex_name, "ui_hud_status_%s_0%d", team, 1+get_rank(current_sect_name.c_str()) );
				
		m_static_rank->InitTexture		(tex_name);
		m_static_rank->TextureOn		();
	}
	else
	{
//.		m_static_current_item->SetText	("[no item selected]");
		m_static_rank->TextureOff		();
	}
}


int	CUIMpTradeWnd::GetItemPrice(CInventoryItem* itm)
{
	return m_item_mngr->GetItemCost(itm->object().cNameSect(), g_mp_restrictions.GetRank());
}

void CUIMpTradeWnd::BindDragDropListEvents(CUIDragDropListEx* lst, bool bDrag)
{
	lst->m_f_item_drop				= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIMpTradeWnd::OnItemDrop);
	lst->m_f_item_db_click			= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIMpTradeWnd::OnItemDbClick);
	lst->m_f_item_selected			= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIMpTradeWnd::OnItemSelected);
	lst->m_f_item_rbutton_click		= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIMpTradeWnd::OnItemRButtonClick);
	if(bDrag)
		lst->m_f_item_start_drag	= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIMpTradeWnd::OnItemStartDrag);

}

bool CUIMpTradeWnd::OnItemDrop(CUICellItem* itm)
{
	CUIDragDropListEx*	_owner_list		= itm->OwnerList();
	dd_list_type		_owner_type		= GetListType	(_owner_list);

	CUIDragDropListEx*	_new_owner		= CUIDragDropListEx::m_drag_item->BackList();
	
	if(!_new_owner || _new_owner==_owner_list)		
		return			true;

	dd_list_type		_new_owner_type	= GetListType	(_new_owner);

	SBuyItemInfo*		iinfo 		= FindItem(itm);

	if(_owner_type==dd_shop)
	{
		bool res		= TryToBuyItem			(iinfo, false, NULL);
		VERIFY			(res);
		return			true;
	}

	if(_new_owner_type==dd_shop)
	{
		bool res		= TryToSellItem			(iinfo);
		VERIFY			(res);
		return			true;
	}
	
	if(_new_owner_type==dd_own_bag)
	{
		CUICellItem* citm				= _owner_list->RemoveItem(itm, false);
		_new_owner->SetItem				(citm);
		UpdateCorrespondingItemsForList	(_owner_list);
	}else
	{
		VERIFY(_new_owner_type==dd_own_slot);
		if( _new_owner==GetMatchedListForItem(iinfo->m_name_sect) )
		{
			CUICellItem* citm				= _owner_list->RemoveItem(itm, false);
			_new_owner->SetItem				(citm);
			UpdateCorrespondingItemsForList	(_new_owner);
		}
	}	
	return							true;
}

bool CUIMpTradeWnd::OnItemStartDrag(CUICellItem* itm)
{
	return							false;
}

bool CUIMpTradeWnd::OnItemDbClick(CUICellItem* itm)
{
	CUIDragDropListEx*	_owner_list		= itm->OwnerList();
	dd_list_type		_owner_type		= GetListType	(_owner_list);

	SBuyItemInfo*		iinfo 		= FindItem(itm);

	switch(_owner_type)
	{
		case dd_shop:
			TryToBuyItem			(iinfo, false, NULL);
			break;

		case dd_own_bag:
		case dd_own_slot:
			TryToSellItem			(iinfo);
			break;
		default:					NODEFAULT;
	};

	return							true;
}

bool CUIMpTradeWnd::OnItemSelected(CUICellItem* itm)
{
	SetCurrentItem					(itm);
	return							false;
}

bool CUIMpTradeWnd::OnItemRButtonClick(CUICellItem* itm)
{
	return							false;
}


CUIMpTradeWnd::dd_list_type CUIMpTradeWnd::GetListType(CUIDragDropListEx* l)
{
	if(l==m_list[e_shop])		return dd_shop;
	if(l==m_list[e_player_bag])	return dd_own_bag;
	
	return						dd_own_slot;
}

CUIDragDropListEx*	CUIMpTradeWnd::GetMatchedListForItem(const shared_str& sect_name)
{
	CUIDragDropListEx*	res		= NULL;
	u8 list_idx					= m_item_mngr->GetItemSlotIdx(sect_name);
	VERIFY						(list_idx<e_total_lists && list_idx!=e_shop);
	res							= m_list[list_idx];

	//special case
	if(list_idx==e_pistol_ammo || list_idx==e_rifle_ammo)
	{
		CUICellItem* ci = (m_list[list_idx-1]->ItemsCount())?m_list[list_idx-1]->GetItemIdx(0):NULL;
		if(!ci)
			return				m_list[e_player_bag];

		CInventoryItem* ii = (CInventoryItem*)ci->m_pData;

		if(!ii->IsNecessaryItem(sect_name))
			return				m_list[e_player_bag];
	}
	return						res;
}


const u32 CUIMpTradeWnd::GetRank() const
{
	return g_mp_restrictions.GetRank();
}


/// iBuyWnd
const u8 CUIMpTradeWnd::GetWeaponIndex(u32 slotNum)
{
	return 0;
}

shared_str _fake;
const shared_str& CUIMpTradeWnd::GetWeaponNameByIndex(u8 grpNum, u8 idx)
{
//	return _fake;
	if (idx >= m_item_mngr->GetItemsCount()) return _fake;
	return m_item_mngr->GetItemName(u32(idx));
}


const u8 CUIMpTradeWnd::GetWeaponIndexInBelt(u32 indexInBelt, u8 &sectionId, u8 &itemId, u8 &count)
{
	return 0;
}

void CUIMpTradeWnd::GetWeaponIndexByName(const shared_str& sectionName, u8 &grpNum, u8 &idx)
{
	
	u32 idx__	= m_item_mngr->GetItemIdx(sectionName);
	grpNum		= 0;
	idx			= (u8)idx__;
}

const u8 CUIMpTradeWnd::GetItemIndex(u32 slotNum, u32 idx, u8 &sectionNum)
{
	return 0;
}

const u8 CUIMpTradeWnd::GetBeltSize()
{
	return 0;
}

void CUIMpTradeWnd::ClearSlots()
{}

void CUIMpTradeWnd::ClearRealRepresentationFlags()
{}

void CUIMpTradeWnd::IgnoreMoneyAndRank(bool ignore)
{}

void CUIMpTradeWnd::IgnoreMoney(bool ignore)
{}

u32 CUIMpTradeWnd::GetMoneyAmount() const
{
	return			m_money;
}

void CUIMpTradeWnd::SetSkin(u8 SkinID)
{}

void CUIMpTradeWnd::SetRank(u32 rank)
{
	g_mp_restrictions.SetRank(rank);
}


void CUIMpTradeWnd::SetMoneyAmount(u32 money)
{
	VERIFY			(money>=0);
	m_money			= money;
}

void CUIMpTradeWnd::ResetItems()
{
	ResetToOrigin						();
	m_store_hierarchy->Reset			();
	UpdateShop							();
	SetCurrentItem						(NULL);
}

bool CUIMpTradeWnd::CanBuyAllItems()
{
	return true;
}

void CUIMpTradeWnd::AddonToSlot(int add_on, int slot, bool bRealRepresentationSet)
{
// own

}

void CUIMpTradeWnd::SectionToSlot(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet)
{
// own
}

bool CUIMpTradeWnd::CheckBuyAvailabilityInSlots()
{
	return true;
}
