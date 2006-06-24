#include "stdafx.h"
#include "UIBagWnd.h"
#include "UIBuyWnd.h"
#include "Restrictions.h"
#include "xrXmlParser.h"
#include "UIXmlInit.h"
#include "UITabButtonMP.h"
#include "UICellCustomItems.h"
//#include "../object_factory.h"
#include "UICellItemFactory.h"
#include <dinput.h>
#include "../HUDManager.h"
//#include "../ui_base.h"

extern CRestrictions g_mp_restrictions;
//using namespace InventoryUtilities;


CUIBagWnd::CUIBagWnd(){
	g_mp_restrictions.InitGroups();

	//for (int i=0; i<4; i++)
 //       subSection_group3[i] = -1;

	m_mlCurrLevel	= mlRoot;

	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
	{
		AttachChild(&m_groups[i]);
	}

	AttachChild(&m_btnBack);

	m_boxesDefs[0].xmlTag		= "btn_bag_shotgun";
	m_boxesDefs[0].filterString	= "shotgun";
	m_boxesDefs[0].gridHeight		= 2;
	m_boxesDefs[0].gridWidth		= 6;

	m_boxesDefs[1].xmlTag		= "btn_bag_assault";
	m_boxesDefs[1].filterString	= "assault_rifle";
	m_boxesDefs[1].gridHeight		= 2;
	m_boxesDefs[1].gridWidth		= 6;

	m_boxesDefs[2].xmlTag		= "btn_bag_sniper";
	m_boxesDefs[2].filterString	= "sniper_rifle";
	m_boxesDefs[2].gridHeight		= 2;
	m_boxesDefs[2].gridWidth		= 6;

	m_boxesDefs[3].xmlTag		= "btn_bag_heavy";
	m_boxesDefs[3].filterString	= "heavy_weapon";
	m_boxesDefs[3].gridHeight		= 2;
	m_boxesDefs[3].gridWidth		= 6;

	m_bIgnoreRank					= false;
	m_bIgnoreMoney					= false;
	subSection_group3[0] = subSection_group3[1] = subSection_group3[2] = subSection_group3[3] = 0;
}

CUIBagWnd::~CUIBagWnd(){
}

void CUIBagWnd::Init(CUIXml& xml, LPCSTR path, LPCSTR sectionName, LPCSTR sectionPrice){
	m_sectionName = sectionName;
	m_sectionPrice = sectionPrice;
	
	CUIXmlInit::InitStatic(xml, path, 0, this);

	for (int i = 0; i < NUMBER_OF_GROUPS; i++){
		CUIXmlInit::InitDragDropListEx(xml, "dragdrop_list_bag", 0, &m_groups[i]);
		m_groups[i].SetMessageTarget(GetParent());
		m_groups[i].m_f_item_db_click		= CUIDragDropListEx::DRAG_DROP_EVENT(static_cast<CUIBuyWnd*>(GetParent()),&CUIBuyWnd::OnItemDbClick);
		m_groups[i].m_f_item_selected		= CUIDragDropListEx::DRAG_DROP_EVENT(static_cast<CUIBuyWnd*>(GetParent()),&CUIBuyWnd::OnItemSelected);
		m_groups[i].m_f_item_drop			= CUIDragDropListEx::DRAG_DROP_EVENT(static_cast<CUIBuyWnd*>(GetParent()),&CUIBuyWnd::OnItemDrop);
		m_groups[i].m_f_item_rbutton_click	= CUIDragDropListEx::DRAG_DROP_EVENT(static_cast<CUIBuyWnd*>(GetParent()),&CUIBuyWnd::OnItemRButtonClick);
	}

	CUIXmlInit::Init3tButton(xml, "bag_back_btn", 0, &m_btnBack);

	InitBoxes(xml);
	InitWpnSectStorage();
	FillUpGroups();
	HideAll();
	SetMenuLevel(mlRoot);
}

