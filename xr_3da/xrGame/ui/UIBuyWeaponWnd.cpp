// UIBuyWeaponWnd.cpp:	������, ��� ������� ������ � ������ CS
//						
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <dinput.h>
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
#include "../actor.h"
#include "../inventory.h"
#include "UITextureMaster.h"

using namespace InventoryUtilities;

int			g_iOkAccelerator, g_iCancelAccelerator;


CUIBuyWeaponWnd::CUIBuyWeaponWnd(LPCSTR strSectionName, LPCSTR strPricesSection)
		: UIBagWnd(BagProc)
{
	m_StrSectionName		= strSectionName;
	Hide();

	m_pCurrentDragDropItem = NULL;
	m_iMoneyAmount = 10000;

//	m_iIconTextureY	= 0;
//	m_iIconTextureX	= 0;

	SetFont(HUD().Font().pFontMedium);

	m_bIgnoreMoney	= false;

	// �������������� ����
	Init(strSectionName, strPricesSection);

	Hide();
}

CUIBuyWeaponWnd::~CUIBuyWeaponWnd()
{
}

int CUIBuyWeaponWnd::FindBestBuy(){
	int iMoney = GetMoneyAmount();
	iMoney += GetPriceOfOwnItems();

	for (int i = 2; i >= 0; i--)
		if (iMoney >= m_presets[i].m_price)
			return i;

	return -1;
}

void CUIBuyWeaponWnd::PerformAutoBuy()
{
	int index = FindBestBuy();
	if (-1 == index)
		return;

	OnBtnClearClicked();

	xr_vector<shared_str>& buy_list = m_presets[index].m_list;

	for (int i = 0; i < (int)buy_list.size(); i++)
	{
		CUIDragDropItemMP* pDDItem = UIBagWnd.GetItemBySectoin(*buy_list[i]);
		if(pDDItem){
			if (pDDItem->IsDragDropEnabled())
                SendMessage(pDDItem, DRAG_DROP_ITEM_DB_CLICK, NULL);
			}
		else{
			Msg("ERROR CUIBuyWeaponWnd::PerformAutoBuy cannot find item with section name=%s",*buy_list[i]);
		}
	}
}

void CUIBuyWeaponWnd::UpdatePresetPrice(Preset& preset){
	preset.m_price = 0;
	for (int i = 0; i < (int)preset.m_list.size(); i++)
	{
		CUIDragDropItemMP* pDDItem = UIBagWnd.GetItemBySectoin(*preset.m_list[i]);
		preset.m_price += GetItemPrice(pDDItem);
	}
}

void CUIBuyWeaponWnd::FillUpPresets(){
	string256 preset[3];


	std::strcpy(preset[0], pSettings->r_string(m_StrSectionName, "autobuy_preset1"));
	std::strcpy(preset[1], pSettings->r_string(m_StrSectionName, "autobuy_preset2"));
	std::strcpy(preset[2], pSettings->r_string(m_StrSectionName, "autobuy_preset3"));


	for (int i = 0; i < 3; i++)
	{
        ParseStrToVector(preset[i], m_presets[i].m_list);
//		UpdatePresetPrice(m_presets[i]);
	}
}

void CUIBuyWeaponWnd::ParseStrToVector(const char* str, xr_vector<shared_str>& vector){
	int itemsCount = _GetItemCount(str);
	char item[64];

	for (int i = 0; i< itemsCount; i++)
	{
		_GetItem(str, i, item);
		vector.push_back(item);
	}
}

void CUIBuyWeaponWnd::Init(LPCSTR strSectionName, LPCSTR strPricesSection)
{
	CUIXml uiXml;
	CStringTable string_table;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, BUY_WND_XML_NAME);
	R_ASSERT2(xml_result, "xml file not found");

	inherited::DetachAll();

	CUIXmlInit xml_init;

	CUIWindow::Init(CUIXmlInit::ApplyAlignX(0, alCenter),
					CUIXmlInit::ApplyAlignY(0, alCenter),
					UI_BASE_WIDTH, UI_BASE_WIDTH);

	AttachChild(&UIBackground);
	UIBackground.Init(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);
	UIBackground.SetShader(GetBuyMenuShader());	

	AttachChild(&UIAutobuyIndication);
	xml_init.InitWindow(uiXml, "autobuy_indication", 0, &UIAutobuyIndication);
	FillUpPresets();

	AttachChild(&UIStaticBelt);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticBelt);

	AttachChild(&UIBagWnd);
	UIBagWnd.Init(uiXml,"bag_background_static", strSectionName, strPricesSection);

	////////////////////////////////////////
	// ���� � ��������� �������� ����
	AttachChild(&UIDescWnd);
	xml_init.InitStatic(uiXml, "desc_static", 0, &UIDescWnd);

	////////////////////////////////////
	//���� � ���������� � ���������
	AttachChild(&UIPersonalWnd);
	xml_init.InitStatic(uiXml, "personal_static", 0, &UIPersonalWnd);

	// Tabcontrol ��� ������
	AttachChild(&UITabControl);
	UITabControl.Init(uiXml, "tab");

	BringToTop(&UITabControl);	

	// ������ OK � Cancel ��� ������ �� ������� ������� ������
	AttachChild(&UIBtnOK);
	xml_init.Init3tButton(uiXml, "ok_button", 0, &UIBtnOK);
	UIBtnOK.EnableTextHighlighting(false);
	g_iOkAccelerator = uiXml.ReadAttribInt("ok_button", 0, "accel");

	AttachChild(&UIBtnCancel);
	xml_init.Init3tButton(uiXml, "cancel_button", 0, &UIBtnCancel);
	UIBtnCancel.EnableTextHighlighting(false);
	g_iCancelAccelerator = uiXml.ReadAttribInt("cancel_button", 0, "accel");

	AttachChild(&UIBtnAutobuy);
	xml_init.Init3tButton(uiXml, "autobuy_button", 0, &UIBtnAutobuy);
	UIBtnAutobuy.EnableTextHighlighting(false);

	AttachChild(&UIBtnClear);
	xml_init.Init3tButton(uiXml, "clear_button", 0, &UIBtnClear);
	UIBtnClear.EnableTextHighlighting(false);

	UIBagWnd.UpdateBuyPossibility();

	//������ Drag&Drop
	AttachChild(&UITopList[BELT_SLOT]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 0, &UITopList[BELT_SLOT]);

	AttachChild(&UITopList[OUTFIT_SLOT]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 7, &UITopList[OUTFIT_SLOT]);

	AttachChild(&UITopList[0]);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 2, &UITopList[0]);
	//-----------------------------------------------------
	//����� ������ ���� ������� ���
	UITopList[0].BlockCustomPlacement();
	UITopList[0].EnableDoubleClick(false);
	UITopList[0].Enable(false);
	//-----------------------------------------------------

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
	UITopList[BELT_SLOT].SetCheckProc(BeltProc);

	//pop-up menu
	AttachChild(&UIPropertiesBox);
	UIPropertiesBox.Init("ui_pop_up",0,0,300,300);
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
//	UIOutfitIcon.SetShader(GetMPCharIconsShader());
	UIOutfitIcon.SetStretchTexture(true);
