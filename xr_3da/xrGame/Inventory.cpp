#include "stdafx.h"
#include "inventory.h"
#include "actor.h"
#include "trade.h"
#include "weapon.h"
//#include "Physics.h"

#include "ui/UIInventoryUtilities.h"

#include "eatable_item.h"
#include "script_engine.h"

using namespace InventoryUtilities;

// CInventorySlot class //////////////////////////////////////////////////////////////////////////

CInventorySlot::CInventorySlot() 
{
	m_pIItem = NULL;
	m_bCanBeActivated = true;
}

CInventorySlot::~CInventorySlot() 
{
}

///////////////////////////////////////////////////////////////////////////////
// CInventory class 
///////////////////////////////////////////////////////////////////////////////

CInventory::CInventory() 
{
	m_fTakeDist = pSettings->r_float("inventory","take_dist"); // 2.f;
	m_fMaxWeight = pSettings->r_float("inventory","max_weight"); // 40.f;
	m_iMaxRuck = pSettings->r_s32("inventory","max_ruck"); // 50;
	m_iMaxBelt = pSettings->r_s32("inventory","max_belt"); // 18;
	//if (Game().type != GAME_SINGLE) m_iMaxBelt+=2;
	
	u32 l_slotsNum = pSettings->r_s32("inventory","slots"); // 7;			// 6 слотов оружия и слот одежды/защиты.
	m_slots.resize(l_slotsNum);
	
	m_iActiveSlot = m_iNextActiveSlot = NO_ACTIVE_SLOT;
	
	u32 i = 0; 
	string256 temp;
	
	do 
	{
		sprintf(temp, "slot_name_%d", i+1);
		if(i < m_slots.size() && pSettings->line_exist("inventory",temp)) 
		{
			m_slots[i].m_name = pSettings->r_string("inventory",temp);
			++i;
		} 
		else break;
	} while(true);
	m_pTarget = NULL;

	m_slots[PDA_SLOT].m_bCanBeActivated = false;
	m_slots[OUTFIT_SLOT].m_bCanBeActivated = false;

	m_bSlotsUseful = true;
	m_bBeltUseful = false;
	m_inventory_mask.resize((RuckWidth()*RuckHeight() - 1)/64 + 1);

	//инициализируем невалидный вес инвентаря
	m_fTotalWeight = -1.f;
}


CInventory::~CInventory() 
{
}



void CInventory::Clear()
{
	//for multiplayer modes
	if (Game().type != GAME_SINGLE) m_iMaxBelt+=2;
	
	m_all.clear();
	m_ruck.clear();
	m_belt.clear();
	
	for(u32 i=0; i<m_slots.size(); i++)
	{
		m_slots[i].m_pIItem = NULL;
	}
	

	m_pOwner = NULL;

	CalcTotalWeight();
}

//разместились ли вещи в рюкзаке
bool CInventory::FreeRuckRoom()
{
	return FreeRoom(m_ruck,RUCK_WIDTH,RUCK_HEIGHT);
}

//разместились ли вещи на поясе
bool CInventory::FreeBeltRoom()
{
	return FreeRoom(m_belt, m_iMaxBelt, 1);
}


bool CInventory::Take(CGameObject *pObj, bool bNotActivate)
{
	CInventoryItem *pIItem = dynamic_cast<CInventoryItem*>(pObj);
	
	if(m_all.find(pIItem) != m_all.end()) 
	{
		Debug.fatal("Item already exist in inventory: %s(%s)",*pObj->cName(),pObj->cNameSect());
	}

	if(!pIItem || !pIItem->Useful() || 
		!(pIItem->Weight() + TotalWeight() < m_fMaxWeight)) return false;

	pIItem->m_pInventory = this;
	pIItem->m_drop = false;
	m_all.insert(pIItem);
		
	//сначала закинуть вещь в рюкзак
	if(pIItem->Ruck()) m_ruck.insert(m_ruck.end(), pIItem); 
		

	//поытаться закинуть в слот
	if(!Slot(pIItem,bNotActivate)) 
	{
		if(pIItem->GetSlot() < NO_ACTIVE_SLOT) 
		{
			if(!Belt(pIItem)) 
				 if(m_ruck.size() > m_iMaxRuck || 
					!pIItem->Ruck() || !FreeRuckRoom()) 
			{
				return !Drop(pIItem,false);
			}
		} 
		else if(!Belt(pIItem)) 
				  if(m_ruck.size() > m_iMaxRuck || !FreeRuckRoom()) 
		{
				return !Drop(pIItem,false);
		}
	} 
	
	//если активного предмета в руках нету, то активизировать
	//то что только что подняли
	else if(m_iActiveSlot == NO_ACTIVE_SLOT && !bNotActivate) 
	{
		Activate(pIItem->GetSlot());
	}

	m_pOwner->OnItemTake		(pIItem);


	CalcTotalWeight();

	return true;
}

