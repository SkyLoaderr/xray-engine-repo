#pragma once
#include "../object_interfaces.h"
#include "UIBuyWndShared.h"

class CUIXml;
class CUITabButtonMP;
class CUICellItem;

class CStoreHierarchy
{
public:
	struct item :public IPureDestroyableObject
	{
								item				():m_parent(NULL),m_button(NULL){}
	virtual void				destroy				();

		shared_str				m_name;
		shared_str				m_btn_xml_name; //debug
		item*					m_parent;
		xr_vector<item*>		m_childs;
		xr_vector<shared_str>	m_items_in_group;
		CUITabButtonMP*			m_button;
		IC u32					ChildCount			()			const				{return m_childs.size();}
		IC const item&			ChildAt				(u32 idx)	const				{VERIFY(idx<ChildCount());return *m_childs[idx];}
		IC bool					HasSubLevels		()			const				{return ChildCount()!=0;}
	};

private:
	const item*				m_current_level;
	item*					m_root;

	void					LoadLevel			(CUIXml& xml, int index, item* _itm, int depth_level);
public:
							CStoreHierarchy		();
							~CStoreHierarchy	();

	void					Init				(CUIXml& xml, LPCSTR path);
	void					InitItemsInGroup	(const shared_str& sect, item* =NULL);
	const item&				GetRoot				()								{VERIFY(m_root); return *m_root;};
	void					Reset				()								{VERIFY(m_root); m_current_level = m_root;};
	IC bool					CurrentIsRoot		()								{return m_current_level == m_root;}

	const item&				CurrentLevel		()								{VERIFY(m_current_level); return *m_current_level;};
	bool					MoveUp				();
	bool					MoveDown			(u32 idx);
};

#include "../associative_vector.h"

