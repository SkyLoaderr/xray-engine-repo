#include "stdafx.h"
#include "inventory.h"
#include "actor.h"
#include "trade.h"
#include "weapon.h"

#include "ui/UIInventoryUtilities.h"
#include "ai_script_lua_space.h"
using namespace InventoryUtilities;


// CInventoryItem class ///////////////////////////////////////////////////////////////////////////////

CInventoryItem::CInventoryItem() 
{
	m_weight = 100.f;
	m_slot = NO_ACTIVE_SLOT;
	m_belt = false;
	m_showHUD = true;
	m_pInventory = NULL;
	m_drop = false;
	m_ruck = true;

	m_fCondition = 1.0f;
	m_bUsingCondition = false;

	m_fK_Burn = 1.0f;
	m_fK_Strike = 1.0f;
	m_fK_Wound = 1.0f;
	m_fK_Radiation = 1.0f;
	m_fK_Telepatic = 1.0f;
	m_fK_Shock = 1.0f;
	m_fK_ChemicalBurn = 1.0f;
	m_fK_Explosion = 1.0f;
	m_fK_FireWound = 1.0f;


	m_iGridWidth = 1;
	m_iGridHeight = 1;

	m_iXPos = 0;
	m_iYPos = 0;
}

CInventoryItem::~CInventoryItem() 
{
}

void CInventoryItem::Load(LPCSTR section) 
{
	inherited::Load(section);

	m_name = pSettings->r_string(section, "inv_name");
	m_nameShort = pSettings->r_string(section, "inv_name_short");
	NameComplex();
	m_weight = pSettings->r_float(section, "inv_weight");

	m_cost = pSettings->r_u32(section, "cost");

	//properties used by inventory menu
	m_iGridWidth = pSettings->r_u32(section, "inv_grid_width");
	m_iGridHeight = pSettings->r_u32(section, "inv_grid_height");

	m_iXPos = pSettings->r_u32(section, "inv_grid_x");
	m_iYPos = pSettings->r_u32(section, "inv_grid_y");
}

void  CInventoryItem::ChangeCondition(float fDeltaCondition)
{
	m_fCondition += fDeltaCondition;

	if(m_fCondition <0.0f) m_fCondition = 0.0f;
	else if(m_fCondition >1.0f) m_fCondition = 1.0f;
}

void CInventoryItem::Hit(float P, Fvector &dir,	
						 CObject* who, s16 element,
						 Fvector position_in_object_space, 
						 float impulse, 
						 ALife::EHitType hit_type)
{
	inherited::Hit(P, dir, who, element, position_in_object_space, 
					  impulse, hit_type);


	if(!m_bUsingCondition) return;

	float hit_power = P/100.f;

	switch(hit_type)
	{
	case eHitTypeBurn:
		hit_power *= m_fK_Burn;
		break;
	case eHitTypeChemicalBurn:
		hit_power *= m_fK_ChemicalBurn;
		break;
	case eHitTypeStrike:
		hit_power *= m_fK_Strike;
		break;
	case eHitTypeTelepatic:
		hit_power *= m_fK_Telepatic;
		break;
	case eHitTypeShock:
		hit_power *= m_fK_Shock;
		break;
	case eHitTypeExplosion:
		hit_power *= m_fK_Explosion;
		break;
	case eHitTypeFireWound:
		hit_power *= m_fK_FireWound;
		break;
	case eHitTypeWound:
		hit_power *= m_fK_Wound;
		break;
	}

	ChangeCondition(-hit_power);
}

const char* CInventoryItem::Name() 
{
	return m_name;
}

const char* CInventoryItem::NameShort() 
{
	return m_nameShort;
}

