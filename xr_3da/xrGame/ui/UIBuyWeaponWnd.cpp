// UIBuyWeaponWnd.cpp:	������, ��� ������� ������ � ������ CS
//						
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIBuyWeaponWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "UISkinSelector.h"

#include "../uigamesp.h"
#include "../hudmanager.h"

#include "../WeaponAmmo.h"
#include "../CustomOutfit.h"
#include "../ArtifactMerger.h"
#include "../level.h"

#include "../script_space.h"
#include "../ai_script_processor.h"

#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;

#include "../InfoPortion.h"

//const int	BELT_SLOT		= 5;
//const int	MP_SLOTS_NUM	= 8;

#define MAX_ITEMS	70

const u32			cDetached				= 0xffffffff;
const u32			cAttached				= 0xff00ff00;
const u32			cUnableToBuy			= 0xffff0000;
const u32			cAbleToBuy				= cDetached;
const u32			cAbleToBuyOwned			= 0xff8080ff;
const u8			uIndicatorWidth			= 17;
const u8			uIndicatorHeight		= 27;
const float			SECTION_ICON_SCALE		= 0.8f;
const char * const	BUY_WND_XML_NAME		= "inventoryMP_new2.xml";
const float			fRealItemSellMultiplier	= 0.5f;

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

	m_iUsedItems	= 0;
	m_iIconTextureY	= 0;
	m_iIconTextureX	= 0;

	SetFont(HUD().pFontMedium);

	m_mlCurrLevel	= mlRoot;
	m_bIgnoreMoney	= false;

	SetMoneyAmount(10000);

	// �������������� ����
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

	CUIWindow::Init(0,0, UI_BASE_WIDTH, UI_BASE_WIDTH);

	AttachChild(&UIStaticTop);
	xml_init.InitStatic(uiXml, "slots_static", 0, &UIStaticTop);

	AttachChild(&UIStaticBelt);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticBelt);

	AttachChild(&UIBagWnd);
	xml_init.InitStatic(uiXml, "bag_background_static", 0, &UIBagWnd);

	////////////////////////////////////////
	//���� � ��������� �������� ����

	AttachChild(&UIDescWnd);
	xml_init.InitStatic(uiXml, "desc_static", 0, &UIDescWnd);

	////////////////////////////////////
	//���� � ���������� � ���������
	AttachChild(&UIPersonalWnd);
	xml_init.InitStatic(uiXml, "personal_static", 0, &UIPersonalWnd);

	// ����������� ��������� ��� ������ ����������� ������ ������
	AttachChild(&UIStaticTabCtrl);
	xml_init.InitStatic(uiXml, "static", 1, &UIStaticTabCtrl);

	// Tabcontrol ��� ������
	AttachChild(&UIWeaponsTabControl);
	xml_init.InitTabControl(uiXml, "tab", 0, &UIWeaponsTabControl);

	// �������������� ������� � ���������� ��������
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

	// ������ OK � Cancel ��� ������ �� ������� ������� ������
	AttachChild(&UIBtnOK);
	xml_init.InitButton(uiXml, "ok_button", 0, &UIBtnOK);
	UIBtnOK.SetTextAlign(CGameFont::alCenter);
	g_iOkAccelerator = uiXml.ReadAttribInt("ok_button", 0, "accel");

	AttachChild(&UIBtnCancel);
	xml_init.InitButton(uiXml, "cancel_button", 0, &UIBtnCancel);
	UIBtnCancel.SetTextAlign(CGameFont::alCenter);
	g_iCancelAccelerator = uiXml.ReadAttribInt("cancel_button", 0, "accel");

	for (int i = 0; i < 20; ++i)
	{
		CUIDragDropList *pNewDDList = xr_new<CUIDragDropList>();
		R_ASSERT(pNewDDList);

		pNewDDList->SetCheckProc(BagProc);
		// ��� ������ ���� ������� ������ ��������, �, ������, � ��� ������ ������ ������� �� 
		// ����� ���������, �� ���������� ����� this, ��� ������� � MessageTarget'e
		pNewDDList->SetMessageTarget(this);
		m_WeaponSubBags.push_back(pNewDDList);
		// ������������� ����� ��� ��������� ����� �����
		pNewDDList->SetItemsScale(SECTION_ICON_SCALE);

		xml_init.InitDragDropList(uiXml, "dragdrop_list", 1, pNewDDList);
	}
	// 1 �������������� ���� ��� ������� � ������
//	CUIDragDropList *pNewDDList = xr_new<CUIDragDropList>();
//	R_ASSERT(pNewDDList);
//	xml_init.InitDragDropList(uiXml, "dragdrop_list", 1, pNewDDList);
//	m_WeaponSubBags.push_back(pNewDDList);
//	pNewDDList->SetCheckProc(BagProc);
//	pNewDDList->SetMessageTarget(this);
	// ��������� ������ �� ������� ������
	ReInitItems(strSectionName);

	//������ Drag&Drop
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

	// ������� ��� ������ ������
	AttachChild(&UITotalMoneyHeader);
	xml_init.InitStatic(uiXml, "total_money_header_static", 0, &UITotalMoneyHeader);

	AttachChild(&UITotalMoney);
	xml_init.InitStatic(uiXml, "total_money_static", 0, &UITotalMoney);

	AttachChild(&UIItemCostHeader);
	xml_init.InitStatic(uiXml, "item_cost_header_static", 0, &UIItemCostHeader);

	AttachChild(&UIItemCost);
	xml_init.InitStatic(uiXml, "item_cost_static", 0, &UIItemCost);

	// ������ ����������� ��������� � �������
	AttachChild(&UIOutfitIcon);
	xml_init.InitStatic(uiXml, "outfit_static", 0, &UIOutfitIcon);
	UIOutfitIcon.SetShader(GetMPCharIconsShader());
	UIOutfitIcon.SetTextureScale(0.65f);
	UIOutfitIcon.ClipperOn();
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
		pSP->str = captionsArr[i - 1];

		++i;
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::ReInitItems	(LPCSTR strSectionName)
{
	// ��������� ������ �� ������� ������
	m_StrSectionName = strSectionName;

	ClearSlots();

	// ������� ������
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

	// ��������� ����������� ������� ��� ������ ����
	CheckBuyAvailabilityInShop();
};

