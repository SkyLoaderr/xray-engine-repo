// File:		UIBag.cpp
// Description:	Bag for BuyWeaponWnd
// Created:		10.02.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua

// Copyright 2005 GSC Game World

#include "StdAfx.h"
#include <dinput.h>
#include "UIBuyWeaponStd.h"
#include "UIBag.h"
#include "UIInventoryUtilities.h"
#include "../MainUI.h"
#include "../HUDManager.h"
#include "xrXMLParser.h"

using namespace InventoryUtilities;

CUIBag::CUIBag(CHECK_PROC proc){

	m_mlCurrLevel	= mlRoot;
	m_pCurrentDDItem = NULL;
	this->m_iMoneyAmount = 10000;

	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
	{
		AttachChild(&m_groups[i]);
		m_groups[i].SetCheckProc(proc);
		m_groups[i].SetItemsScaleXY(SECTION_ICON_SCALE, SECTION_ICON_SCALE);
	}


	// ��������� ������ ���������� � ������
	m_boxesDefs[0].texName		= "ui\\ui_inv_box_misc_weapons";
	m_boxesDefs[0].filterString	= "shotgun";
	m_boxesDefs[0].gridHeight		= 2;
	m_boxesDefs[0].gridWidth		= 6;

	m_boxesDefs[1].texName		= "ui\\ui_inv_box_assault_rifles";
	m_boxesDefs[1].filterString	= "assault_rifle";
	m_boxesDefs[1].gridHeight		= 2;
	m_boxesDefs[1].gridWidth		= 6;

	m_boxesDefs[2].texName		= "ui\\ui_inv_box_sniper_rifles";
	m_boxesDefs[2].filterString	= "sniper_rifle";
	m_boxesDefs[2].gridHeight		= 2;
	m_boxesDefs[2].gridWidth		= 6;

	m_boxesDefs[3].texName		= "ui\\ui_inv_box_heavy_weapons";
	m_boxesDefs[3].filterString	= "heavy_weapon";
	m_boxesDefs[3].gridHeight		= 2;
	m_boxesDefs[3].gridWidth		= 6;
}

CUIBag::~CUIBag(){
	xr_list<CUIDragDropItemMP*>::iterator it;

	for (it = m_allItems.begin(); it != m_allItems.end(); ++it)
		delete *it;
}

void CUIBag::GetWeaponIndexByName(const xr_string sectionName, u8 &grpNum, u8 &idx){
	grpNum	= (u8)(-1);
	idx		= (u8)(-1);

	for (u8 i = 0; i < m_wpnSectStorage.size(); ++i)
	{
		for (u8 j = 0; j < m_wpnSectStorage[i].size(); ++j)
		{
			if (sectionName == m_wpnSectStorage[i][j])
			{
				grpNum	= i;
				idx		= j;
				return;
			}
		}
	}
}

u8 CUIBag::GetItemIndex(CUIDragDropItemMP* pDDItem, u8 &sectionNum){
	sectionNum = 0;
	u8 returnID = static_cast<u8>(-1);

	if (!pDDItem)
		return returnID;
    
	for (WPN_LISTS::const_iterator it = m_wpnSectStorage.begin(); it != m_wpnSectStorage.end(); ++it)
	{
		WPN_SECT_NAMES::difference_type diff = std::distance(
			(*it).begin(), 
			std::find(
						(*it).begin(), 
						(*it).end(), 
						pDDItem->GetSectionName()
					)
															);
		if (diff < static_cast<WPN_SECT_NAMES::difference_type>((*it).size()))
		{
			returnID = static_cast<u8>(diff);

			// ��������� �� ������� ������������ ������� � ������
			if (pDDItem->bAddonsAvailable)
			{
				u8	flags = 0;
				for (int i = 0; i < 3; ++i)
				{
					if (1 == pDDItem->m_AddonInfo[i].iAttachStatus)
						flags |= 1;

					flags = flags << 1;
				}
				flags = flags << 4;

				// � ���������� ������� 3 ���� �������� ������� ��������� �������:
				// FF - Scope, FE - Silencer, FD - Grenade Launcher
				returnID |= flags;
			}
			return returnID;
		}
		++sectionNum;
	}
	return returnID;
}