bool CInventory::Drop(CGameObject *pObj, bool call_drop) 
{
	CInventoryItem *pIItem = dynamic_cast<CInventoryItem*>(pObj);
	
	if(pIItem && (m_all.find(pIItem) != m_all.end())) 
	{
		if (pIItem->GetSlot() == m_iActiveSlot && pIItem->GetSlot() != 0xffffffff &&
			m_slots[pIItem->GetSlot()].m_pIItem == pIItem)
		{
			m_iNextActiveSlot = m_iActiveSlot = NO_ACTIVE_SLOT;
		}

		if (Ruck(pIItem))
		{
			m_ruck.erase(std::find(m_ruck.begin(), m_ruck.end(), pIItem));
			m_all.erase(pIItem);
			pIItem->m_pInventory = NULL;

			if (call_drop && dynamic_cast<CInventoryItem*>(pObj))
				m_pOwner->OnItemDrop	(dynamic_cast<CInventoryItem*>(pObj));
			return true;
		};
	}
	return false;
}

bool CInventory::DropAll()
{
	PSPIItem it;

	for(it = m_all.begin(); m_all.end() != it; ++it)
	{
		PIItem pIItem = *it;
		Ruck(pIItem);
		pIItem->Drop();
	}
	
	CalcTotalWeight();

	return true;
}

void CInventory::ClearAll() 
{
	PSPIItem it;
	
	for(it = m_all.begin(); m_all.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		Ruck(pIItem);
	}
	m_ruck.clear();
	m_all.clear();

	CalcTotalWeight();
}

//положить вещь в слот
bool CInventory::Slot(PIItem pIItem, bool bNotActivate) 
{
	if(!m_bSlotsUseful) return false;

	if(pIItem->GetSlot() < m_slots.size() && 
		!m_slots[pIItem->GetSlot()].m_pIItem) 
	{
		m_slots[pIItem->GetSlot()].m_pIItem = pIItem;

		//удалить из рюкзака или пояса
		PPIItem it = std::find(m_ruck.begin(), m_ruck.end(), pIItem);
		if(m_ruck.end() != it) m_ruck.erase(it);
		it = std::find(m_belt.begin(), m_belt.end(), pIItem);
		if(m_belt.end() != it) m_belt.erase(it);

		if ((m_iActiveSlot == NO_ACTIVE_SLOT) && (!bNotActivate))
			Activate(pIItem->GetSlot());

		return true;
	} 

	return false;
}

bool CInventory::Belt(PIItem pIItem) 
{
	if(!m_bBeltUseful) return false;

	if(!pIItem || !pIItem->Belt()) return false;
	if(m_belt.size() == m_iMaxBelt) return false;
	
	//вещь - уже на поясе
	if(std::find(m_belt.begin(), m_belt.end(), pIItem) != m_belt.end()) return true;
	
	//вещь была в слоте
	if((pIItem->GetSlot() < m_slots.size()) && (m_slots[pIItem->GetSlot()].m_pIItem == pIItem)) 
	{
		if(m_iActiveSlot == pIItem->GetSlot()) Activate(NO_ACTIVE_SLOT);
		m_slots[pIItem->GetSlot()].m_pIItem = NULL;
	}
	
	m_belt.insert(m_belt.end(), pIItem); 

	//если на поясе нет свободного места - выбросить вещь
	if(!FreeBeltRoom()) 
	{
		PPIItem it = std::find(m_belt.begin(), m_belt.end(), pIItem); 
		m_belt.erase(it);
		return false;
	}
	else
	{
		PPIItem it = std::find(m_ruck.begin(), m_ruck.end(), pIItem); 
		if(m_ruck.end() != it) m_ruck.erase(it);

		CalcTotalWeight();
		return true;
	}
}