char* CInventoryItem::NameComplex() 
{
	const char *l_name = Name();
	if(l_name) 
		strcpy(m_nameComplex, l_name); 
	else 
		m_nameComplex[0] = 0;

	/*for(PPIItem l_it = m_subs.begin(); m_subs.end() != l_it; ++l_it) 
	{
		const char *l_subName = (*l_it)->NameShort();
		if(l_subName)
			strcpy(&m_nameComplex[xr_strlen(m_nameComplex)], l_subName);
	}*/


	if(m_bUsingCondition)
	{
		/*if(GetCondition()<0.33)
			strcpy(&m_nameComplex[xr_strlen(m_nameComplex)], " bad");
		else if(GetCondition()<0.66)
			strcpy(&m_nameComplex[xr_strlen(m_nameComplex)], " good");
		else
			strcpy(&m_nameComplex[xr_strlen(m_nameComplex)], " excelent");
		*/
		sprintf(&m_nameComplex[xr_strlen(m_nameComplex)]," %f",GetCondition());
	}


	return m_nameComplex;
}

bool CInventoryItem::Useful() 
{
	// Если IItem нельзя использовать, вернуть false
	return true;
}
/*
bool CInventoryItem::Attach(PIItem pIItem, bool force) 
{
	// Аргумент force всегда равен false
	// наследник должен изменить его на true
	// если данный IItem МОЖЕТ быть к нему присоединен,
	// и вызвать return CInventoryItem::Attach(pIItem, force);
	if(force) 
	{
		m_subs.insert(m_subs.end(), pIItem);
		return true;
	} 
	else 
		return false;
}*/
/*
bool CInventoryItem::Detach(PIItem pIItem, bool force) 
{
	// Аргумент force всегда равен true
	// наследник должен изменить его на false
	// если данный IItem НЕ МОЖЕТ быть отсоединен,
	// и вызвать return CInventoryItem::Detach(pIItem, force);
	if(force) 
	{
		if(m_subs.erase(std::find(m_subs.begin(), m_subs.end(), pIItem)) != m_subs.end()) 
		{
			return true;
		}
		return false;
	}
	else return false; 
}*/

bool CInventoryItem::DetachAll()
{
	if(!m_pInventory || std::find(m_pInventory->m_ruck.begin(), 
								    m_pInventory->m_ruck.end(), this) == 
									m_pInventory->m_ruck.end()) return false;

/*	for(PPIItem it = m_subs.begin(); m_subs.end() != it; ++it) 
	{
		Detach(*it);
		//m_pInventory->m_ruck.insert(m_pInventory->m_ruck.end(), *l_it);
		(*it)->DetachAll();
	}*/
	return true;
}

bool CInventoryItem::Activate() 
{
	// Вызывается при активации слота в котором находится объект
	// Если объект может быть активирован вернуть true, иначе false
	return false;
}

void CInventoryItem::Deactivate() 
{
	// Вызывается при деактивации слота в котором находится объект
}

void CInventoryItem::Drop() 
{
	if(m_pInventory) m_drop = true;
}

s32 CInventoryItem::Sort(PIItem /**pIItem/**/) 
{
	// Если нужно разместить IItem после this - вернуть 1, если
	// перед - -1. Если пофиг то 0.
	return 0;
}

bool CInventoryItem::Merge(PIItem /**pIItem/**/) 
{
	// Если удалось слить вместе вернуть true
	return false;
}

void CInventoryItem::OnH_B_Independent	()
{
	inherited::OnH_B_Independent();

/*
	if(m_pPhysicsShell) 
	{
		Fvector l_fw; l_fw.set(XFORM().k); l_fw.mul(2.f);
		Fvector l_up; l_up.set(XFORM().j); l_up.mul(2.f);
		Fmatrix l_p1, l_p2;
		l_p1.set(XFORM());
		l_p2.set(XFORM()); l_fw.mul(2.f); l_p2.c.add(l_fw);
		m_pPhysicsShell->Activate(l_p1, 0, l_p2);
		XFORM().set(l_p1);
	}*/
}

void CInventoryItem::OnH_B_Chield		()
{
	inherited::OnH_B_Chield		();
/*
	setVisible					(false);
	setEnabled					(false);
	if (m_pPhysicsShell)		m_pPhysicsShell->Deactivate	();

	if(Local()) OnStateSwitch(eHiding);


	if (Local() && (0xffff!=respawnPhantom)) 
	{
		NET_Packet		P;
		u_EventGen		(P,GE_RESPAWN,respawnPhantom);
		u_EventSend		(P);
		respawnPhantom	= 0xffff;
	}
	*/
}

