#pragma once
#include "stdafx.h"
#include "UIBuyWnd.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "UICellCustomItems.h"
#include "UIDragDropListEx.h"
#include "../ui_base.h"
#include "../UICursor.h"
#include "../customoutfit.h"
#include "../scope.h"
#include "../silencer.h"
#include "../grenadelauncher.h"
#include "UIOutfitSlot.h"

CUIBuyWnd::CUIBuyWnd(){
	// background textures
	AttachChild(&m_slotsBack);
	AttachChild(&m_back);

	// buttons
	AttachChild(&m_btnOk);
	AttachChild(&m_btnCancel);
	AttachChild(&m_btnClear);

	// controls
	AttachChild(&m_tab);
	AttachChild(&m_bag);
	AttachChild(&m_itemInfo);
	AttachChild(&m_propertiesBox);

	for (int i = 0; i < MP_SLOT_NUM; i++)
	{
		if (MP_SLOT_OUTFIT == i)
			m_list[i] = xr_new<CUIOutfitDragDropList>();
		else
			m_list[i] = xr_new<CUIDragDropListEx>();
        AttachChild(m_list[i]);
	}
}

CUIBuyWnd::~CUIBuyWnd(){

}

void CUIBuyWnd::Init(LPCSTR sectionName, LPCSTR sectionPrice){
	m_sectionName = sectionName;
	m_sectionPrice = sectionPrice;

	CUIXml xml_doc;
	R_ASSERT(xml_doc.Init(CONFIG_PATH, UI_PATH, "inventoryMP.xml"));

	CUIWindow::Init(0,0,UI_BASE_WIDTH,UI_BASE_WIDTH);

	// background textures
	CUIXmlInit::InitStatic(xml_doc, "slots_background", 0, &m_slotsBack);
	CUIXmlInit::InitStatic(xml_doc, "background", 0, &m_back);

	// buttons
	CUIXmlInit::Init3tButton(xml_doc, "ok_button", 0, &m_btnOk);
	CUIXmlInit::Init3tButton(xml_doc, "cancel_button", 0, &m_btnCancel);
	CUIXmlInit::Init3tButton(xml_doc, "clear_button", 0, &m_btnClear);

	// controls
	m_tab.Init(&xml_doc, "tab");
	m_bag.Init(xml_doc, "bag_background_static", sectionName, sectionPrice);
	CUIXmlInit::InitDragDropListEx(xml_doc, "pistol_list",	0, m_list[MP_SLOT_PISTOL]);
	CUIXmlInit::InitDragDropListEx(xml_doc, "rifle_list",	0, m_list[MP_SLOT_RIFLE]);
	CUIXmlInit::InitDragDropListEx(xml_doc, "belt_list",	0, m_list[MP_SLOT_BELT]);
	CUIXmlInit::InitDragDropListEx(xml_doc, "outfit_list",	0, m_list[MP_SLOT_OUTFIT]);

	BindDragDropListEnents(m_list[MP_SLOT_PISTOL]);
	BindDragDropListEnents(m_list[MP_SLOT_RIFLE]);
	BindDragDropListEnents(m_list[MP_SLOT_BELT]);
	BindDragDropListEnents(m_list[MP_SLOT_OUTFIT]);


	CUIXmlInit::InitWindow(xml_doc, "desc_static", 0, &m_itemInfo);
	m_itemInfo.Init("buy_mp_item.xml");
	
}

bool CUIBuyWnd::OnKeyboard(int dik, EUIMessages keyboard_action){
	if (WINDOW_KEY_RELEASED == keyboard_action) 
		return true;

	if (m_propertiesBox.GetVisible())
		m_propertiesBox.OnKeyboard(dik, keyboard_action);

	switch (m_bag.GetMenuLevel())
	{
	case mlRoot:
		if (m_tab.OnKeyboard(dik, keyboard_action))
			return true;
		break;
	default:
		if (m_bag.OnKeyboard(dik, keyboard_action))
			return true;
	}

	return false;
}

void CUIBuyWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	switch (msg)
	{
	case TAB_CHANGED:
        OnTabChange();	break;
	case XR_MENU_LEVEL_CHANGED:
		OnMenuLevelChange(); break;
	}
}

void CUIBuyWnd::OnTabChange(){
	m_bag.ShowSection(m_tab.GetActiveIndex());
}

void CUIBuyWnd::OnMenuLevelChange(){
	if (m_bag.GetMenuLevel() == mlRoot)
		m_tab.SetActiveState();
}

CUICellItem* CUIBuyWnd::CurrentItem()
{
	return m_pCurrentCellItem;
}

CInventoryItem* CUIBuyWnd::CurrentIItem()
{
	return	(m_pCurrentCellItem)?(CInventoryItem*)m_pCurrentCellItem->m_pData : NULL;
}

void CUIBuyWnd::SetCurrentItem(CUICellItem* itm)
{
	if(m_pCurrentCellItem == itm) return;
	m_pCurrentCellItem				= itm;
	m_itemInfo.InitItem			(CurrentIItem());
}

bool CUIBuyWnd::OnItemStartDrag(CUICellItem* itm)
{
	return false; //default behaviour
}

bool CUIBuyWnd::OnItemSelected(CUICellItem* itm)
{
	SetCurrentItem		(itm);
	return				false;
}

void CUIBuyWnd::BindDragDropListEnents(CUIDragDropListEx* lst)
{
	lst->m_f_item_drop				= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIBuyWnd::OnItemDrop);
	lst->m_f_item_start_drag		= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIBuyWnd::OnItemStartDrag);
	lst->m_f_item_db_click			= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIBuyWnd::OnItemDbClick);
	lst->m_f_item_selected			= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIBuyWnd::OnItemSelected);
	lst->m_f_item_rbutton_click		= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIBuyWnd::OnItemRButtonClick);
}

bool CUIBuyWnd::OnItemDrop(CUICellItem* itm)
{
	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	CUIDragDropListEx*	new_owner		= CUIDragDropListEx::m_drag_item->BackList();
	if(old_owner==new_owner || !old_owner || !new_owner)
					return false;

	EListType t_new		= GetType(new_owner);
	EListType t_old		= GetType(old_owner);
	if(t_new == t_old)	return true;

	switch(t_new){
		case iwSlot:{
			if(GetSlotList(GetLocalSlot(CurrentIItem()->GetSlot()))==new_owner)
				ToSlot	(itm, true);
		}break;
		case iwBag:{
			ToBag	(itm, true);
		}break;
		case iwBelt:{
			ToBelt	(itm, true);
		}break;
	};

	return true;
}

CUIDragDropListEx* CUIBuyWnd::GetSlotList(u32 slot_idx)
{
//	if(slot_idx == NO_ACTIVE_SLOT || GetInventory()->m_slots[slot_idx].m_bPersistent)	return NULL;
	switch (slot_idx)
	{
		case MP_SLOT_PISTOL:
			return m_list[MP_SLOT_PISTOL];
			break;

		case MP_SLOT_RIFLE:
			return m_list[MP_SLOT_RIFLE];
			break;

		case MP_SLOT_OUTFIT:
			return m_list[MP_SLOT_OUTFIT];
			break;
		default:
			NODEFAULT;

	};
	return NULL;
}

EListType CUIBuyWnd::GetType(CUIDragDropListEx* l)
{
	if(l==m_list[MP_SLOT_BELT])		
		return iwBelt;
	if(l==m_list[MP_SLOT_PISTOL])
		return iwSlot;
	if(l==m_list[MP_SLOT_RIFLE])
		return iwSlot;
	if(l==m_list[MP_SLOT_OUTFIT])
		return iwSlot;
	if(m_bag.IsChild(l))
		return iwBag;

	NODEFAULT;
	return iwSlot;
}


bool CUIBuyWnd::OnItemDbClick(CUICellItem* itm)
{
	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	EListType t_old						= GetType(old_owner);

	switch(t_old){
		case iwSlot:{
			ToBag	(itm, false);
		}break;

		case iwBag:{
			if(!ToSlot(itm, false)){
				if( !ToBelt(itm, false) )
					ToSlot	(itm, true);
			}
		}break;

		case iwBelt:{
			ToBag	(itm, false);
		}break;
	};

	return true;
}


