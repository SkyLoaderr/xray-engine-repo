// CUIInventoryWnd.h:  ������ ���������
// 
//////////////////////////////////////////////////////////////////////

#pragma once

class CInventory;

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIButton.h"

#include "UIDragDropItem.h"
#include "UIWpnDragDropItem.h"

#include "UIDragDropList.h"
#include "UIProgressBar.h"

#include "UIPropertiesBox.h"
#include "UIOutfitSlot.h"
#include "UIArtifactMergerWnd.h"
#include "UISleepWnd.h"

//#include "UICharacterInfo.h"
#include "UIOutfitInfo.h"
#include "UIItemInfo.h"

#include "UITimeWnd.h"

//////////////////////////////////////////////////////////////////////////

class CArtefact;

//////////////////////////////////////////////////////////////////////////

class CUIInventoryWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
public:
	CUIInventoryWnd();
	virtual ~CUIInventoryWnd();

	virtual void Init();

	void InitInventory();


	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
	virtual void OnMouse(int x, int y, EUIMessages mouse_action);


	CInventory* GetInventory() {return m_pInv;}

	virtual void Update();
	virtual void Draw();

	virtual void Show();
	virtual void Hide();

	//��� ������ � ����������� ���������� �����
	bool IsArtefactMergeShown() {return UIArtefactMergerWnd.IsShown();}
	void AddArtefactToMerger(CArtefact* pArtefact);
	//��� ���������� ����� ��������� �� ����� ������ � ����������� (�������� 
	//������������� �������)
	void AddItemToBag(PIItem pItem);

	// c���� ������
	bool UndressOutfit();

	// �������� ��������� �� ���� �����. ������������ ��� �������������� ��������� ������� ����
	// �� ������, ��� ����� �� ��� ����� �����
	CUIDragDropList * GetBag() { return &UIBagList; }

	// ��������� �������������� ���� � ������� ����� � ����������� ���������� ��� ��� �����
	// ���������� ������� ���� � ����� � �����
	bool SlotToBag(PIItem pItem, CUIDragDropList *pList, const u32 SlotNum);

protected:
	friend class CUITradeWnd;

	CUIStatic			UIBagWnd;
	CUIStatic			UIMoneyWnd;
	CUIStatic			UIDescrWnd;
	CUIFrameWindow		UIPersonalWnd;

	// ��������� ���
	CUISleepWnd			UISleepWnd;
	
	// ������ ������� ��������� ��������
	CUIButton			UIDropButton;
	CUIButton			UIExitButton;

	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBelt;
	CUIStatic			UIStaticBottom;
	CUITimeWnd			UITimeWnd;

//	CUIStatic			UIStaticDesc;
	CUIStatic			UIStaticPersonal;
		
	#define SLOTS_NUM 5
	//����� ��� ������
	CUIDragDropList		UITopList[SLOTS_NUM]; 
	//��������� ���� ��� ��������
	CUIOutfitSlot		UIOutfitSlot;
	
	CUIDragDropList		UIBagList;
	CUIDragDropList		UIBeltList;


	CUIProgressBar		UIProgressBarHealth;
	CUIProgressBar		UIProgressBarSatiety;
	CUIProgressBar		UIProgressBarPower;
	CUIProgressBar		UIProgressBarRadiation;

	//pop-up ���� ���������� �� ������� ������ ������
	CUIPropertiesBox	UIPropertiesBox;
	//������� ��� ������ � ����������� ������������ ����������
	CUIArtefactMerger	UIArtefactMergerWnd;
	
	//���������� � ���������
	//CUICharacterInfo UICharacterInfo;
	CUIOutfitInfo UIOutfitInfo;
	//���������� � ��������
	CUIItemInfo UIItemInfo;

	//������ ��������� drag drop
	DD_ITEMS_VECTOR	m_vDragDropItems;
	
	//��������� �� ���������, ���������� ����� �������� ����
	CInventory* m_pInv;

	//������� � ������� �������� � ������� ������
	PIItem m_pCurrentItem;
	CUIDragDropItem* m_pCurrentDragDropItem;

	//�������, ����������� ������������ ������������ ������
	//� �������� ����������
	static bool SlotProc0(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc1(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc2(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc3(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc4(CUIDragDropItem* pItem, CUIDragDropList* pList);

	static bool OutfitSlotProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool BagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool BeltProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	//---------------------------------------------------------------------
	static void	SendEvent_Item2Slot			(PIItem	pItem);
	static void	SendEvent_Item2Belt			(PIItem	pItem);
	static void	SendEvent_Item2Ruck			(PIItem	pItem);
	static void	SendEvent_ItemDrop			(PIItem	pItem);
	static void	SendEvent_Item_Eat			(PIItem	pItem);
	//---------------------------------------------------------------------
	//��� ������� ���� �� ������ �������
	void ActivatePropertiesBox();

	//��������� �������
	void DropItem();
	//������ �������
	void EatItem();
	
	//����������� ����
	bool ToSlot();
	bool ToBag();
	bool ToBelt();


	//������ � ��������� ���� ������ � �����������
	void StartArtefactMerger();
	void StopArtefactMerger();

	//�������������/������������ ������� � ������
	void AttachAddon();
	void DetachAddon(const char* addon_name);

	//������������� ������� �������
	void SetCurrentItem(CInventoryItem* pItem);

	//�������������� ������ ��� ���������� �����
	TIItemList ruck_list;
	
	//���� � ������� �� ������������ add-on
	PIItem	m_pItemToUpgrade;

	//���� ������� ��� �������� ����� ������� �������
	u32	m_iCurrentActiveSlot;
};