MENU_LEVELS CUIBag::GetMenuLevel(){
	return m_mlCurrLevel;
}

bool CUIBag::SetMenuLevel(MENU_LEVELS level){
	// check range
	if (level < mlRoot || level > mlWpnSubType) 
		return false;

	// check we really change state
	if (m_mlCurrLevel == level)
		return false;

	m_mlCurrLevel = level;

	GetMessageTarget()->SendMessage(this, XR_MENU_LEVEL_CHANGED, NULL);

	return true;
}

int  CUIBag::GetMoneyAmount(){
	return m_iMoneyAmount;
}

void CUIBag::Init(CUIXml& xml, const char *path, LPCSTR strSectionName, LPCSTR strPricesSection){
	m_StrSectionName = strSectionName;
	m_StrPricesSection = strPricesSection;
	
	CUIXmlInit::InitStatic(xml, path, 0, this);

	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
		CUIXmlInit::InitDragDropList(xml, "dragdrop_list_bag", 0, &m_groups[i]);

	InitBoxes();
	InitWpnSectStorage();
	FillUpInfiniteItemsList();
	FillUpGroups();
	HideAll();
}

bool CUIBag::IsItemInfinite(CUIDragDropItemMP* pDDItem){
	xr_list<shared_str>::iterator it;

	for (it = m_vInfiniteItemsList.begin(); it != m_vInfiniteItemsList.end(); ++it)
        if (0 ==xr_strcmp(pDDItem->GetSectionName(), *it))
			return true;

    return false;
}

void CUIBag::Init(int x, int y, int width, int height){
	CUIStatic::Init(x, y, width, height);
}

void CUIBag::UpdateBuyPossibility(){
	CUIDragDropItemMP* pDDItem;
	CUIDragDropList* currentDDList;
	int money = GetMoneyAmount();
	bool flag;

	// disable items player can't buy
	currentDDList = GetCurrentGroup();
	if (!currentDDList)
		return;


	DRAG_DROP_LIST ddList = currentDDList->GetDragDropItemsList();
	DRAG_DROP_LIST_it it;

	for (it = ddList.begin(); it != ddList.end(); ++it)
	{
		pDDItem = (CUIDragDropItemMP*)(*it);
		flag = !(pDDItem->GetCost() > money);
		EnableDDItem(pDDItem, flag);		
	}

	//xr_list<CUIDragDropItemMP*>::iterator it;

	//for (it = m_allItems.begin(); it != m_allItems.end(); ++it)
	//	if (IsItemInBag(*it))
	//	{
	//		pNewDDItem = (CUIDragDropItemMP*)(*it);
	//		flag = !(pNewDDItem->GetCost() > money);
	//		EnableDDItem(pNewDDItem, flag);
	//	}
}

CUIDragDropItemMP* CUIBag::GetItemBySectoin(const char *sectionName){
	xr_list<CUIDragDropItemMP*>::iterator it;

	for (it = m_allItems.begin(); it != m_allItems.end(); ++it)
		if (0 == xr_strcmp((*it)->GetSectionName(), sectionName))
			return *it;

	return NULL;
}

CUIDragDropItemMP* CUIBag::GetItemBySectoin(const u8 grpNum, u8 uIndexInSlot){
	xr_list<CUIDragDropItemMP*>::iterator it;

	for (it = m_allItems.begin(); it != m_allItems.end(); ++it)
        if (grpNum == (*it)->GetSectionGroupID() && uIndexInSlot == (*it)->GetPosInSectionsGroup())
			return *it;

	return NULL;
}

void CUIBag::ClearRealRepresentationFlags(){
	xr_list<CUIDragDropItemMP*>::iterator it;

	for (it = m_allItems.begin(); it != m_allItems.end(); ++it)
	{
		(*it)->m_bHasRealRepresentation = false;
		EnableDDItem(*it);
	}		
}

void CUIBag::EnableDDItem(CUIDragDropItemMP* pDDItem, bool bEnable){
	bool owned = pDDItem->m_bHasRealRepresentation;
	u32 color = bEnable ? (owned ? cAbleToBuyOwned : cAbleToBuy) : cUnableToBuy;
	pDDItem->SetColor(color);
	pDDItem->EnableDragDrop(bEnable);
}

