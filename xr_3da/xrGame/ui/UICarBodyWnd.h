// UICarBodyWnd.h:  диалог переложени€ вещей из багажника и с трупа 
//					себе в инвентарь
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


	//указатели на инвентари
	CInventory*		m_pInv;
	CInventory*		m_pOthersInv;
	CGameObject*	m_pOurObject;
	CGameObject*	m_pOthersObject;



protected:
	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBottom;

	//дл€ изображени€ предмета крупным планом	
	CUIStatic			UIStaticDesc;
	CUI3dStatic			UI3dStatic;

	CUIFrameWindow		UIOurBagWnd;
	CUIFrameWindow		UIOthersBagWnd;
	CUIDragDropList		UIOurBagList;
	CUIDragDropList		UIOthersBagList;


	//функции, выполн€ющие согласование отображаемых окошек
	//с реальным инвентарем
	static bool OurBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool OthersBagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);

	//провер€ет влезут ли вещи из окна торговли в рюкзак после покупки
	bool IsEnoughtOurRoom(CUIDragDropItem* pItem);
	bool IsEnoughtOthersRoom(CUIDragDropItem* pItem);

	bool ToOurBag();
	bool ToOthersBag();
	
	//произвести торговлю
	void UpdateLists();

	//pop-up меню вызываемое по нажатию правой кнопки
	CUIPropertiesBox UIPropertiesBox;
	//окно с сообщением
	CUIMessageBox	 UIMessageBox;


	static const int MAX_ITEMS = 140;
	CUIDragDropItem m_vDragDropItems[MAX_ITEMS];
	int m_iUsedItems;

	//дл€ сортировки вещей
	TIItemList ruck_list;

	//элемент с которым работают в текущий момент
	PIItem m_pCurrentItem;
	CUIDragDropItem* m_pCurrentDragDropItem;
};