//	UIOutfitIcon.ClipperOn();

	UIDescWnd.AttachChild(&UIItemInfo);
	UIItemInfo.Init(0, 0, UIDescWnd.GetWidth(), UIDescWnd.GetHeight(), BUY_MP_ITEM_XML);
	UIDescWnd.AttachChild(&UIDescRankIcon);
	xml_init.InitStatic(uiXml, "desc_static:rank_icon",0,&UIDescRankIcon);

	UpdatePresetPrices();
	UIAutobuyIndication.SetPrices(m_presets[0].m_price, m_presets[1].m_price, m_presets[2].m_price);
}

void CUIBuyWeaponWnd::OnMouseScroll(float iDirection){

}

void CUIBuyWeaponWnd::InitInventory() 
{
	m_pMouseCapturer = NULL;
}  

//�������� �� ��������� ��������� � �����
// KNIFE SLOT
bool CUIBuyWeaponWnd::SlotProc0(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, KNIFE_SLOT)) 
		return false;

	bool last;
	if (pDDItemMP->HasAmountControl())
		if (pDDItemMP->GetPermissionToBuy(last)){
			if (!last)
				this_inventory->UIBagWnd.CreateCopy(pDDItemMP);
		}
		else
			return false;
	this_inventory->SlotToSection(KNIFE_SLOT);

	// � �������� �� ����� ��������� ����.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - GetItemPrice(pDDItemMP)); 
		pDDItemMP->m_bAlreadyPaid = true;
	}
	return true;
}

// PISTOL SLOT
bool CUIBuyWeaponWnd::SlotProc1(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, PISTOL_SLOT)) return false;

	bool last;
	if (pDDItemMP->HasAmountControl())
		if (pDDItemMP->GetPermissionToBuy(last)){
			if (!last)
				this_inventory->UIBagWnd.CreateCopy(pDDItemMP);
		}
		else
			return false;
	this_inventory->SlotToSection(PISTOL_SLOT);

	// � �������� �� ����� ��������� ����.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - GetItemPrice(pDDItemMP)); 
		pDDItemMP->m_bAlreadyPaid = true;
	}
	return true;
}

// RIFLE_SLOT
bool CUIBuyWeaponWnd::SlotProc2(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP::AddonIDs	addonID;
	CUIDragDropItemMP *pDDItemMP	= smart_cast<CUIDragDropItemMP*>(pItem), 
					  *pAddonOwner	= this_inventory->IsItemAnAddon(pDDItemMP, addonID);

	R_ASSERT(pDDItemMP);

	// ���� �����
	if (pAddonOwner)	
	{
		pAddonOwner->AttachDetachAddon(pDDItemMP, !pAddonOwner->IsAddonAttached(addonID), pDDItemMP->m_bHasRealRepresentation);
		return false;
	}
	// �� �����
	if(!this_inventory->CanPutInSlot(pDDItemMP, RIFLE_SLOT)) 
		return false;

	bool last;
	if (pDDItemMP->HasAmountControl())
		if (pDDItemMP->GetPermissionToBuy(last)){
			if (!last)
				this_inventory->UIBagWnd.CreateCopy(pDDItemMP);
		}
		else
			return false;
	this_inventory->SlotToSection(RIFLE_SLOT);

	// � �������� �� ����� ��������� ����.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - GetItemPrice(pDDItemMP)); 
		pDDItemMP->m_bAlreadyPaid = true;
	}

	return true;
}


// GRENADE_SLOT
bool CUIBuyWeaponWnd::SlotProc3(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	// ���� ���� �� �������
	// � ���� � ����� ��� ���� 1 �������
	if (!this_inventory->CanPutInSlot(pDDItemMP, GRENADE_SLOT) ||
		!pList->GetDragDropItemsList().empty()) 
		return false;

	bool last;
	if (pDDItemMP->HasAmountControl())
		if (pDDItemMP->GetPermissionToBuy(last)){
			if (!last)
				this_inventory->UIBagWnd.CreateCopy(pDDItemMP);
		}
		else
			return false;

	// � �������� �� ����� ��������� ����.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - GetItemPrice(pDDItemMP)); 
		pDDItemMP->m_bAlreadyPaid = true;
	}

	return true;
}

//APPARATUS_SLOT
bool CUIBuyWeaponWnd::SlotProc4(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);

	if(!this_inventory->CanPutInSlot(pDDItemMP, APPARATUS_SLOT)) 
		return false;

	bool last;
	if (pDDItemMP->HasAmountControl())
		if (pDDItemMP->GetPermissionToBuy(last)){
			if (!last)
				this_inventory->UIBagWnd.CreateCopy(pDDItemMP);
		}
		else
			return false;

	this_inventory->SlotToSection(APPARATUS_SLOT);

	// � �������� �� ����� ��������� ����.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - GetItemPrice(pDDItemMP)); 
		pDDItemMP->m_bAlreadyPaid = true;
	}

	return true;
}

//����� ������
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

		// Te���� ���������� ��������������� ������ ���������, � ����������� �� ����� �������, 
		// � ���������� �������

		// ������� �������� ������������������ �� ��������� ������
//		if (pDDItemMP->m_fAdditionalInfo.size() < 2)
//			R_ASSERT(!"Unknown suit");

		R_ASSERT(!pDDItemMP->m_additionalInfo.empty());

		this_inventory->UIOutfitIcon.InitTexture(pDDItemMP->m_additionalInfo.c_str());

//		xr_vector<float>::iterator it = pDDItemMP->m_fAdditionalInfo.begin();
/*		this_inventory->UIOutfitIcon.GetUIStaticItem().SetOriginalRect(
			(*it), 
			(*(it+1)),
			float(SKIN_TEX_WIDTH), float(SKIN_TEX_HEIGHT - 15));*/
		this_inventory->UIOutfitIcon.Show(true);
		this_inventory->UIOutfitIcon.SetColor(pDDItemMP->GetColor());

		// � �������� �� ����� ��������� ����.
		if (!pDDItemMP->m_bAlreadyPaid)
		{
			this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - GetItemPrice(pDDItemMP)); 
			pDDItemMP->m_bAlreadyPaid = true;
		}
		return true;
	}	
	return false;
}

