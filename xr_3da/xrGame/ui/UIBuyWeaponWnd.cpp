// UIBuyWeaponWnd.cpp:	окошко, для покупки оружия в режиме CS
//						
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIBuyWeaponWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../hudmanager.h"
#include "UISkinSelector.h"
#include "../uigamesp.h"
#include "../level.h"
#include "UIInventoryUtilities.h"
#include "../InfoPortion.h"
#include "../game_cl_Deathmatch.h"
#include "../string_table.h"

#include <boost/array.hpp>

//////////////////////////////////////////////////////////////////////////

using namespace InventoryUtilities;

//////////////////////////////////////////////////////////////////////////

#define MAX_ITEMS	70

const u32			cDetached					= 0xffffffff;
const u32			cAttached					= 0xff00ff00;
const u32			cUnableToBuy				= 0xffff0000;
const u32			cAbleToBuy					= cDetached;
const u32			cAbleToBuyOwned				= 0xff8080ff;
const u8			uIndicatorWidth				= 17;
const u8			uIndicatorHeight			= 27;
const float			SECTION_ICON_SCALE			= 4.0f/5.0f;
const char * const	BUY_WND_XML_NAME			= "inventoryMP.xml";
const float			fRealItemSellMultiplier		= 0.5f;
const char * const	weaponFilterName			= "weapon_class";
const char * const	BUY_MP_ITEM_XML				= "buy_mp_item.xml";
const char * const	WEAPON_DESCRIPTION_FIELD	= "description";

int			g_iOkAccelerator, g_iCancelAccelerator;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIBuyWeaponWnd::CUIBuyWeaponWnd(LPCSTR strSectionName, LPCSTR strPricesSection)
{
	m_StrSectionName		= NULL;
	m_StrPricesSection		= strPricesSection;
	m_iCurrentActiveSlot	= NO_ACTIVE_SLOT;
	Hide();

//	SetCurrentItem(NULL);

	m_pCurrentDragDropItem = NULL;
//	m_pItemToUpgrade = NULL;
	m_iMoneyAmount = 10000;

	m_iUsedItems	= 0;
	m_iIconTextureY	= 0;
	m_iIconTextureX	= 0;

	SetFont(HUD().pFontMedium);

	m_mlCurrLevel	= mlRoot;
	m_bIgnoreMoney	= false;

	// Инициализируем вещи
	Init(strSectionName);

	Hide();
}

CUIBuyWeaponWnd::~CUIBuyWeaponWnd()
{
}

void CUIBuyWeaponWnd::Init(LPCSTR strSectionName)
{
	m_StrSectionName = strSectionName;

	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", BUY_WND_XML_NAME);
	R_ASSERT2(xml_result, "xml file not found");

	inherited::DetachAll();

	CUIXmlInit xml_init;

	CUIWindow::Init(CUIXmlInit::ApplyAlignX(0, alCenter),
					CUIXmlInit::ApplyAlignY(0, alCenter),
					UI_BASE_WIDTH, UI_BASE_WIDTH);

	AttachChild(&UIStaticTop);
	xml_init.InitStatic(uiXml, "slots_static", 0, &UIStaticTop);

	AttachChild(&UIStaticBelt);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticBelt);

	AttachChild(&UIBagWnd);
	xml_init.InitStatic(uiXml, "bag_background_static", 0, &UIBagWnd);

	////////////////////////////////////////
	//окно с описанием активной вещи

	AttachChild(&UIDescWnd);
	xml_init.InitStatic(uiXml, "desc_static", 0, &UIDescWnd);

	////////////////////////////////////
	//Окно с информации о персонаже
	AttachChild(&UIPersonalWnd);
	xml_init.InitStatic(uiXml, "personal_static", 0, &UIPersonalWnd);

	// Статический бекграунд для кнопок табконтрола выбора оружия
	AttachChild(&UIStaticTabCtrl);
	xml_init.InitStatic(uiXml, "static", 1, &UIStaticTabCtrl);

	// Tabcontrol для оружия
	AttachChild(&UIWeaponsTabControl);
	xml_init.InitTabControl(uiXml, "tab", 0, &UIWeaponsTabControl);

	// Инициализируем статики с названиями амуниции
	InitBackgroundStatics();

	BringToTop(&UIWeaponsTabControl);

	TABS_VECTOR *pTabV = UIWeaponsTabControl.GetButtonsVector();
	for (u8 i = 0; i != pTabV->size(); ++i)
	{
		switch (i)
		{
		case 0:
			(*pTabV)[i]->SetTextureOffset(60, (*pTabV)[i]->GetHeight() / 2 - 10);
			break;
		case 1:
			(*pTabV)[i]->SetTextureOffset(55, (*pTabV)[i]->GetHeight() / 2 - 5);
			break;
		case 2:
			(*pTabV)[i]->SetTextureOffset(55, (*pTabV)[i]->GetHeight() / 2 - 5);
			break;
		case 3:
			(*pTabV)[i]->SetTextureOffset(55, (*pTabV)[i]->GetHeight() / 2 - 10);
			break;
		case 4:
			(*pTabV)[i]->SetTextureOffset(50, (*pTabV)[i]->GetHeight() / 2 - 5);
			break;
		}
		(*pTabV)[i]->SetPushOffsetX(0);
		(*pTabV)[i]->SetPushOffsetY(0);
	}

	// Indicator
	UIGreenIndicator.Init("ui\\ui_bt_multiplayer_over", 0, 0, uIndicatorWidth, uIndicatorHeight);

	// Кнопки OK и Cancel для выходи из диалога покупки оружия
	AttachChild(&UIBtnOK);
	xml_init.InitButton(uiXml, "ok_button", 0, &UIBtnOK);
	UIBtnOK.SetTextAlign(CGameFont::alCenter);
	g_iOkAccelerator = uiXml.ReadAttribInt("ok_button", 0, "accel");

	AttachChild(&UIBtnCancel);
	xml_init.InitButton(uiXml, "cancel_button", 0, &UIBtnCancel);
	UIBtnCancel.SetTextAlign(CGameFont::alCenter);
	g_iCancelAccelerator = uiXml.ReadAttribInt("cancel_button", 0, "accel");

	// Последний лист является списком ящиков оружия (тяжелое, снайперское, штурмовое, и т.д.)
	for (int i = 0; i < 20; ++i)
	{
		CUIDragDropList *pNewDDList = xr_new<CUIDragDropList>();
		R_ASSERT(pNewDDList);

		pNewDDList->SetCheckProc(BagProc);
		// Так только одно подокно всегда аттачено, а, бывает, у нас работа обычно ведется со 
		// всеми подокнами, то запоминаем адрес this, как парента в MessageTarget'e
		pNewDDList->SetMessageTarget(this);
		m_WeaponSubBags.push_back(pNewDDList);
		// Устанавливаем скейл для элементов этого листа
		pNewDDList->SetItemsScale(SECTION_ICON_SCALE);

		xml_init.InitDragDropList(uiXml, "dragdrop_list", 1, pNewDDList);
	}
	// Заполняем массив со списком оружия
	ReInitItems(strSectionName);
	// Инициализируем ящички с оружием
	InitWeaponBoxes();

	//Списки Drag&Drop
	AttachChild(&UITopList[BELT_SLOT]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 0, &UITopList[BELT_SLOT]);

	AttachChild(&UITopList[OUTFIT_SLOT]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 7, &UITopList[OUTFIT_SLOT]);

	AttachChild(&UITopList[0]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 2, &UITopList[0]);
	UITopList[0].BlockCustomPlacement();

	AttachChild(&UITopList[1]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 3, &UITopList[1]);
	UITopList[1].BlockCustomPlacement();

	AttachChild(&UITopList[2]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 4, &UITopList[2]);
	UITopList[2].BlockCustomPlacement();

	AttachChild(&UITopList[3]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 5, &UITopList[3]);
	UITopList[3].BlockCustomPlacement();

	AttachChild(&UITopList[4]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 6, &UITopList[4]);
	UITopList[4].BlockCustomPlacement();

	UITopList[KNIFE_SLOT].SetCheckProc(SlotProc0);
	UITopList[PISTOL_SLOT].SetCheckProc(SlotProc1);
	UITopList[RIFLE_SLOT].SetCheckProc(SlotProc2);
	UITopList[GRENADE_SLOT].SetCheckProc(SlotProc3);
	UITopList[APPARATUS_SLOT].SetCheckProc(SlotProc4);
	UITopList[OUTFIT_SLOT].SetCheckProc(OutfitSlotProc);

//	UITopList[BELT_SLOT].SetCheckProc(BeltProc);

	//pop-up menu
	AttachChild(&UIPropertiesBox);
	UIPropertiesBox.Init("ui\\ui_pop_up",0,0,300,300);
	UIPropertiesBox.Hide();

	// Статики для вывода текста
	AttachChild(&UITotalMoneyHeader);
	xml_init.InitStatic(uiXml, "total_money_header_static", 0, &UITotalMoneyHeader);

	AttachChild(&UITotalMoney);
	xml_init.InitStatic(uiXml, "total_money_static", 0, &UITotalMoney);

	AttachChild(&UIItemCostHeader);
	xml_init.InitStatic(uiXml, "item_cost_header_static", 0, &UIItemCostHeader);

	AttachChild(&UIItemCost);
	xml_init.InitStatic(uiXml, "item_cost_static", 0, &UIItemCost);

	// Иконка изображения персонажа в костюме
	AttachChild(&UIOutfitIcon);
	xml_init.InitStatic(uiXml, "outfit_static", 0, &UIOutfitIcon);
	UIOutfitIcon.SetShader(GetMPCharIconsShader());
	UIOutfitIcon.SetTextureScale(0.65f);
	UIOutfitIcon.ClipperOn();

	UIDescWnd.AttachChild(&UIItemInfo);
	UIItemInfo.Init(0, 0, UIDescWnd.GetWidth(), UIDescWnd.GetHeight(), BUY_MP_ITEM_XML);
}

//////////////////////////////////////////////////////////////////////////