bool CUIBag::IsItemInBag(CUIDragDropItemMP* pDDItem){\
	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
		if (m_groups[i].IsChild(pDDItem))
			return true;

	return false;
}

void CUIBag::Draw(){
	CUIStatic::Draw();
}

void CUIBag::OnBoxDbClick(CUIDragDropItemMP* pDDItem){
	R_ASSERT2(pDDItem, "OnBoxDbClick");
	shared_str section = pDDItem->GetSectionName();

	int iActiveSection = -1;

	if		(  0 == xr_strcmp(m_boxesDefs[0].filterString, section) )
		iActiveSection = GROUP_31;

	else if (  0 == xr_strcmp(m_boxesDefs[1].filterString, section) )
		iActiveSection = GROUP_32;

	else if (  0 == xr_strcmp(m_boxesDefs[2].filterString, section) )
		iActiveSection = GROUP_33;

	else if (  0 == xr_strcmp(m_boxesDefs[3].filterString, section) )
		iActiveSection = GROUP_34;

	R_ASSERT2( (-1 != iActiveSection), "CUIBag::OnBoxDbClick - invalid section");

	ShowSectionEx(iActiveSection);
}

CUIDragDropItemMP * CUIBag::GetAddonByID(CUIDragDropItemMP *pAddonOwner, CUIDragDropItemMP::AddonIDs ID){
	R_ASSERT(pAddonOwner);
	// ����������� �� ������ ����� � ���� ��� ������ �����
	xr_list<CUIDragDropItemMP*>::iterator it;

	for (it = m_allItems.begin(); it!=m_allItems.end(); ++it)
		for (int j = 0; j < CUIDragDropItemMP::NUM_OF_ADDONS; ++j)
		{
			CUIDragDropItemMP::AddonIDs AID = static_cast<CUIDragDropItemMP::AddonIDs>(j);
			if (pAddonOwner->m_AddonInfo[j].strAddonName == (*it)->GetSectionName() 
				&& AID == ID)
			{
				return *it;
			}
		}
	//for (int i = 0; i < m_iUsedItems; ++i)
	//{
	//	for (int j = 0; j < CUIDragDropItemMP::NUM_OF_ADDONS; ++j)
	//	{
	//		CUIDragDropItemMP::AddonIDs AID = static_cast<CUIDragDropItemMP::AddonIDs>(j);
	//		if (pAddonOwner->m_AddonInfo[j].strAddonName == m_vDragDropItems[i].GetSectionName() &&
	//			AID == ID)
	//		{
	//			return &m_vDragDropItems[i];
	//		}
	//	}
	//}

	return NULL;
}

void CUIBag::OnItemClick(CUIDragDropItemMP* pDDItem){
	if (IsItemInBag(pDDItem))
        m_pCurrentDDItem = pDDItem;
}

CUIDragDropItemMP * CUIBag::IsItemAnAddon(CUIDragDropItemMP *pPossibleAddon, CUIDragDropItemMP::AddonIDs &ID)
{
	R_ASSERT(pPossibleAddon);

	for (int i = GROUP_2; i <=GROUP_34; i++)
	{
		if (!m_groups[i].GetDragDropItemsList().empty())
		{
			CUIDragDropItemMP * pDDItemMP = smart_cast<CUIDragDropItemMP*>(m_groups[i].GetDragDropItemsList().front());

			if (pDDItemMP && pDDItemMP->bAddonsAvailable)
			{
				for (u8 j = 0; j < CUIDragDropItemMP::NUM_OF_ADDONS; ++j)
				{
					// ���� ���� �� ����� �������
					if (pPossibleAddon->GetSectionName() == pDDItemMP->m_AddonInfo[j].strAddonName)
					{
						ID = static_cast<CUIDragDropItemMP::AddonIDs>(j);
						return pDDItemMP;
					}
				}
			}
		}

		if (GROUP_2 == i)
			i = GROUP_31 - 1;
	}

	return NULL;
}

