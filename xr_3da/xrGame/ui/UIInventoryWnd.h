// CUIInventoryWnd.h:  диалог инвентар€
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

	//дл€ работы с сочетателем артефактом извне
	bool IsArtifactMergeShown() {return UIArtifactMergerWnd.IsShown();}
	void AddArtifactToMerger(CArtifact* pArtifact);
	//дл€ добавлени€ новых предметов во врем€ работы с интерфейсом (например 
	//отсоединенных аддонов)
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
	
	//дл€ изображени€ предмета крупным планом
	CUI3dStatic			UI3dStatic;
	//дл€ изображени€ персонажа
	CUI3dStatic			UI3dCharacter;

		
	#define SLOTS_NUM 5
	//слоты дл€ оружи€
	CUIDragDropList		UITopList[SLOTS_NUM]; 
	//отдельный слот дл€ костюмов
	CUIOutfitSlot		UIOutfitSlot;
	
	CUIDragDropList		UIBagList;
	CUIDragDropList		UIBeltList;


	CUIProgressBar		UIProgressBarHealth;
	CUIProgressBar		UIProgressBarSatiety;
	CUIProgressBar		UIProgressBarPower;
	CUIProgressBar		UIProgressBarRadiation;

	//pop-up меню вызываемое по нажатию правой кнопки
	CUIPropertiesBox UIPropertiesBox;
	//менюшка дл€ работы с устройством производства артефактов
	CUIArtifactMerger UIArtifactMergerWnd;

	
	static const int MAX_ITEMS = 70;
	CUIWpnDragDropItem m_vDragDropItems[MAX_ITEMS];
	int m_iUsedItems;

	//указатель на инвентарь, передаетс€ перед запуском меню
	CInventory* m_pInv;

	//элемент с которым работают в текущий момент
	PIItem m_pCurrentItem;
	CUIDragDropItem* m_pCurrentDragDropItem;

	//функции, выполн€ющие согласование отображаемых окошек
	//с реальным инвентарем
	static bool SlotProc0(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc1(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc2(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc3(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc4(CUIDragDropItem* pItem, CUIDragDropList* pList);

	static bool OutfitSlotProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool BagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool BeltProc(CUIDragDropItem* pItem, CUIDragDropList* pList);


	//дл€ запуска меню по правой клавиши
	void ActivatePropertiesBox();

	//описание возоможных дейстивий над предметами инвентар€
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

	//присоединение/отсоединение аддонов к оружию
	void AttachAddon();
	void DetachAddon(const char* addon_name);


	//дополнительные списки дл€ сортировки вещей
	TIItemList ruck_list;
	
	//вещь к которой мы присоедин€ем add-on
	PIItem	m_pItemToUpgrade;
};