// File:		UIBag.cpp
// Description:	Bag for BuyWeaponWnd
// Created:		10.02.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua

// Copyright 2005 GSC Game World

#include "StdAfx.h"
#include "UIBuyWeaponStd.h"
#include "UIBag.h"
#include "UIInventoryUtilities.h"
#include "../MainUI.h"
#include "../HUDManager.h"
#include "xrXMLParser.h"

using namespace InventoryUtilities;

CUIBag::CUIBag(){

	m_mlCurrLevel	= mlRoot;
	m_pCurrentDDItem = NULL;

	for (int i = GROUP_2; i< GROUP_DEFAULT; i++)
	{
		AttachChild(&m_groups[i]);
		m_groups[i].SetCheckProc(BagProc);
	}


	// Заполняем массив информации о ящиках
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

	if (m_mlCurrLevel == mlRoot)
		HideAll();

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

	for (int i = GROUP_2; i<= GROUP_DEFAULT; i++)
	{
		CUIXmlInit::InitDragDropList(xml, "dragdrop_list_bag", 0, &m_groups[i]);
		m_groups[i].SetMessageTarget(this);
	}

	InitBoxes();
	InitWpnSectStorage();	
	FillUpGroups();
	HideAll();
}

void CUIBag::Init(int x, int y, int width, int height){
	CUIStatic::Init(x, y, width, height);
}

void CUIBag::Update(){
	CUIDragDropItemMP* pNewDDItem;

	xr_list<CUIDragDropItemMP*>::iterator it;

	int money = GetMoneyAmount();
	bool flag;

	for (it = m_allItems.begin(); it != m_allItems.end(); it++)
	{
		pNewDDItem = *it;
        flag = !(pNewDDItem->GetCost() > money);
        EnableDDItem(pNewDDItem, flag);
	}
        
}

void CUIBag::EnableDDItem(CUIDragDropItemMP* pDDItem, bool bEnable){
	if (bEnable)
        pDDItem->SetColor(cAbleToBuy);
	else
		pDDItem->SetColor(cUnableToBuy);

}

void CUIBag::Draw(){
	CUIStatic::Draw();
}

void CUIBag::OnBoxDbClick(CUIDragDropItemMP* pDDItem){
	R_ASSERT2(pDDItem, "OnBoxDbClick");

}

void CUIBag::OnItemDbClick(CUIDragDropItemMP* pDDItem){
	m_pCurrentDDItem = pDDItem;    
}

void CUIBag::OnItemDrag(CUIDragDropItemMP* pItem){
//		PIItem pInvItem = (PIItem)((CUIDragDropItemMP*)pWnd)->GetData();
//		SetCurrentItem(pInvItem);
		if (m_pCurrentDDItem) m_pCurrentDDItem->Highlight(false);
		m_pCurrentDDItem = pItem;

		// Cкейлим и увеличиваем текстуру если разрещено перетаскивание
		if (m_pCurrentDDItem->IsDragDropEnabled())
		{
			m_pCurrentDDItem->Rescale(1.0f,1.0f);
		}

		// for BuyWeaponWnd
		//// Disable highliht in all DD lists
		//for (int i = 0; i < PDA_SLOT; ++i)
		//{
		//	UITopList[i].HighlightAllCells(false);
		//}
}