struct BoxInfo
{
	ref_str		texName;
	ref_str		filterString;
	int			gridWidth, gridHeight;
};

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::InitWeaponBoxes()
{
	typedef boost::array<BoxInfo, 4> Boxes;
	Boxes boxesDefs;

	// Заполняем массив информации о ящиках
	boxesDefs[0].texName		= "ui\\ui_inv_box_misc_weapons";
	boxesDefs[0].filterString	= "shotgun";
	boxesDefs[0].gridHeight		= 2;
	boxesDefs[0].gridWidth		= 4;

	boxesDefs[1].texName		= "ui\\ui_inv_box_assault_rifles";
	boxesDefs[1].filterString	= "assault_rifle";
	boxesDefs[1].gridHeight		= 2;
	boxesDefs[1].gridWidth		= 5;

	boxesDefs[2].texName		= "ui\\ui_inv_box_sniper_rifles";
	boxesDefs[2].filterString	= "sniper_rifle";
	boxesDefs[2].gridHeight		= 2;
	boxesDefs[2].gridWidth		= 6;

	boxesDefs[3].texName		= "ui\\ui_inv_box_heavy_weapons";
	boxesDefs[3].filterString	= "heavy_weapon";
	boxesDefs[3].gridHeight		= 2;
	boxesDefs[3].gridWidth		= 6;

	// Последнему листу ассоциируем ящики с оружием
	CUIDragDropList *pBoxesList = m_WeaponSubBags.back();

	// Пробегаемся по всем ящичкам и создаем соответсвующие им айтемы
	for (u32 i = 0; i < boxesDefs.size(); ++i)
	{
		CUIDragDropItemMP &UIDragDropItem = m_vDragDropItems[GetFirstFreeIndex()];
//		UIDragDropItem.SetShader(GetMPCharIconsShader());
		UIDragDropItem.CUIStatic::Init(*boxesDefs[i].texName, 0, 0, INV_GRID_WIDTH, INV_GRID_HEIGHT);
		UIDragDropItem.SetTextureScale(SECTION_ICON_SCALE);
		UIDragDropItem.SetColor(0xffffffff);
		UIDragDropItem.EnableDragDrop(false);

		UIDragDropItem.SetSlot(WEAPON_BOXES_SLOT);
		UIDragDropItem.SetSectionGroupID(WEAPON_BOXES_SLOT);
		UIDragDropItem.SetPosInSectionsGroup(i);

		UIDragDropItem.SetFont(HUD().pFontLetterica16Russian);

		UIDragDropItem.SetGridHeight(boxesDefs[i].gridHeight);
		UIDragDropItem.SetGridWidth(boxesDefs[i].gridWidth);
		UIDragDropItem.GetUIStaticItem().SetOriginalRect(
			0, 0,
			boxesDefs[i].gridWidth * INV_GRID_WIDTH - INV_GRID_WIDTH,
			boxesDefs[i].gridHeight * INV_GRID_HEIGHT - INV_GRID_HEIGHT / 2);

		// Устанавливаем в качестве имени секции для ящика имя типа оружия, по которому будет
		// выполняться сортировка
		UIDragDropItem.SetSectionName(*boxesDefs[i].filterString);

		pBoxesList->AttachChild(&UIDragDropItem);
		UIDragDropItem.SetAutoDelete(false);

		// Сохраняем указатель на лист "хозяин" вещи
		UIDragDropItem.SetOwner(pBoxesList);
		// Задаем специальную дополнительную функцию отрисовки, для
		// отображения номера оружия в углу его иконки
		UIDragDropItem.SetCustomUpdate(static_cast<CUSTOM_UPDATE_PROC>(WpnDrawIndex));
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::InitBackgroundStatics()
{
	static const ref_str	captionsArr[]	= { "small weapons", "main weapons", "grenades", "suits", "equipment" };
	CGameFont				*pNumberF		= HUD().pFontBigDigit, 
							*pCaptionF		= HUD().pFontSmall;
	const float				numberShiftX	= 30.f;
	const float				numberShiftY	= 10.f;
	const float				captionShiftX	= 60.f;
	const float				captionShiftY	= 10.f;
	const u32				numberC			= 0xfff0d9b6;
	const u32				captionC		= 0xfff0d9b6;

	TABS_VECTOR *pTabV = UIWeaponsTabControl.GetButtonsVector();
	int i = 1;

	RECT r;
	CUIMultiTextStatic::SinglePhrase * pSP;

	r = UIStaticTabCtrl.GetAbsoluteRect();
	UIMTStatic.Init(r.left, r.top, r.right - r.left, r.right - r.left);
	AttachChild(&UIMTStatic);

	for (TABS_VECTOR_it it = pTabV->begin(); it != pTabV->end(); ++it)
	{
		r = (*it)->GetAbsoluteRect();

		pSP = UIMTStatic.AddPhrase();
		pSP->outX = r.left - UIMTStatic.GetAbsoluteRect().left + numberShiftX;
		pSP->outY = r.top  - UIMTStatic.GetAbsoluteRect().top + numberShiftY;
		pSP->effect.SetFont(pNumberF);
		pSP->effect.SetTextColor(numberC);
		pSP->SetText("%i.", i);

		pSP = UIMTStatic.AddPhrase();
		pSP->outX = r.left - UIMTStatic.GetAbsoluteRect().left + captionShiftX;
		pSP->outY = r.top - UIMTStatic.GetAbsoluteRect().top + captionShiftY;
		pSP->effect.SetFont(pCaptionF);
		pSP->effect.SetTextColor(captionC);
		pSP->str = CStringTable()(captionsArr[i - 1]);

		++i;
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::ReInitItems	(LPCSTR strSectionName)
{
	// Заполняем массив со списком оружия
	m_StrSectionName = strSectionName;

	ClearSlots();

	// очищаем секции
	for (u32 i=0; i<wpnSectStorage.size(); i++)
	{
		WPN_SECT_NAMES WpnSectName = wpnSectStorage[i];
		for (u32 j=0; j<WpnSectName.size(); j++)
		{
			std::string WpnName = WpnSectName[j];
			WpnName.clear();
		};
		WpnSectName.clear();
	}
	wpnSectStorage.clear();

	InitWpnSectStorage();
	ClearWpnSubBags();
	FillWpnSubBags();

	UIBagWnd.AttachChild(m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex() + 1]);

	// Проверяем возможность покупки для каждой вещи
	CheckBuyAvailabilityInShop();
};

void CUIBuyWeaponWnd::InitInventory() 
{
	m_pMouseCapturer = NULL;
	MenuLevelDown();
	MenuLevelDown();
	ClearWpnSubBag(RIFLE_SLOT);
	FillWpnSubBag(RIFLE_SLOT);
}  

//проверка на помещение инвентаря в слоты
bool CUIBuyWeaponWnd::SlotProc0(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, KNIFE_SLOT)) return false;

	this_inventory->SlotToSection(KNIFE_SLOT);

	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - 
			static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
		pDDItemMP->m_bAlreadyPaid = true;
	}
	return true;
}

bool CUIBuyWeaponWnd::SlotProc1(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, PISTOL_SLOT)) return false;

	this_inventory->SlotToSection(PISTOL_SLOT);

	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - 
			static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
		pDDItemMP->m_bAlreadyPaid = true;
	}
	return true;
}
bool CUIBuyWeaponWnd::SlotProc2(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP::AddonIDs	addonID;
	CUIDragDropItemMP *pDDItemMP	= smart_cast<CUIDragDropItemMP*>(pItem), 
					  *pAddonOwner	= this_inventory->IsItemAnAddon(pDDItemMP, addonID);

	R_ASSERT(pDDItemMP);

	// Если аддон
	if (pAddonOwner)	
	{
		pAddonOwner->AttachDetachAddon(pDDItemMP, !pAddonOwner->IsAddonAttached(addonID), pDDItemMP->m_bHasRealRepresentation);
		return false;
	}
	// Не аддон
	if(!this_inventory->CanPutInSlot(pDDItemMP, RIFLE_SLOT)) return false;

	this_inventory->SlotToSection(RIFLE_SLOT);
	/*
	// Если есть аддоны, то показать их
	if (pDDItemMP->bAddonsAvailable)
	{
		// Подготавливаем аддоны
		for (u8 i = 0; i < CUIDragDropItemMP::NUM_OF_ADDONS; ++i)
		{
			if (pDDItemMP->m_pAddon[i])
				this_inventory->m_WeaponSubBags.back()->AttachChild(pDDItemMP->m_pAddon[i]);
		}
		this_inventory->MenuLevelUp();
		this_inventory->MenuLevelUp();
	}
	*/
	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() -
			static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
		pDDItemMP->m_bAlreadyPaid = true;
	}

	return true;
}

bool CUIBuyWeaponWnd::SlotProc3(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	// Если вещь не граната
	// и если в слоте уже есть 1 граната
	if (!this_inventory->CanPutInSlot(pDDItemMP, GRENADE_SLOT) ||
		!pList->GetDragDropItemsList().empty()) 
		return false;

	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() -
			static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
		pDDItemMP->m_bAlreadyPaid = true;
	}

	return true;
}
bool CUIBuyWeaponWnd::SlotProc4(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, APPARATUS_SLOT)) return false;

	this_inventory->SlotToSection(APPARATUS_SLOT);

	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() -
			static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
		pDDItemMP->m_bAlreadyPaid = true;
	}

	return true;
}

//одеть костюм
bool CUIBuyWeaponWnd::OutfitSlotProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);
	
	if (OUTFIT_SLOT == pDDItemMP->GetSlot())
	{
		this_inventory->SlotToSection(OUTFIT_SLOT);
		pDDItemMP->Show(false);

		// Teперь отображаем соответствующую иконку персонажа, в зависимости от цвета команды, 
		// и купленного костюма

		// Сначала проверим проинициализирован ли правильно костюм
		if (pDDItemMP->m_fAdditionalInfo.size() < 2)
			R_ASSERT(!"Unknown suit");

		xr_vector<float>::iterator it = pDDItemMP->m_fAdditionalInfo.begin();
		this_inventory->UIOutfitIcon.GetUIStaticItem().SetOriginalRect(
			static_cast<int>(*it), 
			static_cast<int>(*(it+1)),
			SKIN_TEX_WIDTH, SKIN_TEX_HEIGHT - 15);
		this_inventory->UIOutfitIcon.Show(true);
		this_inventory->UIOutfitIcon.SetColor(pDDItemMP->GetColor());

//		if (this_inventory->UITopList[OUTFIT_SLOT].GetDragDropItemsList().empty())
//			this_inventory->UIOutfitIcon.SetColor(cAbleToBuy);

		// И отнимаем от денег стоимость вещи.
		if (!pDDItemMP->m_bAlreadyPaid)
		{
			this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() -
				static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
			pDDItemMP->m_bAlreadyPaid = true;
		}

		return true;
	}
	
	return false;
}