void CUIBuyWeaponWnd::InitInventory() 
{
	m_pMouseCapturer = NULL;
}  

//�������� �� ��������� ��������� � �����
bool CUIBuyWeaponWnd::SlotProc0(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = dynamic_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, KNIFE_SLOT)) return false;

	this_inventory->SlotToSection(KNIFE_SLOT);

	// � �������� �� ����� ��������� ����.
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
	CUIBuyWeaponWnd* this_inventory = dynamic_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, PISTOL_SLOT)) return false;

	this_inventory->SlotToSection(PISTOL_SLOT);

	// � �������� �� ����� ��������� ����.
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
	CUIBuyWeaponWnd* this_inventory = dynamic_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP::AddonIDs	addonID;
	CUIDragDropItemMP *pDDItemMP	= dynamic_cast<CUIDragDropItemMP*>(pItem), 
					  *pAddonOwner	= this_inventory->IsItemAnAddon(pDDItemMP, addonID);

	R_ASSERT(pDDItemMP);

	// ���� �����
	if (pAddonOwner)	
	{
		pAddonOwner->AttachDetachAddon(pDDItemMP, !pAddonOwner->IsAddonAttached(addonID));
		return false;
	}
	// �� �����
	if(!this_inventory->CanPutInSlot(pDDItemMP, RIFLE_SLOT)) return false;

	this_inventory->SlotToSection(RIFLE_SLOT);
	/*
	// ���� ���� ������, �� �������� ��
	if (pDDItemMP->bAddonsAvailable)
	{
		// �������������� ������
		for (u8 i = 0; i < CUIDragDropItemMP::NUM_OF_ADDONS; ++i)
		{
			if (pDDItemMP->m_pAddon[i])
				this_inventory->m_WeaponSubBags.back()->AttachChild(pDDItemMP->m_pAddon[i]);
		}
		this_inventory->MenuLevelUp();
		this_inventory->MenuLevelUp();
	}
	*/
	// � �������� �� ����� ��������� ����.
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
	CUIBuyWeaponWnd* this_inventory = dynamic_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	// ���� ���� �� �������
	// � ���� � ����� ��� ���� 1 �������
	if (!this_inventory->CanPutInSlot(pDDItemMP, GRENADE_SLOT) ||
		!pList->GetDragDropItemsList().empty()) 
		return false;

	// � �������� �� ����� ��������� ����.
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
	CUIBuyWeaponWnd* this_inventory = dynamic_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, APPARATUS_SLOT)) return false;

	this_inventory->SlotToSection(APPARATUS_SLOT);

	// � �������� �� ����� ��������� ����.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() -
			static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
		pDDItemMP->m_bAlreadyPaid = true;
	}

	return true;
}

//����� ������
bool CUIBuyWeaponWnd::OutfitSlotProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = dynamic_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);
	
	if (OUTFIT_SLOT == pDDItemMP->GetSlot())
	{
		this_inventory->SlotToSection(OUTFIT_SLOT);
		pDDItemMP->Show(false);

		// Te���� ���������� ��������������� ������ ���������, � ����������� �� ����� �������, 
		// � ���������� �������

		// ������� �������� ������������������ �� ��������� ������
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

		// � �������� �� ����� ��������� ����.
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

//� ������
bool CUIBuyWeaponWnd::BagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	// ��� ��� ������ 1 ������ � ������ ������ ���������� � UIBagWnd, � pList ����� ���� 
	// � �� ���� ������������ ������, �� ����� UIBagWnd �� ��������� � MessageTarget
	CUIBuyWeaponWnd* this_inventory = dynamic_cast<CUIBuyWeaponWnd*>(pList->GetMessageTarget());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(pItem);

	// � ��� �� ����� ���� ������� ���� � ���� �������.
	R_ASSERT(pDDItemMP);
	
	// ������� ������, ������ � ������ ����, ��o ���� �� ����� ��������� ���������
	if (!pDDItemMP->m_bHasRealRepresentation)
		pDDItemMP->AttachDetachAllAddons(false);
	// ������������ �� ������� ���� � ����, ���� �� �� ������ �������
//	if (CUIBuyWeaponWnd::mlAddons == this_inventory->m_mlCurrLevel)
//	{
//		this_inventory->MenuLevelDown();
//		this_inventory->MenuLevelDown();
//	}
	// ���������� ����
	static_cast<CUIDragDropList*>(pDDItemMP->GetParent())->
		DetachChild(pDDItemMP);
	pDDItemMP->GetOwner()->AttachChild(pDDItemMP);

	// ��������� �����
	pDDItemMP->Rescale(pDDItemMP->GetOwner()->GetItemsScale());

	// ���� ����� ������, �� �������� ���� �� ����� ������ � ������������ ���������
	if (OUTFIT_SLOT == pDDItemMP->GetSlot())
		this_inventory->UIOutfitIcon.SetColor(cAbleToBuy);

	// � ���������� � ������� ��������� ����.
	if (pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() +
			static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));

		// ���� � ���� ���� ������, �� ���������� � ����� � �� ���������� ���������
		pDDItemMP->m_bAlreadyPaid = false;
	}

	if (pDDItemMP->GetCost() > this_inventory->GetMoneyAmount() && !pDDItemMP->m_bHasRealRepresentation)
		pDDItemMP->EnableDragDrop(false);

	// ���� ��� �����, �� ��������, ��� �� ���� �������
	if (OUTFIT_SLOT == pDDItemMP->GetSlot())
	{
		pDDItemMP->Show(true);
	}

	return false;
}

