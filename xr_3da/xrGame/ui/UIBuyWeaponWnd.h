// UIBuyWeaponWnd.h:	окошко, для покупки оружия в режиме CS
//						
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIDialogWnd.h"
#include "UIDragDropList.h"
#include "UIProgressBar.h"

#include "UIPropertiesBox.h"
#include "UIOutfitSlot.h"

#include "UICharacterInfo.h"
#include "UIItemInfo.h"
#include "UITabControl.h"
#include "UIListItem.h"
#include "UIMultiTextStatic.h"
#include "UIDragDropItemMP.h"

#define 	BELT_SLOT			5
#define 	MP_SLOTS_NUM		8
#define		WEAPON_BOXES_SLOT	MP_SLOTS_NUM + 1

extern const u32	cDetached;
extern const u32	cAttached;
extern const u32	cUnableToBuy;
extern const u32	cAbleToBuy;
extern const u32	cAbleToBuyOwned;
extern const float	fRealItemSellMultiplier;

class CUIBuyWeaponWnd: public CUIDialogWnd
{
	friend class CUIDragDropItemMP;
private:
	typedef CUIDialogWnd inherited;
public:
	CUIBuyWeaponWnd(LPCSTR strSectionName, LPCSTR strPricesSection);
	virtual ~CUIBuyWeaponWnd();

	virtual void Init(LPCSTR strSectionName);

	void InitInventory();


	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);


//	CInventory* GetInventory() {return m_pInv;}

	virtual void Update();
	virtual void Draw();

	virtual void Show();
	virtual void Hide();

protected:
	//-----------------------------------------------------------------------------/
	//  новые данные относящиеся к покупке оружия 
	//-----------------------------------------------------------------------------/

	CUIStatic			UIBagWnd;
	CUIStatic			UIDescWnd;
	CUIStatic			UIPersonalWnd;

	// Набор закладок для экранов с оружием
	CUITabControl		UIWeaponsTabControl;
	// Статик для  вывода надписей на кнопках
	CUIMultiTextStatic	UIMTStatic;

	// и процедура для их инициализации
	void InitBackgroundStatics();

	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBelt;
	CUIStatic			UIStaticBottom;

	CUIStatic			UIStaticTabCtrl;
	// Зелененькая лампочка, которой мы метим текущую выбранную закладку
	CUIStatic			UIGreenIndicator;

	CUIStatic			UIStaticDesc;
	CUIStatic			UIStaticPersonal;

	// Индикаторы денег
	CUIStatic			UITotalMoneyHeader;
	CUIStatic			UITotalMoney;
	CUIStatic			UIItemCostHeader;
	CUIStatic			UIItemCost;
	CUIListWnd			UIWeaponDescription;

	//слоты для оружия
	CUIDragDropList		UITopList[MP_SLOTS_NUM]; 
	//отдельный слот для костюмов
//	CUIOutfitSlot		UIOutfitSlot;
	CUIStatic			UIOutfitIcon;		

	//pop-up меню вызываемое по нажатию правой кнопки
	CUIPropertiesBox	UIPropertiesBox;

	// разбивка всего оружия на отдельные DragDrop листы по категориям: пистолеты, автоматы, etc.
	DEF_VECTOR			(WEAPON_TYPES, CUIDragDropList*)
	WEAPON_TYPES		m_WeaponSubBags;

	static const int MAX_ITEMS = 70;
	CUIDragDropItemMP m_vDragDropItems[MAX_ITEMS];
	int m_iUsedItems;

	//элемент с которым работают в текущий момент
	PIItem m_pCurrentItem;
	CUIDragDropItemMP* m_pCurrentDragDropItem;

	// Информация о вещи
	CUIItemInfo UIItemInfo;

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
	void StartArtefactMerger();
	void StopArtefactMerger();

	//запуск и остановка меню работы с артефактами
	void StartSleepWnd();
	void StopSleepWnd();

	//присоединение/отсоединение аддонов к оружию
//	void AttachAddon();
//	void DetachAddon(const char* addon_name);

	//устанавливает текущий предмет
//	void SetCurrentItem(CInventoryItem* pItem);


	//дополнительные списки для сортировки вещей
//	TIItemList ruck_list;

	//вещь к которой мы присоединяем add-on