bool CUIBag::IsItemAnAddonSimple(CUIDragDropItemMP *pPossibleAddon) const
{
	R_ASSERT(pPossibleAddon);
	if (!pPossibleAddon) return false;

	xr_string str = pPossibleAddon->GetSectionName();
	return str.find("addon") != xr_string::npos;
}

void CUIBag::OnItemDrop(CUIDragDropItemMP* pItem){
	pItem->GetOwner()->SendMessage(pItem, DRAG_DROP_ITEM_DROP, NULL);
}

void CUIBag::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	CUIDragDropItemMP* pDDItem = smart_cast<CUIDragDropItemMP*>(pWnd);
	switch (msg)
	{
	case DRAG_DROP_ITEM_DROP:
		OnItemDrop(pDDItem);
		return;

		break;

	case DRAG_DROP_ITEM_DB_CLICK:
		if (GetMenuLevel() == mlBoxes)
			OnBoxDbClick(pDDItem);
		else
			OnItemClick(pDDItem);
		break;

	case DRAG_DROP_ITEM_RBUTTON_CLICK:
		OnItemClick(pDDItem); break;

	case DRAG_DROP_ITEM_DRAG: // == LBUTTON_CLICK
		OnItemClick(pDDItem); break;

	//case DRAG_DROP_ITEM_RBUTTON_CLICK:
	//	break;

	//case DRAG_DROP_REFRESH_ACTIVE_ITEM:
	//	if (this->m_pCurrentDDItem)
	//		this->m_pCurrentDDItem->Highlight(true);
	//	break;
	}
	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUIBag::UpdateMoney(int iMoney){
	m_iMoneyAmount = iMoney;
}

void CUIBag::HideAll(){
	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
		m_groups[i].Show(false);
}

// Init Boxes SLOT
void CUIBag::InitBoxes(){
	// ����������� �� ���� ������� � ������� �������������� �� ������
	for (u32 i = 0; i < 4; ++i)
	{
		CUIDragDropItemMP* pNewDDItem = xr_new<CUIDragDropItemMP>();
		pNewDDItem->SetAutoDelete(true);
		pNewDDItem->EnableDragDrop(false);

		m_boxesDefs[i].pDDItem = pNewDDItem;

		pNewDDItem->CUIStatic::Init(*m_boxesDefs[i].texName, 0, 0, INV_GRID_WIDTH, INV_GRID_HEIGHT);
		pNewDDItem->Rescale(SECTION_ICON_SCALE, SECTION_ICON_SCALE);
		pNewDDItem->SetStretchTexture(true);
		pNewDDItem->SetColor(0xffffffff);
		pNewDDItem->EnableDragDrop(false);

		pNewDDItem->SetSlot(WEAPON_BOXES_SLOT);
		pNewDDItem->SetSectionGroupID(WEAPON_BOXES_SLOT);
		pNewDDItem->SetPosInSectionsGroup(i);

		pNewDDItem->SetFont(UI()->Font()->pFontLetterica16Russian);

		pNewDDItem->SetGridHeight(m_boxesDefs[i].gridHeight);
		pNewDDItem->SetGridWidth(m_boxesDefs[i].gridWidth);
		pNewDDItem->GetUIStaticItem().SetOriginalRect(
			0, 0,
			m_boxesDefs[i].gridWidth * INV_GRID_WIDTH - INV_GRID_WIDTH,
			m_boxesDefs[i].gridHeight * INV_GRID_HEIGHT - INV_GRID_HEIGHT / 2);

		pNewDDItem->SetSectionName(*m_boxesDefs[i].filterString);

		m_groups[GROUP_BOXES].AttachChild(pNewDDItem);
		//pNewDDItem->SetAutoDelete(false);

		// ��������� ��������� �� ���� "������" ����
		pNewDDItem->SetOwner(&m_groups[GROUP_BOXES]);
		pNewDDItem->SetMessageTarget(this);
		// ������ ����������� �������������� ������� ���������, ���
		// ����������� ������ ������ � ���� ��� ������
		pNewDDItem->SetCustomDraw(static_cast<CUSTOM_UPDATE_PROC>(WpnDrawIndex));
	}
}

