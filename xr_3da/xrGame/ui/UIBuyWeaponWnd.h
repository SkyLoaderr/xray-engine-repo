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
#include "UIListItem.h"

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


//	CInventory* GetInventory() {return m_pInv;}

	virtual void Update();
	virtual void Draw();

	virtual void Show();
	virtual void Hide();

protected:
//	#define NO_ACTIVE_SLOT			0xffffffff
//	#define KNIFE_SLOT				0
//	#define PISTOL_SLOT				1
//	#define RIFLE_SLOT				2
//	#define GRENADE_SLOT			3
//	#define APPARATUS_SLOT			4
	#define BELT_SLOT				5
//	#define OUTFIT_SLOT				6
//	#define PDA_SLOT				7 
	#define MP_SLOTS_NUM			8

	//-----------------------------------------------------------------------------/
	//  новые данные относящиеся к покупке оружия 
	//-----------------------------------------------------------------------------/
	class CUIDragDropItemMP: public CUIDragDropItem
	{
		typedef CUIDragDropItem inherited;
		// возможность хранения номера слота
		// в который можно переместить данную вещь
		u32				slotNum;
		// хранение номера секции оружия
		u32				sectionNum;
		// xранение порядкового номера оружия в секции
		u32				posInSection;
		// Имя секции оружия
		string128		strName;
		// Запоминаем адрес "хозяина"
		CUIDragDropList *m_pOwner;
		// Игровая стоимость вещи
		int				cost;
	public:
		CUIDragDropItemMP(): slotNum			(0),
							 sectionNum			(0),
							 bAddonsAvailable	(false),
							 cost				(0),
							 m_bAlreadyPaid		(false)
		{
			std::strcpy(m_strAddonTypeNames[0], "Silencer");
			std::strcpy(m_strAddonTypeNames[1], "Grenade Launcher");
			std::strcpy(m_strAddonTypeNames[2], "Scope");

			for (u8 i = 0; i < NUM_OF_ADDONS; ++i)
				m_pAddon[i] = NULL;
		}
		// Для слота
		void SetSlot(u32 slot)						{ R_ASSERT(slot < MP_SLOTS_NUM || slot == static_cast<u32>(-1)); slotNum = slot; }
		u32	 GetSlot()								{ return slotNum; }
		// Получаем номер группы секций (нож - 0, пистолы - 1, et cetera)
		void SetSectionGroupID(u32 section)			{ sectionNum = section; }
		u32	 GetSectionGroupID()					{ return sectionNum; }
		// Функции для запоминания/возвращения имени секции в .ltx файле, этой вещи
		void SetSectionName(const char *pData)		{ std::strcpy(strName, pData); }
		const char * GetSectionName() const			{ return strName; }
		// Запоминаем/возвращеаем указатель на CUIDragDropList которому изначально пренадлежит
		// вещь
		void SetOwner(CUIDragDropList *pOwner)		{ R_ASSERT(pOwner); m_pOwner = pOwner; }
		CUIDragDropList * GetOwner()				{ return m_pOwner; }
		// Номер элемента в группе секций
		void SetPosInSectionsGroup(const u32 pos)	{ posInSection = pos; }
		u32 GetPosInSectionsGroup() const 			{ return posInSection; }

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
		static const u8		NUM_OF_ADDONS = 3;
		enum  AddonIDs		{ ID_SILENCER = 0, ID_GRENADE_LAUNCHER, ID_SCOPE };
		AddonInfo			m_AddonInfo[NUM_OF_ADDONS];
		// Если у вещи вообще нет аддонов, то поднимаем этот флажек, для ускорения проверок
		bool				bAddonsAvailable;
		// Список название типов аддонов
		char				m_strAddonTypeNames[NUM_OF_ADDONS][25];

		// Аттачим/детачим аддоны
		void				AttachDetachAddon(int iAddonIndex, bool bAttach);
		void				AttachDetachAllAddons(bool bAttach);
		bool				IsAddonAttached(int iAddonIndex) { return m_AddonInfo[iAddonIndex].iAttachStatus == 1; }
		// Переопределяем некоторые функции, которые нам нужны для коректной отрисовки оружия с аддонами
		virtual void		ClipperOn();
		virtual void		ClipperOff();
		virtual void		Draw();
		// реальные драг-дроп вещи для тех аддонов, которые существуют для оружия
		CUIDragDropItemMP	*m_pAddon[NUM_OF_ADDONS];

		// Работа с деньгами
		void				SetCost(const int c)	{ cost = c; }
		int					GetCost()	const		{ return cost; }
		// Поднимаем флаг, если уже заплачено. Нужно для беспроблемного перемещения между слотами и поясом
		bool				m_bAlreadyPaid;

		// Массив дополнительных полей которые могут быть специфичны для определенных типов вещей,
		// например для арморов храним координаты иконки в общей текстуре его представляющей
		xr_vector<float>	m_fAdditionalInfo;
	};

	CUIFrameWindow		UIBagWnd;
	CUIStatic			UIDescWnd;
	CUIStatic			UIPersonalWnd;

	//конпка активации окошка сна