//в рюкзак
bool CUIBuyWeaponWnd::BagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	// Так как только 1 саббег в данный момент приаттачен к UIBagWnd, а pList может быть 
	// и не этим приаттаченым листом, то адрес UIBagWnd мы сохранили в MessageTarget
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetMessageTarget());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);

	// У нас не может быть обычная вещь в этом диалоге.
	R_ASSERT(pDDItemMP);
	
	// Удаляем аддоны, только в случае того, чтo вещь не имеет реального прототипа
	pDDItemMP->AttachDetachAllAddons(false);
	// перемещаемся на уровень ниже в меню, если мы на уровне аддонов
//	if (CUIBuyWeaponWnd::mlAddons == this_inventory->m_mlCurrLevel)
//	{
//		this_inventory->MenuLevelDown();
//		this_inventory->MenuLevelDown();
//	}
	// Перемещаем вещь
	static_cast<CUIDragDropList*>(pDDItemMP->GetParent())->
		DetachChild(pDDItemMP);
	pDDItemMP->GetOwner()->AttachChild(pDDItemMP);

	// Применяем скейл
	pDDItemMP->Rescale(pDDItemMP->GetOwner()->GetItemsScale());

	// Если сняли костюм, то изменить цвет на белый иконки с изображением персонажа
	if (OUTFIT_SLOT == pDDItemMP->GetSlot())
		this_inventory->UIOutfitIcon.SetColor(cAbleToBuy);

	// И прибавляем к деньгам стоимость вещи.
	if (pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() +
			static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));

		// Если у вещи есть аддоны, то прибавляем и также и их половинную стоимость
		pDDItemMP->m_bAlreadyPaid = false;
	}

	if (pDDItemMP->GetCost() > this_inventory->GetMoneyAmount() && !pDDItemMP->m_bHasRealRepresentation)
		pDDItemMP->EnableDragDrop(false);

	// Если это армор, то убедимся, что он стал видимым
	if (OUTFIT_SLOT == pDDItemMP->GetSlot())
	{
		pDDItemMP->Show(true);
	}

	return false;
}

//на пояс
bool CUIBuyWeaponWnd::BeltProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	// У нас не может быть обычная вещь в этом диалоге.
	R_ASSERT(pDDItemMP);

	CUIDragDropItemMP::AddonIDs tmp;

	// Если вещь - аддон, то никуда ее не суем
	if (!this_inventory->CanPutInBelt(pDDItemMP)		||
		this_inventory->IsItemAnAddon(pDDItemMP, tmp)) 
		return false;
	
	// И отнимаем от денег стоимость вещи.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() -
			static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
		pDDItemMP->m_bAlreadyPaid = true;
	}

	return true;
}

//------------------------------------------------
//как только подняли элемент, сделать его текущим
void CUIBuyWeaponWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	// Текстуры для иконок кнопок табконтрола
	static const ref_str	inactiveItems[]	= { "ui\\ui_mp_icon_small_weapons", 
												"ui\\ui_mp_icon_main_guns",
												"ui\\ui_mp_icon_grenades",
												"ui\\ui_mp_icon_suits",
												"ui\\ui_mp_icon_equipment" };

	static const ref_str	activeItems[]	= { "ui\\ui_mp_icon_small_weapons_over",
												"ui\\ui_mp_icon_main_guns_over",
												"ui\\ui_mp_icon_grenades_over",
												"ui\\ui_mp_icon_suits_over",
												"ui\\ui_mp_icon_equipment_over" };

	TABS_VECTOR_it	it;

	if(msg == DRAG_DROP_ITEM_DRAG)
	{
//		PIItem pInvItem = (PIItem)((CUIDragDropItemMP*)pWnd)->GetData();

//		SetCurrentItem(pInvItem);
		if (m_pCurrentDragDropItem) m_pCurrentDragDropItem->Highlight(false);
		m_pCurrentDragDropItem = (CUIDragDropItemMP*)pWnd;

		// Cкейлим и увеличиваем текстуру если разрещено перетаскивание
		if (m_pCurrentDragDropItem->IsDragDropEnabled())
		{
			m_pCurrentDragDropItem->Rescale(1.0f);
		}

		// Disable highliht in all DD lists
		for (int i = 0; i < PDA_SLOT; ++i)
		{
			UITopList[i].HighlightAllCells(false);
		}
	}
	else if(msg == DRAG_DROP_ITEM_DB_CLICK)
	{
		CUIDragDropItemMP	*pAddonOwner;

		m_pCurrentDragDropItem = smart_cast<CUIDragDropItemMP*>(pWnd);
		R_ASSERT(m_pCurrentDragDropItem);

		// Проверяем, а не находимся ли мы на уровне ящиков?
		if (m_pCurrentDragDropItem->GetParent() == m_WeaponSubBags.back())
		{
			ApplyFilter(RIFLE_SLOT, weaponFilterName, m_pCurrentDragDropItem->GetSectionName());
			if (mlRoot == m_mlCurrLevel) MenuLevelUp();
			OnKeyboard(m_pCurrentDragDropItem->GetPosInSectionsGroup() + 2, WINDOW_KEY_PRESSED);
		}

		// Проверяем на возможность покупки этой вещи
		if (m_pCurrentDragDropItem->IsDragDropEnabled())
		{
			CUIDragDropItemMP::AddonIDs addonID;	
			pAddonOwner = IsItemAnAddon(m_pCurrentDragDropItem, addonID);

			// "Поднять" вещь для освобождения занимаемого места
			SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DRAG, NULL);
	
			//попытаться закинуть элемент в слот, рюкзак или на пояс
			if(!ToSlot())
				if(!ToBelt())
					if(!ToBag())
					//если нельзя, то просто упорядочить элемент в своем списке
					{
						((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->
							DetachChild(m_pCurrentDragDropItem);
						((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->
							AttachChild(m_pCurrentDragDropItem);
						m_pCurrentDragDropItem->Rescale(((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->GetItemsScale());
					}
		}
	}
	//по нажатию правой кнопки
	else if(msg == DRAG_DROP_ITEM_RBUTTON_CLICK)
	{
		m_pCurrentDragDropItem = (CUIDragDropItemMP*)pWnd;

		ActivatePropertiesBox();
	}
	//сообщение от меню вызываемого правой кнопкой
	else if(pWnd == &UIPropertiesBox &&
		msg == PROPERTY_CLICKED)
	{
		if(UIPropertiesBox.GetClickedItem())
		{
			switch(UIPropertiesBox.GetClickedItem()->GetValue())
			{
			case BUY_ITEM_ACTION:
				SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DB_CLICK, NULL);
				break;
			case CANCEL_BUYING_ACTION:
				m_pCurrentDragDropItem->MoveOnNextDrop();
				UIBagWnd.SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DROP, NULL);
				break;
			case ATTACH_SCOPE_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case ATTACH_SILENCER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case ATTACH_GRENADE_LAUNCHER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case DETACH_SCOPE_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, false, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case DETACH_SILENCER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, false, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case DETACH_GRENADE_LAUNCHER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, false, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			}
		}
	}
	else if (&UIWeaponsTabControl == pWnd && TAB_CHANGED == msg)
	{
		if (mlWpnSubType  == m_mlCurrLevel)
		{
			MenuLevelDown();
			MenuLevelDown();
		}
		if (RIFLE_SLOT == *static_cast<int*>(pData) + 1)
		{
			m_WeaponSubBags.back()->Show(false);
			UIBagWnd.DetachAll();
		}
		else
		{
			m_WeaponSubBags[*static_cast<int*>(pData) + 1]->Show(false);
			UIBagWnd.DetachChild(m_WeaponSubBags[*static_cast<int*>(pData) + 1]);
		}

		if (RIFLE_SLOT == UIWeaponsTabControl.GetActiveIndex() + 1)
		{
			UIBagWnd.AttachChild(m_WeaponSubBags.back());
			m_WeaponSubBags.back()->Show(true);
		}
		else
		{
			m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex() + 1]->Show(true);
			UIBagWnd.AttachChild(m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex() + 1]);
		}
			
		SwitchIndicator(true, UIWeaponsTabControl.GetActiveIndex());

//			if (mlRoot == m_mlCurrLevel)
//			{
//				MenuLevelUp();
//			}
	}
	// Кнопки ОК и Отмена
	else if (&UIBtnOK == pWnd && BUTTON_CLICKED == msg && CanBuyAllItems())
	{
//		HUD().GetUI()->UIGame()->StartStopMenu(this);
//		HUD().GetUI()->UIGame()->OnBuyMenu_Ok();
		Game().StartStopMenu(this);
		game_cl_Deathmatch * dm = smart_cast<game_cl_Deathmatch *>(&(Game()));
		dm->OnBuyMenu_Ok();
	}
	else if (&UIBtnCancel == pWnd && BUTTON_CLICKED == msg)
	{
//		HUD().GetUI()->UIGame()->StartStopMenu(this);
//		HUD().GetUI()->UIGame()->OnBuyMenu_Cancel();
		Game().StartStopMenu(this);
	}
	// Так как у нас при наведении меняется текстура, то обрабатываем эту ситуацию
	else if (STATIC_FOCUS_RECEIVED == msg && &UIWeaponsTabControl == pWnd)
	{
		if (pData)
		{
			int idx = *reinterpret_cast<int*>(pData);
			(*UIWeaponsTabControl.GetButtonsVector())[idx]->GetUIStaticItem().
				Init(*activeItems[idx], "hud\\default", 0, 0, alNone);
		}
	}
	else if (STATIC_FOCUS_LOST == msg && &UIWeaponsTabControl == pWnd)
	{
		if (pData)
		{
			int idx = *reinterpret_cast<int*>(pData);
			(*UIWeaponsTabControl.GetButtonsVector())[idx]->GetUIStaticItem().
				Init(*inactiveItems[idx], "hud\\default", 0, 0, alNone);
		}
	}
	else if (STATIC_FOCUS_RECEIVED == msg && &UIBtnOK == pWnd)
	{
		UIBtnOK.SetTextColor(0xFFF0D9B6);
	}
	else if (STATIC_FOCUS_RECEIVED == msg && &UIBtnCancel == pWnd)
	{
		UIBtnCancel.SetTextColor(0xFFF0D9B6);
	}
	else if (STATIC_FOCUS_LOST == msg && &UIBtnOK == pWnd)
	{
		UIBtnOK.SetTextColor(0xFFEE9B17);
	}
	else if (STATIC_FOCUS_LOST == msg && &UIBtnCancel == pWnd)
	{
		UIBtnCancel.SetTextColor(0xFFEE9B17);
	}
	// Если костюмчик вернулся в слот, то спрятать его
	else if (OUTFIT_RETURNED_BACK == msg && pWnd->GetParent() == &UITopList[OUTFIT_SLOT])
	{
		CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pWnd);
		if (pDDItemMP && OUTFIT_SLOT == pDDItemMP->GetSlot())
		{
			pDDItemMP->Show(false);
		}
	}
	else if (DRAG_DROP_REFRESH_ACTIVE_ITEM == msg)
	{
		if (m_pCurrentDragDropItem) m_pCurrentDragDropItem->Highlight(true);
	}

	CUIWindow::SendMessage(pWnd, msg, pData);
}


