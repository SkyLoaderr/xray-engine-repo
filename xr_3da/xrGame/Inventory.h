#pragma once
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
	float TotalWeight		() const;
	float CalcTotalWeight	();

	// Взять объект. Объект попадает в рюкзак, 
	//если bNotActivate == false, то активировать объект, 
	//если у владельца инвентаря ничего не активировано (e.g. трейдер)
	//strict_placement - сохранить порядок вещей, загруженный из серверного объекта,
	//иначе вещь ложится на первое доступное место
	bool Take(CGameObject *pObj, bool bNotActivate, bool strict_placement);
	//Выбросить предмет
	bool Drop(CGameObject *pObj, bool call_drop = true);							// Выбросить объект
	// Выбросить все
	bool DropAll();	
	// Очистить всё
	void Clear();
	//расположить предметы в инвентаре в соответствии с ItemPlace
	void ReplaceAll		();
	void Replace		(PIItem pItem);

	
	// Вставить объект себе в слот. То что было - на пояс. Нельзя - в рюкзак
	bool Slot(PIItem pIItem, bool bNotActivate = false);	
	// Повесить объект на пояс
	bool Belt(PIItem pIItem);
	// Полжить объект в рюкзак
	bool Ruck(PIItem pIItem);

	//проверяет находится ли элемент в части инвентаря
	bool InSlot(PIItem pIItem) const;
	bool InBelt(PIItem pIItem) const;
	bool InRuck(PIItem pIItem) const;

	//можно ли положить элемент в слот, рюкзак или на пояс
	bool CanPutInSlot(PIItem pIItem) const;
	bool CanPutInBelt(PIItem pIItem) const;
	bool CanPutInRuck(PIItem pIItem) const;

	//можно ли взять вещь
	bool CanTakeItem(CInventoryItem *inventory_item) const;


	// Активировать объект в указанном слоте	
	bool Activate(u32 slot);
	// Возвращает указатель на объект в активном слоте
	PIItem ActiveItem() const;
	// Возвращает указатель на объект в слоте указаным номером
	PIItem ItemFormSlot(u32 slot) const;

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
	PIItem Get	(CLASS_ID cls_id,  bool bSearchRuck) const;
	PIItem item (CLASS_ID cls_id) const;
	
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
	
	void SetPrevActiveSlot(u32 ActiveSlot)  {m_iPrevActiveSlot = ActiveSlot;}
	u32 GetPrevActiveSlot() const {return m_iPrevActiveSlot;}

	void SetActiveSlot (u32 ActiveSlot) { m_iActiveSlot = m_iNextActiveSlot = ActiveSlot; }

	bool IsSlotsUseful() const				{return m_bSlotsUseful;}	 
	void SetSlotsUseful	(bool slots_useful) {m_bSlotsUseful = slots_useful;}
	bool IsBeltUseful() const				{return m_bBeltUseful;}
	void SetBeltUseful(bool belt_useful)	{m_bBeltUseful = belt_useful;}

	// Наборы объектов. m_all - все
	TIItemSet		m_all;
	// Списки объектов. m_ruck - рюкзак, m_belt - пояс
	TIItemList		m_ruck, m_belt;
	// Слоты (фиксированное кол-во)
	TISlotArr		m_slots;

	//возвращает все кроме PDA в слоте и болта
	void AddAvailableItems(TIItemList& items_container) const;

	float GetTakeDist() const			{return m_fTakeDist;}
	
	float GetMaxWeight() const			{return m_fMaxWeight;}
	void  SetMaxWeight(float weight)	{m_fMaxWeight = weight;}

	u32  GetMaxRuck() const				{return m_iMaxRuck;}
	void SetMaxRuck(u32 max_ruck)		{m_iMaxRuck = max_ruck;}
	u32	 GetMaxBelt() const				{return m_iMaxBelt;}

	u32 RuckWidth() const;
	u32 RuckHeight() const;
	u32 BeltWidth() const;
	
	u32 TotalVolume() const;
	u32 GetMaxVolume() const;

	CInventoryOwner*	GetOwner() const { return m_pOwner; }
	

	// Объект на который наведен прицел
	PIItem m_pTarget;

	friend class CInventoryOwner;


	u32	ModifyFrame() const {return m_dwModifyFrame;}

protected:
	// Активный слот и слот который станет активным после смены
    //значения совпадают в обычном состоянии (нет смены слотов)
	u32 m_iActiveSlot;
	u32 m_iNextActiveSlot;
	u32 m_iPrevActiveSlot;

	CInventoryOwner *m_pOwner;

	//буферный список для сортировки
	TIItemList ruck_list;

	//флаг, показывающий наличие пояса в инвенторе
	bool m_bBeltUseful;
	//флаг, допускающий использование слотов
	bool m_bSlotsUseful;

	// максимальный вес инвентаря
	float m_fMaxWeight;
	// текущий вес в инвентаре
	float m_fTotalWeight;

	// Максимальное кол-во объектов в рюкзаке и 
	u32 m_iMaxRuck;
	//на поясе
	u32 m_iMaxBelt;	
	// Максимальное расстояние на котором можно подобрать объект
	float m_fTakeDist;

	//кадр на котором произошло последнее изменение в инвенторе
	u32	m_dwModifyFrame;


	//буфферный список
	TIItemList			l_subs; 
	xr_vector<PIItem>	drop_tasks;

	void		SendActionEvent		(s32 cmd, u32 flags);
};