bool CUIBag::OnKeyboard(int dik, EUIMessages keyboard_action){
	int iGroup;

	switch (GetMenuLevel())
	{
	case mlRoot:
		R_ASSERT2(false,"error: CUIBag on level <mlRoot> can't handle keyboard");
		break;
	case mlBoxes:

		if (DIK_ESCAPE == dik)
		{
			ShowSectionEx(-1);			return true;
		}

		switch (dik)
		{
		case DIK_1:
			ShowSectionEx(GROUP_31);	return true;
		case DIK_2:
			ShowSectionEx(GROUP_32);	return true;
		case DIK_3:
			ShowSectionEx(GROUP_33);	return true;
		case DIK_4:
			ShowSectionEx(GROUP_34);	return true;
		}
		break;

	case mlWpnSubType:		
		iGroup = GetCurrentGroupIndex();
		if (DIK_ESCAPE == dik)
		{
			if (iGroup >= GROUP_31 && iGroup <= GROUP_34 )
				ShowSectionEx(GROUP_BOXES);
			else
				ShowSectionEx(-1);

			return true;
		}

		if (dik <= DIK_0 && dik >= DIK_1)
		{
            CUIDragDropList* pDDList = GetCurrentGroup();
			unsigned int index = static_cast<u32>(dik - 2);
			if (pDDList->GetDragDropItemsList().size() >= index)
			{
				DRAG_DROP_LIST_it it = pDDList->GetDragDropItemsList().begin();
				
				for (; it != pDDList->GetDragDropItemsList().end(); ++it)
				{
					CUIDragDropItemMP *pDDItemMP = static_cast<CUIDragDropItemMP*>(*it);
					
					if (pDDItemMP->GetPosInSectionsGroup() == index)
					{
						GetTop()->SendMessage(pDDItemMP, DRAG_DROP_ITEM_DB_CLICK, NULL);
						return true;
					}
				}
			}
		}
		break;
	default:
		NODEFAULT;
	}

	return false;
}

void CUIBag::BuyReaction(){

}

CUIDragDropList* CUIBag::GetCurrentGroup(){
	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
		if(m_groups[i].IsShown())
			return &m_groups[i];
	return NULL;
}

int CUIBag::GetCurrentGroupIndex(){
	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
		if(m_groups[i].IsShown())
			return i;

	return GROUP_DEFAULT;
}

// for external using
void CUIBag::ShowSection(int iSection){
	R_ASSERT2(  (GROUP_2 <= iSection) && (iSection <= GROUP_6 + 1), "CUIBag::ShowSection() - invalid section number");

	HideAll();

	if (GROUP_6 + 1 != iSection)
	{
        m_groups[iSection].Show(true);

		if ( GROUP_BOXES == iSection )
			SetMenuLevel(mlBoxes);
		else
			SetMenuLevel(mlWpnSubType);
	}
	else
		SetMenuLevel(mlRoot);
}

// for internal using
void CUIBag::ShowSectionEx(int iSection){
	HideAll();

	if (-1 == iSection)
	{
		SetMenuLevel(mlRoot);	return;
	}

	R_ASSERT2((GROUP_2 <= iSection) && (iSection <= GROUP_34), "CUIBag::ShowSectionEx() - invalid section number");

    m_groups[iSection].Show(true);

	if ( GROUP_BOXES == iSection )
		SetMenuLevel(mlBoxes);
	else
		SetMenuLevel(mlWpnSubType);
}

// fill out
//				m_wpnSectStorage
//				m_ConformityTable

