// UIBuyWeaponWnd.h:	окошко, для покупки оружия в режиме CS
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
#include "UITabControl.h"

class CArtifact;

class CUIBuyWeaponWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
public:
	CUIBuyWeaponWnd();
	virtual ~CUIBuyWeaponWnd();

	virtual void Init();

	void InitInventory();


	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);


	CInventory* GetInventory() {return m_pInv;}

	virtual void Update();
	virtual void Draw();

	virtual void Show();
	virtual void Hide();

	//для работы с сочетателем артефактом извне
	bool IsArtifactMergeShown() {return UIArtifactMergerWnd.IsShown();}
	void AddArtifactToMerger(CArtifact* pArtifact);
	//для добавления новых предметов во время работы с интерфейсом (например 
	//отсоединенных аддонов)
	void AddItemToBag(PIItem pItem);
protected:

	CUIFrameWindow		UIBagWnd;
	CUIFrameWindow		UIDescWnd;
	CUIFrameWindow		UIPersonalWnd;

	//конпка активации окошка сна
	CUIButton			UISleepButton;

	// Набор закладок для экранов с оружием
	CUITabControl		UIWeaponsTabControl;

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


#define SLOTS_NUM 5
	//слоты для оружия
	CUIDragDropList		UITopList[SLOTS_NUM]; 
	//отдельный слот для костюмов
	CUIOutfitSlot		UIOutfitSlot;

//	CUIDragDropList		UIBagList;
	CUIDragDropList		UIBeltList;


	CUIProgressBar		UIProgressBarHealth;
	CUIProgressBar		UIProgressBarSatiety;
	CUIProgressBar		UIProgressBarPower;
	CUIProgressBar		UIProgressBarRadiation;

	//pop-up меню вызываемое по нажатию правой кнопки
	CUIPropertiesBox	UIPropertiesBox;
	//менюшка для работы с устройством производства артефактов
	CUIArtifactMerger	UIArtifactMergerWnd;
	//окошко для параметров сна
	CUISleepWnd			UISleepWnd;

	// разбивка всего оружия на отдельные DragDrop листы по категориям: пистолеты, автоматы, etc.
	DEF_VECTOR (WEAPON_TYPES, CUIDragDropList*)
	WEAPON_TYPES		m_WeaponSubBags;

	//информация о персонаже
	CUICharacterInfo UICharacterInfo;
	//информация о предмете
	CUIItemInfo UIItemInfo;


	static const int MAX_ITEMS = 70;
	CUIWpnDragDropItem m_vDragDropItems[MAX_ITEMS];
	int m_iUsedItems;

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


	//для запуска меню по правой клавиши
	void ActivatePropertiesBox();

	//описание возоможных дейстивий над предметами инвентаря
	enum {DROP_ACTION, EAT_ACTION, TO_BELT_ACTION, 
		TO_SLOT_ACTION, TO_BAG_ACTION,
		ARTIFACT_MERGER_ACTIVATE,
		ARTIFACT_MERGER_DEACTIVATE,
		ATTACH_ADDON, 
		DETACH_SCOPE_ADDON,
		DETACH_SILENCER_ADDON,
		DETACH_GRENADE_LAUNCHER_ADDON};

	//выбросить элемент
	void DropItem();
	//съесть элемент
	void EatItem();

	//перемещение вещи
	bool ToSlot();
	bool ToBag();
	bool ToBelt();


	//запуск и остановка меню работы с артефактами
	void StartArtifactMerger();
	void StopArtifactMerger();

	//запуск и остановка меню работы с артефактами
	void StartSleepWnd();
	void StopSleepWnd();

	//присоединение/отсоединение аддонов к оружию
	void AttachAddon();
	void DetachAddon(const char* addon_name);

	//устанавливает текущий предмет
	void SetCurrentItem(CInventoryItem* pItem);


	//дополнительные списки для сортировки вещей
	TIItemList ruck_list;

	//вещь к которой мы присоединяем add-on
	PIItem	m_pItemToUpgrade;

	//слот который был активным перед вызовом менюшки
	u32	m_iCurrentActiveSlot;
};