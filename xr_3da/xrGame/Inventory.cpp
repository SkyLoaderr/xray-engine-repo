#include "stdafx.h"
#include "inventory.h"
#include "actor.h"
#include "trade.h"

// CInventoryItem class ///////////////////////////////////////////////////////////////////////////////

CInventoryItem::CInventoryItem() 
{
	m_weight = 100.f;
	m_slot = 0xffffffff;
	m_belt = false;
	m_showHUD = true;
	m_pInventory = NULL;
	m_drop = false;
	m_ruck = true;
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
	m_sIconTexture = pSettings->r_string(section, "inv_icon");
	m_iGridWidth = pSettings->r_u32(section, "inv_grid_width");
	m_iGridHeight = pSettings->r_u32(section, "inv_grid_height");
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

	for(PPIItem l_it = m_subs.begin(); l_it != m_subs.end(); l_it++) 
	{
		const char *l_subName = (*l_it)->NameShort();
		if(l_subName) 
			strcpy(&m_nameComplex[strlen(m_nameComplex)], l_subName);
	}
	return m_nameComplex;
}

bool CInventoryItem::Useful() 
{
	// Если IItem нельзя использовать, вернуть false
	return true;
}

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
}

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
}

bool CInventoryItem::DetachAll() 
{
	if(!m_pInventory || std::find(m_pInventory->m_ruck.begin(), m_pInventory->m_ruck.end(), this) == m_pInventory->m_ruck.end()) return false;
	for(PPIItem l_it = m_subs.begin(); l_it != m_subs.end(); l_it++) 
	{
		Detach(*l_it);
		//m_pInventory->m_ruck.insert(m_pInventory->m_ruck.end(), *l_it);
		(*l_it)->DetachAll();
	}
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

s32 CInventoryItem::Sort(PIItem pIItem) 
{
	// Если нужно разместить IItem после this - вернуть 1, если
	// перед - -1. Если пофиг то 0.
	return 0;
}

bool CInventoryItem::Merge(PIItem pIItem) 
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
		Position().set(XFORM().c);
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
///////////////////////////////////////////
// CEatableItem class 
///////////////////////////////////////////

CEatableItem::CEatableItem()
{
	m_fHealthInfluence = 0;
	m_fPowerInfluence = 0;
	m_fSatietyInfluence = 0;
	m_fRadiationInfluence = 0;
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
}


// CInventory class ///////////////////////////////////////////////////////////////////////////////

void MergeSame(TIItemList &itemList) 
{
	PPIItem l_it = itemList.begin(), l_it2;
	
	while(l_it != itemList.end()) 
	{
		l_it2 = l_it; l_it2++;
		while(l_it2 != itemList.end()) 
		{
			(*l_it2)->Merge(*l_it);
			if(!(*l_it)->Useful()) break;
			l_it2++;
		}
		if(!(*l_it)->Useful()) (*l_it)->Drop();
		l_it++;
	}
}

void SortRuckAndBelt(CInventory *pInventory) 
{
	for(int i = 0; i < 2; i++) 
	{
		TIItemList &l_list = i?pInventory->m_ruck:pInventory->m_belt;
		PPIItem l_it = l_list.begin(), l_it2, l_first, l_last, l_erase;
		
		while(l_it != l_list.end()) 
		{
			l_first = l_last = l_it; l_last++;
			while((l_last != l_list.end()) && ((*l_first)->Sort(*l_last) > 0)) 
						l_last++;
			
			l_it2 = l_last;
			while(l_it2 != l_list.end()) 
			{
				l_erase = l_list.end();
				s32 l_sort = (*l_first)->Sort(*l_it2);
				if(l_sort > 0)
				{
					l_list.insert(l_last, *l_it2); l_erase = l_it2;
				} 
				else if(l_sort < 0) 
				{
					l_first = l_list.insert(l_first, *l_it2); l_erase = l_it2;
				}
				
				l_it2++;
				if(l_erase != l_list.end()) 
								l_list.erase(l_erase);
			}
			l_it++;
		}
		MergeSame(l_list);
	}
}

//проверяет есть ли доступное место в рюкзаке
//для вещи
//вещи размещаются по тому же принципу, что и 
//в UIInventoryWnd: сначала самые объемные

#define RUCK_HEIGHT 8
#define RUCK_WIDTH 7


//сравнивает элементы по пространству занимаемому ими в рюкзаке
bool CInventory::GreaterRoomInRuck(PIItem item1, PIItem item2)
{
	int item1_room = item1->m_iGridWidth*item1->m_iGridHeight;
	int item2_room = item2->m_iGridWidth*item2->m_iGridHeight;

	if(item1_room > item2_room)
		return true;
	else if (item1_room == item2_room)
	{
		if(item1->m_iGridWidth >= item2->m_iGridWidth)
			return true;
	}

	return false;
}


bool CInventory::FreeRuckRoom() 
{
	bool ruck_room[RUCK_HEIGHT][RUCK_WIDTH];

	int i,j,k,m;
	int place_row = 0,  place_col = 0;
	bool found_place;
	bool can_place;


	for(i=0; i<RUCK_HEIGHT; i++)
		for(j=0; j<RUCK_WIDTH; j++)
			ruck_room[i][j] = false;


	ruck_list = m_ruck;
	
	ruck_list.sort(GreaterRoomInRuck);
	
	found_place = true;

	for(PPIItem it = ruck_list.begin(); (it != ruck_list.end()) && found_place; it++) 
	{
		PIItem pItem = *it;

		//проверить можно ли разместить элемент,
		//проверяем последовательно каждую клеточку
		found_place = false;
	
		for(i=0; (i<RUCK_HEIGHT - pItem->m_iGridHeight+1) && !found_place; i++)
		{
			for(j=0; (j<RUCK_WIDTH - pItem->m_iGridWidth +1) && !found_place; j++)
			{
				can_place = true;

				for(k=0; (k<pItem->m_iGridHeight) && can_place; k++)
				{
					for(m=0; (m<pItem->m_iGridWidth) && can_place; m++)
					{
						if(ruck_room[i+k][j+m])
								can_place =  false;
					}
				}
			
				if(can_place)
				{
					found_place=true;	
					place_row = i;
					place_col = j;
				}

			}
		}

		//разместить элемент на найденном месте
		if(found_place)
		{
			for(k=0; k<pItem->m_iGridHeight; k++)
			{
				for(m=0; m<pItem->m_iGridWidth; m++)
				{
					ruck_room[place_row+k][place_col+m] = true;
				}
			}
		}
	}

	//для какого-то элемента места не нашлось
	if(!found_place) return false;

	return true;
}

CInventory::CInventory() 
{
	m_takeDist = pSettings->r_float("inventory","take_dist"); // 2.f;
	m_maxWeight = pSettings->r_float("inventory","max_weight"); // 40.f;
	m_maxRuck = pSettings->r_s32("inventory","max_ruck"); // 50;
	m_maxBelt = pSettings->r_s32("inventory","max_belt"); // 15;
	u32 l_slotsNum = pSettings->r_s32("inventory","slots"); // 7;			// 6 слотов оружия и слот одежды/защиты.
	m_slots.resize(l_slotsNum);
	m_activeSlot = m_nextActiveSlot = 0xffffffff;
	u32 i = 0; string256 temp;
	
	do 
	{
		sprintf(temp, "slot_name_%d", i+1);
		if(i < m_slots.size() && pSettings->line_exist("inventory",temp)) 
		{
			m_slots[i].m_name = pSettings->r_string("inventory",temp);
			i++;
		} 
		else break;
	} while(true);
	m_pTarget = NULL;
}

CInventory::~CInventory() 
{
}

bool CInventory::Take(CGameObject *pObj, bool bNotActivate) 
{
	CInventoryItem *l_pIItem = dynamic_cast<CInventoryItem*>(pObj);
	//if(l_pIItem && l_pIItem->Useful() && (l_pIItem->m_weight + TotalWeight() < m_maxWeight) && (m_ruck.size() < m_maxRuck) && (m_all.find(l_pIItem) == m_all.end())) {
	if(m_all.find(l_pIItem) != m_all.end()) 
	{
		Debug.fatal("Item already exist in inventory: %s(%s)",pObj->cName(),pObj->cNameSect());
	}
	if(l_pIItem && l_pIItem->Useful() && 
		(l_pIItem->m_weight + TotalWeight() < m_maxWeight) && 
		(m_all.find(l_pIItem) == m_all.end())) 
	{
		l_pIItem->m_pInventory = this;
		l_pIItem->m_drop = false;
		m_all.insert(l_pIItem);
		
		if(l_pIItem->m_ruck) 
			m_ruck.insert(m_ruck.end(), l_pIItem); 
		
		SortRuckAndBelt(this);
		TIItemList l_subs; 
		l_subs.insert(l_subs.end(), l_pIItem->m_subs.begin(), l_pIItem->m_subs.end());
		
		while(l_subs.size()) 
		{
			l_pIItem = *l_subs.begin();
			l_pIItem->m_pInventory = this;
			m_all.insert(l_pIItem);
			l_subs.insert(l_subs.end(), l_pIItem->m_subs.begin(), l_pIItem->m_subs.end());
			l_subs.erase(l_subs.begin());
		}
		if(!Slot(l_pIItem)) 
		{
			if(l_pIItem->m_slot < 0xffffffff) 
			{
				/*if(!m_slots[l_pIItem->m_slot].m_pIItem) {
					Slot(l_pIItem);
					return true;
				} else*/
				if(m_slots[l_pIItem->m_slot].m_pIItem->Attach(l_pIItem)) 
				{
					m_ruck.erase(std::find(m_ruck.begin(), m_ruck.end(), l_pIItem));
					return true;
				} 
				else if(m_ruck.size() > m_maxRuck || !l_pIItem->m_ruck || !FreeRuckRoom()) 
				{
					if(Belt(l_pIItem)) 
						return true;
					else 
						return !Drop(l_pIItem);
				}
			} 
			else if(m_ruck.size() > m_maxRuck || !FreeRuckRoom()) 
			{
				if(Belt(l_pIItem)) 
					return true;
				else 
					return !Drop(l_pIItem);
			}
		} else if(m_activeSlot == 0xffffffff && !bNotActivate) Activate(l_pIItem->m_slot);
		return true;
	}
	return false;
}

bool CInventory::Drop(CGameObject *pObj) 
{
	CInventoryItem *l_pIItem = dynamic_cast<CInventoryItem*>(pObj);
	
	if(l_pIItem && (m_all.find(l_pIItem) != m_all.end()) && Ruck(l_pIItem)) 
	{
		m_ruck.erase(std::find(m_ruck.begin(), m_ruck.end(), l_pIItem)); 
		m_all.erase(l_pIItem);
		l_pIItem->m_pInventory = NULL;
		
		TIItemList l_subs; 
		l_subs.insert(l_subs.end(), l_pIItem->m_subs.begin(), l_pIItem->m_subs.end());
		
		while(l_subs.size()) 
		{
			l_pIItem = *l_subs.begin();
			l_pIItem->m_pInventory = NULL;
			m_all.erase(l_pIItem);
			l_subs.insert(l_subs.end(), l_pIItem->m_subs.begin(), l_pIItem->m_subs.end());
			l_subs.erase(l_subs.begin());
		}
		return true;
	}
	return false;
}

bool CInventory::DropAll() 
{
	PSPIItem l_it;
	
	for(l_it = m_all.begin(); l_it != m_all.end(); l_it++) 
	{
		PIItem l_pIItem = *l_it;
		Ruck(l_pIItem); 
		l_pIItem->Drop();
	}
	return true;
}

void CInventory::ClearAll() 
{
	PSPIItem l_it;
	
	for(l_it = m_all.begin(); l_it != m_all.end(); l_it++) 
	{
		PIItem l_pIItem = *l_it;
		Ruck(l_pIItem);
	}
	m_ruck.clear();
	m_all.clear();
}

bool CInventory::Slot(PIItem pIItem) 
{
	if(pIItem->m_slot < m_slots.size()) 
	{
		//if(m_slots[pIItem->m_slot].m_pIItem && !Belt(m_slots[pIItem->m_slot].m_pIItem)) Ruck(m_slots[pIItem->m_slot].m_pIItem);
		if(!m_slots[pIItem->m_slot].m_pIItem) 
		{
			m_slots[pIItem->m_slot].m_pIItem = pIItem;
			PPIItem l_it = std::find(m_ruck.begin(), m_ruck.end(), pIItem); 
			
			if(l_it != m_ruck.end()) m_ruck.erase(l_it);
			
			//by Dandy, also perform search on the belt
			 l_it = std::find(m_belt.begin(), m_belt.end(), pIItem); 
			 if(l_it != m_belt.end()) m_belt.erase(l_it);
			
			return true;
		} 
		else 
		{
			if(m_slots[pIItem->m_slot].m_pIItem->Attach(pIItem)) 
			{
				PPIItem l_it = std::find(m_ruck.begin(), m_ruck.end(), pIItem); 
				if(l_it != m_ruck.end()) m_ruck.erase(l_it);

				//by Dandy, also perform search on the belt
				 l_it = std::find(m_belt.begin(), m_belt.end(), pIItem); 
				 if(l_it != m_belt.end()) m_belt.erase(l_it);

				return true;
			}
		}
	} 
	else 
	{
		for(u32 i = 0; i < m_slots.size(); i++) 
			if(m_slots[i].m_pIItem && m_slots[i].m_pIItem->Attach(pIItem)) 
			{
				PPIItem l_it = std::find(m_ruck.begin(), m_ruck.end(), pIItem); 
				if(l_it != m_ruck.end()) m_ruck.erase(l_it);
				return true;
			}
	}
	return false;//Belt(pIItem);
}

bool CInventory::Belt(PIItem pIItem) 
{
	if(!pIItem || !pIItem->m_belt) return false;
	if(m_belt.size() == m_maxBelt) return false;
	
	if(std::find(m_belt.begin(), m_belt.end(), pIItem) != m_belt.end()) return true;
	
	if((pIItem->m_slot < m_slots.size()) && (m_slots[pIItem->m_slot].m_pIItem == pIItem)) 
	{
		if(m_activeSlot == pIItem->m_slot) Activate(0xffffffff);
		m_slots[pIItem->m_slot].m_pIItem = NULL;
	}
	
	PPIItem l_it = std::find(m_ruck.begin(), m_ruck.end(), pIItem); 
	
	if(l_it != m_ruck.end()) m_ruck.erase(l_it);
	m_belt.insert(m_belt.end(), pIItem); 
	SortRuckAndBelt(this);
	
	return true;
}

bool CInventory::Ruck(PIItem pIItem) 
{

	if(!pIItem || !pIItem->m_ruck) return false;
	if(std::find(m_ruck.begin(), m_ruck.end(), pIItem) != m_ruck.end()) return true;
	
	if((pIItem->m_slot < m_slots.size()) && (m_slots[pIItem->m_slot].m_pIItem == pIItem)) 
	{
		if(m_activeSlot == pIItem->m_slot) 
            Activate(0xffffffff);
		m_slots[pIItem->m_slot].m_pIItem = NULL;
	}
	
	PPIItem l_it = std::find(m_belt.begin(), m_belt.end(), pIItem); 
	
	if(l_it != m_belt.end()) m_belt.erase(l_it);
	m_ruck.insert(m_ruck.end(), pIItem); 
	SortRuckAndBelt(this);
	return true;
}

bool CInventory::Activate(u32 slot) 
{
	if(/*(slot == 0xffffffff) || */(slot == m_activeSlot) || 
		(slot < m_slots.size() && !m_slots[slot].m_pIItem)) 
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

bool CInventory::Action(s32 cmd, u32 flags) 
{
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
				CActor *l_pA = dynamic_cast<CActor*>(m_pOwner);
				if(l_pA) 
				{
					NET_Packet P;
					l_pA->u_EventGen(P,GE_OWNERSHIP_TAKE,l_pA->ID());
					P.w_u16(u16(m_pTarget->ID()));
					l_pA->u_EventSend(P);
				}
			}
		} break;
	}
	return false;
}

