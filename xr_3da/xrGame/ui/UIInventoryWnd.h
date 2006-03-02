// CUIInventoryWnd.h:  диалог инвентаря
// 
//////////////////////////////////////////////////////////////////////

#pragma once

class CInventory;

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UI3tButton.h"

#include "UIDragDropItem.h"
#include "UIWpnDragDropItem.h"

#include "UIDragDropList.h"
#include "UIProgressBar.h"

#include "UIPropertiesBox.h"
#include "UIOutfitSlot.h"

#include "UIOutfitInfo.h"
#include "UIItemInfo.h"
#include "UITimeWnd.h"


class CArtefact;
class CUISleepWnd;


class CUIInventoryWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
	bool					m_b_need_reinit;
public:
							CUIInventoryWnd();
	virtual					~CUIInventoryWnd();

	virtual void			Init();

	void					InitInventory();
	void					InitInventory_delayed		();
	virtual bool			StopAnyMove					(){return false;}

	virtual void			SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
	virtual bool			OnMouse(float x, float y, EUIMessages mouse_action);


	CInventory*				GetInventory() {return m_pInv;}

	virtual void			Update();
	virtual void			Draw();

	virtual void			Show();
	virtual void			Hide();

	//для добавления новых предметов во время работы с интерфейсом (например 
	//отсоединенных аддонов)
	void					AddItemToBag(PIItem pItem);

	// cнять костюм
	bool					UndressOutfit();

	// Получить указатель на окно сумки. Используется при автоматическом вынимании текущей вещи
	// из слотов, при дропе на них новых вещей
	CUIDragDropList *		GetBag() { return &UIBagList; }

	// Проверить принадлежность вещи к нужному слоту и попробовать освободить для нее место
	// переместив текущую вещь в слоте в сумку
	bool					SlotToBag(PIItem pItem, CUIDragDropList *pList, const u32 SlotNum);
	bool					SlotToBelt(PIItem pItem, CUIDragDropList *pList, const u32 SlotNum);
	virtual bool			OnKeyboard			(int dik, EUIMessages keyboard_action);

protected:
	enum eInventorySndAction{	eInvSndOpen	=0,
								eInvSndClose,
								eInvItemToSlot,
								eInvItemToBelt,
								eInvItemToRuck,
								eInvProperties,
								eInvDropItem,
								eInvAttachAddon,
								eInvDetachAddon,
								eInvSndMax};
	ref_sound				sounds[eInvSndMax];
	void PlaySnd					(eInventorySndAction a);
	friend class CUITradeWnd;

	CUIStatic			UIBagWnd;
	CUIStatic			UIMoneyWnd;
	CUIStatic			UIDescrWnd;
	CUIFrameWindow		UIPersonalWnd;

	// Подокошко сна
	CUISleepWnd*			UISleepWnd;
	// sell all items
	CUI3tButton*			UISellAll;
	
	// Кнопка выброса активного предмета
	CUIButton			UIDropButton;
	CUIButton			UIExitButton;

	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBelt;
	CUIStatic			UIStaticBottom;
	CUITimeWnd			UITimeWnd;

//	CUIStatic			UIStaticDesc;
	CUIStatic			UIStaticPersonal;
		
	#define SLOTS_NUM 5
	//слоты для оружия
	CUIDragDropList		UITopList[SLOTS_NUM]; 
	//отдельный слот для костюмов
	CUIOutfitSlot		UIOutfitSlot;
	
	CUIDragDropList		UIBagList;
	CUIDragDropList		UIBeltList;


	CUIProgressBar		UIProgressBarHealth;
	CUIProgressBar		UIProgressBarSatiety;
	CUIProgressBar		UIProgressBarPsyHealth;
	CUIProgressBar		UIProgressBarRadiation;

	//pop-up меню вызываемое по нажатию правой кнопки
	CUIPropertiesBox	UIPropertiesBox;
	
	//информация о персонаже
	//CUICharacterInfo UICharacterInfo;
	CUIOutfitInfo UIOutfitInfo;
	//информация о предмете
	CUIItemInfo UIItemInfo;

	//список элементов drag drop
	DD_ITEMS_VECTOR	m_vDragDropItems;
	
	//указатель на инвентарь, передается перед запуском меню
	CInventory* m_pInv;

	//элемент с которым работают в текущий момент
	PIItem m_pCurrentItem;
	CUIDragDropItem* m_pCurrentDragDropItem;

	//функции, выполняющие согласование отображаемых окошек
	//с реальным инвентарем
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
	static void	SendEvent_Item_Drop			(PIItem	pItem);
	static void	SendEvent_Item_Eat			(PIItem	pItem);
	static void SendEvent_Item_Sell			(PIItem	pItem);
	static void SendEvent_ActivateArtefact	(PIItem	pItem);
	//---------------------------------------------------------------------
	//для запуска меню по правой клавиши
	void ActivatePropertiesBox();

	//активировать артефакт
	void Activate_Artefact();

	//выбросить элемент
	void DropItem();
	//съесть элемент
	void EatItem();
	
	//перемещение вещи
	bool ToSlot();
	bool ToBag();
	bool ToBelt();
	void SellItem();


	//запуск и остановка меню работы с артефактами
	void StartArtefactMerger();
	void StopArtefactMerger();

	//присоединение/отсоединение аддонов к оружию
	void AttachAddon();
	void DetachAddon(const char* addon_name);

	//устанавливает текущий предмет
	void SetCurrentItem(CInventoryItem* pItem);

	//дополнительные списки для сортировки вещей
	TIItemContainer ruck_list;
	
	//вещь к которой мы присоединяем add-on
	PIItem	m_pItemToUpgrade;

	//слот который был активным перед вызовом менюшки
	u32	m_iCurrentActiveSlot;
};