void CUIBag::InitWpnSectStorage()
{
	WPN_SECT_NAMES		wpnOneType;
	string16			wpnSection;
	shared_str			iconName;
	string256			wpnNames, wpnSingleName;
	// ����� ������ � ��������
	const int			armorSectionIndex = 5;

	// ���� strSectionName ������ ��������� ��� ������
	R_ASSERT(m_StrSectionName != "");
	R_ASSERT2(pSettings->section_exist(m_StrSectionName), "Section doesn't exist");

	for (int i = 1; i < 20; ++i)
	{
		// ������� �����
		wpnOneType.clear();

		// ��� ����
		sprintf(wpnSection, "slot%i", i);
		if (!pSettings->line_exist(m_StrSectionName, wpnSection)) 
		{
			m_wpnSectStorage.push_back(wpnOneType);
			continue;
		}

		// ������ ������ ����� ����
		std::strcpy(wpnNames, pSettings->r_string(m_StrSectionName, wpnSection));
		u32 count	= _GetItemCount(wpnNames);
		// ������ ��� ������ ��� ������, ����������� ��������, ������� � ������
		for (u32 j = 0; j < count; ++j)
		{
			_GetItem(wpnNames, j, wpnSingleName);
			wpnOneType.push_back(wpnSingleName);

			// ��� ������� ������������� �������������� ������� ����������� ������ � 
			// � ������ ��������� � ���� ������
			if (armorSectionIndex == i)
			{
				iconName	= pSettings->r_string(m_StrSectionName, wpnSingleName);
				m_ConformityTable.push_back(std::make_pair<shared_str, shared_str>(wpnSingleName, iconName));
			}
		}

		if (!wpnOneType.empty())
			m_wpnSectStorage.push_back(wpnOneType);
	}
}

void CUIBag::FillUpInfiniteItemsList(){
	string256 itemsList;
	string16 item;
	shared_str ssItem;
	// Get infinite items list
	if(!pSettings->section_exist("multiplayer_extended_settings"))
		return;

	std::strcpy(itemsList, pSettings->r_string("extended_settings", "infinite_items"));
	int itemsCount	= _GetItemCount(itemsList);

	for (int i = 0; i < itemsCount; i++)
	{
		_GetItem(itemsList, i, item);
		ssItem = item;
		m_vInfiniteItemsList.push_back(ssItem);
	}
}

void CUIBag::FillUpGroups()
{	
	for (WPN_LISTS::size_type i = 0; i < m_wpnSectStorage.size(); ++i)
		FillUpGroup(static_cast<u32>(i));
}

//////////////////////////////////////////////////////////////////////////

void CUIBag::FillUpGroup(const u32 group)
{
	for (WPN_SECT_NAMES::size_type j = 0; j < m_wpnSectStorage[group].size(); ++j)
	{
		CUIDragDropItemMP* pNewDDItem = xr_new<CUIDragDropItemMP>();
		FillUpItem(pNewDDItem, group, static_cast<int>(j));
		PutItemToGroup(pNewDDItem, group);
		m_allItems.push_back(pNewDDItem);		
	}
}

