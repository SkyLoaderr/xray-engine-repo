// UIBuyWeaponWnd.h:	������, ��� ������� ������ � ������ CS
//						
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIBuyWeaponStd.h"

#include "UIDialogWnd.h"
#include "UIDragDropList.h"
#include "UIDragDropItemMP.h"

#include "UIProgressBar.h"

#include "UIPropertiesBox.h"
#include "UIOutfitSlot.h"

#include "UICharacterInfo.h"
#include "UIItemInfo.h"
#include "UITabControl.h"
#include "UIListItem.h"
#include "UIMultiTextStatic.h"


#include "UIBuyWeaponTab.h"
#include "UIBag.h"

extern const u32	cDetached;
extern const u32	cAttached;
extern const u32	cUnableToBuy;
extern const u32	cAbleToBuy;
extern const u32	cAbleToBuyOwned;
extern const float	fRealItemSellMultiplier;

//#define MAX_ITEMS 70;


class CUIBuyWeaponWnd: public CUIDialogWnd
{
	friend class CUIDragDropItemMP;
private:
	typedef CUIDialogWnd inherited;
public:
	CUIBuyWeaponWnd(LPCSTR strSectionName, LPCSTR strPricesSection);
	virtual ~CUIBuyWeaponWnd();

	virtual void Init(LPCSTR strSectionName, LPCSTR strPricesSection);

	void InitInventory();

	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
	virtual void OnMouse(int x, int y, EUIMessages mouse_action);
	virtual void OnMouseScroll(int iDirection);


	virtual void Update();
	virtual void Draw();

	virtual void Show();
	virtual void Hide();

protected:
	//-----------------------------------------------------------------------------/
	//  ����� ������ ����������� � ������� ������ 
	//-----------------------------------------------------------------------------/

	CUIBag				UIBagWnd;
	CUIStatic			UIDescWnd;
	CUIStatic			UIPersonalWnd;

	// ����� �������� ��� ������� � �������
	CUIBuyWeaponTab		UITabControl;
	// ������ ���  ������ �������� �� �������
	//CUIMultiTextStatic	UIMTStatic;

	// � ��������� ��� �� �������������
	//void InitBackgroundStatics();

	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBelt;
	CUIStatic			UIStaticBottom;

	//CUIStatic			UIStaticTabCtrl;
	// ����������� ��������, ������� �� ����� ������� ��������� ��������
//	CUIStatic			UIGreenIndicator;

	CUIStatic			UIStaticDesc;
	CUIStatic			UIStaticPersonal;

	// ���������� �����
	CUIStatic			UITotalMoneyHeader;
	CUIStatic			UITotalMoney;
	CUIStatic			UIItemCostHeader;
	CUIStatic			UIItemCost;

	//����� ��� ������
	CUIDragDropList		UITopList[MP_SLOTS_NUM]; 
	//��������� ���� ��� ��������
	CUIStatic			UIOutfitIcon;		

	//pop-up ���� ���������� �� ������� ������ ������
	CUIPropertiesBox	UIPropertiesBox;

	// �������� ����� ������ �� ��������� DragDrop ����� �� ����������: ���������, ��������, etc.
	DEF_VECTOR			(WEAPON_TYPES, CUIDragDropList*)
	WEAPON_TYPES		m_WeaponSubBags;

//	CUIDragDropItemMP m_vDragDropItems[70];
//	int m_iUsedItems;

	//������� � ������� �������� � ������� ������
	PIItem m_pCurrentItem;
	CUIDragDropItemMP* m_pCurrentDragDropItem;

	// ���������� � ����
	CUIItemInfo UIItemInfo;

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

	//����������� ����
	bool ToSlot();
	bool ToBag();
	bool ToBelt();

	//������ � ��������� ���� ������ � �����������
	void StartArtefactMerger();
	void StopArtefactMerger();

	//������ � ��������� ���� ������ � �����������
	void StartSleepWnd();
	void StopSleepWnd();

	//���� ������� ��� �������� ����� ������� �������
	u32	m_iCurrentActiveSlot;

	//-----------------------------------------------------------------------------/
	//  ����� ������ ����������� � ������� ������ 
	//-----------------------------------------------------------------------------/

	//// ��� ������ �� ������� ������ ������
	////!!!!!!!!!!!!!!!!!!!!! delete later next 2 lines
	shared_str		m_StrSectionName;
	shared_str		m_StrPricesSection;

