// UICarBodyWnd.h:  ������ ����������� ����� �� ��������� � � ����� 
//					���� � ���������
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma once


class CInventory;

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIButton.h"
#include "UIDragDropItem.h"
#include "UIDragDropList.h"
#include "UIProgressBar.h"
#include "UIPropertiesBox.h"

#include "UIEditBox.h"
#include "UIListWnd.h"

#include "UICharacterInfo.h"
#include "UIItemInfo.h"

class CUICarBodyWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
	bool			m_b_need_update;
public:
	CUICarBodyWnd();
	virtual ~CUICarBodyWnd();

	virtual void Init();
	virtual bool StopAnyMove					(){return true;}

	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	void InitCarBody(CInventory* pOurInv,    CGameObject* pOurObject,
					 CInventory* pOthersInv, CGameObject* pOthersObject);
	
	virtual void Draw();
	virtual void Update();
		
	virtual void Show();

	void DisableAll();
	void EnableAll();
	virtual bool			OnKeyboard			(int dik, EUIMessages keyboard_action);

	//��������� �� ���������
	CInventory*		m_pInv;
	CInventory*		m_pOthersInv;
	CGameObject*	m_pOurObject;
	CGameObject*	m_pOthersObject;
	void			UpdateLists_delayed			();

protected:
	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBottom;

	//���������� � ��������
	CUIFrameWindow		UIDescWnd;
	CUIStatic			UIStaticDesc;
	CUIItemInfo			UIItemInfo;

	CUIStatic			UIOurBagWnd;
	CUIStatic			UIOthersBagWnd;
	CUIDragDropList		UIOurBagList;
	CUIDragDropList		UIOthersBagList;

	//���������� � ���������� 
	CUIStatic			UIOurIcon;
	CUIStatic			UIOthersIcon;
	CUICharacterInfo	UICharacterInfoLeft;
	CUICharacterInfo	UICharacterInfoRight;

	// ������ "����� ��"
	CUIButton			UITakeAll;
	void				UpdateLists					();

	//�������, ����������� ������������ ������������ ������
	//� �������� ����������
	static bool OurBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool OthersBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	
	void				ActivatePropertiesBox		();
	void				EatItem						();

	bool				ToOurBag					();
	bool				ToOthersBag					();
	

	//pop-up ���� ���������� �� ������� ������ ������
	CUIPropertiesBox	UIPropertiesBox;

	DD_ITEMS_VECTOR		m_vDragDropItems;

	//��� ���������� �����
	TIItemContainer		ruck_list;

	//������� � ������� �������� � ������� ������
	PIItem				m_pCurrentItem;
	CUIDragDropItem*	m_pCurrentDragDropItem;

	//������������� ������� �������
	void				SetCurrentItem				(CInventoryItem* pItem);

	// ����� ���
	void				TakeAll						();
};