void CUIBag::FillUpItem(CUIDragDropItemMP* pDDItem, int group, int j){
		pDDItem->SetAutoDelete(false);

		pDDItem->CUIStatic::Init(0, 0, INV_GRID_WIDTH, INV_GRID_HEIGHT);
		pDDItem->SetShader(GetEquipmentIconsShader());
		pDDItem->SetStretchTexture(true);
		pDDItem->SetColor(0xffffffff);

		//properties used by inventory menu
		int iGridWidth	= pSettings->r_u32(m_wpnSectStorage[group][j].c_str(), "inv_grid_width");
		int iGridHeight	= pSettings->r_u32(m_wpnSectStorage[group][j].c_str(), "inv_grid_height");
		int iXPos		= pSettings->r_u32(m_wpnSectStorage[group][j].c_str(), "inv_grid_x");
		int iYPos		= pSettings->r_u32(m_wpnSectStorage[group][j].c_str(), "inv_grid_y");

		u32 slot;
		if(pSettings->line_exist(m_wpnSectStorage[group][j].c_str(), "slot"))
			slot = pSettings->r_u32(m_wpnSectStorage[group][j].c_str(), "slot");
		else
			slot = NO_ACTIVE_SLOT;
		
		pDDItem->SetSlot(slot);
		pDDItem->SetSectionGroupID(group);
		pDDItem->SetPosInSectionsGroup(static_cast<u32>(j));

		pDDItem->SetFont(UI()->Font()->pFontLetterica16Russian);

		// ������ ��������� ������
		if (pSettings->line_exist(m_StrSectionName, static_cast<xr_string>(m_wpnSectStorage[group][j] + "_cost").c_str()))
			pDDItem->SetCost(pSettings->r_u32(m_StrSectionName, static_cast<xr_string>(m_wpnSectStorage[group][j] + "_cost").c_str()));
		else if (pSettings->line_exist(m_StrPricesSection, m_wpnSectStorage[group][j].c_str()))
			pDDItem->SetCost(pSettings->r_u32(m_StrPricesSection, m_wpnSectStorage[group][j].c_str()));
		else
		{
			string256	buf;
			sprintf(buf, "Cannot find price for an item %s in sections: %s, %s",
					m_wpnSectStorage[group][j].c_str(), *m_StrSectionName, *m_StrPricesSection);
			R_ASSERT2(false, buf);
		}

		//// ���� �� ������ �� ������� �����, �� ��������� ��� ����������� � �������� ������, 
		//// ��� ��� ����������
		//if (pDDItem->GetCost() > GetMoneyAmount())
		//{
		//	pDDItem->SetColor(cUnableToBuy);
		//}

		// ��� ������� ������ ������������� ���������� �� �������� � �������� ���������� ��� �������
		if (OUTFIT_SLOT == slot)
		{
			// ��� ������� ������, ������� ����� ���� � ��� � ������������, ������ ����
			// ��� ������ �������� � ������ ���� � ���� ������ � � ������ ������
			for (CONFORMITY_TABLE_it it = m_ConformityTable.begin(); it != m_ConformityTable.end(); ++it)
			{
				// ���������� � ����� ������ ����
				if (0 == xr_strcmp(it->first, m_wpnSectStorage[group][j].c_str()))
				{
					shared_str modelName = it->second;

					int m_iSkinX = 0, m_iSkinY = 0;
					sscanf(pSettings->r_string("multiplayer_skins", *modelName), "%i,%i", &m_iSkinX, &m_iSkinY);
					pDDItem->m_fAdditionalInfo.push_back(static_cast<float>(m_iSkinX));
					pDDItem->m_fAdditionalInfo.push_back(static_cast<float>(m_iSkinY));
				}
		
			}
		}

		InitAddonsInfo(*pDDItem, m_wpnSectStorage[group][j]);

		pDDItem->SetGridHeight(iGridHeight);
		pDDItem->SetGridWidth(iGridWidth);

		pDDItem->GetUIStaticItem().SetOriginalRect(
			iXPos * INV_GRID_WIDTH,
			iYPos * INV_GRID_HEIGHT,
			iGridWidth * INV_GRID_WIDTH,
			iGridHeight * INV_GRID_HEIGHT);


		pDDItem->SetSectionName(m_wpnSectStorage[group][j].c_str());		
		pDDItem->SetMessageTarget(this);
		pDDItem->SetCustomDraw(static_cast<CUSTOM_UPDATE_PROC>(WpnDrawIndex));
		pDDItem->m_bIsInfinite = IsItemInfinite(pDDItem);
}

CUIDragDropItemMP* CUIBag::CreateCopy(CUIDragDropItemMP *pDDItem){
#ifdef DEBUG
	R_ASSERT2(pDDItem, "CUIBag::CreateCopy - argument is NULL");
#endif
	unsigned int group = pDDItem->GetSectionGroupID();
	unsigned int index = pDDItem->GetPosInSectionsGroup();

	CUIDragDropItemMP* pNewDDItem = xr_new<CUIDragDropItemMP>();
	pNewDDItem->SetAutoDelete(true);

	FillUpItem(pNewDDItem, group, index);
	PutItemToGroup(pNewDDItem, group);

	m_vCopyList.push_back(pNewDDItem);

	return pNewDDItem;
}