void CInventory::Update(u32 deltaT) 
{
	if((m_nextActiveSlot < m_slots.size()) && (m_activeSlot == 0xffffffff || 
		!m_slots[m_activeSlot].m_pIItem->getVisible()))
	{
		if(m_slots[m_nextActiveSlot].m_pIItem)
		{
			if(m_slots[m_nextActiveSlot].m_pIItem->Activate())
			{
				m_activeSlot = m_nextActiveSlot;
				m_nextActiveSlot = 0xffffffff;
			}
			else if(!m_slots[m_activeSlot].m_pIItem->Activate())
			{
				m_activeSlot = m_nextActiveSlot = 0xffffffff;
			}
		}
		else m_activeSlot = m_nextActiveSlot = 0xffffffff;
	}
	
	// Смотрим, что тут можно подобрать
	CActor *l_pA = dynamic_cast<CActor*>(m_pOwner);
	if(l_pA) 
	{
		l_pA->setEnabled(false);
		Collide::ray_query	l_rq;
		if(g_pGameLevel->ObjectSpace.RayPick(Device.vCameraPosition, Device.vCameraDirection, m_takeDist, l_rq)) 
			m_pTarget = dynamic_cast<PIItem>(l_rq.O);
		else 
			m_pTarget = NULL;
		l_pA->setEnabled(true);
	}
	
	//проверить рюкзак и пояс
	for(int i = 0; i < 2; i++) 
	{
		TIItemList &l_list = i?m_ruck:m_belt;
		PPIItem l_it = l_list.begin();
	
		while(l_it != l_list.end()) 
		{
			PIItem l_pIItem = *l_it;
			if(l_pIItem->m_drop) 
			{
				l_pIItem->m_drop = false;
				if(l_pIItem->H_Parent())
				{
					NET_Packet P;
					l_pIItem->u_EventGen(P, GE_OWNERSHIP_REJECT, 
										l_pIItem->H_Parent()->ID());
					P.w_u16(u16(l_pIItem->ID()));
					l_pIItem->u_EventSend(P);
				} 
				else 
					Drop(l_pIItem);
			}
			l_it++;
		}
	}

	//проверить слоты
	for(i=0; i<(int)m_slots.size(); i++)
	{
		PIItem l_pIItem = m_slots[i].m_pIItem;

		if(l_pIItem && l_pIItem->m_drop) 
		{
			l_pIItem->m_drop = false;
			if(l_pIItem->H_Parent())
			{
				NET_Packet P;
				l_pIItem->u_EventGen(P, GE_OWNERSHIP_REJECT, 
									l_pIItem->H_Parent()->ID());
				P.w_u16(u16(l_pIItem->ID()));
				l_pIItem->u_EventSend(P);
			} 
			else 
				Drop(l_pIItem);
		}
	}
}