void CUIBuyWeaponWnd::OnMouse(int x, int y, EUIMessages mouse_action)
{
	//вызов дополнительного меню по правой кнопке
	if(mouse_action == WINDOW_RBUTTON_DOWN)
	{
		if(UIPropertiesBox.IsShown())
		{
			UIPropertiesBox.Hide();
		}
	}

	inherited::OnMouse(x, y, mouse_action);
}

void CUIBuyWeaponWnd::Draw()
{
	inherited::Draw();
}


void CUIBuyWeaponWnd::Update()
{
	if (wpnSectStorage[GRENADE_SLOT].size() != m_WeaponSubBags[GRENADE_SLOT]->GetDragDropItemsList().size())
	{
		ClearWpnSubBag(GRENADE_SLOT);
		FillWpnSubBag(GRENADE_SLOT);
	}

	// Если активная вещь изменилась, то обновляем информацию о ее стоимости
	static CUIDragDropItemMP *pOldCurrentDragDropItem = reinterpret_cast<CUIDragDropItemMP*>(1);
	static string16		str;

	if (pOldCurrentDragDropItem != m_pCurrentDragDropItem && m_pCurrentDragDropItem)
	{
		pOldCurrentDragDropItem	= m_pCurrentDragDropItem;
		pOldCurrentDragDropItem->Highlight(false);

		UIItemCost.SetText("");
		FillItemInfo(NULL);

		if (m_pCurrentDragDropItem)
		{
			if (m_pCurrentDragDropItem->GetSlot() != WEAPON_BOXES_SLOT)
			{
				int cost = m_pCurrentDragDropItem->GetCost();
				if (m_pCurrentDragDropItem->m_bHasRealRepresentation)
					cost = static_cast<int>(cost * fRealItemSellMultiplier);
				sprintf(str, "%i", cost);
				UIItemCost.SetText(str);
				FillItemInfo(m_pCurrentDragDropItem);
			}
			m_pCurrentDragDropItem->Highlight(true);
		}
	}

	// Если деньги иземниличь то обновить и их
	static int oldMoneyAmount	= 0xffffffff;

	if (oldMoneyAmount != m_iMoneyAmount)
	{
		oldMoneyAmount = m_iMoneyAmount;
		sprintf(str, "%i", m_iMoneyAmount);
		UITotalMoney.SetText(str);
	}

	// Ecли в слоте с костюмом армор показывается, то спрятать его.
	static flag = true;
	if (UITopList[OUTFIT_SLOT].GetDragDropItemsList().empty() && flag)
	{
		UIOutfitIcon.GetUIStaticItem().SetOriginalRect(m_iIconTextureX, m_iIconTextureY, SKIN_TEX_WIDTH, SKIN_TEX_HEIGHT);
		flag = false;
	}

	if (!UITopList[OUTFIT_SLOT].GetDragDropItemsList().empty())
		flag = true;

	CUIWindow::Update();
}

void CUIBuyWeaponWnd::DropItem()
{
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	m_pCurrentItem->Drop();

	(smart_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
		DetachChild(m_pCurrentDragDropItem);

//	SetCurrentItem(NULL);
	m_pCurrentDragDropItem = NULL;
}

void CUIBuyWeaponWnd::Show() 
{ 
	m_pMouseCapturer = NULL;
	inherited::Show();

	if (GameID() != GAME_SINGLE)
	{
		CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor) return;

		NET_Packet	P;
		pActor->u_EventGen(P, GEG_PLAYER_INVENTORYMENU_OPEN, pActor->ID());
		pActor->u_EventSend(P);
	}
}

void CUIBuyWeaponWnd::Hide()
{
	inherited::Hide();

	if (GameID() != GAME_SINGLE)
	{
		CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor) return;

		NET_Packet	P;
		pActor->u_EventGen(P, GEG_PLAYER_INVENTORYMENU_CLOSE, pActor->ID());
		pActor->u_EventSend(P);
	}
}


// Контекстное меню
void CUIBuyWeaponWnd::ActivatePropertiesBox()
{
	int x,y;
	RECT rect = GetAbsoluteRect();
	HUD().GetUI()->GetCursor()->GetPos(x,y);

	UIPropertiesBox.RemoveAll();

	// Если обычная вещь
	if((m_pCurrentDragDropItem->GetSlot() < MP_SLOTS_NUM				||
		static_cast<u32>(-1) == m_pCurrentDragDropItem->GetSlot())		&& 
		&UIBagWnd == m_pCurrentDragDropItem->GetParent()->GetParent())
	{
		// Если вещь не аддон, то действие одно, а если аддон, то другое
		CUIDragDropItemMP::AddonIDs		ID;
		CUIDragDropItemMP				*pDDItemMP = IsItemAnAddon(m_pCurrentDragDropItem, ID);

		if (m_pCurrentDragDropItem->GetCost() > GetMoneyAmount())
		{
			return;
		}
		
		if (pDDItemMP)
		{
			m_pCurrentDragDropItem = pDDItemMP;
			UIPropertiesBox.AddItem("Attach Addon", 
									NULL,
									pDDItemMP->IsAddonAttached(ID) ? DETACH_SILENCER_ADDON + static_cast<int>(ID) : ATTACH_SILENCER_ADDON + static_cast<int>(ID));
		}
		else
		{
			UIPropertiesBox.AddItem("Buy Item",  NULL, BUY_ITEM_ACTION);
		}
	}
	else
	{
		UIPropertiesBox.AddItem("Cancel", NULL, CANCEL_BUYING_ACTION);
		// Так как оружие еще в сумке, то просматриваем его список аддонов и модифицируем меню
		if (m_pCurrentDragDropItem->bAddonsAvailable)
		{
			std::string strMenuItem;
			for (int i = 0; i < 3; ++i)
			{
				switch (m_pCurrentDragDropItem->m_AddonInfo[i].iAttachStatus)
				{
					// If addon detached
				case 0:
					// Если денег на аддон хватает
					if (GetAddonByID(m_pCurrentDragDropItem, static_cast<CUIDragDropItemMP::AddonIDs>(i))->GetCost() <= GetMoneyAmount())
					{
						strMenuItem = std::string("Attach ") + m_pCurrentDragDropItem->m_strAddonTypeNames[i];
						UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, ATTACH_SILENCER_ADDON + i);
					}
					break;
				case 1:
					strMenuItem = std::string("Detach ") + m_pCurrentDragDropItem->m_strAddonTypeNames[i];
					UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, DETACH_SILENCER_ADDON + i);
					break;
				default:
					continue;
				}
			}
		}
	}

	UIPropertiesBox.AutoUpdateSize();
	UIPropertiesBox.BringAllToTop();
	UIPropertiesBox.Show(x-rect.left, y-rect.top);
}


bool CUIBuyWeaponWnd::ToSlot()
{
	bool flag = false;
	// Проверить, что вещь лежит в сумке, а не в слоте
	for (WEAPON_TYPES_it it = m_WeaponSubBags.begin(); it != m_WeaponSubBags.end(); ++it)
	{
		if ((*it) == m_pCurrentDragDropItem->GetParent())
		{
			flag = true;
			break;
		}
	}

	if (!flag) return false;

	// Специальное поведение для гранат, так как они у нас не кончаются
	if (GRENADE_SLOT == m_pCurrentDragDropItem->GetSlot() && 
		!UITopList[GRENADE_SLOT].GetDragDropItemsList().empty()) return false;

	// Если вещь аддон
	CUIDragDropItemMP::AddonIDs ID;
	CUIDragDropItemMP * pDDItemMP = IsItemAnAddon(m_pCurrentDragDropItem, ID);
	if (pDDItemMP)
	{
		pDDItemMP->AttachDetachAddon(ID, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
	}

	// Если вещь без номера слота, то ее поместить никуда нельзя (например патроны)
	if (m_pCurrentDragDropItem->GetSlot() < MP_SLOTS_NUM)
	{
		SlotToSection(m_pCurrentDragDropItem->GetSlot());

		m_pCurrentDragDropItem->MoveOnNextDrop();
		UITopList[m_pCurrentDragDropItem->GetSlot()].SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DROP, NULL);

		m_pMouseCapturer = NULL;
		return true;
	}
	return false;
}

bool CUIBuyWeaponWnd::ToBag()
{
	// Проверить, что вещь лежит в сумке, а не в слоте
	if (m_pCurrentDragDropItem->GetParent()->GetParent() == &UIBagWnd) return false;

	// Если вещь на поясе, то выкидываем ее в инвентарь
	if (BeltToSection(m_pCurrentDragDropItem)) return true;

	// Если вещь в слоте
	if (SlotToSection(m_pCurrentDragDropItem->GetSlot())) return true;
	return false;
}

