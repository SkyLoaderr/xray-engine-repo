// CUIInventoryWnd.h:  ������ ���������
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "..\inventory.h"

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIButton.h"

#include "UIDragDropItem.h"
#include "UIWpnDragDropItem.h"

#include "UIDragDropList.h"
#include "UIProgressBar.h"
#include "UI3dStatic.h"

#include "UIPropertiesBox.h"
#include "UIOutfitSlot.h"
#include "UIArtifactMergerWnd.h"




class CArtifact;


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
	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);


	CInventory* GetInventory() {return m_pInv;}

	virtual void Update();
	virtual void Draw();

	void Show();

	//��� ������ � ����������� ���������� �����
	bool IsArtifactMergeShown() {return UIArtifactMergerWnd.IsShown();}
	void AddArtifactToMerger(CArtifact* pArtifact);
	//��� ���������� ����� ��������� �� ����� ������ � ����������� (�������� 
	//������������� �������)
	void AddItemToBag(PIItem pItem);
protected:

	CUIFrameWindow		UIBagWnd;
	CUIFrameWindow		UIDescWnd;
	CUIFrameWindow		UIPersonalWnd;

	CUIButton			UIButton1;
	CUIButton			UIButton2;
	CUIButton			UIButton3;
	CUIButton			UIButton4;
	CUIButton			UIButton5;
	CUIButton			UIButton6;

	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBelt;
	CUIStatic			UIStaticBottom;
	

	CUIStatic			UIStaticDesc;
	CUIStatic			UIStaticPersonal;

	CUIStatic			UIStaticText;
	
	//��� ����������� �������� ������� ������
	CUI3dStatic			UI3dStatic;
	//��� ����������� ���������
	CUI3dStatic			UI3dCharacter;

		
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
	CUIPropertiesBox UIPropertiesBox;
	//������� ��� ������ � ����������� ������������ ����������
	CUIArtifactMerger UIArtifactMergerWnd;

	
	static const int MAX_ITEMS = 70;
	CUIWpnDragDropItem m_vDragDropItems[MAX_ITEMS];
	int m_iUsedItems;

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


	//��� ������� ���� �� ������ �������
	void ActivatePropertiesBox();

	//�������� ���������� ��������� ��� ���������� ���������
	enum {DROP_ACTION, EAT_ACTION, TO_BELT_ACTION, 
		  TO_SLOT_ACTION, TO_BAG_ACTION,
		  ARTIFACT_MERGER_ACTIVATE,
		  ARTIFACT_MERGER_DEACTIVATE,
  		  ATTACH_ADDON, 
		  DETACH_SCOPE_ADDON,
		  DETACH_SILENCER_ADDON,
		  DETACH_GRENADE_LAUNCHER_ADDON};

	//��������� �������
	void DropItem();
	//������ �������
	void EatItem();
	
	//����������� ����
	bool ToSlot();
	bool ToBag();
	bool ToBelt();


	//������ � ��������� ���� ������ � �����������
	void StartArtifactMerger();
	void StopArtifactMerger();

	//�������������/������������ ������� � ������
	void AttachAddon();
	void DetachAddon(const char* addon_name);


	//�������������� ������ ��� ���������� �����
	TIItemList ruck_list;
	
	//���� � ������� �� ������������ add-on
	PIItem	m_pItemToUpgrade;
};