bool CInventory::Ruck(PIItem pIItem) 
{
	if(!pIItem || !pIItem->Ruck()) return false;

	if(std::find(m_ruck.begin(), m_ruck.end(), pIItem) != m_ruck.end()) return true;
	
	//вещь была в слоте
	if((pIItem->GetSlot() < m_slots.size()) && (m_slots[pIItem->GetSlot()].m_pIItem == pIItem)) 
	{
		if(m_iActiveSlot == pIItem->GetSlot())
			Activate(NO_ACTIVE_SLOT);
		m_slots[pIItem->GetSlot()].m_pIItem = NULL;
	}
	else
	{
		//вещь была на поясе
		PPIItem it = std::find(m_belt.begin(), m_belt.end(), pIItem); 
		if(m_belt.end() != it) m_belt.erase(it);
	}
	
	m_ruck.insert(m_ruck.end(), pIItem); 
	CalcTotalWeight();
	return true;
}

bool CInventory::Activate(u32 slot) 
{	
	R_ASSERT2(slot == NO_ACTIVE_SLOT || slot<m_slots.size(), "wrong slot number");

	if(slot != NO_ACTIVE_SLOT && 
		!m_slots[slot].m_bCanBeActivated) return false;
	
	if(m_iActiveSlot == slot || (m_iNextActiveSlot == slot &&
		m_iActiveSlot != NO_ACTIVE_SLOT &&
		m_slots[m_iActiveSlot].m_pIItem &&
		m_slots[m_iActiveSlot].m_pIItem->IsHiding()))
		return false;

	//активный слот не выбран
	if(m_iActiveSlot == NO_ACTIVE_SLOT)
	{
		if(m_slots[slot].m_pIItem)
		{
			m_iNextActiveSlot = slot;
			return true;
		}
		else 
			return false;
	}
	//активный слот задействован
	else if(slot == NO_ACTIVE_SLOT || m_slots[slot].m_pIItem)
	{
		if(m_slots[m_iActiveSlot].m_pIItem)
			m_slots[m_iActiveSlot].m_pIItem->Deactivate();
		m_iNextActiveSlot = slot;
	
		return true;
	}

	return false;
}

PIItem CInventory::ActiveItem()const
{
	return m_iActiveSlot < m_slots.size() ? m_slots[m_iActiveSlot].m_pIItem : NULL;
}

void CInventory::SendActionEvent(s32 cmd, u32 flags) 
{
	CActor *pActor = dynamic_cast<CActor*>(m_pOwner);
	if (!pActor) return;

	NET_Packet		P;
	pActor->u_EventGen		(P,GE_INV_ACTION, pActor->ID());
	P.w_s32					(cmd);
	P.w_u32					(flags);
	P.w_s32					(pActor->GetZoomRndSeed());		//Random Seed
	pActor->u_EventSend		(P);
};

bool CInventory::Action(s32 cmd, u32 flags) 
{
	CActor *pActor = dynamic_cast<CActor*>(m_pOwner);
	
	if (OnClient() && pActor)
	{
		switch(cmd)
		{
		case kUSE:
			{
			}break;
		case kWPN_RELOAD:
			{
				//создать и отправить пакет
				SendActionEvent(cmd, flags);
				return true;
			}break;
		case kDROP:
			{
				//создать и отправить пакет
				SendActionEvent(cmd, flags);
				return true;
			}break;
		case kWPN_FIRE:
		case kWPN_1:
		case kWPN_2:
		case kWPN_3:
		case kWPN_4:
		case kWPN_5:
		case kWPN_6:
		case kTORCH:
			{
				SendActionEvent(cmd, flags);
			}break;
		case kWPN_ZOOM : 
			{
				pActor->SetZoomRndSeed();
				SendActionEvent(cmd, flags);
			}break;
		}
	}


	if(m_iActiveSlot < m_slots.size() && 
			m_slots[m_iActiveSlot].m_pIItem && 
			m_slots[m_iActiveSlot].m_pIItem->Action(cmd, flags)) 
											return true;
	switch(cmd) 
	{
	case kWPN_1:
	case kWPN_2:
	case kWPN_3:
	case kWPN_4:
	case kWPN_5:
	case kWPN_6:
       {
			if(flags&CMD_START)
			{
                if((int)m_iActiveSlot == cmd - kWPN_1 &&
					m_slots[m_iActiveSlot].m_pIItem)
					m_slots[m_iActiveSlot].m_pIItem->Action(kWPN_NEXT, CMD_START);
				else 
					Activate(cmd - kWPN_1);

				return true;
			}
		} 
		break;
	case kUSE: 
		{
			if(flags&CMD_START && m_pTarget && m_pTarget->Useful()) 
			{
				// Generate event
				if(pActor) 
				{
					NET_Packet P;
					pActor->u_EventGen(P,GE_OWNERSHIP_TAKE,pActor->ID());
					P.w_u16(u16(m_pTarget->ID()));
					pActor->u_EventSend(P);
				}
			}
		} break;
	}
	return false;
}