void CInventoryItem::UpdateCL()
{
	inherited::UpdateCL();

/*	if (0==H_Parent() && m_pPhysicsShell)		
	{
		m_pPhysicsShell->Update	();
		XFORM().set			(m_pPhysicsShell->mXFORM);
		XFORM().set			(m_pPhysicsShell->mXFORM);
		Position().set			(XFORM().c);
	}*/
}


void CInventoryItem::OnEvent (NET_Packet& P, u16 type)
{
	inherited::OnEvent(P, type);
}

//процесс отсоединения вещи заключается в спауне новой вещи 
//в инвентаре и установке соответствующих флагов в родительском
//объекте, поэтому функция должна быть переопределена
bool CInventoryItem::Detach(const char* item_section_name) 
{
	CSE_Abstract*		D	= F_entity_Create(item_section_name);
	R_ASSERT		   (D);
	CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = 
							 dynamic_cast<CSE_ALifeDynamicObject*>(D);
	R_ASSERT			(l_tpALifeDynamicObject);
	
	l_tpALifeDynamicObject->m_tNodeID = this->level_vertex_id();
		
	// Fill
	strcpy				(D->s_name, item_section_name);
	strcpy				(D->s_name_replace,"");
	D->s_gameid			=	u8(GameID());
	D->s_RP				=	0xff;
	D->ID				=	0xffff;
	D->ID_Parent		=	u16(this->H_Parent()->ID());
	D->ID_Phantom		=	0xffff;
	D->o_Position		=	Position();
	D->s_flags.set		(M_SPAWN_OBJECT_LOCAL);
	D->RespawnTime		=	0;
	// Send
	NET_Packet			P;
	D->Spawn_Write		(P,TRUE);
	Level().Send		(P,net_flags(TRUE));
	// Destroy
	F_entity_Destroy	(D);

	return true;
}


///////////////////////////////////////////
// CEatableItem class 
///////////////////////////////////////////

CEatableItem::CEatableItem()
{
	m_fHealthInfluence = 0;
	m_fPowerInfluence = 0;
	m_fSatietyInfluence = 0;
	m_fRadiationInfluence = 0;

	m_iPortionsNum = -1;
}
CEatableItem::~CEatableItem()
{
}
void CEatableItem::Load(LPCSTR section)
{
	inherited::Load(section);

	m_fHealthInfluence = pSettings->r_float(section, "eat_health");
	m_fPowerInfluence = pSettings->r_float(section, "eat_power");
	m_fSatietyInfluence = pSettings->r_float(section, "eat_satiety");
	m_fRadiationInfluence = pSettings->r_float(section, "eat_radiation");

	m_iPortionsNum = pSettings->r_s32(section, "eat_portions_num");
}

bool CEatableItem::Useful()
{
	if(!inherited::Useful()) return false;

	//проверить не все ли еще съедено
	if(m_iPortionsNum == 0) return false;

	return true;
}
///////////////////////////////////////////////////////////////////////////////
// CInventory class 
///////////////////////////////////////////////////////////////////////////////

CInventory::CInventory() 
{
	m_takeDist = pSettings->r_float("inventory","take_dist"); // 2.f;
	m_maxWeight = pSettings->r_float("inventory","max_weight"); // 40.f;
	m_maxRuck = pSettings->r_s32("inventory","max_ruck"); // 50;
	m_maxBelt = pSettings->r_s32("inventory","max_belt"); // 18;
	u32 l_slotsNum = pSettings->r_s32("inventory","slots"); // 7;			// 6 слотов оружия и слот одежды/защиты.
	m_slots.resize(l_slotsNum);
	m_activeSlot = m_nextActiveSlot = NO_ACTIVE_SLOT;
	u32 i = 0; string256 temp;
	
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

	m_bSlotsUseful = true;
	m_bBeltUseful = false;
}