void CUIBag::DeleteCopy(CUIDragDropItemMP* pDDItem){
	shared_str section = pDDItem->GetSectionName();
	//unsigned int group = pDDItem->GetSectionGroupID();

	DRAG_DROP_LIST ddList = pDDItem->GetOwner()->GetDragDropItemsList();
	DRAG_DROP_LIST_it it;

	for (it = ddList.begin(); it != ddList.end(); ++it)
	{
        CUIDragDropItemMP* pItem = smart_cast<CUIDragDropItemMP*>(*it);
		shared_str it_section = pItem->GetSectionName();
		if (0 == xr_strcmp(section, it_section))
		{
			pItem->GetParent()->DetachChild(pItem);
			m_vCopyList.remove(pItem);

#ifdef DEBUG
			DRAG_DROP_LIST ddList_2 = pDDItem->GetOwner()->GetDragDropItemsList();
			DRAG_DROP_LIST_it it_2;

		   	for (it_2 = ddList_2.begin(); it_2 != ddList_2.end(); ++it_2)
			{
		        CUIDragDropItemMP* pItem = smart_cast<CUIDragDropItemMP*>(*it_2);
				if (0 == xr_strcmp(section, pItem->GetSectionName()))
					R_ASSERT2(false, "CUIBag::DeleteCopy - two items with one section");
			}
#endif
			return;
		}
	}
    

#ifdef DEBUG
	R_ASSERT2(false, "CUIBag::DeleteCopy - copy not found");
#endif
}

void CUIBag::PutItemToGroup(CUIDragDropItemMP* pDDItem, int iGroup){
	int iActiveSection = -1;
	shared_str weapon_class;

	switch (iGroup)
	{
	case 1:
		iActiveSection = GROUP_2;	break;
	case 2:
		weapon_class = pSettings->r_string(pDDItem->GetSectionName(), weaponFilterName);

		if		(  0 == xr_strcmp(m_boxesDefs[0].filterString, weapon_class) )
			iActiveSection = GROUP_31;

		else if (  0 == xr_strcmp(m_boxesDefs[1].filterString, weapon_class) )
			iActiveSection = GROUP_32;

		else if (  0 == xr_strcmp(m_boxesDefs[2].filterString, weapon_class) )
			iActiveSection = GROUP_33;

		else if (  0 == xr_strcmp(m_boxesDefs[3].filterString, weapon_class) )
			iActiveSection = GROUP_34;
		break;
	case 3:
		iActiveSection = GROUP_4;	break;
	case 4:
		iActiveSection = GROUP_5;	break;
	case 5:
		iActiveSection = GROUP_6;	break;
	default:
		iActiveSection = GROUP_DEFAULT;
	}

	R_ASSERT2( (-1 != iActiveSection), "CUIBag::PutItemToGroup - invalid section");

	m_groups[iActiveSection].AttachChild(pDDItem);
	pDDItem->SetOwner(&m_groups[iActiveSection]);
}

void CUIBag::InitAddonsInfo(CUIDragDropItemMP &DDItemMP, const xr_string &sectioName)
{
	// ��� ������� ������ ��������� ��� ������ � ltx �����, � ��������� ���������

	for (u8 i = 0; i < CUIDragDropItemMP::NUM_OF_ADDONS; ++i)
	{
		xr_string status, strX, strY, name;

		switch (static_cast<CUIDragDropItemMP::AddonIDs>(i))
		{
		case CUIDragDropItemMP::ID_SILENCER:
			status	= "silencer_status";
			name	= "silencer_name";
			strX	= "silencer_x";
			strY	= "silencer_y";
			break;
		case CUIDragDropItemMP::ID_SCOPE:
			status	= "scope_status";
			name	= "scope_name";
			strX	= "scope_x";
			strY	= "scope_y";
			break;
		case CUIDragDropItemMP::ID_GRENADE_LAUNCHER:
			status	= "grenade_launcher_status";
			name	= "grenade_launcher_name";
			strX	= "grenade_launcher_x";
			strY	= "grenade_launcher_y";
			break;
		default:
			R_ASSERT(!"Unknown type of addon");
		}

		if (pSettings->line_exist(sectioName.c_str(), status.c_str()) && 
			pSettings->r_u32(sectioName.c_str(), status.c_str()) == 2)
		{
			DDItemMP.bAddonsAvailable = true;
			DDItemMP.m_AddonInfo[i].iAttachStatus = 0;
			DDItemMP.m_AddonInfo[i].strAddonName = pSettings->r_string(sectioName.c_str(), name.c_str());
			DDItemMP.m_AddonInfo[i].x = pSettings->r_u32(sectioName.c_str(), strX.c_str());
			DDItemMP.m_AddonInfo[i].y = pSettings->r_u32(sectioName.c_str(), strY.c_str());
		}
	}
}