void CInventory::Update() 
{
	bool bActiveSlotVisible;
	
	if(m_iActiveSlot == NO_ACTIVE_SLOT || 
		!m_slots[m_iActiveSlot].m_pIItem ||
        m_slots[m_iActiveSlot].m_pIItem->IsHidden())
	{ 
		bActiveSlotVisible = false;
	}
	else 
	{
		bActiveSlotVisible = true;
	}

//	if (ActiveItem() && ActiveItem()->IsHidden())
//		m_slots[m_iActiveSlot].m_pIItem = 0;

	if(m_iNextActiveSlot != m_iActiveSlot && !bActiveSlotVisible)
	{
		if(m_iNextActiveSlot != NO_ACTIVE_SLOT &&
			m_slots[m_iNextActiveSlot].m_pIItem)
			m_slots[m_iNextActiveSlot].m_pIItem->Activate();

		m_iActiveSlot = m_iNextActiveSlot;
	}
	
	//проверить рюкзак и пояс, есть ли вещи, которые нужно выкинуть
	u32		drop_count = 0;
	for(int i = 0; i < 2; ++i)
	{
		TIItemList &list = i?m_ruck:m_belt;
		PPIItem it = list.begin();
	
		while(list.end() != it)
		{
			PIItem pIItem = *it;
			if(pIItem->m_drop)
			{
				pIItem->m_drop = false;
				if(pIItem->H_Parent())
				{
					NET_Packet P;
					pIItem->u_EventGen(P, GE_OWNERSHIP_REJECT, 
										  pIItem->H_Parent()->ID());
					P.w_u16(u16(pIItem->ID()));
					pIItem->u_EventSend(P);
				}
				else 
					drop_count = Drop(pIItem) ? drop_count + 1 : drop_count;
			}
			++it;
		}
	}

	//проверить слоты
	for(i=0; i<(int)m_slots.size(); ++i)
	{
		PIItem pIItem = m_slots[i].m_pIItem;

		if(pIItem && pIItem->m_drop)
		{
			pIItem->m_drop = false;			 
			
			if(pIItem->H_Parent())
			{
				NET_Packet P;
				pIItem->u_EventGen(P, GE_OWNERSHIP_REJECT, 
									pIItem->H_Parent()->ID());
				P.w_u16(u16(pIItem->ID()));
				pIItem->u_EventSend(P);
			}
			else 
				drop_count = Drop(pIItem) ? drop_count + 1 : drop_count;
		}
	}

	if (drop_count)
		m_pOwner->OnItemDropUpdate	();
}
//ищем на поясе гранату такоже типа
PIItem CInventory::Same(const PIItem pIItem, bool bSearchRuck) const
{
	const TIItemList &list = bSearchRuck ? m_ruck : m_belt;

	for(TIItemList::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		const PIItem l_pIItem = *it;
		
		if((l_pIItem != pIItem) && 
				!xr_strcmp(l_pIItem->cNameSect(), 
				pIItem->cNameSect())) 
			return l_pIItem;
	}
	return NULL;
}

//ищем на поясе вещь для слота 
PIItem CInventory::SameSlot(u32 slot, bool bSearchRuck) const
{
	if(slot == NO_ACTIVE_SLOT) 	return NULL;

	const TIItemList &list = bSearchRuck ? m_ruck : m_belt;
	
	for(TIItemList::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(pIItem->GetSlot() == slot) return pIItem;
	}

	return NULL;
}