void CUIBag::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	switch (msg)
	{
	case DRAG_DROP_ITEM_DRAG:
		OnItemDrag(smart_cast<CUIDragDropItemMP*>(pWnd));
		break;


	case DRAG_DROP_ITEM_DB_CLICK:
		if (GetMenuLevel() == mlBoxes)
			OnBoxDbClick(smart_cast<CUIDragDropItemMP*>(pWnd));
		else
			OnItemDbClick(smart_cast<CUIDragDropItemMP*>(pWnd));
		//CUIDragDropItemMP	*pAddonOwner;

		//m_pCurrentDragDropItem = smart_cast<CUIDragDropItemMP*>(pWnd);
		//R_ASSERT(m_pCurrentDragDropItem);

		//// Проверяем, а не находимся ли мы на уровне ящиков?
		//if (m_pCurrentDragDropItem->GetParent() == m_WeaponSubBags.back())
		//{
		//	ApplyFilter(RIFLE_SLOT, weaponFilterName, m_pCurrentDragDropItem->GetSectionName());
		//	if (mlRoot == m_mlCurrLevel) MenuLevelUp();
		//	OnKeyboard(m_pCurrentDragDropItem->GetPosInSectionsGroup() + 2, WINDOW_KEY_PRESSED);
		//}

		//// Проверяем на возможность покупки этой вещи
		//if (m_pCurrentDragDropItem->IsDragDropEnabled())
		//{
		//	CUIDragDropItemMP::AddonIDs addonID;	
		//	pAddonOwner = IsItemAnAddon(m_pCurrentDragDropItem, addonID);

		//	// "Поднять" вещь для освобождения занимаемого места
		//	SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DRAG, NULL);
	
		//	//попытаться закинуть элемент в слот, рюкзак или на пояс
		//	if(!ToSlot())
		//		if(!ToBelt())
		//			if(!ToBag())
		//			//если нельзя, то просто упорядочить элемент в своем списке
		//			{
  //                      CUIWindow* pWindowParent = m_pCurrentDragDropItem->GetParent();
  //                      pWindowParent->DetachChild(m_pCurrentDragDropItem);
		//				pWindowParent->AttachChild(m_pCurrentDragDropItem);
		//				m_pCurrentDragDropItem->Rescale(	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->GetItemsScaleX(),
		//													((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->GetItemsScaleY()	);
		//			}
		//}
		break;

	case DRAG_DROP_ITEM_RBUTTON_CLICK:
		break;

	case DRAG_DROP_REFRESH_ACTIVE_ITEM:
		if (this->m_pCurrentDDItem)
			this->m_pCurrentDDItem->Highlight(true);
		break;
	}
	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUIBag::UpdateMoney(int iMoney){
	m_iMoneyAmount = iMoney;
}

void CUIBag::HideAll(){
	for (int i = GROUP_2; i<= GROUP_DEFAULT; i++)
		m_groups[i].Show(false);

	SetMenuLevel(mlRoot);
}

// Init Boxes SLOT
void CUIBag::InitBoxes(){
	// Пробегаемся по всем ящичкам и создаем соответсвующие им айтемы
	for (u32 i = 0; i < m_boxesDefs.size(); ++i)
	{
		CUIDragDropItemMP* pNewDDItem = xr_new<CUIDragDropItemMP>();
		pNewDDItem->SetAutoDelete(false);

		m_boxesDefs[i].pDDItem = pNewDDItem;

		pNewDDItem->CUIStatic::Init(*m_boxesDefs[i].texName, 0, 0, INV_GRID_WIDTH, INV_GRID_HEIGHT);
//		pNewDDItem->SetTextureScaleXY(SECTION_ICON_SCALE, SECTION_ICON_SCALE);
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

		// Сохраняем указатель на лист "хозяин" вещи
		pNewDDItem->SetOwner(&m_groups[GROUP_BOXES]);
		pNewDDItem->SetMessageTarget(this);
		// Задаем специальную дополнительную функцию отрисовки, для
		// отображения номера оружия в углу его иконки
		//pNewDDItem->SetCustomDraw(static_cast<CUSTOM_UPDATE_PROC>(WpnDrawIndex));
	}
}

