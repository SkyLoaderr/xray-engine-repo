#include "stdafx.h"
#include "UIMpTradeWnd.h"
#include "UIMpItemsStoreWnd.h"
#include "UITabControl.h"
#include "UIStatic.h"
#include "UICellItem.h"
#include "UIDragDropListEx.h"
#include "../inventory_item.h"
#include "../PhysicsShellHolder.h"
#include "restrictions.h"
#include "../object_broker.h"

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
			VERIFY2			(s==e_own,"incorrect SBuyItemInfo::SetState sequence");
			m_item_state	= s;
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
	m_static_information->ResetAnimation	();
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
	if(m_pCurrentCellItem == itm)	return;
	m_pCurrentCellItem				= itm;

	if (m_pCurrentCellItem)
	{
		const shared_str& current_sect_name = CurrentIItem()->object().cNameSect();

		string256						str;
		sprintf							(str, "%s [%d RU]", current_sect_name.c_str()/*CurrentIItem()->NameShort()*/, GetItemPrice(CurrentIItem()));
		m_static_current_item->SetText	(str);

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
		m_static_current_item->SetText	("[no item selected]");
		m_static_rank->TextureOff		();
	}
}


int	CUIMpTradeWnd::GetItemPrice(CInventoryItem* itm)
{
	return m_item_mngr->GetItemCost(itm->object().cNameSect(), g_mp_restrictions.GetRank());
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

SBuyItemInfo* CUIMpTradeWnd::CreateItem(const shared_str& name_sect, SBuyItemInfo::EItmState type)
{
	SBuyItemInfo* iinfo			= FindItem(name_sect, type);
	if(iinfo)
		return					iinfo;
	iinfo						= xr_new<SBuyItemInfo>();
	m_all_items.push_back		( iinfo );
	iinfo->m_name_sect			= name_sect;
	iinfo->SetState				(type);
	iinfo->m_cell_item			= create_cell_item(CreateItem_internal(name_sect));
	return						iinfo;
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
	return							false;
}

bool CUIMpTradeWnd::OnItemStartDrag(CUICellItem* itm)
{
	return							false;
}

bool CUIMpTradeWnd::OnItemDbClick(CUICellItem* itm)
{
	CUIDragDropListEx*	_owner		= itm->OwnerList();
	dd_list_type		_type		= GetListType	(_owner);

	switch(_type)
	{
		case dd_shop:
			TryToBuyItem			(itm);
			break;

		case dd_own_bag:
		case dd_own_slot:
			TryToSellItem			(itm);
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

bool CUIMpTradeWnd::TryToSellItem(CUICellItem* itm)
{
	SBuyItemInfo* iinfo 				= FindItem(itm);
	u32		_item_cost					= m_item_mngr->GetItemCost(iinfo->m_name_sect, GetRank() );
	
	SetMoneyAmount						(GetMoneyAmount() + _item_cost);

	CUICellItem* _itm					= itm->OwnerList()->RemoveItem(itm, false );

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
		}
	}
	string64					buff;
	sprintf						(buff,"+%d RU",_item_cost);
	SetInfoString				(buff);

	return true;
}

bool CUIMpTradeWnd::TryToBuyItem(CUICellItem* itm)
{
	SBuyItemInfo* iinfo 		= FindItem(itm);
	
	bool	b_can_buy			= CheckBuyPossibility(iinfo->m_name_sect);
	
	if(!b_can_buy)
		return					false;
	
	u32 _item_cost				= m_item_mngr->GetItemCost(iinfo->m_name_sect, GetRank() );
	SetMoneyAmount				(GetMoneyAmount() - _item_cost);
	iinfo->SetState				(SBuyItemInfo::e_bought);

	CUICellItem* i				= itm->OwnerList()->RemoveItem(itm, false );

	CUIDragDropListEx*_new_owner = NULL;
	_new_owner					= GetMatchedListForItem(iinfo->m_name_sect);
	_new_owner->SetItem			(i);

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
		Msg						("[%s] message [%s]",sect_name.c_str(), info_buffer);
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
	CUIDragDropListEx*	pList		= m_list[e_shop];

	SBuyItemInfo* pitem				= CreateItem(sect_name, SBuyItemInfo::e_shop);
	pList->SetItem					(pitem->m_cell_item);
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
	return _fake;
}


const u8 CUIMpTradeWnd::GetWeaponIndexInBelt(u32 indexInBelt, u8 &sectionId, u8 &itemId, u8 &count)
{
	return 0;
}

void CUIMpTradeWnd::GetWeaponIndexByName(const shared_str& sectionName, u8 &grpNum, u8 &idx)
{}

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
{}

bool CUIMpTradeWnd::CanBuyAllItems()
{
	return true;
}

void CUIMpTradeWnd::AddonToSlot(int add_on, int slot, bool bRealRepresentationSet)
{}

void CUIMpTradeWnd::SectionToSlot(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet)
{}

bool CUIMpTradeWnd::CheckBuyAvailabilityInSlots()
{
	return true;
}
