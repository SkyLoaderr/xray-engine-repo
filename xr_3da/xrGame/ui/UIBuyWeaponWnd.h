// UIBuyWeaponWnd.h:	окошко, для покупки оружия в режиме CS
//						
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIButton.h"

#include "UIDragDropItem.h"
//#include "UIWpnDragDropItem.h"

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

// Пояс
#define BELT_SLOT			5

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

protected:

	//-----------------------------------------------------------------------------/
	//  новые данные относящиеся к покупке оружия 
	//-----------------------------------------------------------------------------/
	class CUIDragDropItemMP: public CUIDragDropItem
	{
		typedef CUIDragDropItem inherited;
		// возможность хранения номера слота
		// в который можно переместить данную вещь
		u32 slotNum;
		// хранение номера секции оружия
		u32 sectionNum;
		// xранение порядкового номера оружия в секции
		u32 posInSection;
		// Имя секции оружия
		string128 strName;
		// Запоминаем адрес "хозяина"
		CUIDragDropList *m_pOwner;
	public:
		CUIDragDropItemMP(): slotNum			(0),
							 sectionNum			(0),
							 bAddonsAvailable	(false),
							 m_pGLAddon			(NULL),
							 m_pScopeAddon		(NULL),
							 m_pSilencerAddon	(NULL)
		{
			std::strcpy(m_strAddonTypeNames[0], "Silencer");
			std::strcpy(m_strAddonTypeNames[1], "Grenade Launcher");
			std::strcpy(m_strAddonTypeNames[2], "Scope");
		}
		// Для слота
		void SetSlot(u32 slot)					{ R_ASSERT(slot < 6 || slot == static_cast<u32>(-1)); slotNum = slot; }
		u32	 GetSlot()							{ return slotNum; }
		// Для секций
		void SetSection(u32 section)			{ sectionNum = section; }
		u32	 GetSection()						{ return sectionNum; }
		// Функции для запоминания/возвращения имени секции в .ltx файле, этой вещи
		void SetSectionName(const char *pData)	{ std::strcpy(strName, pData); }
		const char *GetSectionName() const		{ return strName; }
		// Запоминаем/возвращеаем указатель на CUIDragDropList которому изначально пренадлежит
		// вещь
		void SetOwner(CUIDragDropList *pOwner)	{ R_ASSERT(pOwner); m_pOwner = pOwner; }
		CUIDragDropList * GetOwner()			{ return m_pOwner; }
		// Номер элемента в секции
		void SetPosInSection(const u32 pos)		{ posInSection = pos; }
		u32 GetPosInSection() const 			{ return posInSection; }

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
			int					iAttachStatus;
			// Координаты смещения относительно иконки оружия
			int					x, y;
			// Constructor
			tAddonInfo():		iAttachStatus(-1), x(-1), y(-1) {}
		} AddonInfo;

		// У каждого оружия максимум 3 аддона. Будем считать, что в массиве они идут в таком поряке:
		// Scope, Silencer, Grenade Launcher.
		enum  AddonIDs		{ ID_SILENCER = 0, ID_GRENADE_LAUNCHER, ID_SCOPE };
		AddonInfo			m_AddonInfo[3];
		// Если у вещи вообще нет аддонов, то поднимаем этот флажек, для ускорения проверок
		bool				bAddonsAvailable;
		// Список название типов аддонов
		char				m_strAddonTypeNames[3][25];

		// Аттачим/детачим аддоны
		void				AttachDetachAddon(int iAddonIndex, bool bAttach);
		void				AttachDetchAllAddons(bool bAttach);
		bool				IsAddonAttached(int iAddonIndex) { return m_AddonInfo[iAddonIndex].iAttachStatus == 1; }
		// Переопределяем некоторые функции, которые нам нужны для коректной отрисовки оружия с аддонами
		virtual void		ClipperOn();
		virtual void		ClipperOff();
		virtual void		Draw();
		// реальные драг-дроп вещи для тех аддонов, которые существуют для оружия
		CUIDragDropItemMP	*m_pScopeAddon;
		CUIDragDropItemMP	*m_pSilencerAddon;
		CUIDragDropItemMP	*m_pGLAddon;
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


//	#define NO_ACTIVE_SLOT		0xffffffff
//	#define KNIFE_SLOT			0
//	#define PISTOL_SLOT			1
//	#define RIFLE_SLOT			2
//	#define GRENADE_SLOT		3
//	#define APPARATUS_SLOT		4
	#define BELT_SLOT			5
