// UIBuyWeaponWnd.h:	������, ��� ������� ������ � ������ CS
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

// ����
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
	//  ����� ������ ����������� � ������� ������ 
	//-----------------------------------------------------------------------------/
	class CUIDragDropItemMP: public CUIDragDropItem
	{
		typedef CUIDragDropItem inherited;
		// ����������� �������� ������ �����
		// � ������� ����� ����������� ������ ����
		u32 slotNum;
		// �������� ������ ������ ������
		u32 sectionNum;
		// x������� ����������� ������ ������ � ������
		u32 posInSection;
		// ��� ������ ������
		string128 strName;
		// ���������� ����� "�������"
		CUIDragDropList *m_pOwner;
	public:
		CUIDragDropItemMP(): slotNum			(0),
							 sectionNum			(0),
							 bAddonsAvailable	(false)
		{
			std::strcpy(m_strAddonTypeNames[0], "Silencer");
			std::strcpy(m_strAddonTypeNames[1], "Grenade Launcher");
			std::strcpy(m_strAddonTypeNames[2], "Scope");

			for (u8 i = 0; i < NUM_OF_ADDONS; ++i)
				m_pAddon[i] = NULL;
		}
		// ��� �����
		void SetSlot(u32 slot)						{ R_ASSERT(slot < 6 || slot == static_cast<u32>(-1)); slotNum = slot; }
		u32	 GetSlot()								{ return slotNum; }
		// �������� ����� ������ ������ (��� - 0, ������� - 1, et cetera)
		void SetSectionGroupID(u32 section)			{ sectionNum = section; }
		u32	 GetSectionGroupID()					{ return sectionNum; }
		// ������� ��� �����������/����������� ����� ������ � .ltx �����, ���� ����
		void SetSectionName(const char *pData)		{ std::strcpy(strName, pData); }
		const char * GetSectionName() const			{ return strName; }
		// ����������/����������� ��������� �� CUIDragDropList �������� ���������� �����������
		// ����
		void SetOwner(CUIDragDropList *pOwner)		{ R_ASSERT(pOwner); m_pOwner = pOwner; }
		CUIDragDropList * GetOwner()				{ return m_pOwner; }
		// ����� �������� � ������ ������
		void SetPosInSectionsGroup(const u32 pos)	{ posInSection = pos; }
		u32 GetPosInSectionsGroup() const 			{ return posInSection; }

		//-----------------------------------------------------------------------------/
		//  ������ � ��������. ���������� ��������������� CWeapon ������ ����������������
		//	�������� �� �������
		//-----------------------------------------------------------------------------/

		// ��������� ���������� � ������
		typedef struct tAddonInfo
		{
			// ��� ������ ��� ������
			std::string			strAddonName;
			// -1 - ������ ������ �����������, 0 - �� ����������, 1 - ����������
			int					iAttachStatus;
			// ���������� �������� ������������ ������ ������
			int					x, y;
			// Constructor
			tAddonInfo():		iAttachStatus(-1), x(-1), y(-1) {}
		} AddonInfo;

		// � ������� ������ �������� 3 ������. ����� �������, ��� � ������� ��� ���� � ����� ������:
		// Scope, Silencer, Grenade Launcher.
		static const u8		NUM_OF_ADDONS = 3;
		enum  AddonIDs		{ ID_SILENCER = 0, ID_GRENADE_LAUNCHER, ID_SCOPE };
		AddonInfo			m_AddonInfo[NUM_OF_ADDONS];
		// ���� � ���� ������ ��� �������, �� ��������� ���� ������, ��� ��������� ��������
		bool				bAddonsAvailable;
		// ������ �������� ����� �������
		char				m_strAddonTypeNames[NUM_OF_ADDONS][25];

		// �������/������� ������
		void				AttachDetachAddon(int iAddonIndex, bool bAttach);
		void				AttachDetachAllAddons(bool bAttach);
		bool				IsAddonAttached(int iAddonIndex) { return m_AddonInfo[iAddonIndex].iAttachStatus == 1; }
		// �������������� ��������� �������, ������� ��� ����� ��� ��������� ��������� ������ � ��������
		virtual void		ClipperOn();
		virtual void		ClipperOff();
		virtual void		Draw();
		// �������� ����-���� ���� ��� ��� �������, ������� ���������� ��� ������
		CUIDragDropItemMP	*m_pAddon[NUM_OF_ADDONS];
//		CUIDragDropItemMP	*m_pSilencerAddon;
//		CUIDragDropItemMP	*m_pGLAddon;
	};

	CUIFrameWindow		UIBagWnd;
	CUIFrameWindow		UIDescWnd;
	CUIFrameWindow		UIPersonalWnd;

	//������ ��������� ������ ���
	CUIButton			UISleepButton;

	// ����� �������� ��� ������� � �������
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
	//����� ��� ������
	CUIDragDropList		UITopList[MP_SLOTS_NUM]; 
	//��������� ���� ��� ��������
	CUIOutfitSlot		UIOutfitSlot;

