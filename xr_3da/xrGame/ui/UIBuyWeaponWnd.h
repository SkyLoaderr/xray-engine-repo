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
	CUIBuyWeaponWnd(char *strSectionName);
	virtual ~CUIBuyWeaponWnd();

	virtual void Init(char *strSectionName);

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

	//-----------------------------------------------------------------------------/
	//  новые данные относящиеся к покупке оружия 
	//-----------------------------------------------------------------------------/
	class CUIDragDropItemMP: public CUIWpnDragDropItem
	{
		// возможность хранения номера слота
		// в который можно переместить данную вещь
		u32 slotNum;
		// хранение номера секции оружия
		u32 sectionNum;
		// Имя секции оружия
		string128 strName;
		// Запоминаем адрес "хозяина"
		CUIDragDropList *m_pOwner;
	public:
		CUIDragDropItemMP(): slotNum(0), sectionNum(0) {}
		// Для слота
		void SetSlot(u32 slot) { R_ASSERT(slot < 6); slotNum = slot; }
		u32	 GetSlot() { return slotNum; }
		// Для секций
		void SetSection(u32 section) { sectionNum = section; }
		u32	 GetSection() { return sectionNum; }
		// Функции для запоминания/возвращения имени секции в .ltx файле, этой вещи
		void SetSectionName(const char *pData) { std::strcpy(strName, pData); }
		const char *GetSectionName() const { return strName; }
		// Запоминаем/возвращеаем указатель на CUIDragDropList которому изначально пренадлежит
		// вещь
		void SetOwner(CUIDragDropList *pOwner) { R_ASSERT(pOwner); m_pOwner = pOwner; }
		CUIDragDropList * GetOwner() { return m_pOwner; }

		//-----------------------------------------------------------------------------/
		//  Работа с аддонами. Средствами переопределения CWeapon нужную функциональность
		//	получить не удалось
		//-----------------------------------------------------------------------------/

		// Структура информации о аддоне
		typedef struct tAddonInfo
		{
			// Имя секции для аддона
			std::string			strAddonName;
			// -1 - вообще нельзя приаттачить, 0 - не приаттачен, 1 - приаттачен
			int					bIsAttached;
			// Координаты смещения относительно иконки оружия
			int					x, y;
			// Constructor
			tAddonInfo():		bIsAttached(false), x(-1), y(-1) {}
		} AddonInfo;

		// У каждого оружия максимум 3 аддона. Будем считать, что в массиве они идут в таком поряке:
		// Scope, Silencer, Grenade Launcher.
		AddonInfo	m_addonInfo[3];
		
		void AttachDetachAddon(int iAddonIndex, bool bAttach = true)
		{
			R_ASSERT(iAddonIndex >= 0 && iAddonIndex < 4);
			m_addonInfo[iAddonIndex].bIsAttached = bAttach ? 1 : 0;
		}
	};

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
	CUIDragDropItemMP m_vDragDropItems[MAX_ITEMS];
	int m_iUsedItems;

	//указатель на инвентарь, передается перед запуском меню
	CInventory* m_pInv;

	//элемент с которым работают в текущий момент
	PIItem m_pCurrentItem;
	CUIDragDropItemMP* m_pCurrentDragDropItem;

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
	enum {BUY_ITEM_ACTION, CANCEL_BUYING_ACTION,
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

	//-----------------------------------------------------------------------------/
	//  новые данные относящиеся к покупке оружия 
	//-----------------------------------------------------------------------------/

	// массив в котором хратнятся названия секций для оружия 
	DEF_VECTOR(WPN_SECT_NAMES, std::string);
	// Вектор массивов с именами секций для оружия
	DEF_VECTOR(WPN_LISTS, WPN_SECT_NAMES);
	WPN_LISTS	wpnSectStorage;
	// Функция инициализации массива строк - имен секций для оружия, разгруппированных по типам
	void InitWpnSectStorage();
	// заполнить секции оружием которое мы определили предыдущей процедурой
	void FillWpnSubBags();
	// заполнить заданную секцию оружием которое мы определили процедурой InitWpnSectStorage
	void FillWpnSubBag(const u32 slotNum);
	// удаляем все из секций
	void ClearWpnSubBags();
	// удаляем все из заданной секции
	void ClearWpnSubBag(const u32 slotNum);
	// переместить вещь из слота обратно в сответствующую секцию.
	bool SlotToSection(const u32 SlotNum);
	// переместить вещь с пояса в сумку
	bool BeltToSection(CUIDragDropItemMP *pDDItemMP);
	// Проверка возможности помещения вещи в слот
	bool CanPutInSlot(CUIDragDropItemMP *pDDItemMP, const u32 slotNum);
	// Проверка возможности помещения вещи на пояс
	bool CanPutInBelt(CUIDragDropItemMP *pDDItemMP);
	// Запомнить имя секции из которой читать списки оружия
	string64	m_SectionName;
	// Кнопки OK и Cancel
	CUIButton	UIBtnOK, UIBtnCancel;
	// Спецмассив для запоминания индексов гранат в массиве.
	xr_vector<int> m_vGrenadeArr;

public:
	// Получить имя секции в weapon.ltx соответствующий оружию в слоте
	const char *GetWeaponName(u32 slotNum);
	// Получить индекс оружия в массиве доступных, из заданного слота.
	// Первое оружие имеет индекс 0. Если в слоте нет ничего, то возвращаем -1
	const u8 GetWeaponIndex(u32 slotNum);
	// Получить имя оружия по индексу, и номеру слота
	const char *GetWeaponNameByIndex(u32 slotNum, u8 idx);
	// Перезагрузка предметов
	void		ReInitItems	(char *strSectionName);
};