//найти в инвенторе вещь с указанным именем
PIItem CInventory::Get(const char *name, bool bSearchRuck) const
{
	const TIItemList &list = bSearchRuck ? m_ruck : m_belt;
	
	for(TIItemList::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(!xr_strcmp(pIItem->cNameSect(), name) && 
								pIItem->Useful()) 
				return pIItem;
	}
	return NULL;
}


PIItem CInventory::Get(CLASS_ID cls_id, bool bSearchRuck) const
{
	const TIItemList &list = bSearchRuck ? m_ruck : m_belt;
	
	for(TIItemList::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(pIItem->SUB_CLS_ID == cls_id && 
								pIItem->Useful()) 
				return pIItem;
	}
	return NULL;
}

PIItem CInventory::Get(const u16 id, bool bSearchRuck) const
{
	const TIItemList &list = bSearchRuck ? m_ruck : m_belt;

	for(TIItemList::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(pIItem->ID() == id) 
			return pIItem;
	}
	return NULL;
}

float CInventory::TotalWeight() const
{
	VERIFY(m_fTotalWeight>=0.f);
	return m_fTotalWeight;
}


float CInventory::CalcTotalWeight()
{
	float weight = 0;
	for(TIItemSet::const_iterator it = m_all.begin(); m_all.end() != it; ++it) 
		weight += (*it)->Weight();

	m_fTotalWeight = weight;
	return m_fTotalWeight;
}


u32 CInventory::dwfGetSameItemCount(LPCSTR caSection)
{
	u32			l_dwCount = 0;
	TIItemList	&l_list = m_ruck;
	for(PPIItem l_it = l_list.begin(); l_list.end() != l_it; ++l_it) 
	{
		PIItem	l_pIItem = *l_it;
		if (!xr_strcmp(l_pIItem->cNameSect(), caSection))
            ++l_dwCount;
	}
	
	return		(l_dwCount);
}

bool CInventory::bfCheckForObject(ALife::_OBJECT_ID tObjectID)
{
	TIItemSet	&l_list = m_all;
	for(PSPIItem l_it = l_list.begin(); l_list.end() != l_it; ++l_it) 
	{
		PIItem	l_pIItem = *l_it;
		if (l_pIItem->ID() == tObjectID)
			return(true);
	}
	return		(false);
}

CInventoryItem *CInventory::get_object_by_id(ALife::_OBJECT_ID tObjectID)
{
	TIItemSet	&l_list = m_all;
	for(PSPIItem l_it = l_list.begin(); l_list.end() != l_it; ++l_it) 
	{
		PIItem	l_pIItem = *l_it;
		if (l_pIItem->ID() == tObjectID)
			return	(l_pIItem);
	}
	return		(0);
}

//скушать предмет 
bool CInventory::Eat(PIItem pIItem)
{
	//устанаовить съедобна ли вещь
	CEatableItem* pItemToEat = dynamic_cast<CEatableItem*>(pIItem);
	if(!pItemToEat) return false;

	if(pItemToEat->m_iPortionsNum == 0)
	{
		//уничтожить вещь
		pIItem->Drop();
	
		NET_Packet P;
		CGameObject::u_EventGen(P,GE_DESTROY,pIItem->ID());
		P.w_u16(u16(pIItem->ID()));
		CGameObject::u_EventSend(P);

//		Msg		("ge_destroy: [%d] - %s",pIItem->ID(),*pIItem->cName());

		return false;
	}

	CEntityCondition *pCondition = dynamic_cast<CEntityCondition*>(m_pOwner);

	if(!pCondition) return false;

	pCondition->ChangeHealth(pItemToEat->m_fHealthInfluence);
	pCondition->ChangePower(pItemToEat->m_fPowerInfluence);
	pCondition->ChangeSatiety(pItemToEat->m_fSatietyInfluence);
	pCondition->ChangeRadiation(pItemToEat->m_fRadiationInfluence);
	

	//уменьшить количество порций
	if(pItemToEat->m_iPortionsNum == -1) 
		pItemToEat->m_iPortionsNum = 0;
	else
		--(pItemToEat->m_iPortionsNum);


	return true;
}