CInventory::~CInventory() 
{
}

//разместились ли вещи в рюкзаке
bool CInventory::FreeRuckRoom()
{
	return FreeRoom(m_ruck,RUCK_WIDTH,RUCK_HEIGHT);
}

//разместились ли вещи на поясе
bool CInventory::FreeBeltRoom()
{
	return FreeRoom(m_belt, m_maxBelt, 1);
}


bool CInventory::Take(CGameObject *pObj, bool bNotActivate)
{
	CInventoryItem *pIItem = dynamic_cast<CInventoryItem*>(pObj);
	
	if(m_all.find(pIItem) != m_all.end()) 
	{
		Debug.fatal("Item already exist in inventory: %s(%s)",pObj->cName(),pObj->cNameSect());
	}

	if(!pIItem || !pIItem->Useful() || 
		!(pIItem->Weight() + TotalWeight() < m_maxWeight)) return false;

	pIItem->m_pInventory = this;
	pIItem->m_drop = false;
	m_all.insert(pIItem);
		
	//сначала закинуть вещь в рюкзак
	if(pIItem->Ruck()) m_ruck.insert(m_ruck.end(), pIItem); 
		
//	l_subs.insert(l_subs.end(), l_pIItem->m_subs.begin(), l_pIItem->m_subs.end());
		
	/*while(l_subs.size())
	{
		l_pIItem = *l_subs.begin();
		l_pIItem->m_pInventory = this;
		m_all.insert(l_pIItem);
		l_subs.insert(l_subs.end(), l_pIItem->m_subs.begin(), l_pIItem->m_subs.end());
		l_subs.erase(l_subs.begin());
	}*/
		
	//поытаться закинуть в слот
	if(!Slot(pIItem,bNotActivate)) 
	{
		if(pIItem->GetSlot() < NO_ACTIVE_SLOT) 
		{
			/*if(m_slots[pIItem->m_slot].m_pIItem &&
			   m_slots[pIItem->m_slot].m_pIItem->Attach(pIItem))
			{
				m_ruck.erase(std::find(m_ruck.begin(), m_ruck.end(), pIItem));
				return true;
			} 
			else */
			if(!Belt(pIItem)/* || !FreeBeltRoom()*/) 
				 if(m_ruck.size() > m_maxRuck || 
					!pIItem->Ruck() || !FreeRuckRoom()) 
			{
				//return true;
				//else 
				return !Drop(pIItem,false);
			}
		} 
		else if(!Belt(pIItem)/* || !FreeBeltRoom()*/) 
				  if(m_ruck.size() > m_maxRuck || !FreeRuckRoom()) 
		{
//				if(Belt(l_pIItem)) 
//					return true;
//				else 
				return !Drop(pIItem,false);
		}
	} 
	
	//если активного предмета в руках нету, то активизировать
	//то что только что подняли
	else if(m_activeSlot == NO_ACTIVE_SLOT && !bNotActivate) 
	{
		Activate(pIItem->GetSlot());
	}

	m_pOwner->OnItemTake		(pIItem);
	return true;
}

