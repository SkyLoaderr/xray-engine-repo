#pragma once
#include "gameobject.h"
#include "..\xr_level_controller.h"

#define CMD_START	(1<<0)
#define CMD_STOP	(1<<1)

class CInventory;
class CInventoryItem;
typedef CInventoryItem* PIItem;
typedef set<PIItem> TIItemSet;
typedef list<PIItem> TIItemList;
typedef TIItemSet::iterator PSPIItem;
typedef TIItemList::iterator PPIItem;

///////////////////////////////////////////////////////////////////////////////////////////////////
class CInventoryItem : public CGameObject {				// Предок объектов инвентаря
public:
	CInventoryItem();
	virtual ~CInventoryItem();

	virtual void Load(LPCSTR section);

	virtual const char* Name();
	virtual const char* NameShort();
	virtual char* NameComplex();

	virtual bool Useful();									// !!! Переопределить. (см. в Inventory.cpp)
	virtual bool Attach(PIItem pIItem, bool force = false);	// !!! Переопределить. (см. в Inventory.cpp)
	virtual bool Detach(PIItem pIItem, bool force = true);	// !!! Переопределить. (см. в Inventory.cpp)
	virtual bool Activate();								// !!! Переопределить. (см. в Inventory.cpp)
	virtual void Deactivate();								// !!! Переопределить. (см. в Inventory.cpp)
	virtual bool Action(s32 cmd, u32 flags) {return false;}	// true если известная команда, иначе false
	virtual void OnAnimationEnd() {}
	virtual void OnVisible() {}
	virtual s32 Sort(PIItem pIItem);						// !!! Переопределить. (см. в Inventory.cpp)
	virtual bool Merge(PIItem pIItem);						// !!! Переопределить. (см. в Inventory.cpp)

	bool DetachAll();										// Разобрать иерархию объектов. Объект должен быть в рюкзаке
	void Drop();											// Если объект в инвенторе, то он будет выброшен

	f32 m_weight;											// Вес объекта
	u32 m_slot;												// Слот в который можно установить объект (0xffffffff если нельзя)
	bool m_belt;											// Может ли объект быть на поясе
	TIItemList m_subs;										// Присоединенные объекты
	CInventory *m_pInventory;								// Указатель на инвентарь. Всегда полезно знать где находишься :)
	const char *m_name, *m_nameShort;
	char m_nameComplex[255];
	bool m_showHUD;
	bool m_drop;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class CInventorySlot {									// Слот
public:
	CInventorySlot();
	virtual ~CInventorySlot();

	PIItem m_pIItem;
	LPCSTR m_name;
};
typedef vector<CInventorySlot> TISlotArr;

///////////////////////////////////////////////////////////////////////////////////////////////////
class CInventory {										// Инвентарь
public:
	CInventory();
	virtual ~CInventory();

	f32 TotalWeight();										// Вес инвенторя
	bool Take(CGameObject *pObj);							// Взять объект. Объект попадает в рюкзак
	bool Drop(CGameObject *pObj);							// Выбросить объект
	bool Slot(PIItem pIItem);								// Вставить объект себе в слот. То что было - на пояс. Нельзя - в рюкзак
	bool Belt(PIItem pIItem);								// Повесить объект на пояс
	bool Ruck(PIItem pIItem);								// Полжить объект в рюкзак
	bool Activate(u32 slot);								// Активировать объект в указанном слоте
	PIItem ActiveItem();									// Возвращает указатель на объект в актовном слоте
	bool Action(s32 cmd, u32 flags);						// true если известная команда, иначе false
	void Update(u32 deltaT);								// Обновление
	PIItem Same(const PIItem pIItem);						// Ищет на поясе аналогичный IItem
	PIItem Get(const char *name);							// Ищет на поясе IItem с указанным именем (cName())

	TIItemSet m_all;										// Наборы объектов. m_all - все
	TIItemList m_ruck, m_belt;								// Списки объектов. m_ruck - рюкзак, m_belt - пояс
	TISlotArr m_slots;										// Слоты (фиксированное кол-во)
	u32 m_activeSlot, m_nextActiveSlot;						// Активный слот и слот который станет активным после смены
	f32 m_maxWeight;										// Максимальный вес инвентаря
	u32 m_maxRuck, m_maxBelt;								// Максимальное кол-во объектов в рюкзаке и на поясе
	f32 m_takeDist;											// Максимальное расстояние на котором можно подобрать объект
	PIItem m_pTarget;										// Объект на который наведен прицел
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class CInventoryOwner {									// Примесь для актеров и неписей имеющих инвентарь
public:
	CInventoryOwner();
	virtual ~CInventoryOwner();

	CInventory m_inventory;									// Инвентарь
};
