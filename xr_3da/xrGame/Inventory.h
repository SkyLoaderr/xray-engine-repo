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

	
	virtual void OnEvent (NET_Packet& P, u16 type);
	
	virtual bool	Useful();									// !!! ��������������. (��. � Inventory.cpp)
	virtual bool	Attach(PIItem /**pIItem/**/) {return false;}
	virtual bool	Detach(PIItem /**pIItem/**/) {return false;}
	//��� ������ ��������� ����� ���� ��� ������� �������� ������
	virtual bool	Detach(const char* item_section_name);
	//��������� ����� �� ������� ���� �����������
	//�� ��������� ������ ��������
	virtual bool	CanAttach(PIItem /**pIItem/**/) {return false;}
	virtual bool	CanDetach(const char* /**item_section_name/**/) {return false;}


	
	virtual bool	Activate();									// !!! ��������������. (��. � Inventory.cpp)
	virtual void	Deactivate();								// !!! ��������������. (��. � Inventory.cpp)
	virtual bool	Action(s32 /**cmd/**/, u32 /**flags/**/) {return false;}	// true ���� ��������� �������, ����� false
	
	virtual bool	IsHidden()					{return true;}	// ���� �������� � ���������
	virtual bool	IsPending()					{return false;}	// true ���� ���� ���-�� ������
	
	virtual void	OnAnimationEnd() {}
	virtual void	renderable_Render() {}
	
	virtual s32		Sort(PIItem pIItem);						// !!! ��������������. (��. � Inventory.cpp)
	virtual bool	Merge(PIItem pIItem);						// !!! ��������������. (��. � Inventory.cpp)

	virtual void	OnH_B_Chield		();
	virtual void	OnH_B_Independent	();

	virtual void	UpdateCL	();

	virtual	void	Hit	(float P, Fvector &dir,	
								  CObject* who, s16 element,
								  Fvector position_in_object_space, 
								  float impulse, 
								  ALife::EHitType hit_type = eHitTypeWound);

	bool DetachAll();										// ��������� �������� ��������. ������ ������ ���� � �������
	void Drop();											// ���� ������ � ���������, �� �� ����� ��������

	u32		Cost()	const	{return m_cost;}
	float	Weight()const	{return m_weight;}		

	
	CInventory *m_pInventory;								// ��������� �� ���������. ������ ������� ����� ��� ���������� :)
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
	// ���� � ������� ����� ���������� ������ (0xffffffff ���� ������)
	u32 m_slot;
	// ����� �� ������ ���� �� ����� ��� � �������
	bool m_belt, m_ruck;			


	// ���� �� ���������
	u32	m_cost;
	// ��� ������� (��� �������������� �����)
	float m_weight;
	
	//��������� ����, 1.0 - ��������� ���������������
	// 0 - �����������
	float m_fCondition;
	//���� ������������� ��������� ��� ����
	bool m_bUsingCondition;

	//������������ ��������� ����� ����� (�������� ������ ��� ��� ����)
	float m_fK_Burn;
	float m_fK_Strike;
	float m_fK_Shock;
	float m_fK_Wound;
	float m_fK_Radiation;
	float m_fK_Telepatic;
	float m_fK_ChemicalBurn;
	float m_fK_Explosion;
	float m_fK_FireWound;

	int m_iGridWidth;										//������ � ����� ���������
	int m_iGridHeight;										//������ � ����� ���������

	int m_iXPos;											//������� X � ����� ���������
	int m_iYPos;											//������� Y � ����� ���������

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
	virtual bool Useful();

	//������� ��� �������� ���� �� ��������� ������
	float	m_fHealthInfluence;
	float	m_fPowerInfluence;
	float	m_fSatietyInfluence;
	float	m_fRadiationInfluence;

	//���������� ������ ���, 
	//-1 - ������ ���� � ������ �� ������ (���� �� �������� ������� � ����)
	int		m_iPortionsNum;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class CInventorySlot					// ����
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

class CInventory						// ��������� 
{				
public:
	CInventory();
	virtual ~CInventory();

