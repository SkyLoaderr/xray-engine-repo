#pragma once
#include "gameobject.h"

#define CMD_START	(1<<0)
#define CMD_STOP	(1<<1)

class CInventory;
class CInventoryItem;
typedef CInventoryItem*			PIItem;
typedef xr_set<PIItem>			TIItemSet;
typedef xr_list<PIItem>			TIItemList;
typedef TIItemSet::iterator		PSPIItem;
typedef TIItemList::iterator	PPIItem;

///////////////////////////////////////////////////////////////////////////////////////////////////
class CInventoryItem : virtual public CGameObject 					// Предок объектов инвентаря
{
private:
	typedef CGameObject	inherited;
public:
	CInventoryItem();
	virtual ~CInventoryItem();

	virtual void Load(LPCSTR section);

	virtual const char* Name();
	virtual const char* NameShort();
	virtual char*	NameComplex();

	virtual bool	Useful();									// !!! Переопределить. (см. в Inventory.cpp)
	virtual bool	Attach(PIItem pIItem, bool force = false);	// !!! Переопределить. (см. в Inventory.cpp)
	virtual bool	Detach(PIItem pIItem, bool force = true);	// !!! Переопределить. (см. в Inventory.cpp)
	virtual bool	Activate();									// !!! Переопределить. (см. в Inventory.cpp)
	virtual void	Deactivate();								// !!! Переопределить. (см. в Inventory.cpp)
	virtual bool	Action(s32 cmd, u32 flags) {return false;}	// true если известная команда, иначе false
	virtual void	OnAnimationEnd() {}
	virtual void	renderable_Render() {}
	virtual s32		Sort(PIItem pIItem);						// !!! Переопределить. (см. в Inventory.cpp)
	virtual bool	Merge(PIItem pIItem);						// !!! Переопределить. (см. в Inventory.cpp)

	virtual void	OnH_B_Chield		();
	virtual void	OnH_B_Independent	();

	virtual void	UpdateCL	();

	bool DetachAll();										// Разобрать иерархию объектов. Объект должен быть в рюкзаке
	void Drop();											// Если объект в инвенторе, то он будет выброшен

	f32 m_weight;											// Вес объекта
	u32 m_slot;												// Слот в который можно установить объект (0xffffffff если нельзя)
	bool m_belt, m_ruck;									// Может ли объект быть на поясе или в рюкзаке
	TIItemList m_subs;										// Присоединенные объекты
	CInventory *m_pInventory;								// Указатель на инвентарь. Всегда полезно знать где находишься :)
	const char *m_name, *m_nameShort;
	char m_nameComplex[255];
	bool m_showHUD;
	bool m_drop;

	u32	m_cost;												// цена по умолчанию
	u32	Cost() {return m_cost;}


	const char* m_sIconTexture;								//текстура с иконкой для меню
	
	int m_iGridWidth;										//ширина в сетке инвенторя
	int m_iGridHeight;										//высота в сетке инвенторя

};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Съедобная вещь
class CEatableItem : public CInventoryItem 
{
private:
	typedef CInventoryItem	inherited;
public:
	CEatableItem();
	virtual ~CEatableItem();

	virtual void Load(LPCSTR section);

	//влияние при поедании вещи на параметры игрока
	float m_fHealthInfluence;
	float m_fPowerInfluence;
	float m_fSatietyInfluence;
	float m_fRadiationInfluence;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class CInventorySlot					// Слот
{									
public:
	CInventorySlot();
	virtual ~CInventorySlot();

	PIItem m_pIItem;
	LPCSTR m_name;
};
typedef xr_vector<CInventorySlot> TISlotArr;

///////////////////////////////////////////////////////////////////////////////////////////////////
class CInventoryOwner;

class CInventory						// Инвентарь 
{				
public:
	CInventory();
	virtual ~CInventory();

	f32 TotalWeight();										// Вес инвенторя
	bool Take(CGameObject *pObj, bool bNotActivate = false);// Взять объект. Объект попадает в рюкзак, если bNotActivate == false, то активировать объект, если у владельца инвентаря ничего не активировано (e.g. трейдер)
	bool Drop(CGameObject *pObj);							// Выбросить объект
	bool DropAll();											// Выбросить все
	void ClearAll();										// Очистить всё
	bool Slot(PIItem pIItem);								// Вставить объект себе в слот. То что было - на пояс. Нельзя - в рюкзак
	bool Belt(PIItem pIItem);								// Повесить объект на пояс
	bool Ruck(PIItem pIItem);								// Полжить объект в рюкзак
	bool Activate(u32 slot);								// Активировать объект в указанном слоте
	PIItem ActiveItem() const;								// Возвращает указатель на объект в актовном слоте
	bool Action(s32 cmd, u32 flags);						// true если известная команда, иначе false
	void Update(u32 deltaT);								// Обновление
	PIItem Same(const PIItem pIItem);						// Ищет на поясе аналогичный IItem
	PIItem SameSlot(u32 slot);								// Ищет на поясе IItem для указанного слота
	PIItem Get(const char *name, bool bSearchRuck);			// Ищет на поясе или в рюкзаке IItem с указанным именем (cName())
	PIItem Get(const u16  id,	 bool bSearchRuck);			// Ищет на поясе или в рюкзаке IItem с указанным именем (id)
	void   Clear();											// clearing Inventory
	virtual u32		dwfGetSameItemCount(LPCSTR caSection);	// get all the items with the same section name

	bool Eat(PIItem pIItem);								// скушать предмет :)

	TIItemSet m_all;										// Наборы объектов. m_all - все
	TIItemList m_ruck, m_belt;								// Списки объектов. m_ruck - рюкзак, m_belt - пояс
	TISlotArr m_slots;										// Слоты (фиксированное кол-во)
	u32 m_activeSlot, m_nextActiveSlot;						// Активный слот и слот который станет активным после смены
	f32 m_maxWeight;										// Максимальный вес инвентаря
	u32 m_maxRuck, m_maxBelt;								// Максимальное кол-во объектов в рюкзаке и на поясе
	f32 m_takeDist;											// Максимальное расстояние на котором можно подобрать объект
	PIItem m_pTarget;										// Объект на который наведен прицел


	//для проверки свободного места
	static bool GreaterRoomInRuck(PIItem item1, PIItem item2);
	bool FreeRuckRoom();
	//буферный список для сортировки
	TIItemList ruck_list;

	CInventoryOwner *m_pOwner;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class CTrade;

class CInventoryOwner		// Примесь для актеров и неписей имеющих инвентарь 
{							
public:
	CInventoryOwner();
	virtual ~CInventoryOwner();

	CInventory	m_inventory;									// Инвентарь
	CInventory	m_trade_storage;								// склад 
	
	CTrade		*m_trade;										// торговля

	// свойства
	u32					m_dwMoney;
	EStalkerRank		m_tRank;
};
