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

SBuyItemInfo::~SBuyItemInfo()
{
	CInventoryItem*			iitem = (CInventoryItem*)m_cell_item->m_pData;
	xrFactory_Destroy		(&iitem->object());
	delete_data				(m_cell_item);
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
		sprintf							(str, "%s [%d RU]", CurrentIItem()->NameShort(), GetItemPrice(CurrentIItem()));
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
	return m_cost_mngr->GetItemCost(itm->object().cNameSect(), g_mp_restrictions.GetRank());
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
	ITEMS_vec_cit it = m_all_items.begin();
	ITEMS_vec_cit it_e = m_all_items.end();
	for(;it!=it_e;++it)
	{
		SBuyItemInfo* pitm = *it;
		if(pitm->m_name_sect==name_sect && pitm->m_item_state==type)
			return pitm;
	}
	
	m_all_items.push_back		(xr_new<SBuyItemInfo>() );
	SBuyItemInfo* pitm			= m_all_items.back	();
	pitm->m_name_sect			= name_sect;
	pitm->m_item_state			= type;
	pitm->m_cell_item			= create_cell_item(CreateItem_internal(name_sect));
	return						pitm;
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
	return							false;
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