bool CInventory::Drop(CGameObject *pObj, bool call_drop) 
{
	CInventoryItem *pIItem = dynamic_cast<CInventoryItem*>(pObj);
	
	if(pIItem && (m_all.find(pIItem) != m_all.end())) 
	{
		if (pIItem->GetSlot() == m_activeSlot) m_activeSlot = NO_ACTIVE_SLOT;
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
}

bool CInventory::Slot(PIItem pIItem, bool bNotActivate) 
{
	if(!m_bSlotsUseful) return false;

	if(pIItem->GetSlot() < m_slots.size()) 
	{
		//if(m_slots[pIItem->m_slot].m_pIItem && !Belt(m_slots[pIItem->m_slot].m_pIItem)) Ruck(m_slots[pIItem->m_slot].m_pIItem);
		if(!m_slots[pIItem->GetSlot()].m_pIItem) 
		{
			m_slots[pIItem->GetSlot()].m_pIItem = pIItem;
			PPIItem it = std::find(m_ruck.begin(), m_ruck.end(), pIItem); 
			
			if(m_ruck.end() != it) m_ruck.erase(it);
			
			//by Dandy, also perform search on the belt
			 it = std::find(m_belt.begin(), m_belt.end(), pIItem); 
			 if(m_belt.end() != it) m_belt.erase(it);
			
			if ((m_activeSlot == NO_ACTIVE_SLOT) && (!bNotActivate)) Activate(pIItem->GetSlot());
			return true;
		} 
		else 
		{
			/*if(m_slots[pIItem->m_slot].m_pIItem->Attach(pIItem)) 
			{
				PPIItem it = std::find(m_ruck.begin(), m_ruck.end(), pIItem); 
				if(m_ruck.end() != it) m_ruck.erase(it);

				//by Dandy, also perform search on the belt
				 it = std::find(m_belt.begin(), m_belt.end(), pIItem); 
				 if(m_belt.end() != it) m_belt.erase(it);

				return true;
			}*/
		}
	} 
	else 
	{
		/*for(u32 i = 0; i < m_slots.size(); ++i) 
			if(m_slots[i].m_pIItem && m_slots[i].m_pIItem->Attach(pIItem)) 
			{
				PPIItem it = std::find(m_ruck.begin(), m_ruck.end(), pIItem);
				if(m_ruck.end() != it) m_ruck.erase(it);
				return true;
			}*/
	}
	return false;
}

bool CInventory::Belt(PIItem pIItem) 
{
	if(!m_bBeltUseful) return false;

	if(!pIItem || !pIItem->Belt()) return false;
	if(m_belt.size() == m_maxBelt) return false;
	
	//вещь - уже на поясе
	if(std::find(m_belt.begin(), m_belt.end(), pIItem) != m_belt.end()) return true;
	
	//вещь была в слоте
	if((pIItem->GetSlot() < m_slots.size()) && (m_slots[pIItem->GetSlot()].m_pIItem == pIItem)) 
	{
		if(m_activeSlot == pIItem->GetSlot()) Activate(NO_ACTIVE_SLOT);
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
		if(m_activeSlot == pIItem->GetSlot()) Activate(NO_ACTIVE_SLOT);
		m_slots[pIItem->GetSlot()].m_pIItem = NULL;
	}

	//вещь была на поясе
	PPIItem it = std::find(m_belt.begin(), m_belt.end(), pIItem); 
	if(m_belt.end() != it) m_belt.erase(it);
	
	m_ruck.insert(m_ruck.end(), pIItem); 
	return true;
}

bool CInventory::Activate(u32 slot) 
{	
	if(slot == NO_ACTIVE_SLOT)
	{
		if(m_activeSlot < m_slots.size()) 
		{
			m_slots[m_activeSlot].m_pIItem->Deactivate();
			m_activeSlot = slot;
		}
		return false;
	} 

	if(/*(slot == NO_ACTIVE_SLOT) || /*(slot == m_activeSlot) || */
		(slot < m_slots.size() && m_slots[slot].m_pIItem == NULL)) 
        return false;

	//если активный слот чем-то занят, неразрешать его деактивацию 
	if(m_activeSlot < m_slots.size() && m_slots[m_activeSlot].m_pIItem &&
		m_slots[m_activeSlot].m_pIItem->IsPending()) 
		return false;

	if(m_activeSlot < m_slots.size()) 
	{
		m_slots[m_activeSlot].m_pIItem->Deactivate();
		if(slot < m_slots.size()) 
			m_nextActiveSlot = slot;
		else  
			m_activeSlot = slot;
	} 
	else if(m_slots[slot].m_pIItem->Activate()) 
	{
		m_activeSlot = slot;
	}
	
	return false;
}

PIItem CInventory::ActiveItem()const
{
	return m_activeSlot < m_slots.size() ? m_slots[m_activeSlot].m_pIItem : NULL;
}

void CInventory::SendActionEvent(s32 cmd, u32 flags) 
{
	CActor *pActor = dynamic_cast<CActor*>(m_pOwner);
	if (!pActor) return;

	NET_Packet		P;
	pActor->u_EventGen		(P,GE_INV_ACTION, pActor->ID());
	P.w_s32					(cmd);
	P.w_u32					(flags);
	pActor->u_EventSend		(P);
};

bool CInventory::Action(s32 cmd, u32 flags) 
{
	CActor *pActor = dynamic_cast<CActor*>(m_pOwner);
	
	if(!Level().Server->client_Count() && pActor)
	{
		switch(cmd)
		{
		case kUSE:
			{
			}break;
		case kDROP:
			{
				//создать и отправить пакет
				SendActionEvent(cmd, flags);
				return true;
			}break;
		case kWPN_FIRE:
			{
				SendActionEvent(cmd, flags);
			}break;
		}
	}


	if(m_activeSlot < m_slots.size() && 
			m_slots[m_activeSlot].m_pIItem && 
			m_slots[m_activeSlot].m_pIItem->Action(cmd, flags)) 
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
                if((int)m_activeSlot == cmd - kWPN_1)
					m_slots[cmd - kWPN_1].m_pIItem->Action(kWPN_NEXT, CMD_START);
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

void CInventory::Update(u32 /**deltaT/**/) 
{
	bool bActiveSlotVisible;
	

	if(m_activeSlot == NO_ACTIVE_SLOT || (m_slots[m_activeSlot].m_pIItem &&
						m_slots[m_activeSlot].m_pIItem->IsHidden())) 
	{ 
		bActiveSlotVisible = false;
	}
	else 
	{
		bActiveSlotVisible = true;
	}
		


	if((m_nextActiveSlot < m_slots.size()) && !bActiveSlotVisible)
	{
		if(m_slots[m_nextActiveSlot].m_pIItem)
		{
			if(m_slots[m_nextActiveSlot].m_pIItem->Activate())
			{
				m_activeSlot = m_nextActiveSlot;
				m_nextActiveSlot = NO_ACTIVE_SLOT;
			}
			else if(m_activeSlot == NO_ACTIVE_SLOT || 
					!m_slots[m_activeSlot].m_pIItem->Activate())
			{
				m_activeSlot = m_nextActiveSlot = NO_ACTIVE_SLOT;
			}
		}
		else m_activeSlot = m_nextActiveSlot = NO_ACTIVE_SLOT;
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
PIItem CInventory::Same(const PIItem pIItem) 
{
	for(PPIItem it = m_belt.begin(); m_belt.end() != it; ++it) 
	{
		PIItem l_pIItem = *it;
		
		if((l_pIItem != pIItem) && 
				!strcmp(l_pIItem->cNameSect(), 
				pIItem->cNameSect())) 
			return l_pIItem;
	}
	return NULL;
}

//ищем на поясе вещь для слота 
PIItem CInventory::SameSlot(u32 slot) 
{
	if(slot == NO_ACTIVE_SLOT) 	return NULL;
	
	for(PPIItem it = m_belt.begin(); m_belt.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(pIItem->GetSlot() == slot) return pIItem;
	}

	return NULL;
}

//найти в инвенторе вещь с указанным именем
PIItem CInventory::Get(const char *name, bool bSearchRuck) 
{
	TIItemList &list = bSearchRuck ? m_ruck : m_belt;
	
	for(PPIItem it = list.begin(); list.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(!strcmp(pIItem->cNameSect(), name) && 
								pIItem->Useful()) 
				return pIItem;
	}
	return NULL;
}


PIItem CInventory::Get(CLASS_ID cls_id, bool bSearchRuck) 
{
	TIItemList &list = bSearchRuck ? m_ruck : m_belt;
	
	for(PPIItem it = list.begin(); list.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(pIItem->SUB_CLS_ID == cls_id && 
								pIItem->Useful()) 
				return pIItem;
	}
	return NULL;
}




PIItem CInventory::Get(const u32 id, bool bSearchRuck) 
{
	TIItemList &list = bSearchRuck ? m_ruck : m_belt;

	for(PPIItem it = list.begin(); list.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(pIItem->ID() == id) 
			return pIItem;
	}
	return NULL;
}

f32 CInventory::TotalWeight() const
{
	f32 weight = 0;
	for(TIItemSet::const_iterator it = m_all.begin(); m_all.end() != it; ++it) 
					weight += (*it)->Weight();
	
	return weight;
}

void CInventory::Clear()
{
	m_all.clear();
	m_ruck.clear();
	m_belt.clear();
}

u32 CInventory::dwfGetSameItemCount(LPCSTR caSection)
{
	u32			l_dwCount = 0;
	TIItemList	&l_list = m_ruck;
	for(PPIItem l_it = l_list.begin(); l_list.end() != l_it; ++l_it) 
	{
		PIItem	l_pIItem = *l_it;
		if (!strcmp(l_pIItem->cNameSect(), caSection))
            ++l_dwCount;
	}
	
	return		(l_dwCount);
}

bool CInventory::bfCheckForObject(_OBJECT_ID tObjectID)
{
	TIItemList	&l_list = m_ruck;
	for(PPIItem l_it = l_list.begin(); l_list.end() != l_it; ++l_it) 
	{
		PIItem	l_pIItem = *l_it;
		if (l_pIItem->ID() == tObjectID)
			return(true);
	}
	return		(false);
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
		CGameObject* pObject = dynamic_cast<CGameObject*>(m_pOwner);
		pObject->u_EventGen(P,GE_DESTROY,pIItem->ID());
		P.w_u16(u16(pIItem->ID()));
		pObject->u_EventSend(P);

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

bool CInventory::InSlot(PIItem pIItem)
{
	if(pIItem->GetSlot() < m_slots.size() && 
		m_slots[pIItem->GetSlot()].m_pIItem == pIItem)
		return true;
	return false;
}
bool CInventory::InBelt(PIItem pIItem)
{
	if(Get(pIItem->ID(), false)) return true;
	return false;
}
bool CInventory::InRuck(PIItem pIItem)
{
	if(Get(pIItem->ID(), true)) return true;
	return false;
}


bool CInventory::CanPutInSlot(PIItem pIItem)
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
		m_activeSlot = NO_ACTIVE_SLOT;
	}


	if(!Belt(pIItem)) return false;

	bool result = true;
	if(!FreeBeltRoom()) result = false;

	//поместить элемент обратно на место
	if(bInSlot)
	{
		if(active_slot != NO_ACTIVE_SLOT) m_activeSlot = active_slot;
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
		m_activeSlot = NO_ACTIVE_SLOT;
	}	

	if(!Ruck(pIItem))return false;

	bool result = true;

	if(!FreeRuckRoom()) result = false;

	//поместить элемент обратно на место
	if(bInSlot)
	{
		if(active_slot != NO_ACTIVE_SLOT) m_activeSlot = active_slot;
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
		LuaOut	(Lua::eLuaMessageTypeError,"invalid inventory index!");
		return	(0);
	}
	R_ASSERT(false);
	return		(0);
}

CInventoryItem	*CInventory::GetItemFromInventory(LPCSTR caItemName)
{
	TIItemSet	&l_list = m_all;
	for(PSPIItem l_it = l_list.begin(); l_list.end() != l_it; ++l_it)
		if (!strcmp((*l_it)->cName(),caItemName))
			return	(*l_it);
	LuaOut	(Lua::eLuaMessageTypeError,"Object with name %s is not found in the %s inventory!",caItemName,dynamic_cast<CGameObject*>(m_pOwner)->cName());
	return	(0);
}


// CInventorySlot class //////////////////////////////////////////////////////////////////////////

CInventorySlot::CInventorySlot() 
{
	m_pIItem = NULL;
}

CInventorySlot::~CInventorySlot() 
{
}