//�� ����
bool CUIBuyWeaponWnd::BeltProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = dynamic_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(pItem);
	// � ��� �� ����� ���� ������� ���� � ���� �������.
	R_ASSERT(pDDItemMP);

	CUIDragDropItemMP::AddonIDs tmp;

	// ���� ���� - �����, �� ������ �� �� ����
	if (!this_inventory->CanPutInBelt(pDDItemMP)		||
		this_inventory->IsItemAnAddon(pDDItemMP, tmp)) 
		return false;
	
	// � �������� �� ����� ��������� ����.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() -
			static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
		pDDItemMP->m_bAlreadyPaid = true;
	}

	return true;
}

//------------------------------------------------
//��� ������ ������� �������, ������� ��� �������
void CUIBuyWeaponWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	// �������� ��� ������ ������ �����������
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

	if(msg == CUIDragDropItem::ITEM_DRAG)
	{
//		PIItem pInvItem = (PIItem)((CUIDragDropItemMP*)pWnd)->GetData();

//		SetCurrentItem(pInvItem);
		m_pCurrentDragDropItem = (CUIDragDropItemMP*)pWnd;

		// C������ � ����������� �������� ���� ��������� ��������������
		if (m_pCurrentDragDropItem->IsDragDropEnabled())
		{
			m_pCurrentDragDropItem->Rescale(1.0f);
		}
	}
	else if(msg == CUIDragDropItem::ITEM_DB_CLICK)
	{
		CUIDragDropItemMP	*pAddonOwner;

		m_pCurrentDragDropItem = dynamic_cast<CUIDragDropItemMP*>(pWnd);
		R_ASSERT(m_pCurrentDragDropItem);

		// ��������� �� ����������� ������� ���� ����
		if (m_pCurrentDragDropItem->IsDragDropEnabled())
		{
			CUIDragDropItemMP::AddonIDs addonID;	
			pAddonOwner = IsItemAnAddon(m_pCurrentDragDropItem, addonID);

			// ���������, � �� �������� �� ��� ���� ����-�� �������?
//			if (pAddonOwner)
//			{
//				pAddonOwner->AttachDetachAddon(addonID, !pAddonOwner->IsAddonAttached(addonID));
//			}
			// ���� �� ����� ��� �����, �� ������ ������  ������� �� ����
//			else
			{
				// "�������" ���� ��� ������������ ����������� �����
				SendMessage(m_pCurrentDragDropItem, CUIDragDropItem::ITEM_DRAG, NULL);
	
				//���������� �������� ������� � ����, ������ ��� �� ����
				if(!ToSlot())
					if(!ToBelt())
						if(!ToBag())
						//���� ������, �� ������ ����������� ������� � ����� ������
						{
							((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->
								DetachChild(m_pCurrentDragDropItem);
							((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->
								AttachChild(m_pCurrentDragDropItem);
							m_pCurrentDragDropItem->Rescale(((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->GetItemsScale());
						}
			}
		}
	}
	//�� ������� ������ ������
	else if(msg == CUIDragDropItem::ITEM_RBUTTON_CLICK)
	{
		m_pCurrentDragDropItem = (CUIDragDropItemMP*)pWnd;

		ActivatePropertiesBox();
	}
	//��������� �� ���� ����������� ������ �������
	else if(pWnd == &UIPropertiesBox &&
		msg == CUIPropertiesBox::PROPERTY_CLICKED)
	{
		if(UIPropertiesBox.GetClickedItem())
		{
			switch(UIPropertiesBox.GetClickedItem()->GetValue())
			{
			case BUY_ITEM_ACTION:
				SendMessage(m_pCurrentDragDropItem, CUIDragDropItem::ITEM_DB_CLICK, NULL);
				break;
			case CANCEL_BUYING_ACTION:
				m_pCurrentDragDropItem->MoveOnNextDrop();
				UIBagWnd.SendMessage(m_pCurrentDragDropItem, CUIDragDropItem::ITEM_DROP, NULL);
				break;
			case ATTACH_SCOPE_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, true);
				break;
			case ATTACH_SILENCER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, true);
				break;
			case ATTACH_GRENADE_LAUNCHER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, true);
				break;
			case DETACH_SCOPE_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, false);
				break;
			case DETACH_SILENCER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, false);
				break;
			case DETACH_GRENADE_LAUNCHER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, false);
				break;
			}
		}
	}
	else if (&UIWeaponsTabControl == pWnd && CUITabControl::TAB_CHANGED == msg)
	{
		if (mlAddons == m_mlCurrLevel)
		{
			MenuLevelDown();
		}
		else 
		{
			m_WeaponSubBags[*static_cast<int*>(pData) + 1]->Show(false);
			m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex() + 1]->Show(true);
			UIBagWnd.DetachChild(m_WeaponSubBags[*static_cast<int*>(pData) + 1]);
			UIBagWnd.AttachChild(m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex() + 1]);
			SwitchIndicator(true, UIWeaponsTabControl.GetActiveIndex());

			if (mlRoot == m_mlCurrLevel)
			{
				MenuLevelUp();
			}
		}
	}
	// ������ �� � ������
	else if (&UIBtnOK == pWnd && CUIButton::BUTTON_CLICKED == msg && CanBuyAllItems())
	{
		HUD().GetUI()->UIGame()->StartStopMenu(this);
		HUD().GetUI()->UIGame()->OnBuyMenu_Ok();
	}
	else if (&UIBtnCancel == pWnd && CUIButton::BUTTON_CLICKED == msg)
	{
		HUD().GetUI()->UIGame()->StartStopMenu(this);
		HUD().GetUI()->UIGame()->OnBuyMenu_Cancel();
	}
	// ��� ��� � ��� ��� ��������� �������� ��������, �� ������������ ��� ��������
	else if (CUIButton::BUTTON_FOCUS_RECEIVED == msg && &UIWeaponsTabControl == pWnd)
	{
		if (pData)
		{
			int idx = *reinterpret_cast<int*>(pData);
			(*UIWeaponsTabControl.GetButtonsVector())[idx]->GetUIStaticItem().
				Init(*activeItems[idx], "hud\\default", 0, 0, alNone);
		}
	}
	else if (CUIButton::BUTTON_FOCUS_LOST == msg && &UIWeaponsTabControl == pWnd)
	{
		if (pData)
		{
			int idx = *reinterpret_cast<int*>(pData);
			(*UIWeaponsTabControl.GetButtonsVector())[idx]->GetUIStaticItem().
				Init(*inactiveItems[idx], "hud\\default", 0, 0, alNone);
		}
	}
	else if (CUIButton::BUTTON_FOCUS_RECEIVED == msg && &UIBtnOK == pWnd)
	{
		UIBtnOK.SetTextColor(0xFFF0D9B6);
	}
	else if (CUIButton::BUTTON_FOCUS_RECEIVED == msg && &UIBtnCancel == pWnd)
	{
		UIBtnCancel.SetTextColor(0xFFF0D9B6);
	}
	else if (CUIButton::BUTTON_FOCUS_LOST == msg && &UIBtnOK == pWnd)
	{
		UIBtnOK.SetTextColor(0xFFEE9B17);
	}
	else if (CUIButton::BUTTON_FOCUS_LOST == msg && &UIBtnCancel == pWnd)
	{
		UIBtnCancel.SetTextColor(0xFFEE9B17);
	}
	// ���� ��������� �������� � ����, �� �������� ���
	else if (CUIOutfitSlot::OUTFIT_RETURNED_BACK == msg && pWnd->GetParent() == &UITopList[OUTFIT_SLOT])
	{
		CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(pWnd);
		if (pDDItemMP && OUTFIT_SLOT == pDDItemMP->GetSlot())
		{
			pDDItemMP->Show(false);
		}
	}