bool CInventory::InSlot(PIItem pIItem) const
{
	if(pIItem->GetSlot() < m_slots.size() && 
		m_slots[pIItem->GetSlot()].m_pIItem == pIItem)
		return true;
	return false;
}
bool CInventory::InBelt(PIItem pIItem) const
{
	if(Get(pIItem->ID(), false)) return true;
	return false;
}
bool CInventory::InRuck(PIItem pIItem) const
{
	if(Get(pIItem->ID(), true)) return true;
	return false;
}


bool CInventory::CanPutInSlot(PIItem pIItem) const
{
	if(pIItem->GetSlot() < m_slots.size() && 
		m_slots[pIItem->GetSlot()].m_pIItem == NULL)
		return true;
	
	return false;
}
//проверяет можем ли поместить вещь на пояс,
//при этом реально ничего не меняется
bool CInventory::CanPutInBelt(PIItem pIItem)
{
	if(InBelt(pIItem)) return false;

	//запомнить предыдущее место (слот или рюкзак)
	bool bInSlot = InSlot(pIItem);

	u32 active_slot = NO_ACTIVE_SLOT;
	if(bInSlot && pIItem->GetSlot() == GetActiveSlot()) 
	{
		active_slot = GetActiveSlot();
		m_iActiveSlot = NO_ACTIVE_SLOT;
	}


	if(!Belt(pIItem)) return false;

	bool result = true;
	if(!FreeBeltRoom()) result = false;

	//поместить элемент обратно на место
	if(bInSlot)
	{
		if(active_slot != NO_ACTIVE_SLOT) m_iActiveSlot = active_slot;
		Slot(pIItem);
	}
	else
		Ruck(pIItem);

	return result;
}
//проверяет можем ли поместить вещь в рюкзак,
//при этом реально ничего не меняется
bool CInventory::CanPutInRuck(PIItem pIItem)
{
	if(InRuck(pIItem)) return false;
	
	//запомнить предыдущее место (слот или пояс)
	bool bInSlot = InSlot(pIItem);

	u32 active_slot = NO_ACTIVE_SLOT;
	if(bInSlot && pIItem->GetSlot() == GetActiveSlot()) 
	{
		active_slot = GetActiveSlot();
		m_iActiveSlot = NO_ACTIVE_SLOT;
	}	

	if(!Ruck(pIItem))return false;

	bool result = true;

	if(!FreeRuckRoom()) result = false;

	//поместить элемент обратно на место
	if(bInSlot)
	{
		if(active_slot != NO_ACTIVE_SLOT) m_iActiveSlot = active_slot;
		Slot(pIItem);
	}
	else
		Belt(pIItem);
	 
	return result;
}

u32	CInventory::dwfGetObjectCount()
{
	return		(m_all.size());
}

CInventoryItem	*CInventory::tpfGetObjectByIndex(int iIndex)
{
	if ((iIndex >= 0) && (iIndex < (int)m_all.size())) {
		TIItemSet	&l_list = m_all;
		int			i = 0;
		for(PSPIItem l_it = l_list.begin(); l_list.end() != l_it; ++l_it, ++i) 
			if (i == iIndex)
                return	(*l_it);
	}
	else {
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"invalid inventory index!");
		return	(0);
	}
	R_ASSERT(false);
	return		(0);
}

CInventoryItem	*CInventory::GetItemFromInventory(LPCSTR caItemName)
{
	TIItemSet	&l_list = m_all;
	for(PSPIItem l_it = l_list.begin(); l_list.end() != l_it; ++l_it)
		if (!xr_strcmp((*l_it)->cName(),caItemName))
			return	(*l_it);
	ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"Object with name %s is not found in the %s inventory!",caItemName,*dynamic_cast<CGameObject*>(m_pOwner)->cName());
	return	(0);
}

IC	can_take(u64 mask0, u64 mask1)
{
	return			((mask0 ^ mask1) == (mask0 | mask1));
}

