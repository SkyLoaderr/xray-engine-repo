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


#define BOLT_SLOT 5
#define OUTFIT_SLOT 6
#define PDA_SLOT 7 


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

	virtual	void	Hit	(float P, Fvector &dir,	
								  CObject* who, s16 element,
								  Fvector position_in_object_space, 
								  float impulse, 
								  ALife::EHitType hit_type = eHitTypeWound);

	bool DetachAll();										// Разобрать иерархию объектов. Объект должен быть в рюкзаке
	void Drop();											// Если объект в инвенторе, то он будет выброшен

	u32	Cost() {return m_cost;}

	f32 m_weight;											// Вес объекта
	u32 m_slot;												// Слот в который можно установить объект (0xffffffff если нельзя)
	bool m_belt, m_ruck;									// Может ли объект быть на поясе или в рюкзаке
	TIItemList m_subs;										// Присоединенные объекты
	CInventory *m_pInventory;								// Указатель на инвентарь. Всегда полезно знать где находишься :)
	const char *m_name, *m_nameShort;
	char m_nameComplex[255];
	bool m_showHUD;
	bool m_drop;

	
	//const char* m_sIconTexture;								//текстура с иконкой для меню
	
	int m_iGridWidth;										//ширина в сетке инвенторя
	int m_iGridHeight;										//высота в сетке инвенторя

	int m_iXPos;											//позиция X в сетке инвенторя
	int m_iYPos;											//позиция Y в сетке инвенторя


	float GetCondition() {return m_fCondition;}
	void  ChangeCondition(float fDeltaCondition);
	
protected:
	u32	m_cost;												// цена по умолчанию
	
	//состояние вещи, 1.0 - полностью работоспособная
	// 0 - испорченная
	float m_fCondition;
	//флаг использования состояния для вещи
	bool m_bUsingCondition;

	//коэффициенты изменения типов хитов (задается только там где надо)
	float m_fK_Burn;
	float m_fK_Strike;
	float m_fK_Shock;
	float m_fK_Wound;
	float m_fK_Radiation;
	float m_fK_Telepatic;
	float m_fK_ChemicalBurn;
	float m_fK_Explosion;
	float m_fK_FireWound;


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
	virtual bool Useful();

	//влияние при поедании вещи на параметры игрока
	float	m_fHealthInfluence;
	float	m_fPowerInfluence;
	float	m_fSatietyInfluence;
	float	m_fRadiationInfluence;

	//количество порций еды, 
	//-1 - порция одна и больше не бывает (чтоб не выводить надпись в меню)
	int		m_iPortionsNum;
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

	//проверяет находится ли элемент в части инвенторя
	bool InSlot(PIItem pIItem);
	bool InBelt(PIItem pIItem);
	bool InRuck(PIItem pIItem);

	//можно ли положить элемент в слот, рюкзак или на пояс
	bool CanPutInSlot(PIItem pIItem);
	bool CanPutInBelt(PIItem pIItem);
	bool CanPutInRuck(PIItem pIItem);

		
	bool Activate(u32 slot);								// Активировать объект в указанном слоте
	PIItem ActiveItem() const;								// Возвращает указатель на объект в актовном слоте
	bool Action(s32 cmd, u32 flags);						// true если известная команда, иначе false
	void Update(u32 deltaT);								// Обновление
	PIItem Same(const PIItem pIItem);						// Ищет на поясе аналогичный IItem
	PIItem SameSlot(u32 slot);								// Ищет на поясе IItem для указанного слота
	PIItem Get(const char *name, bool bSearchRuck);			// Ищет на поясе или в рюкзаке IItem с указанным именем (cName())
	PIItem Get(const u32  id,	 bool bSearchRuck);			// Ищет на поясе или в рюкзаке IItem с указанным именем (id)
	void   Clear();											// clearing Inventory
	virtual u32		dwfGetSameItemCount(LPCSTR caSection);	// get all the items with the same section name
	virtual bool	bfCheckForObject(_OBJECT_ID tObjectID);	// get all the items with the same object id
			u32		dwfGetObjectCount();
	CInventoryItem	*tpfGetObjectByIndex(int iIndex);
	CInventoryItem	*GetItemFromInventory(LPCSTR caItemName);

	bool Eat(PIItem pIItem);								// скушать предмет :)

	TIItemSet m_all;										// Наборы объектов. m_all - все
	TIItemList m_ruck, m_belt;								// Списки объектов. m_ruck - рюкзак, m_belt - пояс
	TISlotArr m_slots;										// Слоты (фиксированное кол-во)
	u32 m_activeSlot, m_nextActiveSlot;						// Активный слот и слот который станет активным после смены
	f32 m_maxWeight;										// Максимальный вес инвентаря
	u32 m_maxRuck, m_maxBelt;								// Максимальное кол-во объектов в рюкзаке и на поясе
	f32 m_takeDist;											// Максимальное расстояние на котором можно подобрать объект
	PIItem m_pTarget;										// Объект на который наведен прицел

	//буфферный список
	TIItemList l_subs; 

	bool m_bBeltUseful;										//флаг, показывающий наличие пояса в инвенторе

	//для проверки свободного места
	bool FreeRuckRoom();
	bool FreeBeltRoom();
	
	//буферный список для сортировки
	TIItemList ruck_list;

	CInventoryOwner *m_pOwner;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//для владельцев инвенторя
#include "InventoryOwner.h"