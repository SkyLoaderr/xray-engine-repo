// CUIInventoryWnd.h:  ������ ���������
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "..\inventory.h"


#include "UIStatic.h"
#include "UIButton.h"
#include "UIDragDropItem.h"
#include "UIDragDropList.h"
#include "UIProgressBar.h"
#include "UI3dStatic.h"



class CUIInventoryWnd: public CUIWindow  
{
public:
	CUIInventoryWnd();
	virtual ~CUIInventoryWnd();

	virtual void Init();

	void InitInventory(CInventory* pInv);

	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);


	CInventory* GetInventory() {return m_pInv;}

	virtual void Draw();

	void Show(bool status) {m_bIsShown =  status;}

protected:

	CUIFrameWindow		UIBagWnd;
	CUIFrameWindow		UIDescWnd;
	CUIFrameWindow		UIPersonalWnd;

	CUIButton			UIButton1;
	CUIButton			UIButton2;

	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBelt;
	CUIStatic			UIStaticBottom;
	
	CUIStatic			UIStaticDesc;
	CUIStatic			UIStaticPersonal;

	CUIStatic			UIStaticText;
	CUI3dStatic			UI3dStatic; 

		
	#define SLOTS_NUM 5
	CUIDragDropList		UITopList[SLOTS_NUM]; 
	
	CUIDragDropList		UIBagList;
	CUIDragDropList		UIBeltList;


	CUIProgressBar UIProgressBar1;
	CUIProgressBar UIProgressBar2;
	CUIProgressBar UIProgressBar3;
	CUIProgressBar UIProgressBar4;

    
	#define MAX_ITEMS 70
	CUIDragDropItem m_vDragDropItems[MAX_ITEMS];
	int m_iUsedItems;

	//��������� �� ���������, ���������� ����� �������� ����
	CInventory* m_pInv;

	//������� � ������� �������� � ������� ������
	PIItem m_pCurrentItem;

	//���������� �������� �� ������������ ����������� ��� � �������
	//��� ����������
	static bool GreaterRoomInRuck(PIItem item1, PIItem item2);


	//�������, ����������� ������������ ������������ ������
	//� �������� ����������
	static bool SlotProc0(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc1(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc2(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc3(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc4(CUIDragDropItem* pItem, CUIDragDropList* pList);
	
	static bool BagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool BeltProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	
};