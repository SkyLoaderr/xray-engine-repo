// UICarBodyWnd.h:  ������ ����������� ����� �� ��������� � � ����� 
//					���� � ���������
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma once

#include "..\\inventory.h"
#include "..\\gameobject.h"


#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIButton.h"
#include "UIDragDropItem.h"
#include "UIDragDropList.h"
#include "UIProgressBar.h"
#include "UIPropertiesBox.h"

#include "UIEditBox.h"
#include "UIListWnd.h"
#include "UIMessageBox.h"



class CUICarBodyWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
public:
	CUICarBodyWnd();
	virtual ~CUICarBodyWnd();

	virtual void Init();

	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	void InitCarBody(CInventory* pOurInv,    CGameObject* pOurObject,
					 CInventory* pOthersInv, CGameObject* pOthersObject);
	
	virtual void Draw();
	virtual void Update();
		
	virtual void Show();

	void DisableAll();
	void EnableAll();


	//��������� �� ���������
	CInventory*		m_pInv;
	CInventory*		m_pOthersInv;
	CGameObject*	m_pOurObject;
	CGameObject*	m_pOthersObject;



protected:
	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBottom;

	//��� ����������� �������� ������� ������	
	CUIStatic			UIStaticDesc;
	CUI3dStatic			UI3dStatic;

	CUIFrameWindow		UIOurBagWnd;
	CUIFrameWindow		UIOthersBagWnd;
	CUIDragDropList		UIOurBagList;
	CUIDragDropList		UIOthersBagList;


	//�������, ����������� ������������ ������������ ������
	//� �������� ����������
	static bool OurBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool OthersBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);

	//��������� ������ �� ���� �� ���� �������� � ������ ����� �������
	bool IsEnoughtOurRoom(CUIDragDropItem* pItem);
	bool IsEnoughtOthersRoom(CUIDragDropItem* pItem);

	bool ToOurBag();
	bool ToOthersBag();
	
	//���������� ��������
	void UpdateLists();

	//pop-up ���� ���������� �� ������� ������ ������
	CUIPropertiesBox UIPropertiesBox;
	//���� � ����������
	CUIMessageBox	 UIMessageBox;


	static const int MAX_ITEMS = 140;
	CUIWpnDragDropItem m_vDragDropItems[MAX_ITEMS];
	int m_iUsedItems;

	//��� ���������� �����
	TIItemList ruck_list;

	//������� � ������� �������� � ������� ������
	PIItem m_pCurrentItem;
	CUIDragDropItem* m_pCurrentDragDropItem;
};