	//// ������ � ������� ��������� �������� ������ ��� ������ 
	//DEF_VECTOR(WPN_SECT_NAMES, xr_string);
	//// ������ �������� � ������� ������ ��� ������
	//DEF_VECTOR(WPN_LISTS, WPN_SECT_NAMES);
	//WPN_LISTS	wpnSectStorage;
	//// ������� ������������� ������� ����� - ���� ������ ��� ������, ����������������� �� �����
	//void InitWpnSectStorage();
	//// ��������� ������ ������� ������� �� ���������� ���������� ����������
	//void FillWpnSubBags();
	//// ��������� ��������� ������ �������
	//void FillWpnSubBag(const u32 slotNum);
	// �������������� ������ � ����
	/*void InitAddonsInfo(CUIDragDropItemMP &DDItemMP, const xr_string &sectioName);*/
	//// ������� ��� �� ������
	//void ClearWpnSubBags();
	//// ������� ��� �� �������� ������
	//void ClearWpnSubBag(const u32 slotNum);
	// ����������� ���� �� ����� ������� � �������������� ������.
	bool SlotToSection(const u32 SlotNum);
	// ����������� ���� � ����� � �����
	bool BeltToSection(CUIDragDropItemMP *pDDItemMP);
	// �������� ����������� ��������� ���� � ����
	bool CanPutInSlot(CUIDragDropItemMP *pDDItemMP, const u32 slotNum);
	// �������� ����������� ��������� ���� �� ����
	bool CanPutInBelt(CUIDragDropItemMP *pDDItemMP);
	// ������ OK � Cancel
	CUI3tButton UIBtnOK, UIBtnCancel;
	// ������ �������� ������ ���������
	std::set<int> m_iEmptyIndexes;
	// �������� ������ ��������� ������ � ������� m_vDragDropItems
//	int GetFirstFreeIndex();

	// �������������� ������� ��� ��������� ���������� � ����� � ������
	// Params:	slotNum	- ����� ����� (�������) � ������� ���� 
	//			idx		- ���������� ������ ������ � ����� (��� ���� ������ ����� ����� ������ ���� == 0)
	// Return:	��������� �� ������������� ����, ���� NULL, ���� �� �������
	CUIDragDropItemMP * GetWeapon(u32 slotNum, u32 idx = 0);



//	enum MENU_LEVELS	{ mlRoot = 0, mlBoxes, mlWpnSubType };
	// ������� �������
//	MENU_LEVELS			m_mlCurrLevel;

	// ������� �� ����� ������� ���� ���������������
//	bool MenuLevelJump(MENU_LEVELS lvl);
	// ������� �� ������� ����/����. ���������� true, ���� ������� ������, � 
	// false ���� �� ��� ��������� �� ������/������ ������ ����
//	bool MenuLevelUp()		{ return MenuLevelJump(static_cast<MENU_LEVELS>(m_mlCurrLevel + 1)); }
//	bool MenuLevelDown()	{ return MenuLevelJump(static_cast<MENU_LEVELS>(m_mlCurrLevel - 1)); }
	// callback ������� ��� ��������� �������� �������� � ������
	friend void WpnDrawIndex(CUIDragDropItem *pDDItem);
	// ��������, ��������� ��������� �������� ������ �����������
//	void SwitchIndicator(bool bOn, const int activeTabIndex);
	// ������ ���������� ����� � ������

	// ������ � ��������

	// ����������
	int			m_iMoneyAmount;
	// ���������� ��������� �����
	bool		m_bIgnoreMoney;
	// �������� ���� ����� �� ����������� ������� (���������� �� �����?)
	// ���� ���� ���������� ������, �� �������� �� ������� � ��������� �� ��������������
	// ��������� ������ � ������ � ��������� �������
//	void		CheckBuyAvailabilityInShop();

	// ������ � ��������

	// ������� ����� �������� ������.
	// �� ������ ������: - 1 - ���, 0 - ����������, 1 - �������
//	const u8	GetCurrentSuit();
	
	//// ������� ����������� ����� ������ � ������� ������� ����������. ����������� �� ����� ���������� 
	//// ���������� �� ltx ����� ���������������� ���� ������� ����
	//typedef xr_vector<std::pair<shared_str, shared_str> >	CONFORMITY_TABLE;
	//typedef CONFORMITY_TABLE::iterator				CONFORMITY_TABLE_it;
	//CONFORMITY_TABLE								m_ConformityTable;
	
	// ���������� ���������� ����� �� �������� � �������� ���������.
	int			m_iIconTextureX, m_iIconTextureY;
	// ������� �������� �������������� ���� ������ (�������� � �� �����)
	// Params:	idx		- ������ � ������� ������, ������� ��������� ���������
	// Return:	true ���� ������ ��������� � ��������.
//	bool		IsItemInShop(int idx);
	
	// ��������� ������ � ������ � ������������ ������� �� �������� ������ � ltx �����
	void		ApplyFilter(int slotNum, const shared_str &name, const shared_str &value);
	// ��������� ���� � �������
//	void		InitWeaponBoxes();
//	void		RemoveWeapon(CUIDragDropList *shop, CUIDragDropItem *item);
	void		FillItemInfo(CUIDragDropItemMP *pDDItemMP);
	void		BuyReaction();

public:
	// ��������� ���� ����� � ������. ��� ����� ��� ����, ����� ����� �������� ������
	// ����������� ������ ���������� ������ ������������� � ������, ���� ��� �����.
	// Return:	true - ���� � ��� ������ � ������ �� ����� ������.
	bool		CheckBuyAvailabilityInSlots();
	// ���������� true ���� ��� ���� � ������ ����� ������.
	bool		CanBuyAllItems();
	// ������������� ����� ������ � ��������: ������������, ��� ���.
	void		IgnoreMoney(bool ignore)		{ m_bIgnoreMoney = ignore; }
	// �������� ���������� �����
	int			GetMoneyAmount() const;
	// ������������ ���������� �����
	void		SetMoneyAmount(int moneyAmount);