//	CUIButton			UISleepButton;

	// Набор закладок для экранов с оружием
	CUITabControl		UIWeaponsTabControl;

//	CUIButton			UIButton1;
//	CUIButton			UIButton2;
//	CUIButton			UIButton3;
//	CUIButton			UIButton4;
//	CUIButton			UIButton5;
//	CUIButton			UIButton6;

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

	//слоты для оружия
	CUIDragDropList		UITopList[MP_SLOTS_NUM]; 
	//отдельный слот для костюмов
//	CUIOutfitSlot		UIOutfitSlot;
	CUIStatic			UIOutfitIcon;		

//	CUIDragDropList		UIBagList;
//	CUIDragDropList		UIBeltList;

//	CUIProgressBar		UIProgressBarHealth;
//	CUIProgressBar		UIProgressBarSatiety;
//	CUIProgressBar		UIProgressBarPower;
//	CUIProgressBar		UIProgressBarRadiation;

	//pop-up меню вызываемое по нажатию правой кнопки
	CUIPropertiesBox	UIPropertiesBox;
	//менюшка для работы с устройством производства артефактов
//	CUIArtifactMerger	UIArtifactMergerWnd;
	//окошко для параметров сна
//	CUISleepWnd			UISleepWnd;

	// разбивка всего оружия на отдельные DragDrop листы по категориям: пистолеты, автоматы, etc.
	DEF_VECTOR (WEAPON_TYPES, CUIDragDropList*)
	WEAPON_TYPES		m_WeaponSubBags;

	//информация о персонаже
//	CUICharacterInfo UICharacterInfo;
	//информация о предмете
//	CUIItemInfo UIItemInfo;

	static const int MAX_ITEMS = 70;
	CUIDragDropItemMP m_vDragDropItems[MAX_ITEMS];
	int m_iUsedItems;

	//указатель на инвентарь, передается перед запуском меню
//	CInventory* m_pInv;

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
	// функция отрисовки подписей к главным кнопкам покупки оружия
	void DrawBuyButtonCaptions();
	// Включаем, выключаем индикатор активной кнопки табконтрола
	void SwitchIndicator(bool bOn, const int activeTabIndex);
	// Запрос количества денег у игрока

	// Работа с деньгами

	// Количество
	int			m_iMoneyAmount;
	// Получаем количество денег
	int			GetMoneyAmount() const			{ return m_iMoneyAmount; }
	// Устанавливам количество денег
	void		SetMoneyAmount(int moneyAmount)	{ m_iMoneyAmount = moneyAmount; CheckBuyAvailability(); }
	// Проверка всех вещей на возможность покупки (достаточно ли денег?)
	// Если вещь невозможно купить, то помечаем ее красным и запрещаем ее перетаскивание
	void		CheckBuyAvailability();

	// Работа с арморами

	// Вернуть номер текущего армора.
	// На данный момент: - 1 - нет, 0 - экзоскелет, 1 - научный
	const u8	GetCurrentSuit();
	
	// Таблица соответсвия имени армора с именами моделей персонажей. Заполняется на этапе считывания 
	// информации из ltx файла соответствующего типу сетевой игры
	typedef xr_vector<std::pair<ref_str, ref_str> >	CONFORMITY_TABLE;
	typedef CONFORMITY_TABLE::iterator				CONFORMITY_TABLE_it;
	CONFORMITY_TABLE		m_ConformityTable;
	
	// Координаты дефолтного скина на текстуре с иконками сталкеров.
	int			m_iIconTextureX, m_iIconTextureY;
public:

	// Процедура запоминания индекса скина в списке skins типа сетевой игры, после выбора его игроком
	void		SetSkin			(u8 SkinID);	

	// А не является ли данная вещь чьим-то аддоном?
	// Возвращаем адрес хозяина аддона, если нашли и тип аддона
	CUIDragDropItemMP * IsItemAnAddon(CUIDragDropItemMP *pPossibleAddon, CUIDragDropItemMP::AddonIDs &ID);
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
	void		ReInitItems	(char *strSectionName);

	// Процедуры принудительного перемещения оружия (эмуляция даблклика).
	// Params:	grpNum			- номер группы секций
	//			uIndexInSlot	- порядковый уровень оружия в списке группы секций
	void		MoveWeapon(const u8 grpNum, const u8 uIndexInSlot);
	// Params:	sectionName		- имя конфигурационной секции оружия
	void		MoveWeapon(const char *sectionName);
};