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

#define NO_ACTIVE_SLOT		0xffffffff
#define KNIFE_SLOT			0
#define PISTOL_SLOT			1
#define RIFLE_SLOT			2
#define GRENADE_SLOT		3
#define APPARATUS_SLOT		4
#define BOLT_SLOT			5
#define OUTFIT_SLOT			6
#define PDA_SLOT			7 


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

	
	virtual void OnEvent (NET_Packet& P, u16 type);
	
	virtual bool	Useful();									// !!! Переопределить. (см. в Inventory.cpp)
	virtual bool	Attach(PIItem /**pIItem/**/) {return false;}
	virtual bool	Detach(PIItem /**pIItem/**/) {return false;}
	//при детаче спаунится новая вещь при заданно названии секции
	virtual bool	Detach(const char* item_section_name);
	//проверяет может ли элемент быть присоединен
	//не производя самого действия
	virtual bool	CanAttach(PIItem /**pIItem/**/) {return false;}
	virtual bool	CanDetach(const char* /**item_section_name/**/) {return false;}


	
	virtual bool	Activate();									// !!! Переопределить. (см. в Inventory.cpp)
	virtual void	Deactivate();								// !!! Переопределить. (см. в Inventory.cpp)
	virtual bool	Action(s32 /**cmd/**/, u32 /**flags/**/) {return false;}	// true если известная команда, иначе false
	
	virtual bool	IsHidden()					{return true;}	// вещь спрятано в инвентаре
	virtual bool	IsPending()					{return false;}	// true если вещь чем-то занята
	
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

	u32		Cost()	const	{return m_cost;}
	float	Weight()const	{return m_weight;}		

	
	CInventory *m_pInventory;								// Указатель на инвентарь. Всегда полезно знать где находишься :)
	const char *m_name, *m_nameShort;
	char m_nameComplex[255];
	bool m_showHUD;
	bool m_drop;

	virtual int GetGridWidth() {return m_iGridWidth;}
	virtual int GetGridHeight() {return m_iGridHeight;}
	virtual int GetXPos() {return m_iXPos;}
	virtual int GetYPos() {return m_iYPos;}

	float GetCondition() {return m_fCondition;}
	void  ChangeCondition(float fDeltaCondition);

	u32	 GetSlot() {return m_slot;}
	void SetSlot(u32 slot) {m_slot = slot;}

	bool Belt() {return m_belt;}
	void Belt(bool on_belt) {m_belt = on_belt;}
	bool Ruck() {return m_ruck;}
	void Ruck(bool on_ruck) {m_ruck = on_ruck;}
protected:
	// Слот в который можно установить объект (0xffffffff если нельзя)
	u32 m_slot;
	// Может ли объект быть на поясе или в рюкзаке
	bool m_belt, m_ruck;			


	// цена по умолчанию
	u32	m_cost;
	// вес объекта (без подсоединненых вещей)
	float m_weight;
	
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

	int m_iGridWidth;										//ширина в сетке инвенторя
	int m_iGridHeight;										//высота в сетке инвенторя

	int m_iXPos;											//позиция X в сетке инвенторя
	int m_iYPos;											//позиция Y в сетке инвенторя

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

	f32 TotalWeight() const;										// Вес инвенторя
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
	PIItem Get(CLASS_ID cls_id,  bool bSearchRuck);			// Ищет на поясе или в рюкзаке IItem с указанным CLS_ID
	void   Clear();											// clearing Inventory
	virtual u32		dwfGetSameItemCount(LPCSTR caSection);	// get all the items with the same section name
	virtual bool	bfCheckForObject(_OBJECT_ID tObjectID);	// get all the items with the same object id
			u32		dwfGetObjectCount();
	CInventoryItem	*tpfGetObjectByIndex(int iIndex);
	CInventoryItem	*GetItemFromInventory(LPCSTR caItemName);

	bool Eat(PIItem pIItem);								// скушать предмет :)

	u32 GetActiveSlot() {return m_activeSlot;}

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
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//для владельцев инвенторя
#include "InventoryOwner.h"