//�� ����
bool CUIBuyWeaponWnd::BeltProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetParent());
	R_ASSERT2(this_inventory, "wrong parent addressed as inventory wnd");

	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	// � ��� �� ����� ���� ������� ���� � ���� �������.
	R_ASSERT(pDDItemMP);

	CUIDragDropItemMP::AddonIDs tmp;

	// ���� ���� - �����, �� ������ �� �� ����
	if (!this_inventory->CanPutInBelt(pDDItemMP)		||
		this_inventory->IsItemAnAddon(pDDItemMP, tmp)) 
		return false;

	bool last;
	if (pDDItemMP->HasAmountControl())
		if (pDDItemMP->GetPermissionToBuy(last))
		{
			if (!last)
				this_inventory->UIBagWnd.CreateCopy(pDDItemMP);
		}
		else
			return false;
	
	// � �������� �� ����� ��������� ����.
	if (!pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - GetItemPrice(pDDItemMP)); 
		pDDItemMP->m_bAlreadyPaid = true;
	}

	return true;
}

void CUIBuyWeaponWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	TABS_VECTOR_it	it;

	switch (msg)
	{
	case TAB_CHANGED:
        OnTabChange();	break;

	case XR_MENU_LEVEL_CHANGED:
		OnMenuLevelChange(); break;

	case DRAG_DROP_ITEM_DB_CLICK:
		SetCurrentDDItem(pWnd);
		OnDDItemDbClick(); break;

	case BUTTON_CLICKED:
		if (&UIBtnOK == pWnd)
			OnBtnOkClicked();
		else if (&UIBtnCancel == pWnd)
			OnBtnCancelClicked();
		else if (&UIBtnClear == pWnd)
			OnBtnClearClicked();
		else if (&UIBtnAutobuy == pWnd)
			OnBtnAutobuyClicked();
		break;

	case DRAG_DROP_ITEM_DRAG:
		SetCurrentDDItem(pWnd);
		OnDDItemDrag(); break;

	case DRAG_DROP_REFRESH_ACTIVE_ITEM:
		if (m_pCurrentDragDropItem) 
			m_pCurrentDragDropItem->Highlight(true); break;

	case STATIC_FOCUS_RECEIVED:
		if (&UIBtnAutobuy == pWnd)
            OnBtnAutobuyFocusReceive(); break;

	case STATIC_FOCUS_LOST:
		if (&UIBtnAutobuy == pWnd)
            OnBtnAutobuyFocusLost(); break;
	}

	//�� ������� ������ ������
	if(msg == DRAG_DROP_ITEM_RBUTTON_CLICK)
	{
		SetCurrentDDItem(pWnd);
		ActivatePropertiesBox();
	}
	//��������� �� ���� ����������� ������ �������
	else if(pWnd == &UIPropertiesBox &&	msg == PROPERTY_CLICKED)
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
				SetCurrentDDItem(UITopList[RIFLE_SLOT].GetDragDropItemsList().front());
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case ATTACH_SILENCER_ADDON:
//				bool bPistolAvailable = UITopList[PISTOL_SLOT].GetChildWndList().size() ? true : false;
//				bool bRiffleAvailable = UITopList[RIFLE_SLOT].GetChildWndList().size() ? true : false;
				SetCurrentDDItem(UITopList[RIFLE_SLOT].GetDragDropItemsList().front());
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case ATTACH_SILENCER_ADDON_PISTOL:
				SetCurrentDDItem(UITopList[PISTOL_SLOT].GetDragDropItemsList().front());
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case ATTACH_GRENADE_LAUNCHER_ADDON:
				SetCurrentDDItem(UITopList[RIFLE_SLOT].GetDragDropItemsList().front());
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case DETACH_SCOPE_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, false, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case DETACH_SILENCER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, false, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case DETACH_SILENCER_ADDON_PISTOL:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, false, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			case DETACH_GRENADE_LAUNCHER_ADDON:
				m_pCurrentDragDropItem->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, false, m_pCurrentDragDropItem->m_bHasRealRepresentation);
				break;
			}
		}
	}


	// ������ �� � ������
	// ���� ��������� �������� � ����, �� �������� ���
	if (OUTFIT_RETURNED_BACK == msg && pWnd->GetParent() == &UITopList[OUTFIT_SLOT])
	{
		CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pWnd);
		if (pDDItemMP && OUTFIT_SLOT == pDDItemMP->GetSlot())
		{
			pDDItemMP->Show(false);
		}
	}
	
	if (WINDOW_LBUTTON_DOWN == msg && &UIOutfitIcon == pWnd)
		if (UITopList[OUTFIT_SLOT].GetChildWndList().back())
			UITopList[OUTFIT_SLOT].GetChildWndList().back()->Show(true);
	
	//else if (WINDOW_RBUTTON_DOWN == msg && &UIOutfitIcon == pWnd)
	//{
	//	if (UITopList[OUTFIT_SLOT].GetChildWndList().back())
	//	{
	//		UITopList[OUTFIT_SLOT].GetChildWndList().back()->Show(true);
	//		SendMessage(UITopList[OUTFIT_SLOT].GetChildWndList().back(), DRAG_DROP_ITEM_RBUTTON_CLICK, NULL);
	//	}
	//}

	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUIBuyWeaponWnd::OnTabChange(){
	UIBagWnd.ShowSection(UITabControl.GetActiveIndex());
}

void CUIBuyWeaponWnd::OnBtnOkClicked(){
	if (!CanBuyAllItems())
        return;

	Game().StartStopMenu(this,true);
	game_cl_Deathmatch * dm = smart_cast<game_cl_Deathmatch *>(&(Game()));
	dm->OnBuyMenu_Ok();
}

void CUIBuyWeaponWnd::OnBtnCancelClicked(){
	Game().StartStopMenu(this,true);
}

void CUIBuyWeaponWnd::OnBtnAutobuyClicked(){
	PerformAutoBuy();
}

void CUIBuyWeaponWnd::OnBtnAutobuyFocusReceive(){
	UIAutobuyIndication.HighlightItem(FindBestBuy());
}

void CUIBuyWeaponWnd::OnBtnAutobuyFocusLost(){
	UIAutobuyIndication.UnHighlight();
}

void CUIBuyWeaponWnd::OnBtnClearClicked(){
	if (ClearTooExpensiveItems())
		return;

	SlotToSection(PISTOL_SLOT);
	SlotToSection(RIFLE_SLOT);
	SlotToSection(GRENADE_SLOT);
	SlotToSection(APPARATUS_SLOT);
	SlotToSection(OUTFIT_SLOT);

	DRAG_DROP_LIST list = UITopList[BELT_SLOT].GetDragDropItemsList();
	DRAG_DROP_LIST_it it;

	for (it = list.begin(); it != list.end(); ++it)
		BeltToSection(static_cast<CUIDragDropItemMP*>(*it));

	// put free pistol
//	GetTop()->SendMessage(UIBagWnd.GetItemByKey(DIK_1, GROUP_2), DRAG_DROP_ITEM_DB_CLICK);
	game_cl_Deathmatch * dm = smart_cast<game_cl_Deathmatch *>(&(Game()));
	if (dm) dm->OnBuyMenu_DefaultItems();
}

