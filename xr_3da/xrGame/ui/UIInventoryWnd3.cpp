#include "stdafx.h"
#include "UIInventoryWnd.h"
#include "../actor.h"
#include "../mainui.h"
#include "../silencer.h"
#include "../scope.h"
#include "../grenadelauncher.h"
#include "../Artifact.h"
#include "../eatable_item.h"
#include "../WeaponMagazined.h"
#include "../inventory.h"
#include "../game_base.h"
#include "../game_cl_base.h"
#include "../xr_level_controller.h"
#include "UICellItem.h"

void	CUIInventoryWnd::Activate_Artefact()
{
	CActor *pActor							= smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor)								return;

	SendEvent_ActivateArtefact				(CurrentIItem());
};

void CUIInventoryWnd::EatCurrentItem()
{
	CActor *pActor							= smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor)								return;

	SendEvent_Item_Eat						(CurrentIItem());

	if(!CurrentIItem()->Useful())
	{
		R_ASSERT(0);
//.		(smart_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
//.			DetachChild						(m_pCurrentDragDropItem);

//.		DD_ITEMS_VECTOR_IT it				= std::find(m_vDragDropItems.begin(), m_vDragDropItems.end(),m_pCurrentDragDropItem);
//.		VERIFY								(it != m_vDragDropItems.end());
//.		m_pCurrentDragDropItem->Highlight	(false);
		SetCurrentItem						(NULL);
//.		m_pCurrentDragDropItem				= NULL;
	}
}


void CUIInventoryWnd::ActivatePropertiesBox()
{
	float x,y;
	// Флаг-признак для невлючения пункта контекстного меню: Dreess Outfit, если костюм уже надет
	bool bAlreadyDressed = false; 

	Frect rect = GetAbsoluteRect();
	GetUICursor()->GetPos(x,y);
		
	UIPropertiesBox.RemoveAll();
	
	CEatableItem* pEatableItem			= smart_cast<CEatableItem*>		(CurrentIItem());
	CCustomOutfit* pOutfit				= smart_cast<CCustomOutfit*>	(CurrentIItem());
	CArtefact* pArtefact				= smart_cast<CArtefact*>		(CurrentIItem());
	CWeapon* pWeapon					= smart_cast<CWeapon*>			(CurrentIItem());
	CScope* pScope						= smart_cast<CScope*>			(CurrentIItem());
	CSilencer* pSilencer				= smart_cast<CSilencer*>		(CurrentIItem());
	CGrenadeLauncher* pGrenadeLauncher	= smart_cast<CGrenadeLauncher*>	(CurrentIItem());
    

	if(CurrentIItem()->GetSlot()<SLOTS_NUM && m_pInv->CanPutInSlot(CurrentIItem()))
	{
		UIPropertiesBox.AddItem("Move to slot",  NULL, INVENTORY_TO_SLOT_ACTION);
	}
	if(CurrentIItem()->Belt() && m_pInv->CanPutInBelt(CurrentIItem()))
	{
		UIPropertiesBox.AddItem("Move on belt",  NULL, INVENTORY_TO_BELT_ACTION);
	}
	if(CurrentIItem()->Ruck() && m_pInv->CanPutInRuck(CurrentIItem()))
	{
		if(!pOutfit)
			UIPropertiesBox.AddItem("Move to bag",  NULL, INVENTORY_TO_BAG_ACTION);
		else
			UIPropertiesBox.AddItem("Undress outfit",  NULL, INVENTORY_TO_BAG_ACTION);
		bAlreadyDressed = true;
	}
	if(pOutfit  && !bAlreadyDressed /*&& m_pInv->CanPutInSlot(m_pCurrentItem)*/)
	{
		UIPropertiesBox.AddItem("Dress in outfit",  NULL, INVENTORY_TO_SLOT_ACTION);
	}
	
	//отсоединение аддонов от вещи
	if(pWeapon)
	{
		if(pWeapon->GrenadeLauncherAttachable() && pWeapon->IsGrenadeLauncherAttached())
		{
			UIPropertiesBox.AddItem("Detach grenade launcher",  NULL, INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON);
		}
		if(pWeapon->ScopeAttachable() && pWeapon->IsScopeAttached())
		{
			UIPropertiesBox.AddItem("Detach scope",  NULL, INVENTORY_DETACH_SCOPE_ADDON);
		}
		if(pWeapon->SilencerAttachable() && pWeapon->IsSilencerAttached())
		{
			UIPropertiesBox.AddItem("Detach silencer",  NULL, INVENTORY_DETACH_SILENCER_ADDON);
		}
		if(smart_cast<CWeaponMagazined*>(pWeapon) && IsGameTypeSingle())
		{
			bool b = (0!=pWeapon->GetAmmoElapsed());

			if(!b)
			{
				CUICellItem * itm = CurrentItem();
				for(u32 i=0; i<itm->ChildsCount(); ++i)
				{
					pWeapon		= smart_cast<CWeaponMagazined*>((CWeapon*)itm->Child(i)->m_pData);
					if(pWeapon->GetAmmoElapsed())
					{
						b = true;
						break;
					}
				}
			}

			if(b)
				UIPropertiesBox.AddItem("Unload magazine",  NULL, INVENTORY_UNLOAD_MAGAZINE);


		}


	}
	
	//присоединение аддонов к активному слоту (2 или 3)
	if(pScope)
	{
		if(m_pInv->m_slots[PISTOL_SLOT].m_pIItem != NULL &&
		   m_pInv->m_slots[PISTOL_SLOT].m_pIItem->CanAttach(pScope))
		 {
			PIItem tgt = m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
			 UIPropertiesBox.AddItem("Attach scope to pistol",  (void*)tgt, INVENTORY_ATTACH_ADDON);
		 }
		 if(m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pScope))
		 {
			PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
			 UIPropertiesBox.AddItem("Attach scope to rifle",  (void*)tgt, INVENTORY_ATTACH_ADDON);
		 }
	}
	else if(pSilencer)
	{
		 if(m_pInv->m_slots[PISTOL_SLOT].m_pIItem != NULL &&
		   m_pInv->m_slots[PISTOL_SLOT].m_pIItem->CanAttach(pSilencer))
		 {
			PIItem tgt = m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
			 UIPropertiesBox.AddItem("Attach silencer to pistol",  (void*)tgt, INVENTORY_ATTACH_ADDON);
		 }
		 if(m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pSilencer))
		 {
			PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
			 UIPropertiesBox.AddItem("Attach silencer to rifle",  (void*)tgt, INVENTORY_ATTACH_ADDON);
		 }
	}
	else if(pGrenadeLauncher)
	{
		 if(m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pGrenadeLauncher))
		 {
			PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
			 UIPropertiesBox.AddItem("Attach grenade launcher to rifle",  (void*)tgt, INVENTORY_ATTACH_ADDON);
		 }

	}
	
	
	if(pEatableItem)
	{
		UIPropertiesBox.AddItem("Eat",  NULL, INVENTORY_EAT_ACTION);
	}

	if(pArtefact&&pArtefact->CanBeActivated()&&!GetInventory()->isSlotsBlocked())
		UIPropertiesBox.AddItem("Activate artefact",  NULL, INVENTORY_ACTIVATE_ARTEFACT_ACTION);

	if(!CurrentIItem()->IsQuestItem())
		UIPropertiesBox.AddItem("Drop", NULL, INVENTORY_DROP_ACTION);

	if (GameID() == GAME_ARTEFACTHUNT && Game().local_player && 
		Game().local_player->testFlag(GAME_PLAYER_FLAG_ONBASE) && 
		!Game().local_player->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)
		)
	{
		UIPropertiesBox.AddItem("Sell Item",  NULL, INVENTORY_SELL_ITEM);	
	}

	UIPropertiesBox.AutoUpdateSize	();
	UIPropertiesBox.BringAllToTop	();
	UIPropertiesBox.Show			(x-rect.left, y-rect.top);
	PlaySnd							(eInvProperties);
}