bool CUIBuyWnd::OnItemRButtonClick(CUICellItem* itm)
{
	SetCurrentItem				(itm);
	ActivatePropertiesBox		();
	return						false;
}

MP_BUY_SLOT CUIBuyWnd::GetLocalSlot(u32 slot){
	switch (slot){
		case PISTOL_SLOT: return MP_SLOT_PISTOL;
		case RIFLE_SLOT:	return MP_SLOT_RIFLE;		
		case OUTFIT_SLOT:	return MP_SLOT_OUTFIT;
		default:			NODEFAULT; return MP_SLOT_BELT;
	}
}

void CUIBuyWnd::ActivatePropertiesBox()
{
	float x,y;
	// Флаг-признак для невлючения пункта контекстного меню: Dreess Outfit, если костюм уже надет
	bool bAlreadyDressed = false; 

	Frect rect = GetAbsoluteRect();
	GetUICursor()->GetPos(x,y);
		
	m_propertiesBox.RemoveAll();
	
	//CEatableItem* pEatableItem			= smart_cast<CEatableItem*>		(CurrentIItem());
	//CCustomOutfit* pOutfit				= smart_cast<CCustomOutfit*>	(CurrentIItem());
	//CArtefact* pArtefact				= smart_cast<CArtefact*>		(CurrentIItem());
	//CWeapon* pWeapon					= smart_cast<CWeapon*>			(CurrentIItem());
	//CScope* pScope						= smart_cast<CScope*>			(CurrentIItem());
	//CSilencer* pSilencer				= smart_cast<CSilencer*>		(CurrentIItem());
	//CGrenadeLauncher* pGrenadeLauncher	= smart_cast<CGrenadeLauncher*>	(CurrentIItem());
    


	//if(CurrentIItem()->GetSlot()!=NO_ACTIVE_SLOT && m_list[GetLocalSlot(CurrentIItem()->GetSlot())].ItemsCount())
	//{
	//	m_propertiesBox.AddItem("st_move_to_slot",  NULL, INVENTORY_TO_SLOT_ACTION);
	//}
	//if(CurrentIItem()->Belt() && CanPutInBelt(CurrentIItem()))
	//{
	//	m_propertiesBox.AddItem("st_move_on_belt",  NULL, INVENTORY_TO_BELT_ACTION);
	//}
	//if(CurrentIItem()->Ruck() && m_pInv->CanPutInRuck(CurrentIItem()))
	//{
	//	if(!pOutfit)
	//		m_propertiesBox.AddItem("st_move_to_bag",  NULL, INVENTORY_TO_BAG_ACTION);
	//	else
	//		m_propertiesBox.AddItem("st_undress_outfit",  NULL, INVENTORY_TO_BAG_ACTION);
	//	bAlreadyDressed = true;
	//}
	//if(pOutfit  && !bAlreadyDressed )
	//{
	//	m_propertiesBox.AddItem("Dress in outfit",  NULL, INVENTORY_TO_SLOT_ACTION);
	//}
	//
	////отсоединение аддонов от вещи
	//if(pWeapon)
	//{
	//	if(pWeapon->GrenadeLauncherAttachable() && pWeapon->IsGrenadeLauncherAttached())
	//	{
	//		m_propertiesBox.AddItem("st_detach_gl",  NULL, INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON);
	//	}
	//	if(pWeapon->ScopeAttachable() && pWeapon->IsScopeAttached())
	//	{
	//		m_propertiesBox.AddItem("st_detach_scope",  NULL, INVENTORY_DETACH_SCOPE_ADDON);
	//	}
	//	if(pWeapon->SilencerAttachable() && pWeapon->IsSilencerAttached())
	//	{
	//		m_propertiesBox.AddItem("st_detach_silencer",  NULL, INVENTORY_DETACH_SILENCER_ADDON);
	//	}
	//	if(smart_cast<CWeaponMagazined*>(pWeapon) && IsGameTypeSingle())
	//	{
	//		bool b = (0!=pWeapon->GetAmmoElapsed());

	//		if(!b)
	//		{
	//			CUICellItem * itm = CurrentItem();
	//			for(u32 i=0; i<itm->ChildsCount(); ++i)
	//			{
	//				pWeapon		= smart_cast<CWeaponMagazined*>((CWeapon*)itm->Child(i)->m_pData);
	//				if(pWeapon->GetAmmoElapsed())
	//				{
	//					b = true;
	//					break;
	//				}
	//			}
	//		}

	//		if(b)
	//			m_propertiesBox.AddItem("st_unload_magazine",  NULL, INVENTORY_UNLOAD_MAGAZINE);


	//	}


	//}
	//
	////присоединение аддонов к активному слоту (2 или 3)
	//if(pScope)
	//{
	//	if(m_pInv->m_slots[PISTOL_SLOT].m_pIItem != NULL &&
	//	   m_pInv->m_slots[PISTOL_SLOT].m_pIItem->CanAttach(pScope))
	//	 {
	//		PIItem tgt = m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
	//		 m_propertiesBox.AddItem("st_attach_scope_to_pistol",  (void*)tgt, INVENTORY_ATTACH_ADDON);
	//	 }
	//	 if(m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
	//		m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pScope))
	//	 {
	//		PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
	//		 m_propertiesBox.AddItem("st_attach_scope_to_rifle",  (void*)tgt, INVENTORY_ATTACH_ADDON);
	//	 }
	//}
	//else if(pSilencer)
	//{
	//	 if(m_pInv->m_slots[PISTOL_SLOT].m_pIItem != NULL &&
	//	   m_pInv->m_slots[PISTOL_SLOT].m_pIItem->CanAttach(pSilencer))
	//	 {
	//		PIItem tgt = m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
	//		 m_propertiesBox.AddItem("st_attach_silencer_to_pistol",  (void*)tgt, INVENTORY_ATTACH_ADDON);
	//	 }
	//	 if(m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
	//		m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pSilencer))
	//	 {
	//		PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
	//		 m_propertiesBox.AddItem("st_attach_silencer_to_rifle",  (void*)tgt, INVENTORY_ATTACH_ADDON);
	//	 }
	//}
	//else if(pGrenadeLauncher)
	//{
	//	 if(m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
	//		m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pGrenadeLauncher))
	//	 {
	//		PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
	//		 m_propertiesBox.AddItem("st_attach_gl_to_rifle",  (void*)tgt, INVENTORY_ATTACH_ADDON);
	//	 }

	//}
	//
	//
	//if(pEatableItem)
	//{
	//	m_propertiesBox.AddItem("st_eat",  NULL, INVENTORY_EAT_ACTION);
	//}

	//if(pArtefact&&pArtefact->CanBeActivated()&&!GetInventory()->isSlotsBlocked())
	//	m_propertiesBox.AddItem("st_activate_artefact",  NULL, INVENTORY_ACTIVATE_ARTEFACT_ACTION);

	//if(!CurrentIItem()->IsQuestItem()){
	//	m_propertiesBox.AddItem("st_drop", NULL, INVENTORY_DROP_ACTION);

	//	if(CurrentItem()->ChildsCount())
	//		m_propertiesBox.AddItem("st_drop_all", (void*)33, INVENTORY_DROP_ACTION);
	//}

	//if (GameID() == GAME_ARTEFACTHUNT && Game().local_player && 
	//	Game().local_player->testFlag(GAME_PLAYER_FLAG_ONBASE) && 
	//	!Game().local_player->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)
	//	)
	//{
	//	m_propertiesBox.AddItem("st_sell_item",  NULL, INVENTORY_SELL_ITEM);	
	//}

	m_propertiesBox.AutoUpdateSize	();
	m_propertiesBox.BringAllToTop	();
	m_propertiesBox.Show			(x-rect.left, y-rect.top);
//	PlaySnd							(eInvProperties);
}