bool CUIBuyWeaponWnd::ClearTooExpensiveItems(){
	bool f = false;
	bool res;

    res = ClearSlot_ifTooExpensive(PISTOL_SLOT);
	f = f ? true : res;
	res = ClearSlot_ifTooExpensive(RIFLE_SLOT);
	f = f ? true : res;
	res = ClearSlot_ifTooExpensive(GRENADE_SLOT);
	f = f ? true : res;
	res = ClearSlot_ifTooExpensive(APPARATUS_SLOT);
	f = f ? true : res;
	res = ClearSlot_ifTooExpensive(OUTFIT_SLOT);
	f = f ? true : res;

	DRAG_DROP_LIST list = UITopList[BELT_SLOT].GetDragDropItemsList();
	DRAG_DROP_LIST_it it;

	for (it = list.begin(); it != list.end(); ++it)
		if ((*it)->GetColor() == cUnableToBuy)
		{
			GetTop()->SendMessage(*it, DRAG_DROP_ITEM_DB_CLICK);
			f = true;
		}

	return f;
}

bool CUIBuyWeaponWnd::ClearSlot_ifTooExpensive(int slot){
	bool empty;
	DRAG_DROP_LIST_it it;

	empty = UITopList[slot].GetDragDropItemsList().empty();

	if (!empty)
	{
		it = UITopList[slot].GetDragDropItemsList().begin();
		if ((*it)->GetColor() == cUnableToBuy)
		{
			GetTop()->SendMessage(*it, DRAG_DROP_ITEM_DB_CLICK);
			return true;
		}
	}

	return false;
}

void CUIBuyWeaponWnd::OnDDItemDrag(){
	if (m_pCurrentDragDropItem->IsDragDropEnabled())
		m_pCurrentDragDropItem->Rescale(1.0f,1.0f);

	// Disable highliht in all DD lists
	for (int i = 0; i < PDA_SLOT; ++i)
		UITopList[i].HighlightAllCells(false);
}

void CUIBuyWeaponWnd::SetCurrentDDItem(CUIWindow* pWnd){
	if (m_pCurrentDragDropItem) 
		m_pCurrentDragDropItem->Highlight(false);

	m_pCurrentDragDropItem = smart_cast<CUIDragDropItemMP*>(pWnd);

#ifdef DEBUG
	R_ASSERT2(m_pCurrentDragDropItem, "CUIBuyWeaponWnd::SetCurrentDDItem - invalid item");
#endif
}

void CUIBuyWeaponWnd::OnDDItemDbClick(){
	if (m_pCurrentDragDropItem->IsDragDropEnabled())
	{
		SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DRAG, NULL);

		if(!ToSlot())
			if(!ToBelt())
				if(!ToBag())
				//���� ������, �� ������ ����������� ������� � ����� ������
				{
					CUIWindow* pWindowParent = m_pCurrentDragDropItem->GetParent();
					pWindowParent->DetachChild(m_pCurrentDragDropItem);
					pWindowParent->AttachChild(m_pCurrentDragDropItem);
					m_pCurrentDragDropItem->Rescale(	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->GetItemsScaleX(),
														((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->GetItemsScaleY()	);
				}
	}
}

void CUIBuyWeaponWnd::OnMenuLevelChange(){
	if (UIBagWnd.GetMenuLevel() == mlRoot)
		UITabControl.SetActiveState();
}

bool CUIBuyWeaponWnd::OnMouse(float x, float y, EUIMessages mouse_action)
{
	if (WINDOW_LBUTTON_DOWN == mouse_action || WINDOW_RBUTTON_DOWN == mouse_action)
	{
		Frect		r = UIOutfitIcon.GetAbsoluteRect();

		if ( r.in(x,y))
		{
			SendMessage(&UIOutfitIcon, static_cast<s16>(mouse_action), NULL);
		}
	}

	//����� ��������������� ���� �� ������ ������
	if(mouse_action == WINDOW_RBUTTON_DOWN)
	{
		if(UIPropertiesBox.IsShown())
		{
			UIPropertiesBox.Hide();
		}
	}

	return inherited::OnMouse(x, y, mouse_action);
}

void CUIBuyWeaponWnd::Draw()
{
	inherited::Draw();
}


void CUIBuyWeaponWnd::Update()
{
	// Update money amount for Bag
	UIBagWnd.UpdateBuyPossibility();
	UIBagWnd.UpdateMoney(m_iMoneyAmount);

	// ���� �������� ���� ����������, �� ��������� ���������� � �� ���������
	static CUIDragDropItemMP *pOldCurrentDragDropItem = reinterpret_cast<CUIDragDropItemMP*>(1);
	static string16		str;
	//m_pCurrentDragDropItem = UIBagWnd.GetCurrentItem();

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

	// ���� ������ ���������� �� �������� � ��
	static int oldMoneyAmount	= 0xffffffff;

	if (oldMoneyAmount != m_iMoneyAmount)
	{
		oldMoneyAmount = m_iMoneyAmount;
		sprintf(str, "%i", m_iMoneyAmount);
		UITotalMoney.SetText(str);
	}

	// Ec�� � ����� � �������� ����� ������������, �� �������� ���.
	static bool flag = true;
	if (flag && UITopList[OUTFIT_SLOT].GetDragDropItemsList().empty())
	{
		UpdateOutfit();
		flag = false;
	}

	if (!UITopList[OUTFIT_SLOT].GetDragDropItemsList().empty())
		flag = true;

	CUIWindow::Update();
}

void CUIBuyWeaponWnd::UpdateOutfit(){
	UIOutfitIcon.InitTexture(m_current_skin.c_str());
	UIOutfitIcon.RescaleRelative2Rect(CUITextureMaster::GetTextureRect(m_current_skin.c_str()));
}

void CUIBuyWeaponWnd::DropItem()
{
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	m_pCurrentItem->Drop();

	(smart_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
		DetachChild(m_pCurrentDragDropItem);
	m_pCurrentDragDropItem = NULL;
}

void CUIBuyWeaponWnd::Show() 
{ 
	m_pMouseCapturer = NULL;
	inherited::Show();

	if (GameID() != GAME_SINGLE)
	{
		UpdateOutfit();
		CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor) return;
//		pActor->HideCurrentWeapon(GEG_PLAYER_BUYMENU_OPEN);//, false);		
		pActor->SetWeaponHideState(whs_BUY_MENU, TRUE);
	}
	UITabControl.SetActiveState();
	UpdatePresetPrices();
	UIAutobuyIndication.SetPrices(m_presets[0].m_price, m_presets[1].m_price, m_presets[2].m_price);
}

void CUIBuyWeaponWnd::UpdatePresetPrices(){
	for (int i = 0; i < 3; i++)
		UpdatePresetPrice(m_presets[i]);
}

void CUIBuyWeaponWnd::Hide()
{
	inherited::Hide();

	if (GameID() != GAME_SINGLE)
	{
		CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor) return;

//		pActor->RestoreHidedWeapon(GEG_PLAYER_BUYMENU_CLOSE);
		pActor->SetWeaponHideState(whs_BUY_MENU, FALSE);
	}
}

