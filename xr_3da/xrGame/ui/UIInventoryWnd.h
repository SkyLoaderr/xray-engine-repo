// CUIInventoryWnd.h:  диалог инвентаря
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

#include "UICharacterInfo.h"
#include "UIItemInfo.h"

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

	//для работы с сочетателем артефактом извне
	bool IsArtefactMergeShown() {return UIArtefactMergerWnd.IsShown();}
	void AddArtefactToMerger(CArtefact* pArtefact);
	//для добавления новых предметов во время работы с интерфейсом (например 
	//отсоединенных аддонов)
	void AddItemToBag(PIItem pItem);

	// cнять костюм
	bool UndressOutfit();

	// Получить указатель на окно сумки. Используется при автоматическом вынимании текущей вещи
	// из слотов, при дропе на них новых вещей
	CUIDragDropList * GetBag() { return &UIBagList; }

	// Проверить принадлежность вещи к нужному слоту и попробовать освободить для нее место
	// переместив текущую вещь в слоте в сумку
	bool SlotToBag(PIItem pItem, CUIDragDropList *pList, const u32 SlotNum);

protected:
	friend class CUITradeWnd;

	CUIStatic			UIBagWnd;
	CUIStatic			UIDescrWnd;
	CUIFrameWindow		UIPersonalWnd;

	// Подокошко сна
	CUISleepWnd			UISleepWnd;
	
	// Кнопка выброса активного предмета
	CUIButton			UIDropButton;
	CUIButton			UIExitButton;

	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBelt;
	CUIStatic			UIStaticBottom;
	CUIStatic			UIStaticTime;
	CUIStatic			UIStaticWeight;

	// Update current time indicator
	void				UpdateTime();

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
	CUIProgressBar		UIProgressBarPower;
	CUIProgressBar		UIProgressBarRadiation;

	//pop-up меню вызываемое по нажатию правой кнопки
	CUIPropertiesBox	UIPropertiesBox;
	//менюшка для работы с устройством производства артефактов
	CUIArtefactMerger	UIArtefactMergerWnd;
	
	//информация о персонаже
	CUICharacterInfo UICharacterInfo;
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
	static void	SendEvent_ItemDrop			(PIItem	pItem);
	//---------------------------------------------------------------------
	//для запуска меню по правой клавиши
	void ActivatePropertiesBox();

	//выбросить элемент
	void DropItem();
	//съесть элемент
	void EatItem();
	
	//перемещение вещи
	bool ToSlot();
	bool ToBag();
	bool ToBelt();


	//запуск и остановка меню работы с артефактами
	void StartArtefactMerger();
	void StopArtefactMerger();

	//присоединение/отсоединение аддонов к оружию
	void AttachAddon();
	void DetachAddon(const char* addon_name);

	//устанавливает текущий предмет
	void SetCurrentItem(CInventoryItem* pItem);

	// Отобразить вес, который несет актер
	void UpdateWeight();
		

	//дополнительные списки для сортировки вещей
	TIItemList ruck_list;
	
	//вещь к которой мы присоединяем add-on
	PIItem	m_pItemToUpgrade;

	//слот который был активным перед вызовом менюшки
	u32	m_iCurrentActiveSlot;
};