//	CUIDragDropList		UIBagList;
//	CUIDragDropList		UIBeltList;

	CUIProgressBar		UIProgressBarHealth;
	CUIProgressBar		UIProgressBarSatiety;
	CUIProgressBar		UIProgressBarPower;
	CUIProgressBar		UIProgressBarRadiation;

	//pop-up ���� ���������� �� ������� ������ ������
	CUIPropertiesBox	UIPropertiesBox;
	//������� ��� ������ � ����������� ������������ ����������
	CUIArtifactMerger	UIArtifactMergerWnd;
	//������ ��� ���������� ���
	CUISleepWnd			UISleepWnd;

	// �������� ����� ������ �� ��������� DragDrop ����� �� ����������: ���������, ��������, etc.
	DEF_VECTOR (WEAPON_TYPES, CUIDragDropList*)
	WEAPON_TYPES		m_WeaponSubBags;

	//���������� � ���������
	CUICharacterInfo UICharacterInfo;
	//���������� � ��������
	CUIItemInfo UIItemInfo;

	static const int MAX_ITEMS = 70;
	CUIDragDropItemMP m_vDragDropItems[MAX_ITEMS];
	int m_iUsedItems;

	//��������� �� ���������, ���������� ����� �������� ����
	CInventory* m_pInv;

	//������� � ������� �������� � ������� ������
	PIItem m_pCurrentItem;
	CUIDragDropItemMP* m_pCurrentDragDropItem;

	//�������, ����������� ������������ ������������ ������
	//� �������� ����������
	static bool SlotProc0(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc1(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc2(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc3(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool SlotProc4(CUIDragDropItem* pItem, CUIDragDropList* pList);

	static bool OutfitSlotProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool BagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
	static bool BeltProc(CUIDragDropItem* pItem, CUIDragDropList* pList);


	//��� ������� ���� �� ������ �������
	void ActivatePropertiesBox();

	//�������� ���������� ��������� ��� ���������� ���������
	enum {BUY_ITEM_ACTION, CANCEL_BUYING_ACTION,
		ATTACH_SILENCER_ADDON,
		ATTACH_GRENADE_LAUNCHER_ADDON,
		ATTACH_SCOPE_ADDON, 
		DETACH_SILENCER_ADDON,
		DETACH_GRENADE_LAUNCHER_ADDON,
		DETACH_SCOPE_ADDON};

	//��������� �������
	void DropItem();
	//������ �������
//	void EatItem();

	//����������� ����
	bool ToSlot();
	bool ToBag();
	bool ToBelt();

	//������ � ��������� ���� ������ � �����������
	void StartArtifactMerger();
	void StopArtifactMerger();

	//������ � ��������� ���� ������ � �����������
	void StartSleepWnd();
	void StopSleepWnd();

	//�������������/������������ ������� � ������
//	void AttachAddon();
//	void DetachAddon(const char* addon_name);

	//������������� ������� �������
	void SetCurrentItem(CInventoryItem* pItem);


	//�������������� ������ ��� ���������� �����
	TIItemList ruck_list;

	//���� � ������� �� ������������ add-on
	PIItem	m_pItemToUpgrade;

	//���� ������� ��� �������� ����� ������� �������
	u32	m_iCurrentActiveSlot;

	//-----------------------------------------------------------------------------/
	//  ����� ������ ����������� � ������� ������ 
	//-----------------------------------------------------------------------------/

	// ������ � ������� ��������� �������� ������ ��� ������ 
	DEF_VECTOR(WPN_SECT_NAMES, std::string);
	// ������ �������� � ������� ������ ��� ������
	DEF_VECTOR(WPN_LISTS, WPN_SECT_NAMES);
	WPN_LISTS	wpnSectStorage;
	// ������� ������������� ������� ����� - ���� ������ ��� ������, ����������������� �� �����
	void InitWpnSectStorage();
	// ��������� ������ ������� ������� �� ���������� ���������� ����������
	void FillWpnSubBags();
	// ��������� ��������� ������ �������
	void FillWpnSubBag(const u32 slotNum);
	// �������������� ������ � ����
	void InitAddonsInfo(CUIDragDropItemMP &DDItemMP, const std::string &sectioName);
	// ������� ��� �� ������
	void ClearWpnSubBags();
	// ������� ��� �� �������� ������
	void ClearWpnSubBag(const u32 slotNum);
	// ����������� ���� �� ����� ������� � �������������� ������.
	bool SlotToSection(const u32 SlotNum);
	// ����������� ���� � ����� � �����
	bool BeltToSection(CUIDragDropItemMP *pDDItemMP);
	// �������� ����������� ��������� ���� � ����
	bool CanPutInSlot(CUIDragDropItemMP *pDDItemMP, const u32 slotNum);
	// �������� ����������� ��������� ���� �� ����
	bool CanPutInBelt(CUIDragDropItemMP *pDDItemMP);
	// ��������� ��� ������ �� ������� ������ ������ ������
	string64	m_SectionName;
	// ������ OK � Cancel
	CUIButton	UIBtnOK, UIBtnCancel;
	// ������ �������� ������ ���������
	std::set<int> m_iEmptyIndexes;
	// �������� ������ ��������� ������ � ������� m_vDragDropItems
	int GetFirstFreeIndex();

	// �������������� ������� ��� ��������� ���������� � ����� � ������
	// Params:	slotNum	- ����� ����� (�������) � ������� ���� 
	//			idx		- ���������� ������ ������ � ����� (��� ���� ������ ����� ����� ������ ���� == 0)
	// Return:	��������� �� ������������� ����, ���� NULL, ���� �� �������
	CUIDragDropItemMP * GetWeapon(u32 slotNum, u32 idx = 0);
	// Params:	slotNum		- ��. ���������� �������
	//			idx			- ��. ���������� �������
	//			sectionNum	- ���������� ���������� ����� ������ � ������ ������
	// Return:	������ ������������� ����, ���� 0xff(-1), ���� �� �������
	const u8 GetItemIndex(u32 slotNum, u32 idx, u8 &sectionNum);

	// ��������� ������������� ������ ������� ������

	// ���� 3 ������ ������� ����������� ����:
	// 0) root			- ���� ������ �����������
	// 1) wpnsubtype	- ���� ������ ������ ������
	// 2) addons		- ���� ������ ������ �������, ���� ������ ���������
	enum MENU_LEVELS	{ mlRoot = 0, mlWpnSubType, mlAddons };
	// ������� �������
	MENU_LEVELS			m_mlCurrLevel;

	// ������� �� ����� ������� ���� ���������������
	bool MenuLevelJump(MENU_LEVELS lvl);
	// ������� �� ������� ����/����. ���������� true, ���� ������� ������, � 
	// false ���� �� ��� ��������� �� ������/������ ������ ����
	bool MenuLevelUp()		{ return MenuLevelJump(static_cast<MENU_LEVELS>(m_mlCurrLevel + 1)); }
	bool MenuLevelDown()	{ return MenuLevelJump(static_cast<MENU_LEVELS>(m_mlCurrLevel - 1)); }
	// callback ������� ��� ��������� �������� �������� � ������
	friend void WpnDrawIndex(CUIDragDropItem *pDDItem);
public:
	// � �� �������� �� ������ ���� ����-�� �������?
	// ���������� ����� ������� ������, ���� ����� � ��� ������
	CUIDragDropItemMP * IsItemAnAddon(CUIDragDropItemMP *pPossibleAddon, CUIDragDropItemMP::AddonIDs &ID);
	// ���������� ������� �� ������ �����
	virtual bool OnKeyboard(int dik, E_KEYBOARDACTION keyboard_action);

	// �������� ��� ������ � weapon.ltx ��������������� ������ � ����� ��� �� �����
	// Params:	slotNum		- ����� ����� � ������� �������
	// Result:	��������� �� ���������� ����� � ������ (������� ��� �� ����!)
	const char	*GetWeaponName(u32 slotNum);
	// Params:	indexInBelt	- ����� ���� �� �����
	// Result:	��������� �� ���������� ����� � ������ (������� ��� �� ����!)
	const char	*GetWeaponNameInBelt(u32 indexInBelt);

	// �������� ������ ������ � ������� ���������, �� ��������� �����.
	// ������ ������ ����� ������ 0. ���� � ����� ��� ������, �� ���������� -1
	const u8	GetWeaponIndex(u32 slotNum);
	const u8	GetWeaponIndexInBelt(u32 indexInBelt, u8 &sectionId, u8 &itemId);
	
	// �������� ��� ������ �� �������, � ������ �����
	const char	*GetWeaponNameByIndex(u32 grpNum, u8 idx);
	// �������� ����� � ������ ������ � ����� ������ ������ �� �����
	// ���� �� �������, �� grpNum == -1 � idx == -1
	void		GetWeaponIndexByName(const std::string sectionName, u8 &grpNum, u8 &idx);

	// �������� ������ � ������� � ������. ������� 3 ����, ���� ����������� � 1 ��������:
	// 2 - Silencer, 1 - Grenade Launcher, 0 - Scope
	const u8	GetWeaponAddonInfoByIndex(u8 idx);
	// �������� ������ ����� � ���������
	const u8	GetBeltSize();
	// ������������ ���������
	void		ReInitItems	(char *strSectionName);

	// ��������� ��������������� ����������� ������ (�������� ���������).
	// Params:	grpNum			- ����� ������ ������
	//			uIndexInSlot	- ���������� ������� ������ � ������ ������ ������
	void		MoveWeapon(const u8 grpNum, const u8 uIndexInSlot);
	// Params:	sectionName		- ��� ���������������� ������ ������
	void		MoveWeapon(const char *sectionName);

};