#pragma once

#include "UIStatic.h"
#include "UIDragDropListEx.h"
#include "UI3tButton.h"

class CUITabButtonMP;


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

class CUIBagWnd : public CUIStatic{
public:
	CUIBagWnd();
	~CUIBagWnd();

    // own
	void	Init(CUIXml& xml, LPCSTR path, LPCSTR sectionName, LPCSTR sectionPrice);
	void	HideAll();
	void	ShowSection(int iSection);
	MENU_LEVELS		GetMenuLevel();
	CUIDragDropListEx*	GetItemList(CUICellItem* pItem);

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

protected:

	void	InitBoxes(CUIXml& pXml);
	void	InitWpnSectStorage();
	void	FillUpGroups();
	void	FillUpGroup(const u32 group);
//	void	FillUpItem(CUICellItem* pItem, const char* name);
	void	PutItemToGroup(CUICellItem* pItem, int iGroup);
	void	GetWeaponIndexByName(const xr_string sectionName, u8 &grpNum, u8 &idx);
	int		GetCurrentGroupIndex();

	bool	SetMenuLevel(MENU_LEVELS level);
	void	ShowSectionEx(int iSection);


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

	xr_vector<CUICellItem*> m_allItems;

	// “аблица соответсви€ имени армора с именами моделей персонажей. «аполн€етс€ на этапе считывани€ 
	// информации из ltx файла соответствующего типу сетевой игры
	typedef xr_vector<std::pair<shared_str, shared_str> >	CONFORMITY_TABLE;
	typedef CONFORMITY_TABLE::iterator						CONFORMITY_TABLE_it;
	CONFORMITY_TABLE										m_ConformityTable;

	CUIDragDropListEx m_groups[NUMBER_OF_GROUPS];
//	int subSection_group3[4];
};