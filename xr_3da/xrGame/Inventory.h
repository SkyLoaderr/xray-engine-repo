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
};
typedef xr_vector<CInventorySlot> TISlotArr;

///////////////////////////////////////////////////////////////////////////////////////////////////
class CInventoryOwner;

class CInventory						// Инвентарь 
{				
public:
	CInventory();
	virtual ~CInventory();

	float TotalWeight() const;										// Вес инвенторя
	bool Take(CGameObject *pObj, bool bNotActivate = false);// Взять объект. Объект попадает в рюкзак, если bNotActivate == false, то активировать объект, если у владельца инвентаря ничего не активировано (e.g. трейдер)
	bool Drop(CGameObject *pObj, bool call_drop = true);							// Выбросить объект
	bool DropAll();											// Выбросить все
	void ClearAll();										// Очистить всё
	
	
	bool Slot(PIItem pIItem, bool bNotActivate = false);	// Вставить объект себе в слот. То что было - на пояс. Нельзя - в рюкзак
	bool Belt(PIItem pIItem);								// Повесить объект на пояс
	bool Ruck(PIItem pIItem);								// Полжить объект в рюкзак

	//проверяет находится ли элемент в части инвенторя
	bool InSlot(PIItem pIItem) const;
	bool InBelt(PIItem pIItem) const;
	bool InRuck(PIItem pIItem) const;

	//можно ли положить элемент в слот, рюкзак или на пояс
	bool CanPutInSlot(PIItem pIItem) const;
	bool CanPutInBelt(PIItem pIItem);
	bool CanPutInRuck(PIItem pIItem);

		
	bool Activate(u32 slot);								// Активировать объект в указанном слоте
	PIItem ActiveItem() const;								// Возвращает указатель на объект в актовном слоте

	bool Action(s32 cmd, u32 flags);						// true если известная команда, иначе false
	void Update();											// Обновление

	PIItem Same(const PIItem pIItem, bool bSearchRuck) const;// Ищет на поясе аналогичный IItem
	PIItem SameSlot(u32 slot,	 bool bSearchRuck) const;	// Ищет на поясе IItem для указанного слота
	PIItem Get(const char *name, bool bSearchRuck) const;	// Ищет на поясе или в рюкзаке IItem с указанным именем (cName())
	PIItem Get(const u32  id,	 bool bSearchRuck) const;	// Ищет на поясе или в рюкзаке IItem с указанным именем (id)
	PIItem Get(CLASS_ID cls_id,  bool bSearchRuck) const;	// Ищет на поясе или в рюкзаке IItem с указанным CLS_ID
	void   Clear();											// clearing Inventory
	virtual u32		dwfGetSameItemCount(LPCSTR caSection);	// get all the items with the same section name
	virtual bool	bfCheckForObject(_OBJECT_ID tObjectID);	// get all the items with the same object id
			u32		dwfGetObjectCount();
	CInventoryItem	*tpfGetObjectByIndex(int iIndex);
	CInventoryItem	*GetItemFromInventory(LPCSTR caItemName);

	bool Eat(PIItem pIItem);								// скушать предмет :)

	u32 GetActiveSlot() const {return m_activeSlot;}

	bool IsSlotsUseful() {return m_bSlotsUseful;}	 
	void SetSlotsUseful(bool slots_useful) {m_bSlotsUseful = slots_useful;}
	bool IsBeltUseful() {return m_bBeltUseful;}
	void SetBeltUseful(bool belt_useful) {m_bBeltUseful = belt_useful;}

	TIItemSet m_all;										// Наборы объектов. m_all - все
	TIItemList m_ruck, m_belt;								// Списки объектов. m_ruck - рюкзак, m_belt - пояс
	TISlotArr m_slots;										// Слоты (фиксированное кол-во)

	float GetTakeDist() {return m_takeDist;}
	
	float GetMaxWeight() {return m_maxWeight;}
	void  SetMaxWeight(float weight) {m_maxWeight = weight;}

	u32  GetMaxRuck() {return m_maxRuck;}
	void SetMaxRuck(u32 max_ruck) {m_maxRuck = max_ruck;}
	u32 GetMaxBelt() {return m_maxBelt;}

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
	u32 m_activeSlot;
	u32 m_nextActiveSlot;

	CInventoryOwner *m_pOwner;

	//буферный список для сортировки
	TIItemList ruck_list;

	//флаг, показывающий наличие пояса в инвенторе
	bool m_bBeltUseful;
	//флаг, допускающий использование слотов
	bool m_bSlotsUseful;

	// Максимальный вес инвентаря
	float m_maxWeight;
	// Максимальное кол-во объектов в рюкзаке и 
	u32 m_maxRuck;
	//на поясе
	u32 m_maxBelt;	
	// Максимальное расстояние на котором можно подобрать объект
	float m_takeDist;



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


///////////////////////////////////////////////////////////////////////////////////////////////////
//для владельцев инвенторя
//#include "InventoryOwner.h"