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
class CInventoryItem : virtual public CGameObject 					// ������ �������� ���������
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

	virtual bool	Useful();									// !!! ��������������. (��. � Inventory.cpp)
	virtual bool	Attach(PIItem pIItem, bool force = false);	// !!! ��������������. (��. � Inventory.cpp)
	virtual bool	Detach(PIItem pIItem, bool force = true);	// !!! ��������������. (��. � Inventory.cpp)
	virtual bool	Activate();									// !!! ��������������. (��. � Inventory.cpp)
	virtual void	Deactivate();								// !!! ��������������. (��. � Inventory.cpp)
	virtual bool	Action(s32 cmd, u32 flags) {return false;}	// true ���� ��������� �������, ����� false
	virtual void	OnAnimationEnd() {}
	virtual void	renderable_Render() {}
	virtual s32		Sort(PIItem pIItem);						// !!! ��������������. (��. � Inventory.cpp)
	virtual bool	Merge(PIItem pIItem);						// !!! ��������������. (��. � Inventory.cpp)

	virtual void	OnH_B_Chield		();
	virtual void	OnH_B_Independent	();

	virtual void	UpdateCL	();

	bool DetachAll();										// ��������� �������� ��������. ������ ������ ���� � �������
	void Drop();											// ���� ������ � ���������, �� �� ����� ��������

	f32 m_weight;											// ��� �������
	u32 m_slot;												// ���� � ������� ����� ���������� ������ (0xffffffff ���� ������)
	bool m_belt, m_ruck;									// ����� �� ������ ���� �� ����� ��� � �������
	TIItemList m_subs;										// �������������� �������
	CInventory *m_pInventory;								// ��������� �� ���������. ������ ������� ����� ��� ���������� :)
	const char *m_name, *m_nameShort;
	char m_nameComplex[255];
	bool m_showHUD;
	bool m_drop;

	u32	m_cost;												// ���� �� ���������
	u32	Cost() {return m_cost;}


	const char* m_sIconTexture;								//�������� � ������� ��� ����
	
	int m_iGridWidth;										//������ � ����� ���������
	int m_iGridHeight;										//������ � ����� ���������

};


///////////////////////////////////////////////////////////////////////////////////////////////////
// ��������� ����
class CEatableItem : public CInventoryItem 
{
private:
	typedef CInventoryItem	inherited;
public:
	CEatableItem();
	virtual ~CEatableItem();

	virtual void Load(LPCSTR section);

	//������� ��� �������� ���� �� ��������� ������
	float m_fHealthInfluence;
	float m_fPowerInfluence;
	float m_fSatietyInfluence;
	float m_fRadiationInfluence;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class CInventorySlot					// ����
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

class CInventory						// ��������� 
{				
public:
	CInventory();
	virtual ~CInventory();

	f32 TotalWeight();										// ��� ���������
	bool Take(CGameObject *pObj, bool bNotActivate = false);// ����� ������. ������ �������� � ������, ���� bNotActivate == false, �� ������������ ������, ���� � ��������� ��������� ������ �� ������������ (e.g. �������)
	bool Drop(CGameObject *pObj);							// ��������� ������
	bool DropAll();											// ��������� ���
	void ClearAll();										// �������� ��
	bool Slot(PIItem pIItem);								// �������� ������ ���� � ����. �� ��� ���� - �� ����. ������ - � ������
	bool Belt(PIItem pIItem);								// �������� ������ �� ����
	bool Ruck(PIItem pIItem);								// ������� ������ � ������
	bool Activate(u32 slot);								// ������������ ������ � ��������� �����
	PIItem ActiveItem() const;								// ���������� ��������� �� ������ � �������� �����
	bool Action(s32 cmd, u32 flags);						// true ���� ��������� �������, ����� false
	void Update(u32 deltaT);								// ����������
	PIItem Same(const PIItem pIItem);						// ���� �� ����� ����������� IItem
	PIItem SameSlot(u32 slot);								// ���� �� ����� IItem ��� ���������� �����
	PIItem Get(const char *name, bool bSearchRuck);			// ���� �� ����� ��� � ������� IItem � ��������� ������ (cName())
	PIItem Get(const u16  id,	 bool bSearchRuck);			// ���� �� ����� ��� � ������� IItem � ��������� ������ (id)
	void   Clear();											// clearing Inventory
	virtual u32		dwfGetSameItemCount(LPCSTR caSection);	// get all the items with the same section name

	bool Eat(PIItem pIItem);								// ������� ������� :)

	TIItemSet m_all;										// ������ ��������. m_all - ���
	TIItemList m_ruck, m_belt;								// ������ ��������. m_ruck - ������, m_belt - ����
	TISlotArr m_slots;										// ����� (������������� ���-��)
	u32 m_activeSlot, m_nextActiveSlot;						// �������� ���� � ���� ������� ������ �������� ����� �����
	f32 m_maxWeight;										// ������������ ��� ���������
	u32 m_maxRuck, m_maxBelt;								// ������������ ���-�� �������� � ������� � �� �����
	f32 m_takeDist;											// ������������ ���������� �� ������� ����� ��������� ������
	PIItem m_pTarget;										// ������ �� ������� ������� ������


	//��� �������� ���������� �����
	static bool GreaterRoomInRuck(PIItem item1, PIItem item2);
	bool FreeRuckRoom();
	//�������� ������ ��� ����������
	TIItemList ruck_list;

	CInventoryOwner *m_pOwner;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class CTrade;

class CInventoryOwner		// ������� ��� ������� � ������� ������� ��������� 
{							
public:
	CInventoryOwner();
	virtual ~CInventoryOwner();

	CInventory	m_inventory;									// ���������
	CInventory	m_trade_storage;								// ����� 
	
	CTrade		*m_trade;										// ��������

	// ��������
	u32					m_dwMoney;
	EStalkerRank		m_tRank;
};