	// ��������� ����������� ������� ����� � ������ skins ���� ������� ����, ����� ������ ��� �������
	void		SetSkin			(u8 SkinID);	

	// � �� �������� �� ������ ���� ����-�� �������?
	// ���������� ����� ������� ������, ���� ����� � ��� ������
	// �������� ������� ������ ��� ������ � ������
	CUIDragDropItemMP * IsItemAnAddon(CUIDragDropItemMP *pPossibleAddon, CUIDragDropItemMP::AddonIDs &ID);
	// ����������� �������� �� ��, ��� ���� �������� �������
	bool		IsItemAnAddonSimple(CUIDragDropItemMP *pPossibleAddon) const;
	// �������� ��������� �� ����� �� ID. � ������ ��������������� ������ ���������� NULL.
	CUIDragDropItemMP * GetAddonByID(CUIDragDropItemMP *pAddonOwner, CUIDragDropItemMP::AddonIDs ID);
	// ���������� ������� �� ������ �����
	virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);
			void OnTabChange();
			void OnMenuLevelChange();
			void OnDDItemDbClick();
			void OnDDItemDrag();
			void OnButtonClicked(CUIWindow* pWnd);
			void SetCurrentDDItem(CUIWindow* pWnd);

	// �������� ��� ������ � weapon.ltx ��������������� ������ � ����� ��� �� �����
	// Params:	slotNum		- ����� ����� � ������� �������
	// Result:	��������� �� ���������� ����� � ������ (������� ��� �� ����!)
//	const char	*GetWeaponName(u32 slotNum);
	// Params:	indexInBelt	- ����� ���� �� �����
	// Result:	��������� �� ���������� ����� � ������ (������� ��� �� ����!)
//	const char	*GetWeaponNameInBelt(u32 indexInBelt);

	// �������� ������ ������ � ������� ���������, �� ��������� �����.
	// ������ ������ ����� ������ 0. ���� � ����� ��� ������, �� ���������� -1
	const u8	GetWeaponIndex(u32 slotNum);
	const u8	GetWeaponIndexInBelt(u32 indexInBelt, u8 &sectionId, u8 &itemId);
	// Params:	slotNum		- ��. ���������� �������
	//			idx			- ��. ���������� �������
	//			sectionNum	- ���������� ���������� ����� ������ � ������ ������
	// Return:	������ ������������� ����, ���� 0xff(-1), ���� �� �������
	const u8 GetItemIndex(u32 slotNum, u32 idx, u8 &sectionNum);
	
	// �������� ��� ������ �� �������, � ������ �����
	//const char	*GetWeaponNameByIndex(u32 grpNum, u8 idx);
	// �������� ����� � ������ ������ � ����� ������ ������ �� �����
	// ���� �� �������, �� grpNum == -1 � idx == -1
	void		GetWeaponIndexByName(const xr_string sectionName, u8 &grpNum, u8 &idx);

	// �������� ������ � ������� � ������. ������� 3 ����, ���� ����������� � 1 ��������:
	// 2 - Silencer, 1 - Grenade Launcher, 0 - Scope
	const u8	GetWeaponAddonInfoByIndex(u8 idx);
	// �������� ������ ����� � ���������
	const u8	GetBeltSize();
	// ������������ ���������
	void		ReInitItems	(LPCSTR strSectionName);

	// ��������� ��������������� ����������� ������ (�������� ���������).
	// Params:	grpNum					- ����� ������ ������
	//			uIndexInSlot			- ���������� ������� ������ � ������ ������ ������
	//			bRealRepresentationSet	- ����� ����� , ������� ��������� �� ������� � ���� ��������� 
	//									  �������������
	void		MoveWeapon(const u8 grpNum, const u8 uIndexInSlot, bool bRealRepresentationSet) {}
	// Params:	sectionName		- ��� ���������������� ������ ������
	void		MoveWeapon(const char *sectionName, bool bRealRepresentationSet) {}

	// �������������� ����������� ������ �� ������ � ����� (�������), ����������� ���������
	// Params:	grpNum					- ����� ������ ������
	//			uIndexInSlot			- ���������� ������� ������ � ������ ������ ������.
	//									  �������� ������������� ����� �������.
	//			bRealRepresentationSet	- ����, ������� ��������� �� ������� � ���� ��������� 
	//									  �������������
	void		SectionToSlot(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet);
	// Params:	sectionName				- ��� ���������������� ������ ������
	void		SectionToSlot(const char *sectionName, bool bRealRepresentationSet);

	// ������� ����������� ���� ����� �� ������ ������� � ������
	void		ClearSlots();
	// ������� ������ ����� �������� ������������� ���� ��� ���� ����� � ������
	void		ClearRealRepresentationFlags();
};