PIItem CInventory::Same(const PIItem pIItem) 
{
	for(PPIItem l_it = m_belt.begin(); l_it != m_belt.end(); l_it++) 
	{
		PIItem l_pIItem = *l_it;
		//if((l_pIItem != pIItem) && (l_pIItem->SUB_CLS_ID == pIItem->SUB_CLS_ID)) return l_pIItem;
		if((l_pIItem != pIItem) && 
				!strcmp(l_pIItem->cNameSect(), 
				pIItem->cNameSect())) 
			return l_pIItem;
	}
	return NULL;
}

PIItem CInventory::SameSlot(u32 slot) 
{
	if(slot == 0xffffffff) 
		return NULL;
	
	for(PPIItem l_it = m_belt.begin(); l_it != m_belt.end(); l_it++) 
	{
		PIItem l_pIItem = *l_it;
		if(l_pIItem->m_slot == slot) 
			return l_pIItem;
	}
	return NULL;
}

PIItem CInventory::Get(const char *name, bool bSearchRuck) 
{
	TIItemList &l_list = bSearchRuck ? m_ruck : m_belt;
	
	for(PPIItem l_it = l_list.begin(); l_it != l_list.end(); l_it++) 
	{
		PIItem l_pIItem = *l_it;
		if(!strcmp(l_pIItem->cNameSect(), name) && 
								l_pIItem->Useful()) 
				return l_pIItem;
	}
	return NULL;
}