void CUIBag::ShowSection(int iSection){
	R_ASSERT2(  (GROUP_2 <= iSection) && (iSection <= GROUP_6)   ,"invalid section number");

	HideAll();
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
	// Номер секции с арморами
	const int			armorSectionIndex = 5;

	// Поле strSectionName должно содержать имя секции
	R_ASSERT(m_StrSectionName != "");
	R_ASSERT2(pSettings->section_exist(m_StrSectionName), "Section doesn't exist");

	for (int i = 1; i < 20; ++i)
	{
		// Очищаем буфер
		wpnOneType.clear();

		// Имя поля
		sprintf(wpnSection, "slot%i", i);
		if (!pSettings->line_exist(m_StrSectionName, wpnSection)) 
		{
			m_wpnSectStorage.push_back(wpnOneType);
			continue;
		}

		// Читаем данные этого поля
		std::strcpy(wpnNames, pSettings->r_string(m_StrSectionName, wpnSection));
		u32 count	= _GetItemCount(wpnNames);
		// теперь для каждое имя оружия, разделенные запятыми, заносим в массив
		for (u32 j = 0; j < count; ++j)
		{
			_GetItem(wpnNames, j, wpnSingleName);
			wpnOneType.push_back(wpnSingleName);

			// Для арморов дополнительно инициализируем таблицу соответсвия армора и 
			// и иконки персонажа в этом арморе
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


void CUIBag::FillUpGroups()
{
	for (WPN_LISTS::size_type i = 0; i < m_wpnSectStorage.size(); ++i)
	{
		FillUpGroup(static_cast<u32>(i));
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIBag::FillUpGroup(const u32 group)
{
	for (WPN_SECT_NAMES::size_type j = 0; j < m_wpnSectStorage[group].size(); ++j)
	{
		CUIDragDropItemMP* pNewDDItem = xr_new<CUIDragDropItemMP>();
		pNewDDItem->SetAutoDelete(false);

		//CUIDragDropItemMP& UIDragDropItem = m_vDragDropItems[GetFirstFreeIndex()];

		pNewDDItem->CUIStatic::Init(0, 0, INV_GRID_WIDTH, INV_GRID_HEIGHT);
		pNewDDItem->SetShader(GetEquipmentIconsShader());
//		pNewDDItem->SetTextureScaleXY(SECTION_ICON_SCALE, SECTION_ICON_SCALE);
		pNewDDItem->SetStretchTexture(true);
		pNewDDItem->SetColor(0xffffffff);

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
		
		pNewDDItem->SetSlot(slot);
		pNewDDItem->SetSectionGroupID(group);
		pNewDDItem->SetPosInSectionsGroup(static_cast<u32>(j));

		pNewDDItem->SetFont(UI()->Font()->pFontLetterica16Russian);

		// Читаем стоимость оружия
		if (pSettings->line_exist(m_StrSectionName, static_cast<xr_string>(m_wpnSectStorage[group][j] + "_cost").c_str()))
			pNewDDItem->SetCost(pSettings->r_u32(m_StrSectionName, static_cast<xr_string>(m_wpnSectStorage[group][j] + "_cost").c_str()));
		else if (pSettings->line_exist(m_StrPricesSection, m_wpnSectStorage[group][j].c_str()))
			pNewDDItem->SetCost(pSettings->r_u32(m_StrPricesSection, m_wpnSectStorage[group][j].c_str()));
		else
		{
			string256	buf;
			sprintf(buf, "Cannot find price for an item %s in sections: %s, %s",
					m_wpnSectStorage[group][j].c_str(), *m_StrSectionName, *m_StrPricesSection);
			R_ASSERT2(false, buf);
		}

		// Если на оружие не хватает денег, то запрещаем его перемещение и помечаем цветом, 
		// что оно недоступно
		if (pNewDDItem->GetCost() > GetMoneyAmount())
		{
			pNewDDItem->SetColor(cUnableToBuy);
		}

		// Для арморов читаем дополнительно координаты на текстуре с иконками персонажей для арморов
		if (OUTFIT_SLOT == slot)
		{
			// Для каждого армора, который может быть у нас в мультиплеере, читаем инфу
			// для иконки сталкера в полный рост в этом арморе и с нужным цветом
			for (CONFORMITY_TABLE_it it = m_ConformityTable.begin(); it != m_ConformityTable.end(); ++it)
			{
				// Информация о таком арморе есть
				if (0 == xr_strcmp(it->first, m_wpnSectStorage[group][j].c_str()))
				{
					shared_str modelName = it->second;

					int m_iSkinX = 0, m_iSkinY = 0;
					sscanf(pSettings->r_string("multiplayer_skins", *modelName), "%i,%i", &m_iSkinX, &m_iSkinY);
					pNewDDItem->m_fAdditionalInfo.push_back(static_cast<float>(m_iSkinX));
					pNewDDItem->m_fAdditionalInfo.push_back(static_cast<float>(m_iSkinY));
				}
		
			}

			// Изменяем мессажд таргет для возможности реакции на то, что костюм возвращается в слот

			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ATTENTION
			pNewDDItem->SetMessageTarget(this);
		}

		InitAddonsInfo(*pNewDDItem, m_wpnSectStorage[group][j]);

		pNewDDItem->SetGridHeight(iGridHeight);
		pNewDDItem->SetGridWidth(iGridWidth);

		pNewDDItem->GetUIStaticItem().SetOriginalRect(
			iXPos * INV_GRID_WIDTH,
			iYPos * INV_GRID_HEIGHT,
			iGridWidth * INV_GRID_WIDTH,
			iGridHeight * INV_GRID_HEIGHT);


		pNewDDItem->SetSectionName(m_wpnSectStorage[group][j].c_str());


		// Количество доступных секций должно быть не больше затребованных
		//R_ASSERT(group < m_WeaponSubBags.size());
//		m_groups[slot].AttachChild(pNewDDItem);
		PutItemToGroup(pNewDDItem, group);
		//m_WeaponSubBags[group]->AttachChild(pNewDDItem);
		//pNewDDItem->SetAutoDelete(false);

		// Сохраняем указатель на лист "хозяин" вещи
		//pNewDDItem->SetOwner(m_WeaponSubBags[group]);
        
		pNewDDItem->SetMessageTarget(this);

		// Задаем специальную дополнительную функцию отрисовки, для
		// отображения номера оружия в углу его иконки
		//pNewDDItem->SetCustomDraw(static_cast<CUSTOM_UPDATE_PROC>(WpnDrawIndex));

		m_allItems.push_back(pNewDDItem);
	}
}

/// maybe i will delete this bullshit
//void CUIBag::ClearWpnSubBags()
//{
//	// Без "секретной" секции с аддонами
//	for (WEAPON_TYPES::size_type i = 0; i < m_WeaponSubBags.size() - 1; ++i)
//	{
//		ClearWpnSubBag(static_cast<u32>(i));
//	}
//}

/// maybe i will delete this bullshit
//void CUIBag::ClearWpnSubBag(const u32 group)
//{
//	for (DRAG_DROP_LIST_it it = m_WeaponSubBags[group]->GetDragDropItemsList().begin(); it != m_WeaponSubBags[group]->GetDragDropItemsList().end(); ++it)
//	{
//		for (int i = 0; i < m_iUsedItems; ++i)
//		{
//			if (&m_vDragDropItems[i] == (*it))
//				m_iEmptyIndexes.insert(i);
//		}
//	}
//	m_WeaponSubBags[group]->DropAll();
//}

void CUIBag::PutItemToGroup(CUIDragDropItemMP* pDDItem, int iGroup){
	switch (iGroup)
	{
	case 1:
		m_groups[GROUP_2].AttachChild(pDDItem);
		pDDItem->SetOwner(&m_groups[GROUP_2]);
		break;
	case 2:
		if		(  xr_strcmp(m_boxesDefs[0].filterString, pSettings->r_string(pDDItem->GetSectionName(),weaponFilterName))  )
			m_groups[GROUP_31].AttachChild(pDDItem);
		else if (  xr_strcmp(m_boxesDefs[1].filterString, pSettings->r_string(pDDItem->GetSectionName(),weaponFilterName))  )
			m_groups[GROUP_32].AttachChild(pDDItem);
		else if (  xr_strcmp(m_boxesDefs[2].filterString, pSettings->r_string(pDDItem->GetSectionName(),weaponFilterName))  )
			m_groups[GROUP_33].AttachChild(pDDItem);
		else if (  xr_strcmp(m_boxesDefs[3].filterString, pSettings->r_string(pDDItem->GetSectionName(),weaponFilterName))  )
			m_groups[GROUP_34].AttachChild(pDDItem);
		else
			R_ASSERT2(false, "invalid section for GROUP3");		
		break;
	case 3:
		m_groups[GROUP_4].AttachChild(pDDItem);
		pDDItem->SetOwner(&m_groups[GROUP_4]);
		break;
	case 4:
		m_groups[GROUP_5].AttachChild(pDDItem);
		pDDItem->SetOwner(&m_groups[GROUP_5]);
		break;
	case 5:
		m_groups[GROUP_6].AttachChild(pDDItem);
		pDDItem->SetOwner(&m_groups[GROUP_6]);
		break;
	default:
		m_groups[GROUP_DEFAULT].AttachChild(pDDItem);
		pDDItem->SetOwner(&m_groups[GROUP_DEFAULT]);
	}
}

void CUIBag::InitAddonsInfo(CUIDragDropItemMP &DDItemMP, const xr_string &sectioName)
{
	// Для каждого аддона проверяем его запись в ltx файле, и заполняем структуру

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

//void WpnDrawIndex(CUIDragDropItem *pDDItem)
//{
//	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pDDItem);
//	R_ASSERT(pDDItemMP);
//	if (!pDDItemMP) return;
//
//	int left	= pDDItemMP->GetUIStaticItem().GetPosX();
//	int bottom	= pDDItemMP->GetUIStaticItem().GetPosY() + pDDItemMP->GetUIStaticItem().GetRect().height();
//
//	pDDItemMP->GetFont()->SetColor(0xffffffff);
//	UI()->OutText(pDDItem->GetFont(), pDDItemMP->GetSelfClipRect(), float(left), 
//		float(bottom - pDDItemMP->GetFont()->CurrentHeight()),
//		"%d", pDDItemMP->GetPosInSectionsGroup() + 1);
//
//	pDDItemMP->GetFont()->OnRender();
//}

bool CUIBag::BagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	// Так как только 1 саббег в данный момент приаттачен к UIBagWnd, а pList может быть 
	// и не этим приаттаченым листом, то адрес UIBagWnd мы сохранили в MessageTarget
	CUIBag* pBag = smart_cast<CUIBag*>(pList->GetMessageTarget());
	R_ASSERT2(pBag, "wrong parent addressed");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);

	// У нас не может быть обычная вещь в этом диалоге.
	R_ASSERT(pDDItemMP);
	
	// Удаляем аддоны, только в случае того, чтo вещь не имеет реального прототипа
	pDDItemMP->AttachDetachAllAddons(false);

	// Перемещаем вещь!!!!!!!!!!!!!!!!!!!
	//static_cast<CUIDragDropList*>(pDDItemMP->GetParent())->
	//	DetachChild(pDDItemMP);
	//pDDItemMP->GetOwner()->AttachChild(pDDItemMP);

	// Применяем скейл
	pDDItemMP->Rescale(pDDItemMP->GetOwner()->GetItemsScaleX(), pDDItemMP->GetOwner()->GetItemsScaleY());

	// Если сняли костюм, то изменить цвет на белый иконки с изображением персонажа
	//if (OUTFIT_SLOT == pDDItemMP->GetSlot())
	//	this_inventory->UIOutfitIcon.SetColor(cAbleToBuy);

	// И прибавляем к деньгам стоимость вещи.
	//if (pDDItemMP->m_bAlreadyPaid)
	//{
	//	this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() +
	//		static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));

	//	// Если у вещи есть аддоны, то прибавляем и также и их половинную стоимость
	//	pDDItemMP->m_bAlreadyPaid = false;
	//}

	if (pDDItemMP->GetCost() > pBag->GetMoneyAmount() && !pDDItemMP->m_bHasRealRepresentation)
		pDDItemMP->EnableDragDrop(false);

	// Если это армор, то убедимся, что он стал видимым
	if (OUTFIT_SLOT == pDDItemMP->GetSlot())
	{
		pDDItemMP->Show(true);
	}

	return false;
}