bool CInventory::CanTakeItem(CInventoryItem *inventory_item)
{
	inventory_item->BuildInventoryMask(this);

	VERIFY			(inventory_item);
	if ((TotalWeight() + inventory_item->Weight() > GetMaxWeight()) || (TotalVolume() + inventory_item->GetVolume() > GetMaxVolume())) {
		if (inventory_item->m_pInventory && (inventory_item->m_pInventory != this))
			inventory_item->BuildInventoryMask(inventory_item->m_pInventory);
		return		(false);
	}
	
	m_item_buffer.clear		();	
	m_item_buffer.insert	(m_item_buffer.end(),m_all.begin(),m_all.end());
	m_item_buffer.push_back	(inventory_item);

	sort					(m_item_buffer.begin(),m_item_buffer.end(),CItemVolumeSortPredicate());

	m_slot_buffer.resize	(m_slots.size());	
	m_slot_buffer.assign	(m_slot_buffer.size(),0);

	// put available items to slots
	{
		xr_vector<CInventoryItem*>::const_iterator	I = m_item_buffer.begin();
		xr_vector<CInventoryItem*>::const_iterator	E = m_item_buffer.end();
		for ( ; I != E; ++I) {
			if ((*I)->GetSlot() < m_slots.size()) {
				// so we can put this item into the slot
				if (!m_slot_buffer[(*I)->GetSlot()] || (m_slot_buffer[(*I)->GetSlot()]->GetVolume() < (*I)->GetVolume()))
					m_slot_buffer[(*I)->GetSlot()] = *I;
			}
		}
	}

	// remove items in slots and belt
	{
		xr_vector<CInventoryItem*>::iterator I = remove_if(m_item_buffer.begin(),m_item_buffer.end(),CRemoveStoredItems(&m_slot_buffer,BeltWidth()));
		m_item_buffer.erase(I,m_item_buffer.end());
	}

	m_inventory_mask.assign(m_inventory_mask.size(),0);

	xr_vector<CInventoryItem*>::const_iterator	I = m_item_buffer.begin();
	xr_vector<CInventoryItem*>::const_iterator	E = m_item_buffer.end();
	for ( ; I != E; ++I) {
		bool		ok = false;
		u64			item_mask = (*I)->InventoryMask(), item_mask1;
		u32			m = (*I)->GetWidth();
		u32			n = (*I)->GetHeight();
		u32			M = RuckWidth();
		u32			N = RuckHeight();
		xr_vector<u64>::iterator II = m_inventory_mask.begin(), JJ;
		xr_vector<u64>::iterator EE = m_inventory_mask.end();
		for ( ; II != EE; ++II) {
			for (u32 i=0; i<N-n; ++i) {
				for (u32 j=0; j<M-m; ++j) {
					item_mask <<= 1;
					if (can_take(item_mask,*II)) {
						*II	|= item_mask;
						ok	= true;
					}
				}
				if (ok)
					break;
				item_mask	<<= M - m;
			}
			
			if (ok)
				break;
			
			JJ				= II + 1;
			if (JJ == EE) {
				if (inventory_item->m_pInventory && (inventory_item->m_pInventory != this))
					inventory_item->BuildInventoryMask(inventory_item->m_pInventory);
				return		(false);
			}
			
			
			for (u32 ii=n - 1; i<N; ++i, --ii) {
				item_mask1	= (*I)->InventoryMask() >> (M * ii);
				for (u32 j=0; j<M-m; ++j) {
					item_mask <<= 1;
					item_mask1 <<= 1;
					if (can_take(item_mask,*II) && can_take(item_mask1,*JJ)) {
						*II	|= item_mask;
						*JJ	|= item_mask1;
						ok	= true;
					}
				}
				if (ok)
					break;

				item_mask	<<= M - m;
			}
		}
	}
	
	if (inventory_item->m_pInventory && (inventory_item->m_pInventory != this))
		inventory_item->BuildInventoryMask(inventory_item->m_pInventory);

	return			(true);
}


u32 CInventory::RuckWidth() const
{
	return RUCK_WIDTH;
}

u32 CInventory::RuckHeight() const
{
	return RUCK_HEIGHT;
}

u32 CInventory::GetMaxVolume() const
{
	return RUCK_WIDTH*RUCK_HEIGHT + BeltWidth();
}

u32 CInventory::TotalVolume() const
{
	u32 total_volume = 0;

	for(TIItemSet::const_iterator it = m_all.begin(); m_all.end() != it; ++it)
		total_volume += (*it)->GetVolume();

	return total_volume;
}

u32  CInventory::BeltWidth() const
{
	return m_iMaxBelt;
}