//	PIItem	m_pItemToUpgrade;

	//слот который был активным перед вызовом менюшки
	u32	m_iCurrentActiveSlot;

	//-----------------------------------------------------------------------------/
	//  новые данные относящиеся к покупке оружия 
	//-----------------------------------------------------------------------------/

	// Имя секции из которой читать данные
	ref_str		m_StrSectionName;
	ref_str		m_StrPricesSection;

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
	// Кнопки OK и Cancel
	CUIListItem UIBtnOK, UIBtnCancel;
	// массив индексов пустых элементов
	std::set<int> m_iEmptyIndexes;
	// Получить первый свободный индекс в массиве m_vDragDropItems
	int GetFirstFreeIndex();

	// Дополнительные функции для получения информации о вещах в слотах
	// Params:	slotNum	- номер слота (раздела) в котором ищем 
	//			idx		- порядковый индекс оружия в слоте (для всех слотов кроме пояса должно быть == 0)
	// Return:	Указатель на затребованную вещь, либо NULL, если не найдено
	CUIDragDropItemMP * GetWeapon(u32 slotNum, u32 idx = 0);
	// Params:	slotNum		- см. предыдущую функцию
	//			idx			- см. предыдущую функцию
	//			sectionNum	- возвращаем порядковый номер секции в списке секций
	// Return:	Индекс затребованной вещи, либо 0xff(-1), если не найдено
	const u8 GetItemIndex(u32 slotNum, u32 idx, u8 &sectionNum);

	// Поддержка клавиатурного режима покупки оружия

	// Есть 3 уровня глубины вложенности меню:
	// 0) root			- меню уровня табконтрола
	// 1) wpnsubtype	- меню уровня выбора оружия
	// 2) addons		- меню уровня выбора аддонов, если оружие позволяет
	enum MENU_LEVELS	{ mlRoot = 0, mlBoxes, mlWpnSubType };
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
	// Включаем, выключаем индикатор активной кнопки табконтрола
	void SwitchIndicator(bool bOn, const int activeTabIndex);
	// Запрос количества денег у игрока

	// Работа с деньгами

	// Количество
	int			m_iMoneyAmount;
	// Игнорируем изменения денег
	bool		m_bIgnoreMoney;
	// Проверка всех вещей на возможность покупки (достаточно ли денег?)
	// Если вещь невозможно купить, то помечаем ее красным и запрещаем ее перетаскивание
	// проверяем только в листах с доступным оружием
	void		CheckBuyAvailabilityInShop();

	// Работа с арморами

	// Вернуть номер текущего армора.
	// На данный момент: - 1 - нет, 0 - экзоскелет, 1 - научный
	const u8	GetCurrentSuit();
	
	// Таблица соответсвия имени армора с именами моделей персонажей. Заполняется на этапе считывания 
	// информации из ltx файла соответствующего типу сетевой игры
	typedef xr_vector<std::pair<ref_str, ref_str> >	CONFORMITY_TABLE;
	typedef CONFORMITY_TABLE::iterator				CONFORMITY_TABLE_it;
	CONFORMITY_TABLE								m_ConformityTable;
	
	// Координаты дефолтного скина на текстуре с иконками сталкеров.
	int			m_iIconTextureX, m_iIconTextureY;
	// Функция проверки пренадлежности вещи секции (магазину а не слоту)
	// Params:	idx		- индекс в массиве оружия, который требуется проверить
	// Return:	true если оружие находится в магазине.
	bool		IsItemInShop(int idx);
	
	// Применить фильтр к секции с определенным оружием по заданной записи в ltx файле
	void		ApplyFilter(int slotNum, const ref_str &name, const ref_str &value);
	// Заполняем лист с ящиками
	void		InitWeaponBoxes();
	void		RemoveWeapon(CUIDragDropList *shop, CUIDragDropItem *item);
	void		FillItemInfo(CUIDragDropItemMP *pDDItemMP);