bool CUIBuyWnd::CanPutInSlot(CInventoryItem* iitm){
	u32 _slot = GetLocalSlot(iitm->GetSlot());
	return m_list[_slot]->ItemsCount() == 0;
}

bool CUIBuyWnd::CanPutInBag	(CInventoryItem* iitm){
	return true;
}

bool CUIBuyWnd::CanPutInBelt(CInventoryItem* iitem){
	if (PISTOL_SLOT == iitem->GetSlot())
		return false;
	else if (RIFLE_SLOT == iitem->GetSlot())
		return false;
	else if (OUTFIT_SLOT == iitem->GetSlot())
		return false;
	return true;
}

bool CUIBuyWnd::ToSlot(CUICellItem* itm, bool force_place)
{
	CUIDragDropListEx*	old_owner			= itm->OwnerList();
	PIItem	iitem							= (PIItem)itm->m_pData;
	u32 _slot								= GetLocalSlot(iitem->GetSlot());

	if(CanPutInSlot(iitem)){
		CUIDragDropListEx* new_owner		= GetSlotList(_slot);

		//bool result							= GetInventory()->Slot(iitem);
		VERIFY								(new_owner);

		CUICellItem* i						= old_owner->RemoveItem(itm, (old_owner==new_owner) );
		
		new_owner->SetItem					(i);
	
//		SendEvent_Item2Slot					(iitem);

//		SendEvent_ActivateSlot				(iitem);
		
		return								true;
	}else
	{ // in case slot is busy
		if(!force_place ||  iitem->GetSlot()==NO_ACTIVE_SLOT ) return false;

//		PIItem	_iitem						= GetInventory()->m_slots[_slot].m_pIItem;
		CUIDragDropListEx* slot_list		= GetSlotList(_slot);
		VERIFY								(slot_list->ItemsCount()==1);

		CUICellItem* slot_cell				= slot_list->GetItemIdx(0);

		bool result							= ToBag(slot_cell, false);
		VERIFY								(result);

		return ToSlot						(itm, false);
	}
}

