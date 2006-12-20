#include "stdafx.h"
#include "UIMpTradeWnd.h"
#include "UIDragDropListEx.h"
#include "UICellItem.h"
#include "../weaponmagazinedwgrenade.h"
#include "../../xr_input.h"

void CUIMpTradeWnd::OnBtnPistolAmmoClicked(CUIWindow* w, void* d)
{
	CUIDragDropListEx*	res		= m_list[e_pistol];
	CUICellItem* ci				= (res->ItemsCount())?res->GetItemIdx(0):NULL;
	if(!ci)	
		return;

	CInventoryItem* ii			= (CInventoryItem*)ci->m_pData;
	CWeapon*		wpn			= smart_cast<CWeapon*>(ii);
	R_ASSERT		(wpn);

	u32 ammo_idx				= (pInput->iGetAsyncKeyState(DIK_LSHIFT))?1:0;
	
	const shared_str& ammo_name	= wpn->m_ammoTypes[ammo_idx];

	SBuyItemInfo* pitem			= CreateItem		(ammo_name, SBuyItemInfo::e_undefined, false);
	bool b_res					= TryToBuyItem		(pitem, false );
	if(!b_res)
		DestroyItem				(pitem);
}

void CUIMpTradeWnd::OnBtnPistolSilencerClicked(CUIWindow* w, void* d)
{}

void CUIMpTradeWnd::OnBtnRifleAmmoClicked(CUIWindow* w, void* d)
{
	CUIDragDropListEx*	res		= m_list[e_rifle];
	CUICellItem* ci				= (res->ItemsCount())?res->GetItemIdx(0):NULL;
	if(!ci)	
		return;

	CInventoryItem* ii			= (CInventoryItem*)ci->m_pData;
	CWeapon*		wpn			= smart_cast<CWeapon*>(ii);
	R_ASSERT		(wpn);

	u32 ammo_idx				= (pInput->iGetAsyncKeyState(DIK_LSHIFT))?1:0;
	
	const shared_str& ammo_name	= wpn->m_ammoTypes[ammo_idx];

	SBuyItemInfo* pitem			= CreateItem		(ammo_name, SBuyItemInfo::e_undefined, false);
	bool b_res					= TryToBuyItem		(pitem, false );
	if(!b_res)
		DestroyItem				(pitem);
}

void CUIMpTradeWnd::OnBtnRifleSilencerClicked(CUIWindow* w, void* d)
{}

void CUIMpTradeWnd::OnBtnRifleScopeClicked(CUIWindow* w, void* d)
{}

void CUIMpTradeWnd::OnBtnRifleGLClicked(CUIWindow* w, void* d)
{}

void CUIMpTradeWnd::OnBtnRifleAmmo2Clicked(CUIWindow* w, void* d)
{
	CUIDragDropListEx*	res		= m_list[e_rifle];
	CUICellItem* ci				= (res->ItemsCount())?res->GetItemIdx(0):NULL;
	if(!ci)	
		return;

	CInventoryItem* ii			= (CInventoryItem*)ci->m_pData;
	CWeaponMagazinedWGrenade* wpn = smart_cast<CWeaponMagazinedWGrenade*>(ii);
	R_ASSERT					(wpn);

	u32 ammo_idx				= 0;
	
	const shared_str& ammo_name	= wpn->m_ammoTypes2[ammo_idx];

	SBuyItemInfo* pitem			= CreateItem		(ammo_name, SBuyItemInfo::e_undefined, false);
	bool b_res					= TryToBuyItem		(pitem, false );
	if(!b_res)
		DestroyItem				(pitem);
}

bool CUIMpTradeWnd::TryToAttachItemAsAddon(SBuyItemInfo* buy_itm)
{
	bool	b_res						= false;
	for(u32 i=0; i<2;++i)
	{
		u32 list_idx					= (i==0) ? e_rifle : e_pistol;
		CUIDragDropListEx*	_list		= m_list[list_idx];
		
		VERIFY							(_list->ItemsCount() <= 1);

		CUICellItem* ci					= (_list->ItemsCount()) ? _list->GetItemIdx(0) : NULL;
		if(!ci)	
			return	false;

		CInventoryItem* item_to_upgrade	= (CInventoryItem*)ci->m_pData;
		CInventoryItem* item_to_attach	= (CInventoryItem*)buy_itm->m_cell_item->m_pData;
		
		if(item_to_upgrade->CanAttach	(item_to_attach))
		{
			item_to_upgrade->Attach		(item_to_attach, false);
			b_res = true;
			break;
		}

	}		

	return				b_res;
}

void CUIMpTradeWnd::SellItemAddons(SBuyItemInfo* sell_itm, u8 addon_id)
{
	CInventoryItem* item_	= (CInventoryItem*)sell_itm->m_cell_item->m_pData;
	CWeapon* w				= smart_cast<CWeapon*>(item_);
	if(!w)					return; //ammo,medkit etc.

	if(addon_id==0 && w->ScopeAttachable() && w->IsScopeAttached())
	{
		const shared_str& _name = w->GetScopeName();
		u32 _item_cost			= m_item_mngr->GetItemCost(_name, GetRank() );
		SetMoneyAmount			(GetMoneyAmount() + _item_cost);
		
		w->Detach				(_name.c_str(), false);
	};

	if(addon_id==1 && w->SilencerAttachable() && w->IsSilencerAttached())
	{
		const shared_str& _name = w->GetSilencerName();
		u32 _item_cost			= m_item_mngr->GetItemCost(_name, GetRank() );
		SetMoneyAmount			(GetMoneyAmount() + _item_cost);

		w->Detach				(_name.c_str(), false);
	};

	if(addon_id==2 && w->GrenadeLauncherAttachable() && w->IsGrenadeLauncherAttached())
	{
		const shared_str& _name = w->GetGrenadeLauncherName();
		u32 _item_cost			= m_item_mngr->GetItemCost(_name, GetRank() );
		SetMoneyAmount			(GetMoneyAmount() + _item_cost);
	
		w->Detach				(_name.c_str(), false);
	}
}