public:
	// Проверяем цены вещей в слотах. Это нужно для того, чтобы после респавна небыло
	// возможности купить предыдущее оружие установленное в слотах, если нет денег.
	// Return:	true - если в все оружие в слотах мы можем купить.
	bool		CheckBuyAvailabilityInSlots();
	// Возвращаем true если все вещи в слотах можно купить.
	bool		CanBuyAllItems();
	// Устанавливаем режим работы с деньгами: игнорировать, или нет.
	void		IgnoreMoney(bool ignore)		{ m_bIgnoreMoney = ignore; }
	// Получаем количество денег
	int			GetMoneyAmount() const;
	// Устанавливам количество денег
	void		SetMoneyAmount(int moneyAmount);

	// Процедура запоминания индекса скина в списке skins типа сетевой игры, после выбора его игроком
	void		SetSkin			(u8 SkinID);	

	// А не является ли данная вещь чьим-то аддоном?
	// Возвращаем адрес хозяина аддона, если нашли и тип аддона
	// Проверка ведется только для оружия в слотах
	CUIDragDropItemMP * IsItemAnAddon(CUIDragDropItemMP *pPossibleAddon, CUIDragDropItemMP::AddonIDs &ID);
	// Примитивная проверка на то, что вещь является аддоном
	bool		IsItemAnAddonSimple(CUIDragDropItemMP *pPossibleAddon) const;
	// Получить указатель на аддон по ID. В случае несуществования аддона возвращаем NULL.
	CUIDragDropItemMP * GetAddonByID(CUIDragDropItemMP *pAddonOwner, CUIDragDropItemMP::AddonIDs ID);
	// Обработчик нажатий на кнопки клавы
	virtual bool OnKeyboard(int dik, E_KEYBOARDACTION keyboard_action);

	// Получить имя секции в weapon.ltx соответствующий оружию в слоте или на поясе
	// Params:	slotNum		- номер слота в котором смотрим
	// Result:	указатель на внутренний буфер с именем (удалять его не надо!)
	const char	*GetWeaponName(u32 slotNum);
	// Params:	indexInBelt	- номер вещи на поясе
	// Result:	указатель на внутренний буфер с именем (удалять его не надо!)
	const char	*GetWeaponNameInBelt(u32 indexInBelt);

	// Получить индекс оружия в массиве доступных, из заданного слота.
	// Первое оружие имеет индекс 0. Если в слоте нет ничего, то возвращаем -1
	const u8	GetWeaponIndex(u32 slotNum);
	const u8	GetWeaponIndexInBelt(u32 indexInBelt, u8 &sectionId, u8 &itemId);
	
	// Получить имя оружия по индексу, и номеру слота
	const char	*GetWeaponNameByIndex(u32 grpNum, u8 idx);
	// Получить номер в списке секций и номер списка секций по имени
	// Если не найдено, то grpNum == -1 и idx == -1
	void		GetWeaponIndexByName(const std::string sectionName, u8 &grpNum, u8 &idx);

	// Получить данные о аддонах к оружию. Младшие 3 бита, если установлены в 1 означают:
	// 2 - Silencer, 1 - Grenade Launcher, 0 - Scope
	const u8	GetWeaponAddonInfoByIndex(u8 idx);
	// Получить размер пояса в элементах
	const u8	GetBeltSize();
	// Перезагрузка предметов
	void		ReInitItems	(LPCSTR strSectionName);

	// Процедуры принудительного перемещения оружия (эмуляция даблклика).
	// Params:	grpNum					- номер группы секций
	//			uIndexInSlot			- порядковый уровень оружия в списке группы секций
	//			bRealRepresentationSet	- новое флага , который указывает на наличие у вещи реального 
	//									  представления
	void		MoveWeapon(const u8 grpNum, const u8 uIndexInSlot, bool bRealRepresentationSet) {}
	// Params:	sectionName		- имя конфигурационной секции оружия
	void		MoveWeapon(const char *sectionName, bool bRealRepresentationSet) {}

	// Принудительное перемещение оружия из секций в слоты (покупка), посредством даблклика
	// Params:	grpNum					- номер группы секций
	//			uIndexInSlot			- порядковый уровень оружия в списке группы секций.
	//									  Возможны установленные флаги аддонов.
	//			bRealRepresentationSet	- флаг, который указывает на наличие у вещи реального 
	//									  представления
	void		SectionToSlot(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet);
	// Params:	sectionName				- имя конфигурационной секции оружия
	void		SectionToSlot(const char *sectionName, bool bRealRepresentationSet);

	// Функция перемещения всех вещей из слотов обратно в секции
	void		ClearSlots();
	// Функция сброса флага реальной репрезентации вещи для всех вещей в слотах
	void		ClearRealRepresentationFlags();
};