bool CUIBuyWeaponWnd::CanAttachAddOn(CUIDragDropItemMP* pAddonOwner, CUIDragDropItemMP* pAddOn){
	for (int j = 0; j < CUIDragDropItemMP::NUM_OF_ADDONS; ++j)
		if (pAddonOwner->m_AddonInfo[j].strAddonName == pAddOn->GetSectionName())
			return true;

	return false;
}


// ����������� ����
void CUIBuyWeaponWnd::ActivatePropertiesBox()
{
	float x,y;
	Frect rect = GetAbsoluteRect();
	GetUICursor()->GetPos(x,y);

	UIPropertiesBox.RemoveAll();

	// ���� ������� ����
	if((m_pCurrentDragDropItem->GetSlot() < MP_SLOTS_NUM				||
		static_cast<u32>(-1) == m_pCurrentDragDropItem->GetSlot())		&& 
		&UIBagWnd == m_pCurrentDragDropItem->GetParent()->GetParent())
	{
		// ���� ���� �� �����, �� �������� ����, � ���� �����, �� ������
		CUIDragDropItemMP::AddonIDs		ID;
		CUIDragDropItemMP				*pDDItemMP = IsItemAnAddon(m_pCurrentDragDropItem, ID);

		if (m_pCurrentDragDropItem->GetCost() > GetMoneyAmount())
		{
			return;
		}
		
		if (pDDItemMP)
		{
			
			bool bPistolAvailable = UITopList[PISTOL_SLOT].GetDragDropItemsList().size() ? true : false;
			bool bRiffleAvailable = UITopList[RIFLE_SLOT].GetDragDropItemsList().size() ? true : false;
			


			//m_pCurrentDragDropItem = pDDItemMP;			
			if (bRiffleAvailable)
				if (CanAttachAddOn((CUIDragDropItemMP*)UITopList[RIFLE_SLOT].GetDragDropItemsList().front(),m_pCurrentDragDropItem))
				{
						switch(ID){
						case CUIDragDropItemMP::ID_SILENCER:
							UIPropertiesBox.AddItem("Attach add-on to riffle", NULL, pDDItemMP->IsAddonAttached(ID) ? DETACH_SILENCER_ADDON /*+ static_cast<int>(ID)*/ : ATTACH_SILENCER_ADDON /*+ static_cast<int>(ID)*/);
							break;
						case CUIDragDropItemMP::ID_SCOPE:
							UIPropertiesBox.AddItem("Attach add-on to riffle", NULL, pDDItemMP->IsAddonAttached(ID) ? DETACH_SCOPE_ADDON /*+ static_cast<int>(ID)*/ : ATTACH_SCOPE_ADDON /*+ static_cast<int>(ID)*/);
							break;
						case CUIDragDropItemMP::ID_GRENADE_LAUNCHER:
							UIPropertiesBox.AddItem("Attach add-on to riffle", NULL, pDDItemMP->IsAddonAttached(ID) ? DETACH_GRENADE_LAUNCHER_ADDON /*+ static_cast<int>(ID)*/ : ATTACH_GRENADE_LAUNCHER_ADDON /*+ static_cast<int>(ID)*/);
                    
					}
				}
		
			if (bPistolAvailable)
				if (CanAttachAddOn((CUIDragDropItemMP*)UITopList[PISTOL_SLOT].GetDragDropItemsList().front(),m_pCurrentDragDropItem))
					UIPropertiesBox.AddItem("Attach add-on to pistol", 
						NULL,
						pDDItemMP->IsAddonAttached(ID) ? DETACH_SILENCER_ADDON_PISTOL + static_cast<int>(ID) : ATTACH_SILENCER_ADDON_PISTOL + static_cast<int>(ID));
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
			xr_string strMenuItem;
			CUIDragDropItemMP* pItem;
			for (int i = 0; i < 3; ++i)
			{
				switch (m_pCurrentDragDropItem->m_AddonInfo[i].iAttachStatus)
				{
					// If addon detached
				case 0:
					// ���� ����� �� ����� �������
					pItem = GetAddonByID(m_pCurrentDragDropItem, static_cast<CUIDragDropItemMP::AddonIDs>(i));
					if (pItem->GetCost() <= GetMoneyAmount() && pItem->IsDragDropEnabled())
					{
						if (!this->UIBagWnd.IsItemInBag(pItem))
							break;
						strMenuItem = xr_string("Attach ") + m_pCurrentDragDropItem->m_strAddonTypeNames[i];
						
						switch(static_cast<CUIDragDropItemMP::AddonIDs>(i)){
						case CUIDragDropItemMP::ID_SILENCER:
							if (UITopList[RIFLE_SLOT].GetDragDropItemsList().front() == m_pCurrentDragDropItem)
								UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, ATTACH_SILENCER_ADDON);
							else
								UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, ATTACH_SILENCER_ADDON_PISTOL);
							break;
						case CUIDragDropItemMP::ID_SCOPE:
								UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, ATTACH_SCOPE_ADDON);
							break;
						case CUIDragDropItemMP::ID_GRENADE_LAUNCHER:
								UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, ATTACH_GRENADE_LAUNCHER_ADDON);
							break;
						}
					}
					break;
				case 1:
					strMenuItem = xr_string("Detach ") + m_pCurrentDragDropItem->m_strAddonTypeNames[i];
					switch(static_cast<CUIDragDropItemMP::AddonIDs>(i)){
					case CUIDragDropItemMP::ID_SILENCER:
						if (UITopList[RIFLE_SLOT].GetDragDropItemsList().front() == m_pCurrentDragDropItem)
							UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, DETACH_SILENCER_ADDON + i);
						else
							UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, DETACH_SILENCER_ADDON_PISTOL + i);
						break;
					case CUIDragDropItemMP::ID_SCOPE:
						UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, DETACH_SCOPE_ADDON);
						break;
					case CUIDragDropItemMP::ID_GRENADE_LAUNCHER:
						UIPropertiesBox.AddItem(strMenuItem.c_str(), NULL, DETACH_GRENADE_LAUNCHER_ADDON);
						break;
					}

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
	if ( !UIBagWnd.IsItemInBag(m_pCurrentDragDropItem) )
		return false;


	// ����������� ��������� ��� ������
	if (GRENADE_SLOT == m_pCurrentDragDropItem->GetSlot() && 
		!UITopList[GRENADE_SLOT].GetDragDropItemsList().empty()) return false;

	// ���� ���� �����
	CUIDragDropItemMP::AddonIDs ID;
	CUIDragDropItemMP * pDDItemMP = IsItemAnAddon(m_pCurrentDragDropItem, ID);
	if (pDDItemMP && !UIBagWnd.IsItemInBag(pDDItemMP))
        pDDItemMP->AttachDetachAddon(ID, true, m_pCurrentDragDropItem->m_bHasRealRepresentation);
	

	// ���� ���� ��� ������ �����, �� �� ��������� ������ ������ (�������� �������)
	int itemSlot = m_pCurrentDragDropItem->GetSlot();
	if (itemSlot < MP_SLOTS_NUM && itemSlot >= 0)
	{
		SlotToSection(itemSlot);

		m_pCurrentDragDropItem->MoveOnNextDrop();
//		m_pCurrentDragDropItem->GetParent()->SendMessage(
		UITopList[itemSlot].SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DROP, NULL);

		BuyReaction();

		m_pMouseCapturer = NULL;
		return true;
	}
	return false;
}