// Init Boxes SLOT
void CUIBagWnd::InitBoxes(CUIXml& xml){

	for (u32 i = 0; i < 4; ++i)
	{
		CUITabButtonMP* pNewBtn = xr_new<CUITabButtonMP>();
		pNewBtn->SetAutoDelete(true);
		pNewBtn->SetOrientation(O_HORIZONTAL);
		pNewBtn->SetMessageTarget(this);
		CUIXmlInit::Init3tButton(xml, *m_boxesDefs[i].xmlTag, 0, pNewBtn);
		m_boxesDefs[i].pButton = pNewBtn;
		m_groups[GROUP_BOXES].AttachChild(pNewBtn);
	}
}

// fill out
//				m_wpnSectStorage
//				m_ConformityTable

void CUIBagWnd::InitWpnSectStorage()
{
	WPN_SECT_NAMES		wpnOneType;
	string64			wpnSection;
	shared_str			iconName;
	string1024			wpnNames, wpnSingleName;
	// Номер секции с арморами
	const int			armorSectionIndex = 5;

	// Поле strSectionName должно содержать имя секции
	R_ASSERT(m_sectionName != "");
	R_ASSERT2(pSettings->section_exist(m_sectionName), "Section doesn't exist");

	for (int i = 1; i < 20; ++i)
	{
		// Очищаем буфер
		wpnOneType.clear();

		// Имя поля
		sprintf(wpnSection, "slot%i", i);
		if (!pSettings->line_exist(m_sectionName, wpnSection)) 
		{
			m_wpnSectStorage.push_back(wpnOneType);
			continue;
		}

		// Читаем данные этого поля
		std::strcpy(wpnNames, pSettings->r_string(m_sectionName, wpnSection));
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
				iconName	= pSettings->r_string(m_sectionName, wpnSingleName);
				m_ConformityTable.push_back(std::make_pair<shared_str, shared_str>(wpnSingleName, iconName));
			}
		}

		if (!wpnOneType.empty())
			m_wpnSectStorage.push_back(wpnOneType);
	}

	wpnOneType.clear();

	CInifile::Sect &sect = pSettings->r_section(m_sectionPrice.c_str());
	for (CInifile::SectIt it = sect.begin(); it != sect.end(); it++)
	{
		u8 group_id, index;
		GetWeaponIndexByName((*it).first.c_str(), group_id, index);

		if ((u8)(-1) == group_id || (u8)(-1) == index) // item not found
		{
			wpnOneType.push_back((*it).first.c_str());			//
		}	
	}
	if (!wpnOneType.empty())
			m_wpnSectStorage.push_back(wpnOneType);
}

void CUIBagWnd::FillUpGroups()
{	
	for (WPN_LISTS::size_type i = 0; i < m_wpnSectStorage.size(); ++i)
		FillUpGroup(static_cast<u32>(i));
}

extern "C"
DLL_Pure*	__cdecl xrFactory_Create		(CLASS_ID clsid);

void CUIBagWnd::FillUpGroup(const u32 group)
{
	string64 tmp_str;

	for (WPN_SECT_NAMES::size_type j = 0; j < m_wpnSectStorage[group].size(); ++j)
	{
		LPCSTR sect					= m_wpnSectStorage[group][j].c_str();
		CLASS_ID					class_id = pSettings->r_clsid(sect,"class");
		DLL_Pure					*dll_pure = xrFactory_Create(class_id);
		VERIFY						(dll_pure);
		CInventoryItem*				pIItem = smart_cast<CInventoryItem*>(dll_pure);
		pIItem->object().Load		(sect);
		VERIFY						(pIItem);
		CUICellItem*				itm = create_cell_item(pIItem);
//		itm->m_pData				= pIItem;

		itoa						(int(j+1), tmp_str ,10);
		CBuyItemCustomDrawCell* p	= xr_new<CBuyItemCustomDrawCell>(tmp_str,UI()->Font()->pFontLetterica16Russian);
		itm->SetCustomDraw			(p);		

		PutItemToGroup(itm, group);
		m_allItems.push_back(itm);		
	}
}