	f32 TotalWeight() const;										// ��� ���������
	bool Take(CGameObject *pObj, bool bNotActivate = false);// ����� ������. ������ �������� � ������, ���� bNotActivate == false, �� ������������ ������, ���� � ��������� ��������� ������ �� ������������ (e.g. �������)
	bool Drop(CGameObject *pObj);							// ��������� ������
	bool DropAll();											// ��������� ���
	void ClearAll();										// �������� ��
	
	
	bool Slot(PIItem pIItem);								// �������� ������ ���� � ����. �� ��� ���� - �� ����. ������ - � ������
	bool Belt(PIItem pIItem);								// �������� ������ �� ����
	bool Ruck(PIItem pIItem);								// ������� ������ � ������

	//��������� ��������� �� ������� � ����� ���������
	bool InSlot(PIItem pIItem);
	bool InBelt(PIItem pIItem);
	bool InRuck(PIItem pIItem);

	//����� �� �������� ������� � ����, ������ ��� �� ����
	bool CanPutInSlot(PIItem pIItem);
	bool CanPutInBelt(PIItem pIItem);
	bool CanPutInRuck(PIItem pIItem);

		
	bool Activate(u32 slot);								// ������������ ������ � ��������� �����
	PIItem ActiveItem() const;								// ���������� ��������� �� ������ � �������� �����
	bool Action(s32 cmd, u32 flags);						// true ���� ��������� �������, ����� false
	void Update(u32 deltaT);								// ����������
	PIItem Same(const PIItem pIItem);						// ���� �� ����� ����������� IItem
	PIItem SameSlot(u32 slot);								// ���� �� ����� IItem ��� ���������� �����
	PIItem Get(const char *name, bool bSearchRuck);			// ���� �� ����� ��� � ������� IItem � ��������� ������ (cName())
	PIItem Get(const u32  id,	 bool bSearchRuck);			// ���� �� ����� ��� � ������� IItem � ��������� ������ (id)
	void   Clear();											// clearing Inventory
	virtual u32		dwfGetSameItemCount(LPCSTR caSection);	// get all the items with the same section name
	virtual bool	bfCheckForObject(_OBJECT_ID tObjectID);	// get all the items with the same object id
			u32		dwfGetObjectCount();
	CInventoryItem	*tpfGetObjectByIndex(int iIndex);
	CInventoryItem	*GetItemFromInventory(LPCSTR caItemName);

	bool Eat(PIItem pIItem);								// ������� ������� :)

	u32 GetActiveSlot() {return m_activeSlot;}

	bool IsSlotsUseful() {return m_bSlotsUseful;}	 
	void SetSlotsUseful(bool slots_useful) {m_bSlotsUseful = slots_useful;}
	bool IsBeltUseful() {return m_bBeltUseful;}
	void SetBeltUseful(bool belt_useful) {m_bBeltUseful = belt_useful;}

	TIItemSet m_all;										// ������ ��������. m_all - ���
	TIItemList m_ruck, m_belt;								// ������ ��������. m_ruck - ������, m_belt - ����
	TISlotArr m_slots;										// ����� (������������� ���-��)

	float GetTakeDist() {return m_takeDist;}
	
	float GetMaxWeight() {return m_maxWeight;}
	void  SetMaxWeight(float weight) {m_maxWeight = weight;}

	u32  GetMaxRuck() {return m_maxRuck;}
	void SetMaxRuck(u32 max_ruck) {m_maxRuck = max_ruck;}
	u32 GetMaxBelt() {return m_maxBelt;}

	// ������ �� ������� ������� ������
	PIItem m_pTarget;

	friend class CInventoryOwner;

protected:
	//��� �������� ���������� �����
	bool FreeRuckRoom();
	bool FreeBeltRoom();


	// �������� ���� � ���� ������� ������ �������� ����� �����
	u32 m_activeSlot;
	u32 m_nextActiveSlot;

	CInventoryOwner *m_pOwner;

	//�������� ������ ��� ����������
	TIItemList ruck_list;

	//����, ������������ ������� ����� � ���������
	bool m_bBeltUseful;
	//����, ����������� ������������� ������
	bool m_bSlotsUseful;

	// ������������ ��� ���������
	float m_maxWeight;
	// ������������ ���-�� �������� � ������� � 
	u32 m_maxRuck;
	//�� �����
	u32 m_maxBelt;	
	// ������������ ���������� �� ������� ����� ��������� ������
	float m_takeDist;



	//��������� ������
	TIItemList l_subs; 

	void		SendActionEvent		(s32 cmd, u32 flags);
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//��� ���������� ���������
#include "InventoryOwner.h"