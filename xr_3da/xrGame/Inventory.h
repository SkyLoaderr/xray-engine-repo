#pragma once
#include "gameobject.h"
#include "inventory_item.h"

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
	//����� �� ���� ����� ��������
	bool m_bCanBeActivated;
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
	float TotalWeight() const;										
	// ����� ������. ������ �������� � ������, 
	//���� bNotActivate == false, �� ������������ ������, 
	//���� � ��������� ��������� ������ �� ������������ (e.g. �������)
	bool Take(CGameObject *pObj, bool bNotActivate = false);
	//��������� �������
	bool Drop(CGameObject *pObj, bool call_drop = true);							// ��������� ������
	// ��������� ���
	bool DropAll();	
	// �������� ��
	void ClearAll();
	
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
	bool CanPutInRuck(PIItem pIItem);

	// ������������ ������ � ��������� �����	
	bool Activate(u32 slot);
	// ���������� ��������� �� ������ � �������� �����
	PIItem ActiveItem() const;

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
	PIItem Get(CLASS_ID cls_id,  bool bSearchRuck) const;
	
	// clearing Inventory
	void   Clear();											
	// get all the items with the same section name
	virtual u32		dwfGetSameItemCount(LPCSTR caSection);	
	// get all the items with the same object id
	virtual bool	bfCheckForObject(ALife::_OBJECT_ID tObjectID);	
			u32		dwfGetObjectCount();
	CInventoryItem	*tpfGetObjectByIndex(int iIndex);
	CInventoryItem	*GetItemFromInventory(LPCSTR caItemName);

	// ������� ������� :)
	bool Eat(PIItem pIItem);								

	u32 GetActiveSlot() const {return m_iActiveSlot;}
	void SetActiveSlot (u32 ActiveSlot) { m_iActiveSlot = m_iNextActiveSlot = ActiveSlot; }

	bool IsSlotsUseful() {return m_bSlotsUseful;}	 
	void SetSlotsUseful(bool slots_useful) {m_bSlotsUseful = slots_useful;}
	bool IsBeltUseful() {return m_bBeltUseful;}
	void SetBeltUseful(bool belt_useful) {m_bBeltUseful = belt_useful;}

	// ������ ��������. m_all - ���
	TIItemSet		m_all;
	// ������ ��������. m_ruck - ������, m_belt - ����
	TIItemList		m_ruck, m_belt;
	// ����� (������������� ���-��)
	TISlotArr		m_slots;

	float GetTakeDist() {return m_fTakeDist;}
	
	float GetMaxWeight() {return m_fMaxWeight;}
	void  SetMaxWeight(float weight) {m_fMaxWeight = weight;}

	u32  GetMaxRuck() {return m_iMaxRuck;}
	void SetMaxRuck(u32 max_ruck) {m_iMaxRuck = max_ruck;}
	u32 GetMaxBelt() {return m_iMaxBelt;}

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
    //�������� ��������� � ������� ��������� (��� ����� ������)
	u32 m_iActiveSlot;
	u32 m_iNextActiveSlot;

	CInventoryOwner *m_pOwner;

	//�������� ������ ��� ����������
	TIItemList ruck_list;

	//����, ������������ ������� ����� � ���������
	bool m_bBeltUseful;
	//����, ����������� ������������� ������
	bool m_bSlotsUseful;

	// ������������ ��� ���������
	float m_fMaxWeight;
	// ������������ ���-�� �������� � ������� � 
	u32 m_iMaxRuck;
	//�� �����
	u32 m_iMaxBelt;	
	// ������������ ���������� �� ������� ����� ��������� ������
	float m_fTakeDist;



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