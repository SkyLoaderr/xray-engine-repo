// File:		UIBag.h
// Description:	Bag for BuyWeaponWnd
// Created:		10.02.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua

// Copyright 2005 GSC Game World

#pragma once

#include "UIStatic.h"
#include "xrXMLParser.h"
#include "UIDragDropList.h"
#include "UIDragDropItemMP.h"

#include <boost/array.hpp>

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
	GROUP_DEFAULT
};

enum MENU_LEVELS { 
	mlRoot = 0, 
	mlBoxes, 
	mlWpnSubType 
};

struct BoxInfo
{
	shared_str			texName;
	shared_str			filterString;
	int					gridWidth, gridHeight;
	CUIDragDropItemMP*	pDDItem;
};

// custom draw function for DragDropItem
void WpnDrawIndex(CUIDragDropItem *pDDItem);

class CUIBag : public CUIStatic {	
public:
	CUIBag();
	virtual ~CUIBag();

	virtual void Init(CUIXml& pXml, const char* path, LPCSTR strSectionName, LPCSTR strPricesSection);
	virtual void Init(int x, int y, int width, int height);
	virtual	void Update();
	virtual void UpdateMoney(int iMoney);
	virtual void Draw();
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = 0);
			void HideAll();
			void ShowSection(int iSection);
			
	MENU_LEVELS  GetMenuLevel();
protected:
			bool SetMenuLevel(MENU_LEVELS level);
			void EnableDDItem(CUIDragDropItemMP* pDDItem, bool bEnable = true);			
			void OnItemSelect();
			void OnBoxDbClick(CUIDragDropItemMP* pDDItem);
			void OnItemDbClick(CUIDragDropItemMP* pDDItem);
			void OnItemDrag(CUIDragDropItemMP* pItem);
    		int  GetMoneyAmount();

	// INIT Functions
			void InitBoxes();
			void InitAddonsInfo(CUIDragDropItemMP &DDItemMP, const xr_string &sectioName);
			void PutItemToGroup(CUIDragDropItemMP* pDDItem, int iGroup);
			void InitWpnSectStorage();
			void FillUpGroups();
			void FillUpGroup(const u32 group);
	static	bool BagProc(CUIDragDropItem* pItem, CUIDragDropList* pList);
//			void ClearWpnSubBags();					// удаляем все из секций
//			void ClearWpnSubBag(const u32 slotNum); // удаляем все из заданной секции

	shared_str			m_StrSectionName;
	shared_str			m_StrPricesSection;
	int					m_iMoneyAmount;
	Irect				m_rectWorkSpace;
	MENU_LEVELS			m_mlCurrLevel;
	CUIDragDropItemMP*	m_pCurrentDDItem;

	typedef boost::array<BoxInfo, 4> Boxes;
	Boxes m_boxesDefs;

	DEF_VECTOR(WPN_SECT_NAMES, xr_string); // vector of weapons. it represents ONE section 
	DEF_VECTOR(WPN_LISTS, WPN_SECT_NAMES); // vector of sections
	WPN_LISTS	m_wpnSectStorage;

	// bad code
	// we update all items
	// though we can update only visible
	// maybe i'll fix it someday :)
	xr_list<CUIDragDropItemMP*> m_allItems;

	// Таблица соответсвия имени армора с именами моделей персонажей. Заполняется на этапе считывания 
	// информации из ltx файла соответствующего типу сетевой игры
	typedef xr_vector<std::pair<shared_str, shared_str> >	CONFORMITY_TABLE;
	typedef CONFORMITY_TABLE::iterator						CONFORMITY_TABLE_it;
	CONFORMITY_TABLE										m_ConformityTable;
   
	CUIDragDropList m_groups[GROUP_DEFAULT + 1];
};