bool CUIBuyWeaponWnd::ToBelt()
{
	// Ecли вещь не впоясобросаемая
	// Если вещь уже на поясе
	// Если места больше нет
	// Если вещь из подтипа дополнительной аппаратуры (аддоны, фонарь)

	if (!CanPutInBelt(m_pCurrentDragDropItem)							||
		&UITopList[BELT_SLOT] == m_pCurrentDragDropItem->GetParent()	||
		!UITopList[BELT_SLOT].CanPlaceItem(m_pCurrentDragDropItem)		||
		IsItemAnAddonSimple(m_pCurrentDragDropItem))
		return false;

	static_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	UITopList[BELT_SLOT].AttachChild(m_pCurrentDragDropItem);

	if (!m_pCurrentDragDropItem->m_bAlreadyPaid)
	{
		SetMoneyAmount(GetMoneyAmount() -
			static_cast<int>(m_pCurrentDragDropItem->GetCost() * (m_pCurrentDragDropItem->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
		m_pCurrentDragDropItem->m_bAlreadyPaid = true;
	}

	m_pMouseCapturer = NULL;

	return true;
}

//-----------------------------------------------------------------------------/
//  Buy weapon stuff
//-----------------------------------------------------------------------------/
void CUIBuyWeaponWnd::InitWpnSectStorage()
{
	WPN_SECT_NAMES		wpnOneType;
	string16			wpnSection;
	ref_str				iconName;	
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
			wpnSectStorage.push_back(wpnOneType);
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
				m_ConformityTable.push_back(std::make_pair<ref_str, ref_str>(wpnSingleName, iconName));
			}
		}

		if (!wpnOneType.empty())
			wpnSectStorage.push_back(wpnOneType);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::FillWpnSubBags()
{
	for (WPN_LISTS::size_type i = 0; i < wpnSectStorage.size(); ++i)
	{
		FillWpnSubBag(static_cast<u32>(i));
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::FillWpnSubBag(const u32 slotNum)
{
	for (WPN_SECT_NAMES::size_type j = 0; j < wpnSectStorage[slotNum].size(); ++j)
	{
		CUIDragDropItemMP& UIDragDropItem = m_vDragDropItems[GetFirstFreeIndex()];

		UIDragDropItem.CUIStatic::Init(0, 0, INV_GRID_WIDTH, INV_GRID_HEIGHT);
		UIDragDropItem.SetShader(GetEquipmentIconsShader());
		UIDragDropItem.SetTextureScale(SECTION_ICON_SCALE);
		UIDragDropItem.SetColor(0xffffffff);

		//properties used by inventory menu
		int m_iGridWidth	= pSettings->r_u32(wpnSectStorage[slotNum][j].c_str(), "inv_grid_width");
		int m_iGridHeight	= pSettings->r_u32(wpnSectStorage[slotNum][j].c_str(), "inv_grid_height");

		int m_iXPos			= pSettings->r_u32(wpnSectStorage[slotNum][j].c_str(), "inv_grid_x");
		int m_iYPos			= pSettings->r_u32(wpnSectStorage[slotNum][j].c_str(), "inv_grid_y");

		u32 m_slot;

		if(pSettings->line_exist(wpnSectStorage[slotNum][j].c_str(), "slot"))
			m_slot = pSettings->r_u32(wpnSectStorage[slotNum][j].c_str(), "slot");
		else
			m_slot = NO_ACTIVE_SLOT;
		
		UIDragDropItem.SetSlot(m_slot);
		UIDragDropItem.SetSectionGroupID(slotNum);
		UIDragDropItem.SetPosInSectionsGroup(static_cast<u32>(j));

		UIDragDropItem.SetFont(HUD().pFontLetterica16Russian);

		// Читаем стоимость оружия
		if (pSettings->line_exist(m_StrSectionName, static_cast<std::string>(wpnSectStorage[slotNum][j] + "_cost").c_str()))
			UIDragDropItem.SetCost(pSettings->r_u32(m_StrSectionName, static_cast<std::string>(wpnSectStorage[slotNum][j] + "_cost").c_str()));
		else if (pSettings->line_exist(m_StrPricesSection, wpnSectStorage[slotNum][j].c_str()))
			UIDragDropItem.SetCost(pSettings->r_u32(m_StrPricesSection, wpnSectStorage[slotNum][j].c_str()));
		else
		{
			string256	buf;
			sprintf(buf, "Cannot find price for an item %s in sections: %s, %s",
					wpnSectStorage[slotNum][j].c_str(), *m_StrSectionName, *m_StrPricesSection);
			R_ASSERT2(false, buf);
		}

		// Если на оружие не хватает денег, то запрещаем его перемещение и помечаем цветом, 
		// что оно недоступно
		if (UIDragDropItem.GetCost() > GetMoneyAmount())
		{
			UIDragDropItem.SetColor(cUnableToBuy);
//			UIDragDropItem.EnableDragDrop(false);
		}

		// Для арморов читаем дополнительно координаты на текстуре с иконками персонажей для арморов
		if (OUTFIT_SLOT == m_slot)
		{
			// Для каждого армора, который может быть у нас в мультиплеере, читаем инфу
			// для иконки сталкера в полный рост в этом арморе и с нужным цветом
			for (CONFORMITY_TABLE_it it = m_ConformityTable.begin(); it != m_ConformityTable.end(); ++it)
			{
				// Информация о таком арморе есть
				if (0 == xr_strcmp(it->first, wpnSectStorage[slotNum][j].c_str()))
				{
					ref_str modelName = it->second;

					int m_iSkinX = 0, m_iSkinY = 0;
					sscanf(pSettings->r_string("multiplayer_skins", *modelName), "%i,%i", &m_iSkinX, &m_iSkinY);
					UIDragDropItem.m_fAdditionalInfo.push_back(static_cast<float>(m_iSkinX));
					UIDragDropItem.m_fAdditionalInfo.push_back(static_cast<float>(m_iSkinY));
				}
		
			}

			// Изменяем мессажд таргет для возможности реакции на то, что костюм возвращается в слот
			UIDragDropItem.SetMessageTarget(this);
		}

		InitAddonsInfo(UIDragDropItem, wpnSectStorage[slotNum][j]);

		UIDragDropItem.SetGridHeight(m_iGridHeight);
		UIDragDropItem.SetGridWidth(m_iGridWidth);

		UIDragDropItem.GetUIStaticItem().SetOriginalRect(
			m_iXPos * INV_GRID_WIDTH,
			m_iYPos * INV_GRID_HEIGHT,
			m_iGridWidth * INV_GRID_WIDTH,
			m_iGridHeight * INV_GRID_HEIGHT);

		// Количество доступных секций должно быть не больше затребованных
		R_ASSERT(slotNum < m_WeaponSubBags.size());
		m_WeaponSubBags[slotNum]->AttachChild(&UIDragDropItem);
		UIDragDropItem.SetAutoDelete(false);

		// Сохраняем указатель на лист "хозяин" вещи
		UIDragDropItem.SetOwner(m_WeaponSubBags[slotNum]);
		// Задаем специальную дополнительную функцию отрисовки, для
		// отображения номера оружия в углу его иконки
		UIDragDropItem.SetCustomUpdate(static_cast<CUSTOM_UPDATE_PROC>(WpnDrawIndex));

		UIDragDropItem.SetSectionName(wpnSectStorage[slotNum][j].c_str());
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::InitAddonsInfo(CUIDragDropItemMP &DDItemMP, const std::string &sectioName)
{
	// Для каждого аддона проверяем его запись в ltx файле, и заполняем структуру

	for (u8 i = 0; i < CUIDragDropItemMP::NUM_OF_ADDONS; ++i)
	{
		std::string status, strX, strY, name;

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

			// Теперь инициализируем изображение

//			const char* section  = DDItemMP.m_AddonInfo[i].strAddonName.c_str();
//			u32 iGridWidth = pSettings->r_u32(section, "inv_grid_width");
//			u32 iGridHeight = pSettings->r_u32(section, "inv_grid_height");
//			u32 iXPos = pSettings->r_u32(section, "inv_grid_x");
//			u32 iYPos = pSettings->r_u32(section, "inv_grid_y");
//			int cost  = pSettings->r_u32(section, "cost");
//
//			// А теперь создаем реальную вещь
//			CUIDragDropItemMP& UIDragDropItem = m_vDragDropItems[GetFirstFreeIndex()];
//
//			UIDragDropItem.CUIStatic::Init(0,0, 50,50);
//			UIDragDropItem.SetShader(GetEquipmentIconsShader());
//
//			// Наш последний секретный слот предназначен именно для аддонов
//			UIDragDropItem.SetSlot(static_cast<u32>(-1));
//			UIDragDropItem.SetSectionGroupID(m_WeaponSubBags.size() - 1);
//			UIDragDropItem.SetPosInSectionsGroup(i);
//
//			UIDragDropItem.SetGridHeight(iGridHeight);
//			UIDragDropItem.SetGridWidth(iGridWidth);
//
//			UIDragDropItem.SetCost(cost);
//
//			UIDragDropItem.SetFont(HUD().pFontLetterica16Russian);
//
//			UIDragDropItem.GetUIStaticItem().SetOriginalRect(
//				iXPos		* INV_GRID_WIDTH,
//				iYPos		* INV_GRID_HEIGHT,
//				iGridWidth	* INV_GRID_WIDTH,
//				iGridHeight	* INV_GRID_HEIGHT);
//
//			// Количество доступных секций должно быть не больше затребованных
//			UIDragDropItem.SetAutoDelete(false);
//
//			// Задаем специальную дополнительную функцию отрисовки, для
//			// отображения номера оружия в углу его иконки
//			UIDragDropItem.SetCustomUpdate(static_cast<CUSTOM_UPDATE_PROC>(WpnDrawIndex));
//			// Берем адрес аддона и запоминаем его
//			DDItemMP.m_pAddon[i] = &UIDragDropItem;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::ClearWpnSubBags()
{
	// Без "секретной" секции с аддонами
	for (WEAPON_TYPES::size_type i = 0; i < m_WeaponSubBags.size() - 1; ++i)
	{
		ClearWpnSubBag(static_cast<u32>(i));
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::ClearWpnSubBag(const u32 slotNum)
{
	for (DRAG_DROP_LIST_it it = m_WeaponSubBags[slotNum]->GetDragDropItemsList().begin(); it != m_WeaponSubBags[slotNum]->GetDragDropItemsList().end(); ++it)
	{
		for (int i = 0; i < m_iUsedItems; ++i)
		{
			if (&m_vDragDropItems[i] == (*it))
				m_iEmptyIndexes.insert(i);
		}
	}
	m_WeaponSubBags[slotNum]->DropAll();
}

//////////////////////////////////////////////////////////////////////////

bool CUIBuyWeaponWnd::SlotToSection(const u32 SlotNum)
{
	if (SlotNum >= MP_SLOTS_NUM) return false;
	// Выкидываем вещь из слота если есть.
	if (!UITopList[SlotNum].GetDragDropItemsList().empty())
	{
		CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*UITopList[SlotNum].GetDragDropItemsList().begin());
		// Берем текущее оружие в слоте
		pDDItemMP->MoveOnNextDrop();
		// убираем все аддоны
		pDDItemMP->AttachDetachAllAddons(false);
		// делаем вещь текущей
//		m_pCurrentDragDropItem = pDDItemMP;
		// и посылаем ему сообщение переместиться в сумку
		m_WeaponSubBags[pDDItemMP->GetSectionGroupID()]->SendMessage(pDDItemMP, 
									DRAG_DROP_ITEM_DROP, NULL);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIBuyWeaponWnd::CanPutInSlot(CUIDragDropItemMP *pDDItemMP, const u32 slotNum)
{
	return slotNum == pDDItemMP->GetSlot();
}

//////////////////////////////////////////////////////////////////////////

const char * CUIBuyWeaponWnd::GetWeaponName(u32 slotNum)
{
	CUIDragDropItemMP *pDDItemMP = GetWeapon(slotNum, 0);
	if (pDDItemMP)
		return pDDItemMP->GetSectionName();
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

const char * CUIBuyWeaponWnd::GetWeaponNameInBelt(u32 indexInBelt)
{
	CUIDragDropItemMP *pDDItemMP = GetWeapon(BELT_SLOT, indexInBelt);
	if (pDDItemMP)
		return pDDItemMP->GetSectionName();
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

const u8 CUIBuyWeaponWnd::GetItemIndex(u32 slotNum, u32 idx, u8 &sectionNum)
{
	u8 returnID = static_cast<u8>(-1);
	sectionNum = 0;

	// Отменяем фильтр
	ApplyFilter(RIFLE_SLOT, NULL, NULL);

	CUIDragDropItemMP *pDDItemMP = GetWeapon(slotNum, idx);
	if (!pDDItemMP) return returnID;

	for (WPN_LISTS::const_iterator it = wpnSectStorage.begin(); it != wpnSectStorage.end(); ++it)
	{
		WPN_SECT_NAMES::difference_type diff = std::distance((*it).begin(),
			std::find((*it).begin(), (*it).end(), pDDItemMP->GetSectionName()));
		if (diff < static_cast<WPN_SECT_NAMES::difference_type>((*it).size()))
		{
			returnID = static_cast<u8>(diff);

			// Проверяем на наличие приаттаченых аддонов к оружию
			if (pDDItemMP->bAddonsAvailable)
			{
				u8	flags = 0;
				for (int i = 0; i < 3; ++i)
				{
					if (1 == pDDItemMP->m_AddonInfo[i].iAttachStatus)
						flags |= 1;

					flags = flags << 1;
				}
				flags = flags << 4;

				// В результате старшие 3 бита являются флагами признаков аддонов:
				// FF - Scope, FE - Silencer, FD - Grenade Launcher
				returnID |= flags;
			}
			return returnID;
		}
		++sectionNum;
	}
	return returnID;
}

//////////////////////////////////////////////////////////////////////////

const u8 CUIBuyWeaponWnd::GetWeaponIndex(u32 slotNum)
{
	u8 tmp;
	return GetItemIndex(slotNum, 0, tmp);
}

//////////////////////////////////////////////////////////////////////////

const u8 CUIBuyWeaponWnd::GetWeaponIndexInBelt(u32 indexInBelt, u8 &sectionId, u8 &itemId)
{
	itemId = GetItemIndex(BELT_SLOT, indexInBelt, sectionId);
	return itemId;
}

//////////////////////////////////////////////////////////////////////////

const char * CUIBuyWeaponWnd::GetWeaponNameByIndex(u32 grpNum, u8 idx)
{
	// Удаляем информацию о аддонах
	idx &= 0x1f;

	if (wpnSectStorage.size() <= grpNum || idx > wpnSectStorage[grpNum].size()) return NULL;
	return wpnSectStorage[grpNum][idx].c_str();
}

void CUIBuyWeaponWnd::GetWeaponIndexByName(const std::string sectionName, u8 &grpNum, u8 &idx)
{
	grpNum	= (u8)(-1);
	idx		= (u8)(-1);

	for (u8 i = 0; i < wpnSectStorage.size(); ++i)
	{
		for (u8 j = 0; j < wpnSectStorage[i].size(); ++j)
		{
			if (sectionName == wpnSectStorage[i][j])
			{
				grpNum	= i;
				idx		= j;
				return;
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////

CUIDragDropItemMP * CUIBuyWeaponWnd::GetWeapon(u32 slotNum, u32 idx)
{
	R_ASSERT(slotNum < MP_SLOTS_NUM);
	if (UITopList[slotNum].GetDragDropItemsList().empty()) return NULL;
	R_ASSERT(idx < UITopList[slotNum].GetDragDropItemsList().size());

	DRAG_DROP_LIST_it it = UITopList[slotNum].GetDragDropItemsList().begin(); 
	std::advance(it, idx);
	return smart_cast<CUIDragDropItemMP*>(*it);
}

//////////////////////////////////////////////////////////////////////////

const u8 CUIBuyWeaponWnd::GetWeaponAddonInfoByIndex(u8 idx)
{
	return (idx & 0xe0) >> 5;
}

//////////////////////////////////////////////////////////////////////////

bool CUIBuyWeaponWnd::CanPutInBelt(CUIDragDropItemMP *pDDItemMP)
{
	return pDDItemMP->GetSlot() == GRENADE_SLOT || pDDItemMP->GetSlot() > 7;
}

//////////////////////////////////////////////////////////////////////////

bool CUIBuyWeaponWnd::BeltToSection(CUIDragDropItemMP *pDDItemMP)
{
	R_ASSERT(pDDItemMP);

	if (&UITopList[BELT_SLOT] != pDDItemMP->GetParent()) return false;
	// Берем вещь
	pDDItemMP->MoveOnNextDrop();
	// ...и посылаем ему сообщение переместиться в сумку
	m_WeaponSubBags[pDDItemMP->GetSectionGroupID()]->SendMessage(pDDItemMP, DRAG_DROP_ITEM_DROP, NULL);
	return true;
}

//////////////////////////////////////////////////////////////////////////

int CUIBuyWeaponWnd::GetFirstFreeIndex()
{
	R_ASSERT(m_iUsedItems < MAX_ITEMS);

	if (!m_iEmptyIndexes.empty())
	{
		int result = *m_iEmptyIndexes.begin();
		m_iEmptyIndexes.erase(result);
		return result;
	}
	return m_iUsedItems++;
}

//////////////////////////////////////////////////////////////////////////

const u8 CUIBuyWeaponWnd::GetBeltSize()
{
	return static_cast<u8>(UITopList[BELT_SLOT].GetDragDropItemsList().size());
}

//////////////////////////////////////////////////////////////////////////

bool CUIBuyWeaponWnd::MenuLevelJump(MENU_LEVELS lvl)
{
	if (lvl < mlRoot || lvl > mlWpnSubType) return false;
	// Если уровень назначения не тот на котором мы сейчас
	if (m_mlCurrLevel != lvl)
	{
		switch (lvl)
		{
		case mlRoot:
			SwitchIndicator(false, 0);
			break;
		// Второй уровень - уровень коробок. Если у данного типа вооружения нет подразделения на
		// на коробки, то перескакиваем автоматически на уровень выбора оружия
		case mlBoxes:
			if (RIFLE_SLOT == UIWeaponsTabControl.GetActiveIndex() + 1)
			{
				UIBagWnd.DetachAll();
				UIBagWnd.AttachChild(m_WeaponSubBags.back());
				m_WeaponSubBags.back()->HighlightAllCells(false);
				// Зажигаем нашу зеленую лампочку
				SwitchIndicator(true, UIWeaponsTabControl.GetActiveIndex());
				break;
			}
			lvl = static_cast<MENU_LEVELS>(mlWpnSubType - m_mlCurrLevel);
			MenuLevelJump(lvl);
			break;
		case mlWpnSubType:
			// Зажигаем нашу зеленую лампочку
			SwitchIndicator(true, UIWeaponsTabControl.GetActiveIndex());
			// Спрятали
			m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex() + 1]->Show(true);
			UIBagWnd.DetachChild(m_WeaponSubBags.back());
			UIBagWnd.AttachChild(m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex() + 1]);
			break;
		}

		// Запоминаем предыдущий уровень
		m_mlCurrLevel = lvl;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIBuyWeaponWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if (WINDOW_KEY_RELEASED == keyboard_action) return false;

	switch (m_mlCurrLevel)
	{
	// Первый уровень - выбор типа оружия (по табконтролу)
	case mlRoot:
		if (UIWeaponsTabControl.OnKeyboard(dik, keyboard_action))
		{
			if (mlRoot == m_mlCurrLevel)
				MenuLevelUp();
		}
		break;

	case mlBoxes:

		if (dik < DIK_0 && dik > DIK_ESCAPE)
		{
			for (DRAG_DROP_LIST_it it = m_WeaponSubBags.back()->GetDragDropItemsList().begin(); it != m_WeaponSubBags.back()->GetDragDropItemsList().end(); ++it)
			{
				CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*it);
				R_ASSERT(pDDItemMP);
				if (static_cast<u32>(dik - 2) == pDDItemMP->GetPosInSectionsGroup())
				{
					ApplyFilter(RIFLE_SLOT, weaponFilterName, pDDItemMP->GetSectionName());
					MenuLevelUp();
					break;
				}
			}
		}
		break;
	// Третий уровень - выбор конкретного оружия в данной группе
	case mlWpnSubType:

		// Быстрая проверка на возможность дальнейшего плодотворного сотрудничетва
		if (dik < DIK_0 && dik > DIK_ESCAPE)
		{
			// Глубокий (более медленный и точный) поиск
			if (m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex() + 1]->GetDragDropItemsList().size() >= static_cast<u32>(dik - 2))
			{
				DRAG_DROP_LIST_it it = m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex() + 1]->GetDragDropItemsList().begin();
				// Пробегаемся по всем вещам и ищем, нет ли вещи с нужным номером
				for (; it != m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex() + 1]->GetDragDropItemsList().end(); ++it)
				{
					CUIDragDropItemMP *pDDItemMP = static_cast<CUIDragDropItemMP*>(*it);
					// Опа! Нашли.
					if (pDDItemMP->GetPosInSectionsGroup() == static_cast<u32>(dik - 2))
					{
						// Муваем ее в слот
						SendMessage(pDDItemMP, DRAG_DROP_ITEM_DB_CLICK, NULL);
						MenuLevelDown();
						MenuLevelDown();
						return true;
					}
				}
			}
		}
		break;
	default:
		NODEFAULT;
	}

	if (DIK_ESCAPE == dik)
	{
		if (mlRoot != m_mlCurrLevel)
		{
			MenuLevelDown();
			return true;
		}
	}

	if (dik == g_iOkAccelerator)
	{
		SendMessage(&UIBtnOK, BUTTON_CLICKED, NULL);
//		CUIDragDropItemMP *p = GetAddonByID(smart_cast<CUIDragDropItemMP*>(UITopList[RIFLE_SLOT].GetDragDropItemsList().front()), CUIDragDropItemMP::ID_SILENCER);
		return true;
	}

	if (dik == g_iCancelAccelerator)
	{
		if (mlRoot == m_mlCurrLevel)
		{
			//--------------------- for E3 ---------------------
///			SendMessage(&UIBtnOK, CUIButton::BUTTON_CLICKED, NULL);
			//--------------------------------------------------
			SendMessage(&UIBtnCancel, BUTTON_CLICKED, NULL);
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

void WpnDrawIndex(CUIDragDropItem *pDDItem)
{
	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pDDItem);
	R_ASSERT(pDDItemMP);
	if (!pDDItemMP) return;

	int left	= pDDItemMP->GetUIStaticItem().GetPosX();
	int bottom	= pDDItemMP->GetUIStaticItem().GetPosY() + pDDItemMP->GetUIStaticItem().GetRect().height();

	pDDItemMP->GetFont()->SetColor(0xffffffff);
	HUD().OutText(pDDItem->GetFont(), pDDItemMP->GetClipRect(), float(left), 
		float(bottom - pDDItemMP->GetFont()->CurrentHeight()),
		"%d", pDDItemMP->GetPosInSectionsGroup() + 1);

	pDDItemMP->GetFont()->OnRender();
}

////////////////////////////////////////////////////////////////////////////////

//void CUIBuyWeaponWnd::MoveWeapon(const u8 grpNum, const u8 uIndexInSlot, bool bRealRepresentationSet)
//{
//	CUIDragDropItemMP *pDDItemMP = NULL;
//
//	// Получаем оружие
//	for (int i = 0; i < m_iUsedItems; ++i)
//	{
//		if (grpNum == m_vDragDropItems[i].GetSectionGroupID() &&
//			uIndexInSlot == m_vDragDropItems[i].GetPosInSectionsGroup())
//		{
//			pDDItemMP = &m_vDragDropItems[i];
//			pDDItemMP->m_bHasRealRepresentation = bRealRepresentationSet;
//			break;
//		}
//	}
//
//	if (pDDItemMP)
//		SendMessage(pDDItemMP, CUIDragDropItem::ITEM_DB_CLICK, NULL);
//}

////////////////////////////////////////////////////////////////////////////////

//void CUIBuyWeaponWnd::MoveWeapon(const char *sectionName, bool bRealRepresentationSet)
//{
//	u8 grpNum, idx;
//	GetWeaponIndexByName(sectionName, grpNum, idx);
//	if (grpNum	!= static_cast<u8>(-1)	&& 
//		idx		!= static_cast<u8>(-1))
//	{
//		MoveWeapon(grpNum, idx, bRealRepresentationSet);
//	}
//}

////////////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::SwitchIndicator(bool bOn, const int activeTabIndex)
{
	if (bOn)
	{
		R_ASSERT(activeTabIndex < UIWeaponsTabControl.GetTabsCount());
		RECT r	= (*UIWeaponsTabControl.GetButtonsVector())[activeTabIndex]->GetAbsoluteRect();
		UIGreenIndicator.SetWndRect(r.left + 5, r.top + 22, uIndicatorWidth, uIndicatorHeight);
		AttachChild(&UIGreenIndicator);
	}
	else
	{
		DetachChild(&UIGreenIndicator);
	}
}

//////////////////////////////////////////////////////////////////////////

const u8 CUIBuyWeaponWnd::GetCurrentSuit()
{
	if (!UITopList[OUTFIT_SLOT].GetDragDropItemsList().empty())
	{
		CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(UITopList[OUTFIT_SLOT].GetDragDropItemsList().front());
		if (0 == xr_strcmp(pDDItemMP->GetSectionName(), "a"))
		{

		}
	}
	return static_cast<u8>(-1);
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::CheckBuyAvailabilityInShop()
{
	// Пробегаемся по всем вещам и проверяем иx на возможность покупки
	for (WEAPON_TYPES_it it = m_WeaponSubBags.begin(); it != m_WeaponSubBags.end() - 1; ++it)	
	{
		for (DRAG_DROP_LIST_it it2 = (*it)->GetDragDropItemsList().begin(); it2 != (*it)->GetDragDropItemsList().end(); ++it2)
		{
			// В данном диалоге вещь не UIDragDropItemMP быть не может
			CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*it2);
			R_ASSERT(pDDItemMP);

			if (pDDItemMP->GetCost() <= GetMoneyAmount())
			{
				// Если эта вещь была при актере во время вызова меню покупки, то ее цвет - полупрозрачный
				if (pDDItemMP->m_bHasRealRepresentation)
					pDDItemMP->SetColor(cAbleToBuyOwned);
				else
					pDDItemMP->SetColor(cAbleToBuy);
				pDDItemMP->EnableDragDrop(true);
			}
			else
			{
				if (m_pCurrentDragDropItem != pDDItemMP)
				{
					if (!pDDItemMP->m_bHasRealRepresentation)
					{
						pDDItemMP->SetColor(cUnableToBuy);
						pDDItemMP->EnableDragDrop(false);
					}
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

bool CUIBuyWeaponWnd::CheckBuyAvailabilityInSlots()
{
	// массив приоритетов слотов при проверке
	const int arrSize = 5;
	int priorityArr[arrSize] =
	{
		RIFLE_SLOT,
		PISTOL_SLOT,
		OUTFIT_SLOT,
		GRENADE_SLOT,
		KNIFE_SLOT
	};

	bool status = true;

	// Пробегаемся по вещам в слотах, и смотрим есть ли
	// денежки на эту вещь. Если есть, то отнимаем цену вещи
	// от суммы денег, и продолжанм провнрку
	for (int j = 0; j < arrSize; ++j)
	{
		// Если вещь есть
		if (!UITopList[priorityArr[j]].GetDragDropItemsList().empty())
		{
			DRAG_DROP_LIST &pList = UITopList[priorityArr[j]].GetDragDropItemsList();

			for (DRAG_DROP_LIST_it it = pList.begin(); it != pList.end(); ++it)
			{
				CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*it);
				R_ASSERT(pDDItemMP);

				// И если на нее хватает денег, то отнимаем ее цену от количества денег
				// Если эта вещь принесена игроком, то деньги не отнимаем
				if (!pDDItemMP->m_bHasRealRepresentation)
				{
					if (pDDItemMP->GetCost() <= GetMoneyAmount())
					{
						SetMoneyAmount(GetMoneyAmount() - pDDItemMP->GetCost());
						pDDItemMP->SetColor(cAbleToBuy);
					}
					else
					{
						pDDItemMP->SetColor(cUnableToBuy);
						status = false;

						// Для армора закрашиваем дополнительно и иконку с изображением персонажа
						if (OUTFIT_SLOT == pDDItemMP->GetSlot())
							UIOutfitIcon.SetColor(cUnableToBuy);
						pDDItemMP->m_bAlreadyPaid = false;
					}
				}
				else
				{
					pDDItemMP->SetColor(cAbleToBuyOwned);

					// Для армора закрациваем дополнительно и иконку с изображением персонажа
					if (OUTFIT_SLOT == pDDItemMP->GetSlot())
						UIOutfitIcon.SetColor(cAbleToBuyOwned);
				}
			}
		}
	}

	// У пояса наименьший приоритет
	for (DRAG_DROP_LIST_it it = UITopList[BELT_SLOT].GetDragDropItemsList().begin();
		 it != UITopList[BELT_SLOT].GetDragDropItemsList().end();
		 ++it)
	{
		CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*it);
		R_ASSERT(pDDItemMP);

		if (!pDDItemMP->m_bHasRealRepresentation)
		{
			if (pDDItemMP->GetCost() <= GetMoneyAmount())
			{
				SetMoneyAmount(GetMoneyAmount() - pDDItemMP->GetCost());
				pDDItemMP->SetColor(cAbleToBuy);
			}
			else
			{
				pDDItemMP->SetColor(cUnableToBuy);
				pDDItemMP->m_bAlreadyPaid = false;
				status = false;
			}
		}
		else
			pDDItemMP->SetColor(cAbleToBuyOwned);
	}

	return status;
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::SetSkin(u8 SkinID)
{
	const ref_str		skinsParamName = "skins";
	ref_str				tmpStr;

	// Читаем список скинов
	tmpStr = pSettings->r_string(m_StrSectionName, *skinsParamName);

	R_ASSERT(_GetItemCount(*tmpStr) > SkinID);

	// Получаем имя скина, и координаты соответствующей иконки
	string32 a;
	tmpStr = _GetItem(*tmpStr, SkinID, a);
	sscanf(pSettings->r_string("multiplayer_skins", a), "%i,%i", &m_iIconTextureX, &m_iIconTextureY);
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::ClearSlots()
{
	// очищаем слоты
	IgnoreMoney(true);
	SlotToSection(KNIFE_SLOT		);
	SlotToSection(PISTOL_SLOT		);
	SlotToSection(RIFLE_SLOT		);
	SlotToSection(GRENADE_SLOT		);
	SlotToSection(APPARATUS_SLOT	);
	SlotToSection(OUTFIT_SLOT		);

	// очищаем пояс
	for (int i = 0; i < m_iUsedItems; ++i)
	{
		BeltToSection(&m_vDragDropItems[i]);
	}
	IgnoreMoney(false);
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::SectionToSlot(const char *sectionName, bool bRealRepresentationSet)
{
	for (int i = 0; i < m_iUsedItems; ++i)
	{
		if (0 == xr_strcmp(m_vDragDropItems[i].GetSectionName(), sectionName))
		{
			if (IsItemInShop(i))
			{
				CUIDragDropItemMP &DDItemMP = m_vDragDropItems[i];
				// Если слот не пуст то не перемещаем
				if (UITopList[DDItemMP.GetSlot()].GetDragDropItemsList().empty() || GRENADE_SLOT == DDItemMP.GetSlot())
				{
					DDItemMP.m_bHasRealRepresentation = bRealRepresentationSet;
					SendMessage(&DDItemMP, DRAG_DROP_ITEM_DB_CLICK, NULL);
				}
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::SectionToSlot(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet)
{
	// Получаем оружие

	// "Выкусываем" флаги аддонов
	u8 uAddonFlags = GetWeaponAddonInfoByIndex(uIndexInSlot);
	uIndexInSlot &= 0x1f; // 0x1f = 00011111;
		
	for (int i = 0; i < m_iUsedItems; ++i)
	{
		if (grpNum == m_vDragDropItems[i].GetSectionGroupID() &&
			uIndexInSlot == m_vDragDropItems[i].GetPosInSectionsGroup())
		{
			if (IsItemInShop(i))
			{
				CUIDragDropItemMP &DDItemMP = m_vDragDropItems[i];
				// Если слот не пуст то не перемещаем
				if (UITopList[DDItemMP.GetSlot()].GetDragDropItemsList().empty() || GRENADE_SLOT == DDItemMP.GetSlot() || NO_ACTIVE_SLOT == DDItemMP.GetSlot())
				{
					// Устанавливаем режим работы с денежками
//					bool bPrevMoneyMode = m_bIgnoreMoney;
//					if (DDItemMP.GetCost() > GetMoneyAmount())
//						IgnoreMoney(true);
//					else
//						IgnoreMoney(false);

					
					DDItemMP.m_bHasRealRepresentation = bRealRepresentationSet;
					SendMessage(&DDItemMP, DRAG_DROP_ITEM_DB_CLICK, NULL);
					// Проверяем индекс на наличие флагов аддонов, и если они есть, то 
					// аттачим аддоны к мувнутому оружию
					if (uAddonFlags != 0)
					{
						if ((uAddonFlags & 1) != 0)
							DDItemMP.AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, true, bRealRepresentationSet);
						if ((uAddonFlags & 2) != 0)
							DDItemMP.AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, true, bRealRepresentationSet);
						if ((uAddonFlags & 4) != 0)
							DDItemMP.AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, true, bRealRepresentationSet);
					}

//					IgnoreMoney(bPrevMoneyMode);
				}
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

bool CUIBuyWeaponWnd::IsItemInShop(int idx)
{
	CUIDragDropItemMP &DDItemMP = m_vDragDropItems[idx];
	if (std::find(m_WeaponSubBags[DDItemMP.GetSectionGroupID()]->GetDragDropItemsList().begin(),
		m_WeaponSubBags[DDItemMP.GetSectionGroupID()]->GetDragDropItemsList().end(),
		&DDItemMP) != m_WeaponSubBags[DDItemMP.GetSectionGroupID()]->GetDragDropItemsList().end())
	{
		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::ClearRealRepresentationFlags()
{
	for (int i = 0; i < m_iUsedItems; ++i)
	{
		m_vDragDropItems[i].m_bHasRealRepresentation = false;
		m_vDragDropItems[i].SetColor(cAbleToBuy);
		m_vDragDropItems[i].EnableDragDrop(true);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::SetMoneyAmount(int moneyAmount)
{
	m_iMoneyAmount = moneyAmount;
	if (!m_bIgnoreMoney)
	{
		CheckBuyAvailabilityInShop();
	}
}

//////////////////////////////////////////////////////////////////////////

bool CUIBuyWeaponWnd::CanBuyAllItems()
{
	for (int i = 0; i < MP_SLOTS_NUM; ++i)
	{
		for (DRAG_DROP_LIST_it it = UITopList[i].GetDragDropItemsList().begin();
			 it != UITopList[i].GetDragDropItemsList().end(); ++it)
		{
			if (cUnableToBuy == (*it)->GetColor())
				return false;
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////

int CUIBuyWeaponWnd::GetMoneyAmount() const
{
	if (!m_bIgnoreMoney)
	{
		return m_iMoneyAmount;
	}
	// Retuin some really big value
	return 10000000;
}

//////////////////////////////////////////////////////////////////////////

CUIDragDropItemMP * CUIBuyWeaponWnd::GetAddonByID(CUIDragDropItemMP *pAddonOwner, CUIDragDropItemMP::AddonIDs ID)
{
	R_ASSERT(pAddonOwner);
	// Пробегаемся по списку вещей и ищем там нужный аддон
	for (int i = 0; i < MAX_ITEMS; ++i)
	{
		for (int j = 0; j < CUIDragDropItemMP::NUM_OF_ADDONS; ++j)
		{
			if (pAddonOwner->m_AddonInfo[j].strAddonName == m_vDragDropItems[i].GetSectionName() &&
				static_cast<CUIDragDropItemMP::AddonIDs>(j) == ID)
			{
				return &m_vDragDropItems[i];
			}
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

CUIDragDropItemMP * CUIBuyWeaponWnd::IsItemAnAddon(CUIDragDropItemMP *pPossibleAddon, CUIDragDropItemMP::AddonIDs &ID)
{
	R_ASSERT(pPossibleAddon);

	// проверяем не аддон ли pPossibleAddon вещи в слот(е/ах)
	if (!UITopList[RIFLE_SLOT].GetDragDropItemsList().empty())
	{
		CUIDragDropItemMP * pDDItemMP = smart_cast<CUIDragDropItemMP*>(UITopList[RIFLE_SLOT].GetDragDropItemsList().front());

		if (pDDItemMP && pDDItemMP->bAddonsAvailable)
		{
			for (u8 j = 0; j < CUIDragDropItemMP::NUM_OF_ADDONS; ++j)
			{
				// Если один из типов аддонов
				if (pPossibleAddon->GetSectionName() == pDDItemMP->m_AddonInfo[j].strAddonName)
				{
					ID = static_cast<CUIDragDropItemMP::AddonIDs>(j);
					return pDDItemMP;
				}
			}
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

bool CUIBuyWeaponWnd::IsItemAnAddonSimple(CUIDragDropItemMP *pPossibleAddon) const
{
	R_ASSERT(pPossibleAddon);
	if (!pPossibleAddon) return false;

	std::string str = pPossibleAddon->GetSectionName();
	return str.find("addon") != std::string::npos;
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::ApplyFilter(int slotNum, const ref_str &name, const ref_str &value)
{
	CUIDragDropList *shop = m_WeaponSubBags[slotNum];

	// Сначала восстанавливаем полный список оружия
	ClearWpnSubBag(slotNum);
	FillWpnSubBag(slotNum);

	// Если параметры name и value пусты, то ничего не делаем. Это эквивалентно отмене фильтра
	if (!name || !value) return;

	int idx = 0;
	// Пробегаемся по всему списку и перекидываем в последний список оружие затребованного типа
	for (DRAG_DROP_LIST_it it = shop->GetDragDropItemsList().begin(); it != shop->GetDragDropItemsList().end();)
	{
		CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*it++);
		R_ASSERT(pDDItemMP);

		if (0 != xr_strcmp(value, pSettings->r_string(pDDItemMP->GetSectionName(), *name)))
		{
			// Нашли оружие не соответствующее фильтру
			RemoveWeapon(shop, pDDItemMP);
		}
		else
		{
			if (!UITopList[RIFLE_SLOT].GetDragDropItemsList().empty() &&
				0 == xr_strcmp(pDDItemMP->GetSectionName(),	smart_cast<CUIDragDropItemMP*>(UITopList[RIFLE_SLOT].GetDragDropItemsList().front())->GetSectionName()))
			{
				RemoveWeapon(shop, pDDItemMP);
				++idx;
			}
			else
			{
				pDDItemMP->SetPosInSectionsGroup(idx++);
			}
		}
	}

	shop->RearrangeItems();
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::RemoveWeapon(CUIDragDropList *shop, CUIDragDropItem *item)
{
	shop->DetachChild(item);

	for (int i = 0; i < m_iUsedItems; ++i)
	{
		if (&m_vDragDropItems[i] == item)
		{
			m_iEmptyIndexes.insert(i);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::FillItemInfo(CUIDragDropItemMP *pDDItemMP)
{
	UIItemInfo.UIName.SetText("");
	UIItemInfo.UIWeight.SetText("");
	UIItemInfo.UIItemImage.TextureOff();
	UIItemInfo.UIDesc.RemoveAll();

	if (pDDItemMP)
	{

		CStringTable stbl;
		if (pSettings->line_exist(pDDItemMP->GetSectionName(), "inv_name"))
		{
			UIItemInfo.UIName.SetText(*stbl(pSettings->r_string(pDDItemMP->GetSectionName(), "inv_name")));
			//		UIItemInfo.AlignRight(UIItemInfo.UIName)
		}

		UIItemInfo.UIItemImage.SetShader(pDDItemMP->GetUIStaticItem().GetShader());
		Irect r = pDDItemMP->GetUIStaticItem().GetOriginalRect();
		UIItemInfo.UIItemImage.GetUIStaticItem().SetOriginalRect(r.x1, r.y1, r.width(), r.height());
		UIItemInfo.UIItemImage.TextureOn();

		// Центрируем или скейлим текстуру в зависимости от ее размера
		RECT r2 = UIItemInfo.UIItemImage.GetWndRect();

		if ((r2.right - r2.left >= r.width()) && (r2.bottom - r2.top >= r.height()))
		{
			UIItemInfo.UIItemImage.SetTextureOffset((r2.right - r2.left - r.width()) / 2, (r2.bottom - r2.top - r.height()) / 2);
			UIItemInfo.UIItemImage.SetTextureScale(1.0f);
		}
		else
		{
			float xFactor = (r2.right - r2.left) / static_cast<float>(r.width()) ;
			float yFactor = (r2.bottom - r2.top) / static_cast<float>(r.height());
			float scale = std::min(xFactor, yFactor);
	
			int xOffset = 0, yOffset = 0;

			if (xFactor > yFactor)
			{
				xOffset = (r2.right - r2.left - r.width()) / 2;
			}
			else
			{
				yOffset = (r2.bottom - r2.top - r.height()) / 2;
			}

			UIItemInfo.UIItemImage.SetTextureOffset(xOffset, yOffset);
			UIItemInfo.UIItemImage.SetTextureScale(scale);
		}

		//	string128 buf;
		//	if (pSettings->line_exist(pDDItemMP->GetSectionName(), "inv_weight"))
		//	{
		//		strconcat(buf, fieldsCaptionColor, *stbl("weight"), " %cdefault", pSettings->r_string(pDDItemMP->GetSectionName(), "inv_weight"));
		//		UIItemInfo.UIWeight.SetText(buf);
		//	}
		//	else
		//		UIItemInfo.UIWeight.SetText("");

		//	sprintf(buf, "%s%s %s%i", fieldsCaptionColor, *stbl("cost"), " %cdefault", pDDItemMP->GetCost());
		//	UIItemInfo.UICost.SetText(buf);

		if (pSettings->line_exist(pDDItemMP->GetSectionName(), WEAPON_DESCRIPTION_FIELD))
		{
			CUIString str;
			str.SetText(*CStringTable()(pSettings->r_string(pDDItemMP->GetSectionName(), WEAPON_DESCRIPTION_FIELD)));
			CUIStatic::PreprocessText(str.m_str, UIItemInfo.UIDesc.GetItemWidth() - 5, UIItemInfo.UIDesc.GetFont());
			UIItemInfo.UIDesc.AddParsedItem<CUIListItem>(str, 0, UIItemInfo.UIDesc.GetTextColor());
		}
	}
}