PIItem CInventory::Get(const u16 id, bool bSearchRuck) 
{
	TIItemList &l_list = bSearchRuck ? m_ruck : m_belt;

	for(PPIItem l_it = l_list.begin(); l_it != l_list.end(); l_it++) 
	{
		PIItem l_pIItem = *l_it;
		if(l_pIItem->ID() == id) 
			return l_pIItem;
	}
	return NULL;
}

f32 CInventory::TotalWeight() 
{
	f32 l_weight = 0;
	for(PSPIItem l_it = m_all.begin(); l_it != m_all.end(); l_it++) 
					l_weight += (*l_it)->m_weight;
	return l_weight;
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
	for(PPIItem l_it = l_list.begin(); l_it != l_list.end(); l_it++) 
	{
		PIItem	l_pIItem = *l_it;
		if (!strcmp(l_pIItem->cNameSect(), caSection))
            l_dwCount++;
	}
	
	return		(l_dwCount);
}

//скушать предмет 
bool CInventory::Eat(PIItem pIItem)
{
	//устанаовить съедобна ли вещь
	CEatableItem* pItemToEat = dynamic_cast<CEatableItem*>(pIItem);
	if(!pItemToEat) return false;

	CEntityCondition *pCondition = dynamic_cast<CEntityCondition*>(m_pOwner);

	if(!pCondition) return false;

	pCondition->ChangeHealth(pItemToEat->m_fHealthInfluence);
	pCondition->ChangePower(pItemToEat->m_fPowerInfluence);
	pCondition->ChangeSatiety(pItemToEat->m_fSatietyInfluence);
	pCondition->ChangeRadiation(pItemToEat->m_fRadiationInfluence);
	
	
	//уничтожить вещь
	pIItem->Drop();
	
	NET_Packet P;
	CGameObject* pObject = dynamic_cast<CGameObject*>(m_pOwner);
	pObject->u_EventGen(P,GE_DESTROY,pIItem->ID());
	P.w_u16(u16(pIItem->ID()));
	pObject->u_EventSend(P);


	return true;
}

// CInventorySlot class //////////////////////////////////////////////////////////////////////////

CInventorySlot::CInventorySlot() 
{
	m_pIItem = NULL;
}

CInventorySlot::~CInventorySlot() 
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// CInventoryOwner class //////////////////////////////////////////////////////////////////////////
CInventoryOwner::CInventoryOwner()
 {
	m_inventory.m_pOwner		= this;
	m_trade_storage.m_pOwner	= this;
	m_trade						= NULL;

	m_dwMoney					= 0;
	m_tRank						= eStalkerRankNone;
}

CInventoryOwner::~CInventoryOwner() 
{
}