void CUIBuyWeaponWnd::BuyReaction(){
	int iActiveIndex = UITabControl.GetActiveIndex();

	if (2 == iActiveIndex || 4 == iActiveIndex)
	{
		; // do nothing
	}
	else
		UITabControl.SetActiveState();
}

bool CUIBuyWeaponWnd::ToBag()
{
	// if in bag
	if (UIBagWnd.IsItemInBag(m_pCurrentDragDropItem))
		return false;

	// if in belt
	if (BeltToSection(m_pCurrentDragDropItem)) 
		return true;

	// if in slot
	if (SlotToSection(m_pCurrentDragDropItem->GetSlot())) 
		return true;

	return false;
}

bool CUIBuyWeaponWnd::ToBelt()
{
	// cheking
	if (!CanPutInBelt(m_pCurrentDragDropItem)							||
		&UITopList[BELT_SLOT] == m_pCurrentDragDropItem->GetParent()	||
		!UITopList[BELT_SLOT].CanPlaceItem(m_pCurrentDragDropItem)		||
		IsItemAnAddonSimple(m_pCurrentDragDropItem)						||
		!UIBagWnd.IsItemInBag(m_pCurrentDragDropItem)
		)
		return false;

	m_pCurrentDragDropItem->MoveOnNextDrop();
	m_pCurrentDragDropItem->AttachDetachAllAddons(false);
	UITopList[BELT_SLOT].SendMessage(m_pCurrentDragDropItem, DRAG_DROP_ITEM_DROP, NULL);

	m_pMouseCapturer = NULL;

	return true;
}

bool CUIBuyWeaponWnd::SlotToSection(const u32 SlotNum)
{
	if (SlotNum >= MP_SLOTS_NUM) return false;
	// ���������� ���� �� ����� ���� ����.
	if (!UITopList[SlotNum].GetDragDropItemsList().empty())
	{
		CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*UITopList[SlotNum].GetDragDropItemsList().begin());
		pDDItemMP->MoveOnNextDrop();
		pDDItemMP->AttachDetachAllAddons(false);
		UIBagWnd.SendMessage(pDDItemMP, DRAG_DROP_ITEM_DROP, NULL);
	}

	return true;
}

bool CUIBuyWeaponWnd::CanPutInSlot(CUIDragDropItemMP *pDDItemMP, const u32 slotNum)
{
	return slotNum == pDDItemMP->GetSlot();
}

const u8 CUIBuyWeaponWnd::GetItemIndex(u32 slotNum, u32 idx, u8 &sectionNum)
{
	CUIDragDropItemMP *pDDItemMP = GetWeapon(slotNum, idx);

	return UIBagWnd.GetItemIndex(pDDItemMP, sectionNum);	
}

const u8 CUIBuyWeaponWnd::GetWeaponIndex(u32 slotNum)
{
	u8 tmp;
	return GetItemIndex(slotNum, 0, tmp);
}

const u8 CUIBuyWeaponWnd::GetWeaponIndexInBelt(u32 indexInBelt, u8 &sectionId, u8 &itemId)
{
	itemId = GetItemIndex(BELT_SLOT, indexInBelt, sectionId);
	return itemId;
}

void CUIBuyWeaponWnd::GetWeaponIndexByName(const xr_string sectionName, u8 &grpNum, u8 &idx)
{
	UIBagWnd.GetWeaponIndexByName(sectionName, grpNum, idx);
}

char*	CUIBuyWeaponWnd::GetWeaponNameByIndex(u8 grpNum, u8 idx)
{
	return UIBagWnd.GetWeaponNameByIndex(grpNum, idx);
}

CUIDragDropItemMP * CUIBuyWeaponWnd::GetWeapon(u32 slotNum, u32 idx)
{
	R_ASSERT(slotNum < MP_SLOTS_NUM);

	if (UITopList[slotNum].GetDragDropItemsList().empty()) 
		return NULL;

	R_ASSERT(idx < UITopList[slotNum].GetDragDropItemsList().size());

	DRAG_DROP_LIST_it it = UITopList[slotNum].GetDragDropItemsList().begin(); 
	std::advance(it, idx);
	return smart_cast<CUIDragDropItemMP*>(*it);
}

const u8 CUIBuyWeaponWnd::GetWeaponAddonInfoByIndex(u8 idx)
{
	return (idx & 0xe0) >> 5;
}

bool CUIBuyWeaponWnd::CanPutInBelt(CUIDragDropItemMP *pDDItemMP)
{
	return pDDItemMP->GetSlot() == GRENADE_SLOT || pDDItemMP->GetSlot() > 7;
}

bool CUIBuyWeaponWnd::BeltToSection(CUIDragDropItemMP *pDDItemMP)
{
	R_ASSERT(pDDItemMP);

	if (&UITopList[BELT_SLOT] != pDDItemMP->GetParent()) return false;
	// ����� ����
	pDDItemMP->MoveOnNextDrop();
	// ...� �������� ��� ��������� ������������� � �����
	UIBagWnd.SendMessage(pDDItemMP, DRAG_DROP_ITEM_DROP, NULL);

	return true;
}

const u8 CUIBuyWeaponWnd::GetBeltSize()
{
	return static_cast<u8>(UITopList[BELT_SLOT].GetDragDropItemsList().size());
}

