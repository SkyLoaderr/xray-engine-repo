#pragma once
#include "inventory_item.h"

class CInventory;
class CInventoryItem;
class CHudItem;

///////////////////////////////////////////////////////////////////////////////////////////////////
class CInventorySlot					// ����
{									
public:
	CInventorySlot();
	virtual ~CInventorySlot();

	PIItem	m_pIItem;
	shared_str m_name;
	//����� �� ���� ����� ��������
	bool m_bCanBeActivated;
	//����� �� ���� ������ ���� �����������
	bool m_bUsable;

};
typedef xr_vector<CInventorySlot> TISlotArr;

///////////////////////////////////////////////////////////////////////////////////////////////////
class CInventoryOwner;

class CInventory						// ��������� 
{				
public:
	CInventory();
	virtual ~CInventory();

	// ��� ���������
	float TotalWeight		() const;
	float CalcTotalWeight	();

	// ����� ������. ������ �������� � ������, 
	//���� bNotActivate == false, �� ������������ ������, 
	//���� � ��������� ��������� ������ �� ������������ (e.g. �������)
	//strict_placement - ��������� ������� �����, ����������� �� ���������� �������,
	//����� ���� ������� �� ������ ��������� �����
	void Take(CGameObject *pObj, bool bNotActivate, bool strict_placement);
	//��������� �������
	bool Drop(CGameObject *pObj, bool call_drop = true);							// ��������� ������
	// ��������� ���
	bool DropAll();	
	// �������� ��
	void Clear();

	
	// �������� ������ ���� � ����. �� ��� ���� - �� ����. ������ - � ������
	bool Slot(PIItem pIItem, bool bNotActivate = false);	
	// �������� ������ �� ����
	bool Belt(PIItem pIItem);
	// ������� ������ � ������
	bool Ruck(PIItem pIItem);

	//��������� ��������� �� ������� � ����� ���������
	bool InSlot(PIItem pIItem) const;
	bool InBelt(PIItem pIItem) const;
	bool InRuck(PIItem pIItem) const;

	//����� �� �������� ������� � ����, ������ ��� �� ����
	bool CanPutInSlot(PIItem pIItem) const;
	bool CanPutInBelt(PIItem pIItem);
	bool CanPutInRuck(PIItem pIItem) const;

	//����� �� ����� ����
	bool CanTakeItem(CInventoryItem *inventory_item) const;


	// ������������ ������ � ��������� �����	
	bool Activate(u32 slot, bool force=false);
	// ���������� ��������� �� ������ � �������� �����
	PIItem ActiveItem				()const					{return m_iActiveSlot==NO_ACTIVE_SLOT ? NULL :m_slots[m_iActiveSlot].m_pIItem;}

	// ���������� ��������� �� ������ � ����� �������� �������
	PIItem ItemFromSlot(u32 slot) const;

	// true ���� ��������� �������, ����� false
	bool Action(s32 cmd, u32 flags);
	// ���������� �� �����
	void Update();

	// ���� �� ����� ����������� IItem
	PIItem Same(const PIItem pIItem, bool bSearchRuck) const;
	// ���� �� ����� IItem ��� ���������� �����
	PIItem SameSlot(u32 slot,	 bool bSearchRuck) const;
	// ���� �� ����� ��� � ������� IItem � ��������� ������ (cName())
	PIItem Get(const char *name, bool bSearchRuck) const;
	// ���� �� ����� ��� � ������� IItem � ��������� ������ (id)
	PIItem Get(const u16  id,	 bool bSearchRuck) const;
	// ���� �� ����� ��� � ������� IItem � ��������� CLS_ID
	PIItem Get	(CLASS_ID cls_id,  bool bSearchRuck) const;
	PIItem GetAny(const char *name) const;//search both (ruck and belt)

	PIItem item (CLASS_ID cls_id) const;
	
	// get all the items with the same section name
	virtual u32		dwfGetSameItemCount(LPCSTR caSection);	
	// get all the items with the same object id
	virtual bool	bfCheckForObject(ALife::_OBJECT_ID tObjectID);	
	CInventoryItem	*CInventory::get_object_by_id(ALife::_OBJECT_ID tObjectID);

			u32		dwfGetObjectCount();
	CInventoryItem	*tpfGetObjectByIndex(int iIndex);
	CInventoryItem	*GetItemFromInventory(LPCSTR caItemName);

	// ������� ������� :)
	bool Eat(PIItem pIItem);								

	u32 GetActiveSlot() const {return m_iActiveSlot;}
	
	void SetPrevActiveSlot(u32 ActiveSlot)  {m_iPrevActiveSlot = ActiveSlot;}
	u32 GetPrevActiveSlot() const {return m_iPrevActiveSlot;}

	void SetActiveSlot (u32 ActiveSlot) { m_iActiveSlot = m_iNextActiveSlot = ActiveSlot; }

	bool IsSlotsUseful() const				{return m_bSlotsUseful;}	 
	void SetSlotsUseful	(bool slots_useful) {m_bSlotsUseful = slots_useful;}
	bool IsBeltUseful() const				{return m_bBeltUseful;}
	void SetBeltUseful(bool belt_useful)	{m_bBeltUseful = belt_useful;}

	// ������ ��������. m_all - ���
	TIItemContainer		m_all;
	// ������ ��������. m_ruck - ������, m_belt - ����
	TIItemContainer		m_ruck, m_belt;
	// ����� (������������� ���-��)
	TISlotArr		m_slots;

	//���������� ��� ����� PDA � ����� � �����
	void				AddAvailableItems(TIItemContainer& items_container, bool for_trade = false) const;

	float				GetTakeDist					() const				{return m_fTakeDist;}
	
	float				GetMaxWeight				() const				{return m_fMaxWeight;}
	void				SetMaxWeight				(float weight)			{m_fMaxWeight = weight;}

	u32					BeltWidth					() const;

	inline	CInventoryOwner*GetOwner				() const				{ return m_pOwner; }
	

	// ������ �� ������� ������� ������
	PIItem m_pTarget;

	friend class CInventoryOwner;


	u32			ModifyFrame							() const				{return m_dwModifyFrame;}
	void		Items_SetCurrentEntityHud			(bool current_entity);
	bool		isBeautifulForActiveSlot			(CInventoryItem *pIItem);
	bool		isSlotsBlocked						()			{return m_bDoBlockAllSlots>0;}
	void		setSlotsBlocked						(bool b);
protected:
	// �������� ���� � ���� ������� ������ �������� ����� �����
    //�������� ��������� � ������� ��������� (��� ����� ������)
	u32 m_iActiveSlot;
	u32 m_iNextActiveSlot;
	u32 m_iPrevActiveSlot;

	CInventoryOwner *m_pOwner;

	//�������� ������ ��� ����������
	TIItemContainer ruck_list;

	//����, ������������ ������� ����� � ���������
	bool m_bBeltUseful;
	//����, ����������� ������������� ������
	bool m_bSlotsUseful;

	// ������������ ��� ���������
	float m_fMaxWeight;
	// ������� ��� � ���������
	float m_fTotalWeight;

	// ������������ ���-�� ��������
	//�� �����
	u32 m_iMaxBelt;	
	// ������������ ���������� �� ������� ����� ��������� ������
	float m_fTakeDist;

	//���� �� ������� ��������� ��������� ��������� � ���������
	u32	m_dwModifyFrame;
	int	m_bDoBlockAllSlots;

	//��������� ������
	TIItemContainer			l_subs; 
	xr_vector<PIItem>	drop_tasks;
	bool				m_drop_last_frame;

	void		SendActionEvent		(s32 cmd, u32 flags);

};