class CItemCostMgr
{
	struct _i{
		u32			foo[_RANK_COUNT];
	};
	typedef associative_vector<shared_str, _i>	COST_MAP;
	typedef COST_MAP::iterator									COST_MAP_IT;
	typedef COST_MAP::const_iterator							COST_MAP_CIT;
	COST_MAP				m_items;
public:
	void					Load		(const shared_str& sect);
	u32						GetItemCost	(const shared_str& sect_name, u32 rank);
	void					Dump		() const;
};
/*

#include "UIStatic.h"
#include "UIDragDropListEx.h"
#include "UI3tButton.h"
#include "../xrServer_Objects_ALife_Items.h"

class CUITabButtonMP;

#define PRICE_RESTR_COLOR			(0xffff8080)

#define SET_RANK_RESTR_COLOR(x)		x->SetColor(0x88ff9999)
#define SET_PRICE_RESTR_COLOR(x)	x->SetColor(PRICE_RESTR_COLOR)
#define SET_NO_RESTR_COLOR(x)		x->SetColor(0xffffffff)
#define SET_EXTERNAL_COLOR(x)		x->SetColor(0xff9696ff)

#define HIGHTLIGHT_ITEM(x)			x->SetLightAnim("ui_slow_blinking", true, true, false, true)
#define UNHIGHTLIGHT_ITEM(x)		x->SetLightAnim(NULL, true, true, false, true); \
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
	int					gridWidth;
	int					gridHeight;
	CUITabButtonMP*		pButton;
};

typedef enum {
		SILENCER,
		GRENADELAUNCHER,
		SCOPE
} eWpnAddon;


class CUIMpItemsStoreWnd : public CUIStatic
{
	typedef CUIStatic inherited;
public:
				CUIMpItemsStoreWnd		();
				~CUIMpItemsStoreWnd		();

    // own
	void		Init					(CUIXml& xml, LPCSTR path, const shared_str& sectionName, const shared_str& sectionPrice);
	void		HideAll					();
	void		ShowSection				(int iSection);
MENU_LEVELS		GetMenuLevel			();
CUIDragDropListEx*	GetItemList			(CUICellItem* pItem);
	void		SetRank					(int r);	
	bool		IsInBag					(CUICellItem* pItem);
	bool		IsActive				(CUICellItem* pItem);
	void		BuyItem					(CUICellItem* pItem);
	void		SellItem				(CUICellItem* pItem);
	bool		CanBuy					(CUICellItem* pItem);
	bool		CanBuy					(LPCSTR item);
	void		IgnoreRank				(bool ignore)					{m_bIgnoreRank = ignore; };
	void		IgnoreMoney				(bool ignore)					{m_bIgnoreMoney = ignore; };
	int			GetItemPrice			(CUICellItem* itm);
CUICellItem*	CreateItem				(const shared_str& name_sect);
	void		DestroyItem				(CUICellItem* itm);
	void		ClearAmmoHighlight		();
	void		HightlightAmmo			(LPCSTR ammo);
	bool		IsBlueTeamItem			(CUICellItem* itm);
	int			GetItemRank				(CUICellItem* itm);
	void		InitItems				();
	void		DestroyAllItems			();


	u8		GetItemIndex				(CUICellItem* pItem, u8 &sectionNum);
	void	GetWeaponIndexByName		(const shared_str& sectionName, u8 &grpNum, u8 &idx);
	const shared_str& GetWeaponNameByIndex	(u8 grpNum, u8 idx);

	CUICellItem* GetItemBySectoin		(const shared_str& sectionName, bool bCreateOnFail = false);
	CUICellItem* GetItemBySectoin		(const u8 grpNum, u8 uIndexInSlot);
	CUICellItem* CreateNewItem			(const u8 grpNum, u8 uIndexInSlot);

	void	SetExternal					(CUICellItem* itm, bool status);
	bool	GetExternal					(CUICellItem* itm);
	void	ClearExternalStatus			();
	void	AttachAddon					(CUICellItem* itm, CSE_ALifeItemWeapon::EWeaponAddonState add_on, bool external);
	void	ReloadItemsPrices			();
	bool	HasEnoughtMoney				(CUICellItem* itm);

	// handlers
	void	OnBtnShotgunsClicked		();
	void	OnBtnMachinegunsClicked		();
	void	OnBtnSniperClicked			();
	void	OnBtnHeavyClicked			();
	void	OnBackClick					();

	// drag drop handlers
	bool xr_stdcall	OnItemDrop			(CUICellItem* itm);
	bool xr_stdcall	OnItemStartDrag		(CUICellItem* itm);
	bool xr_stdcall	OnItemDbClick		(CUICellItem* itm);
	bool xr_stdcall	OnItemSelected		(CUICellItem* itm);
	bool xr_stdcall	OnItemRButtonClick	(CUICellItem* itm);

	// CUIWindow
	virtual bool	OnKeyboard			(int dik, EUIMessages keyboard_action);
	virtual void	SendMessage			(CUIWindow* pWnd, s16 msg, void* pData = 0);
	void			UpdateBuyPossibility();
	virtual void	Update				();

	int				m_money;

protected:

	void			InitBoxes			(CUIXml& pXml);
	void			InitWpnSectStorage	();
	void			FillUpGroups		();
	void			FillUpGroup			(const u32 group);
	void			PutItemToGroup		(CUICellItem* pItem, int iGroup);
	int				GetCurrentGroupIndex();
CUICellItem*		GetItemByKey		(int dik, int section);

	bool			SetMenuLevel		(MENU_LEVELS level);
	void			ShowSectionEx		(int iSection);

IC	bool			UpdateRank			(CUICellItem* pItem);
IC	bool			UpdatePrice			(CUICellItem* pItem, int index);

	CUI3tButton		m_btnBack;
	shared_str		m_sectionName;
	shared_str		m_sectionPrice;
	MENU_LEVELS		m_mlCurrLevel;
	bool			m_bIgnoreRank;
	bool			m_bIgnoreMoney;

	BoxInfo			m_boxesDefs[4];

	DEF_VECTOR		(WPN_SECT_NAMES, shared_str);
	DEF_VECTOR		(WPN_LISTS, WPN_SECT_NAMES);
	
	WPN_LISTS		m_wpnSectStorage;

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

	xr_vector<CUICellItem*>				m_allItems;
	xr_vector<ItmInfo>					m_info;

	CUIDragDropListEx					m_groups[NUMBER_OF_GROUPS];
	int subSection_group3				[4];
};
*/