//	#define OUTFIT_SLOT			6
//	#define PDA_SLOT			7 
	#define MP_SLOTS_NUM		8
	//слоты для оружия
	CUIDragDropList		UITopList[MP_SLOTS_NUM]; 
	//отдельный слот для костюмов
	CUIOutfitSlot		UIOutfitSlot;

//	CUIDragDropList		UIBagList;
//	CUIDragDropList		UIBeltList;

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
		ATTACH_SILENCER_ADDON,
		ATTACH_GRENADE_LAUNCHER_ADDON,
		ATTACH_SCOPE_ADDON, 
		DETACH_SILENCER_ADDON,
		DETACH_GRENADE_LAUNCHER_ADDON,
		DETACH_SCOPE_ADDON};

	//выбросить элемент
	void DropItem();
	//съесть элемент
//	void EatItem();

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
//	void AttachAddon();
//	void DetachAddon(const char* addon_name);

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
	// заполнить отдельную секцию оружием
	void FillWpnSubBag(const u32 slotNum);
	// Инициализируем аддоны у вещи
	void InitAddonsInfo(CUIDragDropItemMP &DDItemMP, const std::string &sectioName);
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
	// массив индексов пустых элементов
	std::set<int> m_iEmptyIndexes;
	// Получить первый свободный индекс в массиве m_vDragDropItems
	int GetFirstFreeIndex();

	// Дополнительные функции для получения информации о вещах в слотах
	CUIDragDropItemMP * GetWeapon(u32 slotNum, u32 idx = 0);
	const u8 GetItemIndex(u32 slotNum, u32 idxInArr, u8 &sectionNum);

	// Поддержка клавиатурного режима покупки оружия

	// Есть 3 уровня глубины вложенности меню:
	// 0) root			- меню уровня табконтрола
	// 1) wpnsubtype	- меню уровня выбора оружия
	// 2) addons		- меню уровня выбора аддонов, если оружие позволяет
	enum MENU_LEVELS	{ mlRoot = 0, mlWpnSubType, mlAddons };
	// Текущий уровень
	MENU_LEVELS			m_mlCurrLevel;

	// Переход на любой уровень меню непосредственно
	bool MenuLevelJump(MENU_LEVELS lvl);
	// Переход на уровень выше/ниже. Возвращаем true, если переход удачен, и 
	// false если мы уже находимся на высшем/низшем уровне меню
	bool MenuLevelUp()		{ return MenuLevelJump(static_cast<MENU_LEVELS>(m_mlCurrLevel + 1)); }
	bool MenuLevelDown()	{ return MenuLevelJump(static_cast<MENU_LEVELS>(m_mlCurrLevel - 1)); }
	// callback функция для отрисовки цифровых подписей к оружию
	friend void WpnDrawIndex(CUIDragDropItem *pDDItem);
	// Удалить элемент из листа по его позиции в списке секций.
	void RemoveItemByPos(const u32 sectionNum, CUIDragDropList *pDDList);
public:
	// Обработчик нажатий на кнопки клавы
	virtual bool OnKeyboard(int dik, E_KEYBOARDACTION keyboard_action);
	// Получить имя секции в weapon.ltx соответствующий оружию в слоте или на поясе
	const char *GetWeaponName(u32 slotNum);
	const char *GetWeaponNameInBelt(u32 indexInBelt);
	// Получить индекс оружия в массиве доступных, из заданного слота.
	// Первое оружие имеет индекс 0. Если в слоте нет ничего, то возвращаем -1
	const u8 GetWeaponIndex(u32 slotNum);
	const u8 GetWeaponIndexInBelt(u32 indexInBelt, u8 &sectionId, u8 &itemId);
	// Получить имя оружия по индексу, и номеру слота
	const char *GetWeaponNameByIndex(u32 slotNum, u8 idx);
	// Получить данные о аддонах к оружию. Младшие 3 бита, если установлены в 1 означают:
	// 2 - Silencer, 1 - Grenade Launcher, 0 - Scope
	const u8 GetWeaponAddonInfoByIndex(u8 idx);
	// Получить размер пояса в элементах
	const u8 GetBeltSize();
	// Перезагрузка предметов
	void		ReInitItems	(char *strSectionName);
};