//	else if (CUIWindow::LBUTTON_DOWN == msg && &UIOutfitIcon == pWnd)
//	{
//		if (!UITopList[OUTFIT_SLOT].GetDragDropItemsList().empty())
//		{
//			UITopList[OUTFIT_SLOT].GetDragDropItemsList().front()->Enable(true);
//		}
//	}

	CUIWindow::SendMessage(pWnd, msg, pData);
}


void CUIBuyWeaponWnd::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	//����� ��������������� ���� �� ������ ������
	if(mouse_action == RBUTTON_DOWN)
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

	// ���� �������� ���� ����������, �� ��������� ���������� � �� ���������
	static CUIDragDropItemMP *pOldCurrentDragDropItem = reinterpret_cast<CUIDragDropItemMP*>(1);
	static string16		str;

	if (pOldCurrentDragDropItem != m_pCurrentDragDropItem)
	{
		pOldCurrentDragDropItem	= m_pCurrentDragDropItem;
		if (m_pCurrentDragDropItem)
		{
			sprintf(str, "%i", m_pCurrentDragDropItem->GetCost());
			UIItemCost.SetText(str);
		}
		else
			UIItemCost.SetText("");
	}

	// ���� ������ ���������� �� �������� � ��
	static int oldMoneyAmount	= 0xffffffff;

	if (oldMoneyAmount != m_iMoneyAmount)
	{
		oldMoneyAmount = m_iMoneyAmount;
		sprintf(str, "%i", m_iMoneyAmount);
		UITotalMoney.SetText(str);
	}

	// Ec�� � ����� � �������� ����� ������������, �� �������� ���.
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
	CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	m_pCurrentItem->Drop();

	(dynamic_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
		DetachChild(m_pCurrentDragDropItem);

//	SetCurrentItem(NULL);
	m_pCurrentDragDropItem = NULL;
}

void CUIBuyWeaponWnd::Show() 
{ 
	InitInventory();
	inherited::Show();
}

void CUIBuyWeaponWnd::Hide()
{
	inherited::Hide();
}