void CUIInventoryWnd::ProcessPropertiesBoxClicked	()
{
	if(UIPropertiesBox.GetClickedItem())
	{
		switch(UIPropertiesBox.GetClickedItem()->GetValue())
		{
		case INVENTORY_SELL_ITEM:
			SellItem();
			break;
		case INVENTORY_TO_SLOT_ACTION:	
			ToSlot(CurrentItem(), true);
			break;
		case INVENTORY_TO_BELT_ACTION:	
			ToBelt(CurrentItem(),false);
			break;
		case INVENTORY_TO_BAG_ACTION:	
			ToBag(CurrentItem(),false);
			break;
		case INVENTORY_DROP_ACTION:
			DropCurrentItem();
			break;
		case INVENTORY_EAT_ACTION:
			EatCurrentItem();
			break;
		case INVENTORY_ATTACH_ADDON:
			AttachAddon((PIItem)(UIPropertiesBox.GetClickedItem()->GetData()));
			break;
		case INVENTORY_DETACH_SCOPE_ADDON:
			DetachAddon(*(smart_cast<CWeapon*>(CurrentIItem()))->GetScopeName());
			break;
		case INVENTORY_DETACH_SILENCER_ADDON:
			DetachAddon(*(smart_cast<CWeapon*>(CurrentIItem()))->GetSilencerName());
			break;
		case INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON:
			DetachAddon(*(smart_cast<CWeapon*>(CurrentIItem()))->GetGrenadeLauncherName());
			break;
		case INVENTORY_ACTIVATE_ARTEFACT_ACTION:
			Activate_Artefact();
			break;
		case INVENTORY_RELOAD_MAGAZINE:
			(smart_cast<CWeapon*>(CurrentIItem()))->Action(kWPN_RELOAD, CMD_START);
			break;
		case INVENTORY_UNLOAD_MAGAZINE:
			{
				CUICellItem * itm = CurrentItem();
				(smart_cast<CWeaponMagazined*>((CWeapon*)itm->m_pData))->UnloadMagazine();
				for(u32 i=0; i<itm->ChildsCount(); ++i)
				{
					CUICellItem * child_itm			= itm->Child(i);
					(smart_cast<CWeaponMagazined*>((CWeapon*)child_itm->m_pData))->UnloadMagazine();
				}
			}break;
		}
	}
}