bool CUIBuyWnd::ToBag(CUICellItem* itm, bool b_use_cursor_pos)
{
	PIItem	iitem						= (PIItem)itm->m_pData;

	if(CanPutInBag(iitem))
	{
		CUIDragDropListEx*	old_owner		= itm->OwnerList();
		CUIDragDropListEx*	new_owner		= NULL;
		if(b_use_cursor_pos){
				new_owner					= CUIDragDropListEx::m_drag_item->BackList();
//				VERIFY						(new_owner==m_pUIBagList);
		}else
				new_owner					= m_bag.GetItemList(itm);


//		bool result							= GetInventory()->Ruck(iitem);
//		VERIFY								(result);
		CUICellItem* i						= old_owner->RemoveItem(itm, (old_owner==new_owner) );
		
		if(b_use_cursor_pos)
			new_owner->SetItem				(i,old_owner->GetDragItemPosition());
		else
			new_owner->SetItem				(i);

//		SendEvent_Item2Ruck					(iitem);
		return true;
	}
	return false;
}

bool CUIBuyWnd::ToBelt(CUICellItem* itm, bool b_use_cursor_pos)
{
	PIItem	iitem						= (PIItem)itm->m_pData;

	if(CanPutInBelt(iitem))
	{
		CUIDragDropListEx*	old_owner		= itm->OwnerList();
		CUIDragDropListEx*	new_owner		= NULL;
		if(b_use_cursor_pos){
				new_owner					= CUIDragDropListEx::m_drag_item->BackList();
				VERIFY						(new_owner==m_list[MP_SLOT_BELT]);
		}else
				new_owner					= m_list[MP_SLOT_BELT];

//		bool result							= GetInventory()->Belt(iitem);
//		VERIFY								(result);
		CUICellItem* i						= old_owner->RemoveItem(itm, (old_owner==new_owner) );
		
	//.	UIBeltList.RearrangeItems();
		if(b_use_cursor_pos)
			new_owner->SetItem				(i,old_owner->GetDragItemPosition());
		else
			new_owner->SetItem				(i);

//		SendEvent_Item2Belt					(iitem);
		return								true;
	}
	return									false;
}