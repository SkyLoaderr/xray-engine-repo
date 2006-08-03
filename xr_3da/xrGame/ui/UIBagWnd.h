#pragma once

#include "UIStatic.h"
#include "UIDragDropListEx.h"
#include "UI3tButton.h"

class CUITabButtonMP;

#define SET_RANK_RESTR_COLOR(x) x->SetColor(0x88ff9999)
#define SET_PRICE_RESTR_COLOR(x) x->SetColor(0xffff8080)
#define SET_NO_RESTR_COLOR(x) x->SetColor(0xffffffff)
#define SET_EXTERNAL_COLOR(x) x->SetColor(0xff9696ff)

#define HIGHTLIGHT_ITEM(x)		x->SetLightAnim("ui_slow_blinking", true, true, false, true)
#define UNHIGHTLIGHT_ITEM(x)	x->SetLightAnim(NULL, true, true, false, true); \
								x->SetColor(0xffffffff)


enum Groups {
	GROUP_2,
	GROUP_BOXES,		
	GROUP_4,
	GROUP_5,
	GROUP_6,
	GROUP_31,
	GROUP_32,
	GROUP_33,
	GROUP_34,
	GROUP_DEFAULT,
	NUMBER_OF_GROUPS
};

enum MENU_LEVELS { 
	mlRoot = 0, 
	mlBoxes, 
	mlWpnSubType 
};

struct BoxInfo
{
	shared_str			xmlTag;
	shared_str			filterString;
	int					gridWidth, gridHeight;
	CUITabButtonMP*		pButton;
};

typedef enum {
		SILENCER,
		GRENADELAUNCHER,
		SCOPE
	} eWpnAddon;

class CUIBagWnd : public CUIStatic{
public:
	CUIBagWnd();
	~CUIBagWnd();

    // own
	void		Init(CUIXml& xml, LPCSTR path, LPCSTR sectionName, LPCSTR sectionPrice);
	void		HideAll();
	void		ShowSection(int iSection);
MENU_LEVELS		GetMenuLevel();
CUIDragDropListEx*	GetItemList(CUICellItem* pItem);
	void		SetRank(int r);	
	bool		IsInBag(CUICellItem* pItem);
	bool		IsActive(CUICellItem* pItem);
	void		BuyItem(CUICellItem* pItem);
	void		SellItem(CUICellItem* pItem);
	bool		CanBuy(CUICellItem* pItem);
	bool		CanBuy(LPCSTR item);
	void		IgnoreRank(bool ignore)		{m_bIgnoreRank = ignore; };
	void		IgnoreMoney(bool ignore)	{m_bIgnoreMoney = ignore; };
	int			GetItemPrice(CUICellItem* itm);
CUICellItem*	CreateItem(LPCSTR name);
	void		DestroyItem(CUICellItem* itm);
	void		ClearAmmoHighlight();
	void		HightlightAmmo(LPCSTR ammo);


	u8		GetItemIndex(CUICellItem* pItem, u8 &sectionNum);
	void	GetWeaponIndexByName(const xr_string sectionName, u8 &grpNum, u8 &idx);
	char*	GetWeaponNameByIndex(u8 grpNum, u8 idx);

	CUICellItem* GetItemBySectoin(const char *sectionName, bool bCreateOnFail = false);
	CUICellItem* GetItemBySectoin(const u8 grpNum, u8 uIndexInSlot);
	void	SetExternal(CUICellItem* itm, bool status);
	bool	GetExternal(CUICellItem* itm);
	void	ClearExternalStatus();
	void	AttachAddon(CUICellItem* itm, eWpnAddon add_on, bool external);
	void	ReloadItemsPrices();
	bool	HasEnoughtMoney(CUICellItem* itm);
//	void	PayForItem(CUICellItem* itm);
//	void	GetMoneyFor(CUICellItem* itm);

	// handlers
	void	OnBtnShotgunsClicked();
	void	OnBtnMachinegunsClicked();
	void	OnBtnSniperClicked();
	void	OnBtnHeavyClicked();
	void	OnBackClick();

	// drag drop handlers
	bool		xr_stdcall		OnItemDrop			(CUICellItem* itm);
	bool		xr_stdcall		OnItemStartDrag		(CUICellItem* itm);
	bool		xr_stdcall		OnItemDbClick		(CUICellItem* itm);
	bool		xr_stdcall		OnItemSelected		(CUICellItem* itm);
	bool		xr_stdcall		OnItemRButtonClick	(CUICellItem* itm);

	// CUIWindow
	bool OnKeyboard(int dik, EUIMessages keyboard_action);
	void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = 0);
	void UpdateBuyPossibility();
	void Update();

	int m_money;

protected:

	void	InitBoxes(CUIXml& pXml);
	void	InitWpnSectStorage();
	void	FillUpGroups();
	void	FillUpGroup(const u32 group);
//	void	FillUpItem(CUICellItem* pItem, const char* name);
	void	PutItemToGroup(CUICellItem* pItem, int iGroup);
	int		GetCurrentGroupIndex();
CUICellItem* GetItemByKey(int dik, int section);

	bool	SetMenuLevel(MENU_LEVELS level);
	void	ShowSectionEx(int iSection);

IC	bool	UpdateRank(CUICellItem* pItem);
IC	bool	UpdatePrice(CUICellItem* pItem, int index);

	CUI3tButton					m_btnBack;
	shared_str					m_sectionName;
	shared_str					m_sectionPrice;
	MENU_LEVELS					m_mlCurrLevel;
	bool						m_bIgnoreRank;
	bool						m_bIgnoreMoney;

	BoxInfo m_boxesDefs[4];
	DEF_VECTOR(WPN_SECT_NAMES, xr_string); // vector of weapons. it represents ONE section 
	DEF_VECTOR(WPN_LISTS, WPN_SECT_NAMES); // vector of sections
	WPN_LISTS	m_wpnSectStorage;

	enum  AddonIDs { 
		ID_NONE				= -1, 
		ID_SILENCER			= 0, 
		ID_GRENADE_LAUNCHER, 
		ID_SCOPE,
		ADDONS_NUM
	};

	struct ItmInfo{
		int		price;
		u32		short_cut;				// number button 1,2,3,4...9,0
		int		group_index;			// index of drag drop list
		int		section;
		int		pos_in_section;
		bool	bought;					// 
		bool	active;					// can be moved
		bool	external;
	};

	xr_vector<CUICellItem*> m_allItems;
	xr_vector<ItmInfo>		m_info;

	// “аблица соответсви€ имени армора с именами моделей персонажей. «аполн€етс€ на этапе считывани€ 
	// информации из ltx файла соответствующего типу сетевой игры
	typedef xr_vector<std::pair<shared_str, shared_str> >	CONFORMITY_TABLE;
	typedef CONFORMITY_TABLE::iterator						CONFORMITY_TABLE_it;
	CONFORMITY_TABLE										m_ConformityTable;

	CUIDragDropListEx m_groups[NUMBER_OF_GROUPS];
	int subSection_group3[4];
};