bool CUIBuyWeaponWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if (WINDOW_KEY_RELEASED == keyboard_action) 
		return true;

	if (UIPropertiesBox.GetVisible())
		UIPropertiesBox.OnKeyboard(dik, keyboard_action);

	switch (UIBagWnd.GetMenuLevel())
	{
	case mlRoot:
		if (UITabControl.OnKeyboard(dik, keyboard_action))
			return true;
		break;
	default:
		if (UIBagWnd.OnKeyboard(dik, keyboard_action))
			return true;
	}

	if (DIK_ESCAPE == dik)
		UIBtnCancel.OnClick();

	else if (g_iOkAccelerator == dik || DIK_SPACE == dik)
		UIBtnOK.OnClick();

	else if (DIK_B == dik)
		UITabControl.SetActiveState();

	else if (DIK_C == dik)
		UIBtnClear.OnClick();
	
	else if (DIK_A == dik)
		UIBtnAutobuy.OnClick();

	return true;
}

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
			DRAG_DROP_LIST &pList = UITopList[priorityArr[j]].GetDragDropItemsList();

			for (DRAG_DROP_LIST_it it = pList.begin(); it != pList.end(); ++it)
			{
				CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*it);
				R_ASSERT(pDDItemMP);

				// � ���� �� ��� ������� �����, �� �������� �� ���� �� ���������� �����
				// ���� ��� ���� ��������� �������, �� ������ �� ��������
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
	}

	// � ����� ���������� ���������
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

void CUIBuyWeaponWnd::SetSkin(u8 SkinID)
{
	LPCSTR skins = pSettings->r_string(m_StrSectionName, "skins");

	R_ASSERT(_GetItemCount(skins) > SkinID);

	// �������� ��� �����, � ���������� ��������������� ������
	string64 item;
	_GetItem(skins, SkinID, item);
//	Frect r = CUITextureMaster::GetTextureRect(item);
	m_current_skin = item;
//	m_iIconTextureX = r.x1;
//	m_iIconTextureY = r.x2;
}

void CUIBuyWeaponWnd::ClearSlots()
{
	IgnoreMoney(true);
	SlotToSection(KNIFE_SLOT);
	SlotToSection(PISTOL_SLOT);
	SlotToSection(RIFLE_SLOT);
	SlotToSection(GRENADE_SLOT);
	SlotToSection(APPARATUS_SLOT);
	SlotToSection(OUTFIT_SLOT);

	DRAG_DROP_LIST list = UITopList[BELT_SLOT].GetDragDropItemsList();
	DRAG_DROP_LIST_it it;

	for (it = list.begin(); it != list.end(); ++it)
		BeltToSection(static_cast<CUIDragDropItemMP*>(*it));

	IgnoreMoney(false);
}

int CUIBuyWeaponWnd::GetPriceOfOwnItems(){
	int price = 0;

	price += GetPriceOfItemInSlot(KNIFE_SLOT);
	price += GetPriceOfItemInSlot(PISTOL_SLOT);
	price += GetPriceOfItemInSlot(RIFLE_SLOT);
	price += GetPriceOfItemInSlot(GRENADE_SLOT);	
	price += GetPriceOfItemInSlot(APPARATUS_SLOT);
	price += GetPriceOfItemInSlot(OUTFIT_SLOT);

	DRAG_DROP_LIST list = UITopList[BELT_SLOT].GetDragDropItemsList();
	DRAG_DROP_LIST_it it;

	for (it = list.begin(); it != list.end(); ++it)
		price += GetItemPrice((CUIDragDropItemMP*)*it);

	return price;
}

int CUIBuyWeaponWnd::GetPriceOfItemInSlot(int slot){
	if (slot >= MP_SLOTS_NUM) 
		return 0;


	if (!UITopList[slot].GetDragDropItemsList().empty())
	{
		CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(*UITopList[slot].GetDragDropItemsList().begin());
		if (!pDDItemMP->m_bAlreadyPaid)
			return 0;
        return GetItemPrice(pDDItemMP);	
	}

	return 0;

}

void CUIBuyWeaponWnd::SectionToSlot(const char *sectionName, bool bRealRepresentationSet)
{
	CUIDragDropItemMP* pDDItem = UIBagWnd.GetItemBySectoin(sectionName);	
	int iSlot = pDDItem->GetSlot();
	if (UIBagWnd.IsItemInBag(pDDItem))
		if (UITopList[iSlot].GetDragDropItemsList().empty() || GRENADE_SLOT == iSlot)
		{
			pDDItem->m_bHasRealRepresentation = bRealRepresentationSet;
			SendMessage(pDDItem, DRAG_DROP_ITEM_DB_CLICK, NULL);
		}
}

void CUIBuyWeaponWnd::SectionToSlot(const char* sectionName, u8 addon_info, bool bRealRepresentationSet){
	CUIDragDropItemMP* pDDItem = UIBagWnd.GetItemBySectoin(sectionName, /* bCreateOnFail = */true);
	int iSlot = pDDItem->GetSlot();
	if (UIBagWnd.IsItemInBag(pDDItem))
		if (UITopList[iSlot].GetDragDropItemsList().empty() || GRENADE_SLOT == iSlot)
		{
			pDDItem->m_bHasRealRepresentation = bRealRepresentationSet;
			SendMessage(pDDItem, DRAG_DROP_ITEM_DB_CLICK, NULL);
			if (addon_info != 0)
			{
				if ((addon_info & 1) != 0)
					pDDItem->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, true, bRealRepresentationSet);
				if ((addon_info & 2) != 0)
					pDDItem->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, true, bRealRepresentationSet);
				if ((addon_info & 4) != 0)
					pDDItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, true, bRealRepresentationSet);
			}
		}
}

void CUIBuyWeaponWnd::SectionToSlot(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet)
{
	// "����������" ����� �������
	u8 uAddonFlags = GetWeaponAddonInfoByIndex(uIndexInSlot);
	uIndexInSlot &= 0x1f; // 0x1f = 00011111;

	CUIDragDropItemMP* pDDItem = UIBagWnd.GetItemBySectoin(grpNum, uIndexInSlot);

	if (UIBagWnd.IsItemInBag(pDDItem))
//		if (UITopList[pDDItem->GetSlot()].GetDragDropItemsList().empty() || GRENADE_SLOT == pDDItem->GetSlot() || NO_ACTIVE_SLOT == pDDItem->GetSlot() || GRENADE_SLOT == pDDItem->GetSlot())
		{
			pDDItem->m_bHasRealRepresentation = bRealRepresentationSet;
			SendMessage(pDDItem, DRAG_DROP_ITEM_DB_CLICK, NULL);
			// ��������� ������ �� ������� ������ �������, � ���� ��� ����, �� 
			// ������� ������ � ��������� ������
			if (uAddonFlags != 0)
			{
				if ((uAddonFlags & 1) != 0)
					pDDItem->AttachDetachAddon(CUIDragDropItemMP::ID_SCOPE, true, bRealRepresentationSet);
				if ((uAddonFlags & 2) != 0)
					pDDItem->AttachDetachAddon(CUIDragDropItemMP::ID_GRENADE_LAUNCHER, true, bRealRepresentationSet);
				if ((uAddonFlags & 4) != 0)
					pDDItem->AttachDetachAddon(CUIDragDropItemMP::ID_SILENCER, true, bRealRepresentationSet);
			}
		}
}