// ����������� ����
void CUIBuyWeaponWnd::ActivatePropertiesBox()
{
	int x,y;
	RECT rect = GetAbsoluteRect();
	HUD().GetUI()->GetCursor()->GetPos(x,y);

	UIPropertiesBox.RemoveAll();

	// ���� ������� ����
	if((m_pCurrentDragDropItem->GetSlot() < MP_SLOTS_NUM				||
		static_cast<u32>(-1) == m_pCurrentDragDropItem->GetSlot())		&& 
		&UIBagWnd == m_pCurrentDragDropItem->GetParent()->GetParent())
	{
		// ���� ���� �� �����, �� �������� ����, � ���� �����, �� ������
		CUIDragDropItemMP::AddonIDs		ID;
		CUIDragDropItemMP				*pDDItemMP = IsItemAnAddon(m_pCurrentDragDropItem, ID);

		if (pDDItemMP)
		{
			m_pCurrentDragDropItem = pDDItemMP;
			UIPropertiesBox.AddItem(pDDItemMP->IsAddonAttached(ID) ? "Detach Addon" : "Attach Addon", 
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
		// ��� ��� ������ ��� � �����, �� ������������� ��� ������ ������� � ������������ ����
		if (m_pCurrentDragDropItem->bAddonsAvailable)
		{
			std::string strMenuItem;
			for (int i = 0; i < 3; ++i)
			{
				switch (m_pCurrentDragDropItem->m_AddonInfo[i].iAttachStatus)
				{
					// If addon detached
				case 0:
					strMenuItem = std::string("Attach ") + m_pCurrentDragDropItem->m_strAddonTypeNames[i];
					UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, ATTACH_SILENCER_ADDON + i);
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
	// ���������, ��� ���� ����� � �����, � �� � �����
	for (WEAPON_TYPES_it it = m_WeaponSubBags.begin(); it != m_WeaponSubBags.end(); ++it)
	{
		if ((*it) == m_pCurrentDragDropItem->GetParent())
		{
			flag = true;
			break;
		}
	}

	if (!flag) return false;

	// ����������� ��������� ��� ������, ��� ��� ��� � ��� �� ���������
	if (GRENADE_SLOT == m_pCurrentDragDropItem->GetSlot() && 
		!UITopList[GRENADE_SLOT].GetDragDropItemsList().empty()) return false;

	// ���� ���� �����
	CUIDragDropItemMP::AddonIDs ID;
	CUIDragDropItemMP * pDDItemMP = IsItemAnAddon(m_pCurrentDragDropItem, ID);
	if (pDDItemMP)
	{
		pDDItemMP->AttachDetachAddon(ID, true);
	}

	// ���� ���� ��� ������ �����, �� �� ��������� ������ ������ (�������� �������)
	if (m_pCurrentDragDropItem->GetSlot() < MP_SLOTS_NUM)
	{
		SlotToSection(m_pCurrentDragDropItem->GetSlot());

		m_pCurrentDragDropItem->MoveOnNextDrop();
		UITopList[m_pCurrentDragDropItem->GetSlot()].SendMessage(m_pCurrentDragDropItem, CUIDragDropItem::ITEM_DROP, NULL);

		m_pMouseCapturer = NULL;
		return true;
	}
	return false;
}

bool CUIBuyWeaponWnd::ToBag()
{
	// ���������, ��� ���� ����� � �����, � �� � �����
	if (m_pCurrentDragDropItem->GetParent()->GetParent() == &UIBagWnd) return false;

	// ���� ���� �� �����, �� ���������� �� � ���������
	if (BeltToSection(m_pCurrentDragDropItem)) return true;

	// ���� ���� � �����
	if (SlotToSection(m_pCurrentDragDropItem->GetSlot())) return true;
	return false;
}

bool CUIBuyWeaponWnd::ToBelt()
{
	// Ec�� ���� �� ���������������
	// ���� ���� ��� �� �����
	// ���� ����� ������ ���
	// ���� ���� �� ������� �������������� ���������� (������, ������)

	if (!CanPutInBelt(m_pCurrentDragDropItem)							||
		&UITopList[BELT_SLOT] == m_pCurrentDragDropItem->GetParent()	||
		!UITopList[BELT_SLOT].CanPlaceItem(m_pCurrentDragDropItem)		||
		5 == m_pCurrentDragDropItem->GetSectionGroupID())
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
			wpnSectStorage.push_back(wpnOneType);
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

		UIDragDropItem.CUIStatic::Init(0,0, 50,50);
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

		// ������ ��������� ������
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

		// ���� �� ������ �� ������� �����, �� ��������� ��� ����������� � �������� ������, 
		// ��� ��� ����������
		if (UIDragDropItem.GetCost() > GetMoneyAmount())
		{
			UIDragDropItem.SetColor(cUnableToBuy);
//			UIDragDropItem.EnableDragDrop(false);
		}

		// ��� ������� ������ ������������� ���������� �� �������� � �������� ���������� ��� �������
		if (OUTFIT_SLOT == m_slot)
		{
			// ��� ������� ������, ������� ����� ���� � ��� � ������������, ������ ����
			// ��� ������ �������� � ������ ���� � ���� ������ � � ������ ������
			for (CONFORMITY_TABLE_it it = m_ConformityTable.begin(); it != m_ConformityTable.end(); ++it)
			{
				// ���������� � ����� ������ ����
				if (0 == xr_strcmp(it->first, wpnSectStorage[slotNum][j].c_str()))
				{
					ref_str modelName = it->second;

					int m_iSkinX = 0, m_iSkinY = 0;
					sscanf(pSettings->r_string("multiplayer_skins", *modelName), "%i,%i", &m_iSkinX, &m_iSkinY);
					UIDragDropItem.m_fAdditionalInfo.push_back(static_cast<float>(m_iSkinX));
					UIDragDropItem.m_fAdditionalInfo.push_back(static_cast<float>(m_iSkinY));
				}
		
			}

			// �������� ������� ������ ��� ����������� ������� �� ��, ��� ������ ������������ � ����
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

		// ���������� ��������� ������ ������ ���� �� ������ �������������
		R_ASSERT(slotNum < m_WeaponSubBags.size());
		m_WeaponSubBags[slotNum]->AttachChild(&UIDragDropItem);
		UIDragDropItem.SetAutoDelete(false);

		// ��������� ��������� �� ���� "������" ����
		UIDragDropItem.SetOwner(m_WeaponSubBags[slotNum]);
		// ������ ����������� �������������� ������� ���������, ���
		// ����������� ������ ������ � ���� ��� ������
		UIDragDropItem.SetCustomUpdate(static_cast<CUSTOM_UPDATE_PROC>(WpnDrawIndex));

		UIDragDropItem.SetSectionName(wpnSectStorage[slotNum][j].c_str());
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::InitAddonsInfo(CUIDragDropItemMP &DDItemMP, const std::string &sectioName)
{
	// ��� ������� ������ ��������� ��� ������ � ltx �����, � ��������� ���������

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

			// ������ �������������� �����������

//			const char* section  = DDItemMP.m_AddonInfo[i].strAddonName.c_str();
//			u32 iGridWidth = pSettings->r_u32(section, "inv_grid_width");
//			u32 iGridHeight = pSettings->r_u32(section, "inv_grid_height");
//			u32 iXPos = pSettings->r_u32(section, "inv_grid_x");
//			u32 iYPos = pSettings->r_u32(section, "inv_grid_y");
//			int cost  = pSettings->r_u32(section, "cost");
//
//			// � ������ ������� �������� ����
//			CUIDragDropItemMP& UIDragDropItem = m_vDragDropItems[GetFirstFreeIndex()];
//
//			UIDragDropItem.CUIStatic::Init(0,0, 50,50);
//			UIDragDropItem.SetShader(GetEquipmentIconsShader());
//
//			// ��� ��������� ��������� ���� ������������ ������ ��� �������
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
//			// ���������� ��������� ������ ������ ���� �� ������ �������������
//			UIDragDropItem.SetAutoDelete(false);
//
//			// ������ ����������� �������������� ������� ���������, ���
//			// ����������� ������ ������ � ���� ��� ������
//			UIDragDropItem.SetCustomUpdate(static_cast<CUSTOM_UPDATE_PROC>(WpnDrawIndex));
//			// ����� ����� ������ � ���������� ���
//			DDItemMP.m_pAddon[i] = &UIDragDropItem;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::ClearWpnSubBags()
{
	// ��� "���������" ������ � ��������
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
	// ���������� ���� �� ����� ���� ����.
	if (!UITopList[SlotNum].GetDragDropItemsList().empty())
	{
		CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(*UITopList[SlotNum].GetDragDropItemsList().begin());
		// ����� ������� ������ � �����
		pDDItemMP->MoveOnNextDrop();
		// ������� ��� ������
		pDDItemMP->AttachDetachAllAddons(false);
		// ������ ���� �������
//		m_pCurrentDragDropItem = pDDItemMP;
		// � �������� ��� ��������� ������������� � �����
		m_WeaponSubBags[pDDItemMP->GetSectionGroupID()]->SendMessage(pDDItemMP, 
									CUIDragDropItem::ITEM_DROP, NULL);
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

	CUIDragDropItemMP *pDDItemMP = GetWeapon(slotNum, idx);
	if (!pDDItemMP) return returnID;

	for (WPN_LISTS::const_iterator it = wpnSectStorage.begin(); it != wpnSectStorage.end(); ++it)
	{
		WPN_SECT_NAMES::difference_type diff = std::distance((*it).begin(),
			std::find((*it).begin(), (*it).end(), pDDItemMP->GetSectionName()));
		if (diff < static_cast<WPN_SECT_NAMES::difference_type>((*it).size()))
		{
			returnID = static_cast<u8>(diff);

			// ��������� �� ������� ������������ ������� � ������
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
	// ������� ���������� � �������
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
	return dynamic_cast<CUIDragDropItemMP*>(*it);
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
	// ����� ����
	pDDItemMP->MoveOnNextDrop();
	// ...� �������� ��� ��������� ������������� � �����
	m_WeaponSubBags[pDDItemMP->GetSectionGroupID()]->SendMessage(pDDItemMP, CUIDragDropItem::ITEM_DROP, NULL);
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
	if (lvl < mlRoot || lvl > mlAddons) return false;
	// ���� ������� ���������� �� ��� �� ������� �� ������
	if (m_mlCurrLevel != lvl)
	{
		switch (lvl)
		{
		case mlRoot:
			SwitchIndicator(false, 0);
			break;
		case mlWpnSubType:
			// �������� ���� ������� ��������
			SwitchIndicator(true, UIWeaponsTabControl.GetActiveIndex());
			// ���� ������ � mlAddons
			if (m_mlCurrLevel == mlAddons)
			{
//				ClearWpnSubBag(m_WeaponSubBags.size() -1);
				// ��������
				m_WeaponSubBags.back()->DropAll();
				m_WeaponSubBags.back()->Show(false);
				m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex() + 1]->Show(true);
				UIBagWnd.DetachChild(m_WeaponSubBags.back());
				UIBagWnd.AttachChild(m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex() + 1]);
			}
			break;
		case mlAddons:
			// ���������� ���� � ��������
			m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex() + 1]->Show(false);
			m_WeaponSubBags.back()->Show(true);
			UIBagWnd.DetachChild(m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex() + 1]);
			UIBagWnd.AttachChild(m_WeaponSubBags.back());
			break;
		}

		// ���������� ���������� �������
		m_mlCurrLevel = lvl;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIBuyWeaponWnd::OnKeyboard(int dik, E_KEYBOARDACTION keyboard_action)
{
	if (KEY_RELEASED == keyboard_action) return false;

	switch (m_mlCurrLevel)
	{
	// ������ ������� - ����� ���� ������ (�� �����������)
	case mlRoot:
		if (UIWeaponsTabControl.OnKeyboard(dik, keyboard_action))
		{
			if (mlRoot == m_mlCurrLevel)
				MenuLevelUp();
		}
		break;
	// ������ ������� - ����� ����������� ������ � ������ ������
	case mlWpnSubType:

		// ������� �������� �� ����������� ����������� ������������� �������������
		if (dik < DIK_0)
		{
			// �������� (����� ��������� � ������) �����
			if (m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex() + 1]->GetDragDropItemsList().size() >= static_cast<u32>(dik - 2))
			{
				DRAG_DROP_LIST_it it = m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex() + 1]->GetDragDropItemsList().begin();
				// ����������� �� ���� ����� � ����, ��� �� ���� � ������ �������
				for (; it != m_WeaponSubBags[UIWeaponsTabControl.GetActiveIndex() + 1]->GetDragDropItemsList().end(); ++it)
				{
					CUIDragDropItemMP *pDDItemMP = static_cast<CUIDragDropItemMP*>(*it);
					// ���! �����.
					if (pDDItemMP->GetPosInSectionsGroup() == static_cast<u32>(dik - 2))
					{
						// ������ �� � ����
						SendMessage(pDDItemMP, CUIDragDropItem::ITEM_DB_CLICK, NULL);
						return true;
						break;
					}
				}
			}
		}
		break;
	// ������ ������� - ����� ������ � ������, ���� ������� ����
//	case mlAddons:
//		if (dik > DIK_ESCAPE && dik < DIK_4)
//		{
//			// ���������� ��������� �� ����� �� ������� ������
//
//			// ���� � �������� � ��� ������ ���������
//			DRAG_DROP_LIST_it it = m_WeaponSubBags.back()->GetDragDropItemsList().begin();
//			// �������� �������� �� ������ �������
//			std::advance(it, dik - 2);
//			// ������ ���� ������ - ������� ������
//			CUIDragDropItemMP::AddonIDs ID;
//			CUIDragDropItemMP *pDDItemMP = IsItemAnAddon(static_cast<CUIDragDropItemMP*>(*it), ID);
//
//			// R_ASSERT(pDDItemMP);
//			if (!pDDItemMP) return false;
//
//			pDDItemMP->AttachDetachAddon(ID, 0 == pDDItemMP->m_AddonInfo[ID].iAttachStatus);
//		}
//		break;
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
		SendMessage(&UIBtnOK, CUIButton::BUTTON_CLICKED, NULL);
//		CUIDragDropItemMP *p = GetAddonByID(dynamic_cast<CUIDragDropItemMP*>(UITopList[RIFLE_SLOT].GetDragDropItemsList().front()), CUIDragDropItemMP::ID_SILENCER);
		return true;
	}

	if (dik == g_iCancelAccelerator)
	{
		if (mlRoot == m_mlCurrLevel)
		{
			//--------------------- for E3 ---------------------
///			SendMessage(&UIBtnOK, CUIButton::BUTTON_CLICKED, NULL);
			//--------------------------------------------------
			SendMessage(&UIBtnCancel, CUIButton::BUTTON_CLICKED, NULL);
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

void WpnDrawIndex(CUIDragDropItem *pDDItem)
{
	CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(pDDItem);
	R_ASSERT(pDDItemMP);
	if (!pDDItemMP) return;

//	RECT rect = pDDItemMP->GetAbsoluteRect();
	int left	= pDDItemMP->GetUIStaticItem().GetPosX();
	int bottom	= pDDItemMP->GetUIStaticItem().GetPosY() + pDDItemMP->GetUIStaticItem().GetRect().height();

//	HUD().OutText(static_cast<const void*>(pDDItem), pDDItem->GetFont(), float(rect.left), 
//		float(rect.bottom - pDDItemMP->GetFont()->CurrentHeight()- 2),
//		"%d", pDDItemMP->GetPosInSectionsGroup() + 1);

	pDDItemMP->GetFont()->SetColor(0xffffffff);
	HUD().OutText(pDDItem->GetFont(), pDDItemMP->GetClipRect(), float(left), 
		float(bottom - pDDItemMP->GetFont()->CurrentHeight()),
		"%d", pDDItemMP->GetPosInSectionsGroup() + 1);

//	HUD().OutText(pDDItem->GetFont(), pDDItemMP->GetClipRect(), float(rect.left), 
//		float(rect.bottom - pDDItemMP->GetFont()->CurrentHeight()- 2),
//		"%d", pDDItemMP->GetPosInSectionsGroup() + 1);

	pDDItemMP->GetFont()->OnRender();
}

////////////////////////////////////////////////////////////////////////////////

//void CUIBuyWeaponWnd::MoveWeapon(const u8 grpNum, const u8 uIndexInSlot, bool bRealRepresentationSet)
//{
//	CUIDragDropItemMP *pDDItemMP = NULL;
//
//	// �������� ������
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
		CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(UITopList[OUTFIT_SLOT].GetDragDropItemsList().front());
		if (0 == xr_strcmp(pDDItemMP->GetSectionName(), "a"))
		{

		}
	}
	return static_cast<u8>(-1);
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::CheckBuyAvailabilityInShop()
{
	// ����������� �� ���� ����� � ��������� �x �� ����������� �������
	for (WEAPON_TYPES_it it = m_WeaponSubBags.begin(); it != m_WeaponSubBags.end(); ++it)	
	{
		for (DRAG_DROP_LIST_it it2 = (*it)->GetDragDropItemsList().begin(); it2 != (*it)->GetDragDropItemsList().end(); ++it2)
		{
			// � ������ ������� ���� �� UIDragDropItemMP ���� �� �����
			CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(*it2);
			R_ASSERT(pDDItemMP);

			if (pDDItemMP->GetCost() <= GetMoneyAmount())
			{
				// ���� ��� ���� ���� ��� ������ �� ����� ������ ���� �������, �� �� ���� - ��������������
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
	// ������ ����������� ������ ��� ��������
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

	// ����������� �� ����� � ������, � ������� ���� ��
	// ������� �� ��� ����. ���� ����, �� �������� ���� ����
	// �� ����� �����, � ���������� ��������
	for (int j = 0; j < arrSize; ++j)
	{
		// ���� ���� ����
		if (!UITopList[priorityArr[j]].GetDragDropItemsList().empty())
		{
			CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(UITopList[priorityArr[j]].GetDragDropItemsList().front());
			R_ASSERT(pDDItemMP);

			// � ���� �� ��� ������� �����, �� �������� �� ���� �� ���������� �����
			// ���� ��� ���� ��������� �������, �� ������ �� ��������
			if (!pDDItemMP->m_bHasRealRepresentation)
			{
				if (pDDItemMP->GetCost() <= GetMoneyAmount())
					SetMoneyAmount(GetMoneyAmount() - pDDItemMP->GetCost());
				else
				{
					pDDItemMP->SetColor(cUnableToBuy);
					status = false;

					// ��� ������ ����������� ������������� � ������ � ������������ ���������
					if (OUTFIT_SLOT == pDDItemMP->GetSlot())
						UIOutfitIcon.SetColor(cUnableToBuy);
					pDDItemMP->m_bAlreadyPaid = false;
				}
			}
			else
			{
				pDDItemMP->SetColor(cAbleToBuyOwned);

				// ��� ������ ����������� ������������� � ������ � ������������ ���������
				if (OUTFIT_SLOT == pDDItemMP->GetSlot())
					UIOutfitIcon.SetColor(cAbleToBuyOwned);
			}
		}
	}

	// � ����� ���������� ���������
	for (DRAG_DROP_LIST_it it = UITopList[BELT_SLOT].GetDragDropItemsList().begin();
		 it != UITopList[BELT_SLOT].GetDragDropItemsList().end();
		 ++it)
	{
		CUIDragDropItemMP *pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(*it);
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

	// ������ ������ ������
	tmpStr = pSettings->r_string(m_StrSectionName, *skinsParamName);

	R_ASSERT(_GetItemCount(*tmpStr) > SkinID);

	// �������� ��� �����, � ���������� ��������������� ������
	string32 a;
	tmpStr = _GetItem(*tmpStr, SkinID, a);
	sscanf(pSettings->r_string("multiplayer_skins", a), "%i,%i", &m_iIconTextureX, &m_iIconTextureY);
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::ClearSlots()
{
	// ������� �����
	IgnoreMoney(true);
	SlotToSection(KNIFE_SLOT		);
	SlotToSection(PISTOL_SLOT		);
	SlotToSection(RIFLE_SLOT		);
	SlotToSection(GRENADE_SLOT		);
	SlotToSection(APPARATUS_SLOT	);
	SlotToSection(OUTFIT_SLOT		);

	// ������� ����
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
				// ���� ���� �� ���� �� �� ����������
				if (UITopList[DDItemMP.GetSlot()].GetDragDropItemsList().empty() || GRENADE_SLOT == DDItemMP.GetSlot())
				{
					DDItemMP.m_bHasRealRepresentation = bRealRepresentationSet;
					SendMessage(&DDItemMP, CUIDragDropItem::ITEM_DB_CLICK, NULL);
				}
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIBuyWeaponWnd::SectionToSlot(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet)
{
	// �������� ������

	// "����������" ����� �������
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
				// ���� ���� �� ���� �� �� ����������
				if (UITopList[DDItemMP.GetSlot()].GetDragDropItemsList().empty() || GRENADE_SLOT == DDItemMP.GetSlot())
				{
					// ������������� ����� ������ � ���������
//					bool bPrevMoneyMode = m_bIgnoreMoney;
//					if (DDItemMP.GetCost() > GetMoneyAmount())
//						IgnoreMoney(true);
//					else
//						IgnoreMoney(false);

					DDItemMP.m_bHasRealRepresentation = bRealRepresentationSet;
					SendMessage(&DDItemMP, CUIDragDropItem::ITEM_DB_CLICK, NULL);
					// ��������� ������ �� ������� ������ �������, � ���� ��� ����, �� 
					// ������� ������ � ��������� ������
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
//		m_vDragDropItems[i].SetColor(cAbleToBuy);
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
	// ����������� �� ������ ����� � ���� ��� ������ �����
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

	// ��������� �� ����� �� pPossibleAddon ���� � ����(�/��)
	if (!UITopList[RIFLE_SLOT].GetDragDropItemsList().empty())
	{
		CUIDragDropItemMP * pDDItemMP = dynamic_cast<CUIDragDropItemMP*>(UITopList[RIFLE_SLOT].GetDragDropItemsList().front());

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

	return NULL;
}