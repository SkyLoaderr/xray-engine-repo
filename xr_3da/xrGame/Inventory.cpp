#include "stdafx.h"
#include "inventory.h"
#include "actor.h"
#include "trade.h"
#include "weapon.h"
//#include "Physics.h"

#include "ui/UIInventoryUtilities.h"

#include "eatable_item.h"
#include "script_engine.h"
#include "xrmessages.h"
#include "game_cl_base.h"
#include "xr_level_controller.h"
#include "level.h"

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
	
	u32 l_slotsNum = pSettings->r_s32("inventory","slots"); // 7;			// 6 ������ ������ � ���� ������/������.
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

	//�������������� ���������� ��� ���������
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

bool CInventory::Take(CGameObject *pObj, bool bNotActivate)
{
	CInventoryItem *pIItem = dynamic_cast<CInventoryItem*>(pObj);
	VERIFY(pIItem);

	if(!CanTakeItem(pIItem)) return false;
	
	pIItem->m_pInventory = this;
	pIItem->m_drop = false;

	m_all.insert(pIItem);
	
	if(CanPutInSlot(pIItem))
	{
		bool result = Slot(pIItem, bNotActivate); R_ASSERT(result);
	} 
	else if (CanPutInBelt(pIItem))
	{
		bool result = Belt(pIItem); R_ASSERT(result);
	}
	else
	{
		bool result = Ruck(pIItem); R_ASSERT(result);
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

		if (InRuck(pIItem) || Ruck(pIItem))
		{
			m_ruck.erase(std::find(m_ruck.begin(), m_ruck.end(), pIItem));
			m_all.erase(pIItem);
			pIItem->m_pInventory = NULL;

			if (call_drop && dynamic_cast<CInventoryItem*>(pObj))
				m_pOwner->OnItemDrop	(dynamic_cast<CInventoryItem*>(pObj));

			CalcTotalWeight();

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

//�������� ���� � ����
bool CInventory::Slot(PIItem pIItem, bool bNotActivate) 
{
	VERIFY(pIItem);
	//Msg("put item %s in inventory slot %d", *pIItem->cName(), pIItem->GetSlot());
	if (Game().type != GAME_SINGLE)
	{
		bNotActivate = true;
	}

	if(!CanPutInSlot(pIItem)) return false;


	m_slots[pIItem->GetSlot()].m_pIItem = pIItem;

	//������� �� ������� ��� �����
	PPIItem it = std::find(m_ruck.begin(), m_ruck.end(), pIItem);
	if(m_ruck.end() != it) m_ruck.erase(it);
	it = std::find(m_belt.begin(), m_belt.end(), pIItem);
	if(m_belt.end() != it) m_belt.erase(it);

	if ((m_iActiveSlot == NO_ACTIVE_SLOT) && (!bNotActivate))
		Activate(pIItem->GetSlot());


	pIItem->m_eItemPlace = eItemPlaceSlot;

	pIItem->OnMoveToSlot();

	return true;
}

bool CInventory::Belt(PIItem pIItem) 
{
	if(!CanPutInBelt(pIItem))	return false;
	
	//���� ���� � �����
	bool in_slot = InSlot(pIItem);
	if(in_slot) 
	{
		if(m_iActiveSlot == pIItem->GetSlot()) Activate(NO_ACTIVE_SLOT);
		m_slots[pIItem->GetSlot()].m_pIItem = NULL;
	}
	
	m_belt.insert(m_belt.end(), pIItem); 

	if(!in_slot)
	{
		PPIItem it = std::find(m_ruck.begin(), m_ruck.end(), pIItem); 
		if(m_ruck.end() != it) m_ruck.erase(it);
	}

	CalcTotalWeight();
	pIItem->m_eItemPlace = eItemPlaceBelt;
	pIItem->OnMoveToBelt();
	return true;
}

bool CInventory::Ruck(PIItem pIItem) 
{
	if(!CanPutInRuck(pIItem)) return false;
	
	//���� ���� � �����
	if(InSlot(pIItem)) 
	{
		if(m_iActiveSlot == pIItem->GetSlot()) Activate(NO_ACTIVE_SLOT);
		m_slots[pIItem->GetSlot()].m_pIItem = NULL;
	}
	else
	{
		//���� ���� �� ����� ��� ������ ������ ������� � �����
		PPIItem it = std::find(m_belt.begin(), m_belt.end(), pIItem); 
		if(m_belt.end() != it) m_belt.erase(it);
	}
	
	m_ruck.insert(m_ruck.end(), pIItem); 
	
	CalcTotalWeight();
	pIItem->m_eItemPlace = eItemPlaceRuck;
	pIItem->OnMoveToRuck();
	return true;
}

bool CInventory::Activate(u32 slot) 
{	
	R_ASSERT2(slot == NO_ACTIVE_SLOT || slot<m_slots.size(), "wrong slot number");


#ifdef _DEBUG
	//Msg("slot: %d active slot: %d next_active: %d", slot, m_iActiveSlot, m_iNextActiveSlot);
//	if(slot != NO_ACTIVE_SLOT && m_slots[slot].m_pIItem)
//		Msg("activating %s item", m_slots[slot].m_pIItem->cName());
#endif

	if(slot != NO_ACTIVE_SLOT && !m_slots[slot].m_bCanBeActivated) 
	{
#ifdef _DEBUG
		//Msg("slot %d, cant be activaterd", slot);
#endif
		return false;
	}
	

	if(m_iActiveSlot == slot || (m_iNextActiveSlot == slot &&
		m_iActiveSlot != NO_ACTIVE_SLOT &&
		m_slots[m_iActiveSlot].m_pIItem &&
		m_slots[m_iActiveSlot].m_pIItem->IsHiding()))
	{
#ifdef _DEBUG
		//Msg("m_iActiveSlot == slot || (m_iNextActiveSlot == slot &&	m_iActiveSlot != NO_ACTIVE_SLOT &&	m_slots[m_iActiveSlot].m_pIItem &&	m_slots[m_iActiveSlot].m_pIItem->IsHiding())");
#endif

		return false;
	}

	//�������� ���� �� ������
	if(m_iActiveSlot == NO_ACTIVE_SLOT)
	{
		if(m_slots[slot].m_pIItem)
		{
			m_iNextActiveSlot = slot;
			return true;
		}
		else 
		{
#ifdef _DEBUG
			//Msg("m_slots[slot].m_pIItem == NULL");
#endif
			return false;
		}
	}
	//�������� ���� ������������
	else if(slot == NO_ACTIVE_SLOT || m_slots[slot].m_pIItem)
	{
		if(m_slots[m_iActiveSlot].m_pIItem)
			m_slots[m_iActiveSlot].m_pIItem->Deactivate();
		m_iNextActiveSlot = slot;
	
		return true;
	}

#ifdef _DEBUG
	//Msg("Why where are here???");
#endif

	return false;
}

PIItem CInventory::ActiveItem()const
{
	return m_iActiveSlot < m_slots.size() ? m_slots[m_iActiveSlot].m_pIItem : NULL;
}

PIItem CInventory::ItemFormSlot(u32 slot) const
{
	VERIFY(NO_ACTIVE_SLOT != slot);
	return m_slots[slot].m_pIItem;
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
				//������� � ��������� �����
				SendActionEvent(cmd, flags);
				return true;
			}break;
		case kDROP:
			{
				//������� � ��������� �����
				SendActionEvent(cmd, flags);
				return true;
			}break;
		case kWPN_FIRE:
		case kWPN_FUNC:
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

/*
bool RemoveInvItem(PIItem pItem)
{
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
			return false;
		}
		else 
		{
			drop_count = Drop(pIItem) ? drop_count + 1 : drop_count;
			return true;
		}
	}
	return false;
}
*/

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
	
	//��������� ������ � ����, ���� �� ����, ������� ����� ��������
	u32		drop_count = 0;
	for		(int i = 0; i < 2; ++i)	{
		TIItemList &list = i?m_ruck:m_belt;
		PPIItem it = list.begin();
	
		while(list.end() != it)
		{
			PIItem pIItem = *it;
			if(pIItem->m_drop)
			{
				pIItem->m_drop = false;
				if(pIItem->H_Parent())	{
					NET_Packet P;
					pIItem->u_EventGen(P, GE_OWNERSHIP_REJECT, 
										  pIItem->H_Parent()->ID());
					P.w_u16(u16(pIItem->ID()));
					if (OnServer()) pIItem->u_EventSend(P);
				}
				else 
					drop_tasks.push_back(pIItem);
			}
			++it;
		}
	}

	//��������� �����
	for(i=0; i<(int)m_slots.size(); ++i)	{
		PIItem pIItem = m_slots[i].m_pIItem;

		if(pIItem && pIItem->m_drop)	{
			pIItem->m_drop = false;			 
			
			if(pIItem->H_Parent())
			{
				NET_Packet P;
				pIItem->u_EventGen(P, GE_OWNERSHIP_REJECT, 
									pIItem->H_Parent()->ID());
				P.w_u16(u16(pIItem->ID()));
				if (OnServer()) pIItem->u_EventSend(P);
			}
			else
				drop_tasks.push_back	(pIItem);
		}
	}
	while	(drop_tasks.size())	{
		drop_count			= Drop(drop_tasks.back()) ? drop_count + 1 : drop_count;
		drop_tasks.pop_back	();
	}

	if (drop_count)
		m_pOwner->OnItemDropUpdate	();
}
//���� �� ����� ������� ������ ����
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

//���� �� ����� ���� ��� ����� 
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

//����� � ��������� ���� � ��������� ������
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

PIItem CInventory::item(CLASS_ID cls_id) const
{
	const TIItemSet &list = m_all;

	for(TIItemSet::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(pIItem->SUB_CLS_ID == cls_id && 
			pIItem->Useful()) 
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

//������� ������� 
bool CInventory::Eat(PIItem pIItem)
{
	//����������� �������� �� ����
	CEatableItem* pItemToEat = dynamic_cast<CEatableItem*>(pIItem);
	if(!pItemToEat) return false;

	if(pItemToEat->m_iPortionsNum == 0)
	{
		//���������� ����
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
	

	//��������� ���������� ������
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
	if(!m_bSlotsUseful) return false;

	if(pIItem->GetSlot() < m_slots.size() && 
		m_slots[pIItem->GetSlot()].m_pIItem == NULL)
		return true;
	
	return false;
}
//��������� ����� �� ��������� ���� �� ����,
//��� ���� ������� ������ �� ��������
bool CInventory::CanPutInBelt(PIItem pIItem) const
{
	if(InBelt(pIItem)) return false;
	if(!m_bBeltUseful) return false;
	if(!pIItem || !pIItem->Belt()) return false;
	if(m_belt.size() == BeltWidth()) return false;

	TIItemList buf_list;
	buf_list.clear();
	buf_list.insert(buf_list.begin(), m_belt.begin(), m_belt.end());
	buf_list.push_back(pIItem);
	bool result = FreeRoom(buf_list, BeltWidth(), 1);
	
	return result;
}
//��������� ����� �� ��������� ���� � ������,
//��� ���� ������� ������ �� ��������
bool CInventory::CanPutInRuck(PIItem pIItem) const
{
	if(InRuck(pIItem)) return false;
	return true;
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
		if (!xr_strcmp((*l_it)->cNameSect(),caItemName))
			return	(*l_it);
	ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"Object with name %s is not found in the %s inventory!",caItemName,*dynamic_cast<CGameObject*>(m_pOwner)->cName());
	return	(0);
}


bool CInventory::CanTakeItem(CInventoryItem *inventory_item) const
{
	VERIFY			(inventory_item);
	VERIFY			(m_pOwner);

	CActor* pActor = dynamic_cast<CActor*>(m_pOwner);
	//����� ������ ����� ����� ����
	if(!pActor && (TotalWeight() + inventory_item->Weight() > m_pOwner->MaxCarryWeight()))
		return	false;

	return	true;
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

void  CInventory::AddAvailableItems(TIItemList& items_container) const
{
	items_container.insert(items_container.end(), m_ruck.begin(), m_ruck.end());
	
	if(m_bBeltUseful)
		items_container.insert(items_container.end(), m_belt.begin(), m_belt.end());
	
	if(m_bSlotsUseful)
	{
		if(m_slots[KNIFE_SLOT].m_pIItem)
			items_container.push_back(m_slots[KNIFE_SLOT].m_pIItem);
		if(m_slots[RIFLE_SLOT].m_pIItem)
			items_container.push_back(m_slots[RIFLE_SLOT].m_pIItem);
		if(m_slots[GRENADE_SLOT].m_pIItem)
			items_container.push_back(m_slots[GRENADE_SLOT].m_pIItem);
		if(m_slots[APPARATUS_SLOT].m_pIItem)
			items_container.push_back(m_slots[APPARATUS_SLOT].m_pIItem);
		if(m_slots[OUTFIT_SLOT].m_pIItem)
			items_container.push_back(m_slots[OUTFIT_SLOT].m_pIItem);
	}		
}