void CUIBagWnd::PutItemToGroup(CUICellItem* pItem, int iGroup){
	int iActiveSection = -1;
	shared_str weapon_class;
	CInventoryItem* pIItem;
	string64 tmp_str;

	switch (iGroup)
	{
	case 1:
		iActiveSection = GROUP_2;	break;
	case 2:
		pIItem = (CInventoryItem*)pItem->m_pData;		
		weapon_class = pSettings->r_string(pIItem->object().cNameSect(), "weapon_class");		

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

	m_groups[iActiveSection].SetItem(pItem);	

	if ( 2 == iGroup)
	{
		++subSection_group3[iActiveSection - GROUP_31];
		
		itoa						(subSection_group3[iActiveSection - GROUP_31], tmp_str ,10);
		CBuyItemCustomDrawCell* p	= xr_new<CBuyItemCustomDrawCell>(tmp_str,UI()->Font()->pFontLetterica16Russian);
		pItem->SetCustomDraw			(p);
	}
	else if (3 == iGroup){
        pItem->SetCustomDraw			(NULL);
	}
}

CUIDragDropListEx*	CUIBagWnd::GetItemList(CUICellItem* pItem){
	WPN_SECT_NAMES		wpnOneType;
	string64			wpnSection;
	shared_str			iconName;
//	string1024			wpnNames, wpnSingleName;
	// Номер секции с арморами
//	const int			armorSectionIndex = 5;
	shared_str			weapon_class;

	// Поле strSectionName должно содержать имя секции
	R_ASSERT(m_sectionName != "");
	R_ASSERT2(pSettings->section_exist(m_sectionName), "Section doesn't exist");

	CInventoryItem* pIItem = (CInventoryItem*)pItem->m_pData;

	for (int i = 1; i < 20; ++i)
	{
		// Имя поля
		sprintf(wpnSection, "slot%i", i);
		if (!pSettings->line_exist(m_sectionName, wpnSection)) 
		{
			continue;
		}

		if(strstr(pSettings->r_string(m_sectionName, wpnSection), *pIItem->object().cNameSect()))
		{
			switch (i)
			{
			case 1:
				continue; // knife slot
			case 2:			// pistols slot
				return &m_groups[GROUP_2];
			case 3:			// main weapons
				VERIFY(pSettings->line_exist(pIItem->object().cNameSect(), "weapon_class"));					
				weapon_class = pSettings->r_string(pIItem->object().cNameSect(), "weapon_class");		
				if		(  0 == xr_strcmp(m_boxesDefs[0].filterString, weapon_class) )
		            return &m_groups[GROUP_31];
			    else if (  0 == xr_strcmp(m_boxesDefs[1].filterString, weapon_class) )
					return &m_groups[GROUP_32];
	            else if (  0 == xr_strcmp(m_boxesDefs[2].filterString, weapon_class) )
		            return &m_groups[GROUP_33];
			    else if (  0 == xr_strcmp(m_boxesDefs[3].filterString, weapon_class) )
				    return &m_groups[GROUP_34];
			case 4:
				return &m_groups[GROUP_4];
			case 5:
				return &m_groups[GROUP_5];
			case 6:
				return &m_groups[GROUP_6];
            default:
                return &m_groups[GROUP_DEFAULT];
			}
		}
	}

	NODEFAULT;

	return NULL;
}

//void CUIBagWnd::FillUpItem(CUICellItem* pDDItem, const char* name){
//
//
//}

void CUIBagWnd::GetWeaponIndexByName(const xr_string sectionName, u8 &grpNum, u8 &idx){
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

void CUIBagWnd::HideAll(){
	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
		m_groups[i].Show(false);
}

bool CUIBagWnd::SetMenuLevel(MENU_LEVELS level){
	// check range
	if (level < mlRoot || level > mlWpnSubType) 
		return false;

	m_btnBack.Enable(mlRoot != level);

	// check we really change state
	if (m_mlCurrLevel == level)
		return false;

	m_mlCurrLevel = level;	

	GetMessageTarget()->SendMessage(this, XR_MENU_LEVEL_CHANGED, NULL);

	return true;
}

void CUIBagWnd::ShowSection(int iSection){
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

MENU_LEVELS CUIBagWnd::GetMenuLevel(){
	return m_mlCurrLevel;
}

bool CUIBagWnd::OnKeyboard(int dik, EUIMessages keyboard_action){
	int iGroup;

	if (DIK_ESCAPE == dik)
	{
		m_btnBack.OnClick();
		return true;
	}

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
			OnBtnShotgunsClicked();					return true;
		case DIK_2:
			OnBtnMachinegunsClicked();				return true;
		case DIK_3:
			OnBtnSniperClicked();					return true;
		case DIK_4:
			OnBtnHeavyClicked();					return true;
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

		Log("[[[[[[move item to other list there]]]]]]]");

		/*if (dik <= DIK_0 && dik >= DIK_1)
		{
			CUIDragDropItemMP* pDDItemMP = GetItemByKey(dik,GetCurrentGroupIndex());
			if (pDDItemMP)
				GetTop()->SendMessage(pDDItemMP, DRAG_DROP_ITEM_DB_CLICK, NULL);
		}*/
		break;
	default:
		NODEFAULT;
	}

	return false;
}

void CUIBagWnd::ShowSectionEx(int iSection){
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

void CUIBagWnd::OnBtnShotgunsClicked(){
	m_boxesDefs[0].pButton->OnClick();
	m_boxesDefs[0].pButton->OnMouse(-1, -1, WINDOW_MOUSE_MOVE);
	ShowSectionEx(GROUP_31);
}
void CUIBagWnd::OnBtnMachinegunsClicked(){
	m_boxesDefs[1].pButton->OnClick();
	m_boxesDefs[1].pButton->OnMouse(-1, -1, WINDOW_MOUSE_MOVE);
	ShowSectionEx(GROUP_32);
}
void CUIBagWnd::OnBtnSniperClicked(){
	m_boxesDefs[2].pButton->OnClick();
	m_boxesDefs[2].pButton->OnMouse(-1, -1, WINDOW_MOUSE_MOVE);
	ShowSectionEx(GROUP_33);
}
void CUIBagWnd::OnBtnHeavyClicked(){
	m_boxesDefs[3].pButton->OnClick();
	m_boxesDefs[3].pButton->OnMouse(-1, -1, WINDOW_MOUSE_MOVE);
	ShowSectionEx(GROUP_34);
}

void CUIBagWnd::OnBackClick(){
	int iGroup;

	switch (GetMenuLevel())
	{
	case mlRoot:
		R_ASSERT2(false,"error: CUIBag on level <mlRoot> can't handle OnBackClick");
		break;
	case mlBoxes:
		ShowSectionEx(-1);
		break;
	case mlWpnSubType:		
		iGroup = GetCurrentGroupIndex();
		if (iGroup >= GROUP_31 && iGroup <= GROUP_34 )
			ShowSectionEx(GROUP_BOXES);
		else
			ShowSectionEx(-1);
		break;
	default:
		NODEFAULT;
	}
}

int CUIBagWnd::GetCurrentGroupIndex(){
	for (int i = 0; i < NUMBER_OF_GROUPS; i++)
		if(m_groups[i].IsShown())
			return i;

	return GROUP_DEFAULT;
}

void CUIBagWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	switch (msg)
	{
		// we using our super-puper tab buttons enstead of DragDropItems
		// those buttons uses TAB_CHANGED message for Click event
	case TAB_CHANGED:
		if		(pWnd == m_boxesDefs[0].pButton)
			OnBtnShotgunsClicked();
		else if (pWnd == m_boxesDefs[1].pButton)
			OnBtnMachinegunsClicked();
		else if (pWnd == m_boxesDefs[2].pButton)
			OnBtnSniperClicked();
		else if (pWnd == m_boxesDefs[3].pButton)
			OnBtnHeavyClicked();
		break;

	case BUTTON_CLICKED:
		if (&m_btnBack == pWnd)
			OnBackClick();
		break;
	}
	CUIWindow::SendMessage(pWnd, msg, pData);
}