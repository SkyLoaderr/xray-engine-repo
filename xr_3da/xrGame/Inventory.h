#pragma once
#include "gameobject.h"
#include "inventory_item.h"

#define CMD_START	(1<<0)
#define CMD_STOP	(1<<1)

class CInventory;
class CInventoryItem;

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

	float TotalWeight() const;										// ��� ���������
	bool Take(CGameObject *pObj, bool bNotActivate = false);// ����� ������. ������ �������� � ������, ���� bNotActivate == false, �� ������������ ������, ���� � ��������� ��������� ������ �� ������������ (e.g. �������)
	bool Drop(CGameObject *pObj, bool call_drop = true);							// ��������� ������
	bool DropAll();											// ��������� ���
	void ClearAll();										// �������� ��
	
	
	bool Slot(PIItem pIItem, bool bNotActivate = false);	// �������� ������ ���� � ����. �� ��� ���� - �� ����. ������ - � ������
	bool Belt(PIItem pIItem);								// �������� ������ �� ����
	bool Ruck(PIItem pIItem);								// ������� ������ � ������

	//��������� ��������� �� ������� � ����� ���������
	bool InSlot(PIItem pIItem) const;
	bool InBelt(PIItem pIItem) const;
	bool InRuck(PIItem pIItem) const;

	//����� �� �������� ������� � ����, ������ ��� �� ����
	bool CanPutInSlot(PIItem pIItem) const;
	bool CanPutInBelt(PIItem pIItem);
	bool CanPutInRuck(PIItem pIItem);

		
	bool Activate(u32 slot);								// ������������ ������ � ��������� �����
	PIItem ActiveItem() const;								// ���������� ��������� �� ������ � �������� �����

	bool Action(s32 cmd, u32 flags);						// true ���� ��������� �������, ����� false
	void Update();											// ����������

	PIItem Same(const PIItem pIItem, bool bSearchRuck) const;// ���� �� ����� ����������� IItem
	PIItem SameSlot(u32 slot,	 bool bSearchRuck) const;	// ���� �� ����� IItem ��� ���������� �����
	PIItem Get(const char *name, bool bSearchRuck) const;	// ���� �� ����� ��� � ������� IItem � ��������� ������ (cName())
	PIItem Get(const u32  id,	 bool bSearchRuck) const;	// ���� �� ����� ��� � ������� IItem � ��������� ������ (id)
	PIItem Get(CLASS_ID cls_id,  bool bSearchRuck) const;	// ���� �� ����� ��� � ������� IItem � ��������� CLS_ID
	void   Clear();											// clearing Inventory
	virtual u32		dwfGetSameItemCount(LPCSTR caSection);	// get all the items with the same section name
	virtual bool	bfCheckForObject(_OBJECT_ID tObjectID);	// get all the items with the same object id
			u32		dwfGetObjectCount();
	CInventoryItem	*tpfGetObjectByIndex(int iIndex);
	CInventoryItem	*GetItemFromInventory(LPCSTR caItemName);

	bool Eat(PIItem pIItem);								// ������� ������� :)

	u32 GetActiveSlot() const {return m_activeSlot;}

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

	u32 RuckWidth() const;
	u32 RuckHeight() const;
	u32 BeltWidth() const;
	
	u32 TotalVolume() const;
	u32 GetMaxVolume() const;
	

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
//��� ���������� ���������
//#include "InventoryOwner.h"