void CUIBuyWeaponWnd::ClearRealRepresentationFlags()
{
	UIBagWnd.ClearRealRepresentationFlags();
}

void CUIBuyWeaponWnd::SetMoneyAmount(int moneyAmount)
{
	m_iMoneyAmount = moneyAmount;
	if (!m_bIgnoreMoney)
		UIBagWnd.UpdateBuyPossibility();
}

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

int CUIBuyWeaponWnd::GetMoneyAmount() const
{
	if (!m_bIgnoreMoney)
	{
		return m_iMoneyAmount;
	}
	// Retuin some really big value
	return 10000000;
}

CUIDragDropItemMP * CUIBuyWeaponWnd::GetAddonByID(CUIDragDropItemMP *pAddonOwner, CUIDragDropItemMP::AddonIDs ID)
{
	return UIBagWnd.GetAddonByID(pAddonOwner, ID);
}

CUIDragDropItemMP * CUIBuyWeaponWnd::IsItemAnAddon(CUIDragDropItemMP *pPossibleAddon, CUIDragDropItemMP::AddonIDs &ID)
{
	R_ASSERT(pPossibleAddon);

	// ��������� �� ����� �� pPossibleAddon ���� � ����(�/��)
	if (!UITopList[RIFLE_SLOT].GetDragDropItemsList().empty())
	{
		CUIDragDropItemMP * pDDItemMP = smart_cast<CUIDragDropItemMP*>(UITopList[RIFLE_SLOT].GetDragDropItemsList().front());

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

	if (!UITopList[PISTOL_SLOT].GetDragDropItemsList().empty())
	{
		CUIDragDropItemMP * pDDItemMP = smart_cast<CUIDragDropItemMP*>(UITopList[PISTOL_SLOT].GetDragDropItemsList().front());

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

int CUIBuyWeaponWnd::GetItemPrice(CUIDragDropItemMP* pDDItemMP){
	if (NULL == pDDItemMP)
		return 0;

	return static_cast<int>(pDDItemMP->GetCost() * (pDDItemMP->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1));
}

bool CUIBuyWeaponWnd::IsItemAnAddonSimple(CUIDragDropItemMP *pPossibleAddon) const
{
	return UIBagWnd.IsItemAnAddonSimple(pPossibleAddon);
}

void CUIBuyWeaponWnd::FillItemInfo(CUIDragDropItemMP *pDDItemMP)
{
	{
		UIItemInfo.UIName.SetText			("");
		UIItemInfo.UIWeight.SetText			("");
		UIItemInfo.UIItemImage.TextureOff	();
		UIItemInfo.UIDesc.RemoveAll			();
		if (pDDItemMP)
		{
			CStringTable stbl;
			if (pSettings->line_exist(pDDItemMP->GetSectionName(), "inv_name"))
				UIItemInfo.UIName.SetText(*stbl(pSettings->r_string(pDDItemMP->GetSectionName(), "inv_name")));

			UIItemInfo.UIItemImage.SetShader(pDDItemMP->GetUIStaticItem().GetShader());
			Frect o_r = pDDItemMP->GetUIStaticItem().GetOriginalRect();
			Frect v_r = pDDItemMP->GetUIStaticItem().GetRect();
			UIItemInfo.UIItemImage.GetUIStaticItem().SetOriginalRect(o_r.x1, o_r.y1, o_r.width(), o_r.height());
			UIItemInfo.UIItemImage.GetUIStaticItem().SetRect(v_r);
			UIItemInfo.UIItemImage.SetStretchTexture(true);
			UIItemInfo.UIItemImage.SetWidth	(_min(v_r.width(),UIItemInfo.UIItemImageSize.x));
			UIItemInfo.UIItemImage.SetHeight(_min(v_r.height(),UIItemInfo.UIItemImageSize.y));
			UIItemInfo.UIItemImage.TextureOn();

			
			if (pDDItemMP->m_iRank>=0 && pDDItemMP->m_iRank<=4)
			{
				string64 tex_name;
				string64 foo;
				strconcat(tex_name,"rank_",itoa(pDDItemMP->m_iRank,foo,10));
				UIDescRankIcon.InitTexture(tex_name);
			}
			
			

			if (pSettings->line_exist(pDDItemMP->GetSectionName(), WEAPON_DESCRIPTION_FIELD))
			{
				CUIString str;
				str.SetText(*CStringTable()(pSettings->r_string(pDDItemMP->GetSectionName(), WEAPON_DESCRIPTION_FIELD)));
				UIItemInfo.UIDesc.AddParsedItem<CUIListItem>(str, 0, UIItemInfo.UIDesc.GetTextColor());
			}
		}
	}
}

bool CUIBuyWeaponWnd::BagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	CUIBuyWeaponWnd* this_inventory = smart_cast<CUIBuyWeaponWnd*>(pList->GetTop());
	R_ASSERT2(this_inventory, "wrong parent addressed");

	CUIDragDropItemMP* pDDItemMP = smart_cast<CUIDragDropItemMP*>(pItem);
	R_ASSERT(pDDItemMP);
	
	pDDItemMP->AttachDetachAllAddons(false);

	// Need to delete copy before adding
	if (pDDItemMP->HasAmountControl())
        pDDItemMP->OnReturn();
	this_inventory->UIBagWnd.DeleteCopy(pDDItemMP);

	pDDItemMP->GetParent()->DetachChild(pDDItemMP);
	pDDItemMP->GetOwner()->AttachChild(pDDItemMP);
	pDDItemMP->Rescale(pDDItemMP->GetOwner()->GetItemsScaleX(), pDDItemMP->GetOwner()->GetItemsScaleY());

	if (OUTFIT_SLOT == pDDItemMP->GetSlot())
		this_inventory->UIOutfitIcon.SetColor(cAbleToBuy);

	if (pDDItemMP->m_bAlreadyPaid)
	{
		this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() + GetItemPrice(pDDItemMP)); 		

		// ���� � ���� ���� ������, �� ���������� � ����� � �� ���������� ���������
		pDDItemMP->m_bAlreadyPaid = false;
	}

	if (pDDItemMP->GetCost() > this_inventory->GetMoneyAmount() && !pDDItemMP->m_bHasRealRepresentation)
		pDDItemMP->EnableDragDrop(false);

	// ���� ��� �����, �� ��������, ��� �� ���� �������
	if (OUTFIT_SLOT == pDDItemMP->GetSlot())
		pDDItemMP->Show(true);

	return false;
}

void	CUIBuyWeaponWnd::ReloadItemsPrices	()
{
	UIBagWnd.ReloadItemsPrices();
}