#pragma once
#include "gameobject.h"
#include "inventory_item.h"

class CInventory;
class CInventoryItem;

///////////////////////////////////////////////////////////////////////////////////////////////////
class CInventorySlot					// Слот
{									
public:
	CInventorySlot();
	virtual ~CInventorySlot();

	PIItem	m_pIItem;
	ref_str m_name;
	//может ли слот стать активным
	bool m_bCanBeActivated;
};
typedef xr_vector<CInventorySlot> TISlotArr;

///////////////////////////////////////////////////////////////////////////////////////////////////
class CInventoryOwner;

class CInventory						// Инвентарь 
{				
public:
	CInventory();
	virtual ~CInventory();

	// Вес инвенторя
	float TotalWeight() const;										
	// Взять объект. Объект попадает в рюкзак, 
	//если bNotActivate == false, то активировать объект, 
	//если у владельца инвентаря ничего не активировано (e.g. трейдер)
	bool Take(CGameObject *pObj, bool bNotActivate = false);
	//Выбросить предмет
	bool Drop(CGameObject *pObj, bool call_drop = true);							// Выбросить объект
	// Выбросить все
	bool DropAll();	
	// Очистить всё
	void ClearAll();
	
	// Вставить объект себе в слот. То что было - на пояс. Нельзя - в рюкзак
	bool Slot(PIItem pIItem, bool bNotActivate = false);	
	// Повесить объект на пояс
	bool Belt(PIItem pIItem);
	// Полжить объект в рюкзак
	bool Ruck(PIItem pIItem);

	//проверяет находится ли элемент в части инвенторя
	bool InSlot(PIItem pIItem) const;
	bool InBelt(PIItem pIItem) const;
	bool InRuck(PIItem pIItem) const;

	//можно ли положить элемент в слот, рюкзак или на пояс
	bool CanPutInSlot(PIItem pIItem) const;
	bool CanPutInBelt(PIItem pIItem);
	bool CanPutInRuck(PIItem pIItem);

	// Активировать объект в указанном слоте	
	bool Activate(u32 slot);
	// Возвращает указатель на объект в актовном слоте
	PIItem ActiveItem() const;

	// true если известная команда, иначе false
	bool Action(s32 cmd, u32 flags);
	// Обновление на кадре
	void Update();

	// Ищет на поясе аналогичный IItem
	PIItem Same(const PIItem pIItem, bool bSearchRuck) const;
	// Ищет на поясе IItem для указанного слота
	PIItem SameSlot(u32 slot,	 bool bSearchRuck) const;
	// Ищет на поясе или в рюкзаке IItem с указанным именем (cName())
	PIItem Get(const char *name, bool bSearchRuck) const;
	// Ищет на поясе или в рюкзаке IItem с указанным именем (id)
	PIItem Get(const u16  id,	 bool bSearchRuck) const;
	// Ищет на поясе или в рюкзаке IItem с указанным CLS_ID
	PIItem Get(CLASS_ID cls_id,  bool bSearchRuck) const;
	
	// clearing Inventory
	void   Clear();											
	// get all the items with the same section name
	virtual u32		dwfGetSameItemCount(LPCSTR caSection);	
	// get all the items with the same object id
	virtual bool	bfCheckForObject(ALife::_OBJECT_ID tObjectID);	
	CInventoryItem	*CInventory::get_object_by_id(ALife::_OBJECT_ID tObjectID);

			u32		dwfGetObjectCount();
	CInventoryItem	*tpfGetObjectByIndex(int iIndex);
	CInventoryItem	*GetItemFromInventory(LPCSTR caItemName);

	// скушать предмет :)
	bool Eat(PIItem pIItem);								

	u32 GetActiveSlot() const {return m_iActiveSlot;}
	void SetActiveSlot (u32 ActiveSlot) { m_iActiveSlot = m_iNextActiveSlot = ActiveSlot; }

	bool IsSlotsUseful() {return m_bSlotsUseful;}	 
	void SetSlotsUseful(bool slots_useful) {m_bSlotsUseful = slots_useful;}
	bool IsBeltUseful() {return m_bBeltUseful;}
	void SetBeltUseful(bool belt_useful) {m_bBeltUseful = belt_useful;}

	// Наборы объектов. m_all - все
	TIItemSet		m_all;
	// Списки объектов. m_ruck - рюкзак, m_belt - пояс
	TIItemList		m_ruck, m_belt;
	// Слоты (фиксированное кол-во)
	TISlotArr		m_slots;

	float GetTakeDist() {return m_fTakeDist;}
	
	float GetMaxWeight() {return m_fMaxWeight;}
	void  SetMaxWeight(float weight) {m_fMaxWeight = weight;}

	u32  GetMaxRuck() {return m_iMaxRuck;}
	void SetMaxRuck(u32 max_ruck) {m_iMaxRuck = max_ruck;}
	u32 GetMaxBelt() {return m_iMaxBelt;}

	u32 RuckWidth() const;
	u32 RuckHeight() const;
	u32 BeltWidth() const;
	
	u32 TotalVolume() const;
	u32 GetMaxVolume() const;
	

	// Объект на который наведен прицел
	PIItem m_pTarget;

	friend class CInventoryOwner;

protected:
	//для проверки свободного места
	bool FreeRuckRoom();
	bool FreeBeltRoom();


	// Активный слот и слот который станет активным после смены
    //значения совпадают в обычном состоянии (нет смены слотов)
	u32 m_iActiveSlot;
	u32 m_iNextActiveSlot;

	CInventoryOwner *m_pOwner;

	//буферный список для сортировки
	TIItemList ruck_list;

	//флаг, показывающий наличие пояса в инвенторе
	bool m_bBeltUseful;
	//флаг, допускающий использование слотов
	bool m_bSlotsUseful;

	// Максимальный вес инвентаря
	float m_fMaxWeight;
	// Максимальное кол-во объектов в рюкзаке и 
	u32 m_iMaxRuck;
	//на поясе
	u32 m_iMaxBelt;	
	// Максимальное расстояние на котором можно подобрать объект
	float m_fTakeDist;



	//буфферный список
	TIItemList l_subs; 

	void		SendActionEvent		(s32 cmd, u32 flags);

public:
	bool CanTakeItem(CInventoryItem *inventory_item);
protected:
	xr_vector<CInventoryItem*>	m_item_buffer;
	xr_vector<CInventoryItem*>	m_slot_buffer;
	xr_vector<u64>				m_inventory_mask;

private:
	struct CItemVolumeSortPredicate {
		IC bool							operator()							(const CInventoryItem *inventory_item1, const CInventoryItem *inventory_item2)  const
		{
			return						(inventory_item1->GetVolume() > inventory_item2->GetVolume());
		};
	};

	struct CRemoveStoredItems {
		int								m_iMaxCount;
		xr_vector<CInventoryItem*>		*m_slots;

		CRemoveStoredItems				(xr_vector<CInventoryItem*> *slots, int dwMaxCount) : m_iMaxCount(dwMaxCount)
		{
			m_slots						= slots;
		}

		IC bool							operator()							(const CInventoryItem *inventory_item)
		{
			if ((inventory_item->GetSlot() < m_slots->size()) && ((*m_slots)[inventory_item->GetSlot()] == inventory_item))
				return					(true);
			
			if ((inventory_item->GetHeight() == 1) && (inventory_item->GetWidth() < m_iMaxCount)) {
				m_iMaxCount			-= inventory_item->GetWidth();
				return					(true);
			}

			return						(false);
		}
	};
};