// UIInventoryWnd.h:  ������ ���������
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIInventoryWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../string_table.h"

#include "../actor.h"
#include "../uigamesp.h"
#include "../hudmanager.h"

#include "../CustomOutfit.h"
#include "../ArtifactMerger.h"

#include "../weapon.h"
//#include "../silencer.h"
//#include "../scope.h"
//#include "../grenadelauncher.h"

#include "../script_space.h"
#include "../script_process.h"

#include "../eatable_item.h"
#include "../inventory.h"

#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;


#include "../InfoPortion.h"
#include "../level.h"
#include "../game_base_space.h"
#include "../entitycondition.h"

#define MAX_ITEMS	70

const char * const INVENTORY_ITEM_XML		= "inventory_item.xml";
const char * const INVENTORY_XML			= "inventory_new.xml";
const char * const INVENTORY_CHARACTER_XML	= "inventory_character.xml";



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIInventoryWnd::CUIInventoryWnd()
{
	m_iCurrentActiveSlot = NO_ACTIVE_SLOT;
	Hide();

	SetCurrentItem(NULL);

	m_pCurrentDragDropItem = NULL;
	m_pItemToUpgrade = NULL;

	Init();

	SetFont(HUD().Font().pFontMedium);
}

void CUIInventoryWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, INVENTORY_XML);
	R_ASSERT3(xml_result, "xml file not found", INVENTORY_XML);

	CUIXmlInit xml_init;
	CStringTable string_table;

	CUIWindow::Init(CUIXmlInit::ApplyAlignX(0, alCenter),
					CUIXmlInit::ApplyAlignY(0, alCenter),
					UI_BASE_WIDTH, UI_BASE_HEIGHT);

	AttachChild(&UIStaticTop);
	UIStaticTop.Init("ui\\ui_inv_quick_slots", 0,0,UI_BASE_WIDTH,128);
	AttachChild(&UIStaticBottom);
	UIStaticBottom.Init("ui\\ui_bottom_background", 0, UI_BASE_HEIGHT-32, UI_BASE_WIDTH,32);

	AttachChild(&UIStaticBelt);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticBelt);
	
	AttachChild(&UIBagWnd);
	xml_init.InitStatic(uiXml, "bag_static", 0, &UIBagWnd);
	
	UIBagWnd.AttachChild(&UIMoneyWnd);
	xml_init.InitStatic(uiXml, "money_static", 0, &UIMoneyWnd);

	////////////////////////////////////////
	//���� � ��������� �������� ����
	
	//��� ������ � �����������
	AttachChild(&UIArtefactMergerWnd);
	xml_init.InitWindow(uiXml, "frame_window", 0, &UIArtefactMergerWnd);
	UIArtefactMergerWnd.Hide();

	AttachChild(&UIDescrWnd);
	xml_init.InitStatic(uiXml, "descr_static", 0, &UIDescrWnd);

	//���������� � ��������
	UIDescrWnd.AttachChild(&UIItemInfo);
	UIItemInfo.Init(0, 0, UIDescrWnd.GetWidth(), UIDescrWnd.GetHeight(), INVENTORY_ITEM_XML);

	// ������ Drop
	UIDescrWnd.AttachChild(&UIDropButton);
	xml_init.InitButton(uiXml, "drop_button", 0, &UIDropButton);
	UIDropButton.SetMessageTarget(this);
	UIDropButton.SetTextAlign(CGameFont::alLeft);

	////////////////////////////////////
	//���� � ���������� � ���������
	AttachChild(&UIPersonalWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 1, &UIPersonalWnd);

	//������ ���������
	UIPersonalWnd.AttachChild(&UIProgressBarHealth);
	xml_init.InitProgressBar(uiXml, "progress_bar", 0, &UIProgressBarHealth);
	
	UIPersonalWnd.AttachChild(&UIProgressBarSatiety);
	xml_init.InitProgressBar(uiXml, "progress_bar", 1, &UIProgressBarSatiety);

	UIPersonalWnd.AttachChild(&UIProgressBarPsyHealth);
	xml_init.InitProgressBar(uiXml, "progress_bar", 2, &UIProgressBarPsyHealth);

	UIPersonalWnd.AttachChild(&UIProgressBarRadiation);
	xml_init.InitProgressBar(uiXml, "progress_bar", 3, &UIProgressBarRadiation);

	UIPersonalWnd.AttachChild(&UIStaticPersonal);
	UIStaticPersonal.Init("ui\\ui_inv_personal_over_b", -1, UIPersonalWnd.GetHeight() - 175, 260, 260);

	//���������� � ���������
	// attributs suit of character (actor)
	//UIStaticPersonal.AttachChild(&UICharacterInfo);
	//UICharacterInfo.Init(0, 0, UIStaticPersonal.GetWidth(), UIStaticPersonal.GetHeight(), INVENTORY_CHARACTER_XML);

	UIStaticPersonal.AttachChild(&UIOutfitInfo);
	xml_init.InitStatic(uiXml, "outfit_info_window",0, &UIOutfitInfo);
	UIOutfitInfo.SetText(string_table);

	//�������� ��������������� ����������
	xml_init.InitAutoStatic(uiXml, "auto_static", this);

	//������ ��� ������� ���������� ���
	AttachChild(&UISleepWnd);
	xml_init.InitStatic(uiXml, "sleep_window", 0, &UISleepWnd);
	UISleepWnd.Init();
//	UISleepWnd.SetText(*string_table("rest"));

	//������ Drag&Drop
	AttachChild(&UIBeltList);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 0, &UIBeltList);
		
	UIBagWnd.AttachChild(&UIBagList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 1, &UIBagList);
	UIBagList.SetItemsScaleXY(TRADE_ICONS_SCALE,TRADE_ICONS_SCALE);

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
		
	AttachChild(&UIOutfitSlot);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 7, &UIOutfitSlot);
	UIOutfitSlot.BlockCustomPlacement();

	UITopList[0].SetCheckProc(SlotProc0);
	UITopList[1].SetCheckProc(SlotProc1);
	UITopList[2].SetCheckProc(SlotProc2);
	UITopList[3].SetCheckProc(SlotProc3);
	UITopList[4].SetCheckProc(SlotProc4);
	UIOutfitSlot.SetCheckProc(OutfitSlotProc);

	UIBeltList.SetCheckProc(BeltProc);
	UIBagList.SetCheckProc(BagProc);


	//pop-up menu
	AttachChild(&UIPropertiesBox);
	UIPropertiesBox.Init("ui\\ui_pop_up",0,0,300,300);
	UIPropertiesBox.Hide();

	// Time indicator
	AttachChild(&UITimeWnd);
	xml_init.InitStatic(uiXml, "time_static", 0, &UITimeWnd);
//	UITimeWnd.SetText(*string_table("time"));

	// Exit button
	AttachChild(&UIExitButton);
	xml_init.InitButton(uiXml, "exit_button", 0, &UIExitButton);
//	UIExitButton.SetText(*string_table("exit"));
}

CUIInventoryWnd::~CUIInventoryWnd()
{
	ClearDragDrop(m_vDragDropItems);
}

void CUIInventoryWnd::OnMouse(int x, int y, EUIMessages mouse_action)
{
	//����� ��������������� ���� �� ������ ������
	if(mouse_action == WINDOW_RBUTTON_DOWN)
	{
		if(UIPropertiesBox.IsShown())
		{
			UIPropertiesBox.Hide();
		}
	}

	CUIWindow::OnMouse(x, y, mouse_action);
}

void CUIInventoryWnd::Draw()
{
	CUIWindow::Draw();
}


void CUIInventoryWnd::Update()
{
	//CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	CEntityAlive *pEntityAlive = smart_cast<CEntityAlive*>(Level().CurrentEntity());

	if(pEntityAlive) 
	{
		UIProgressBarHealth.SetProgressPos(s16(pEntityAlive->conditions().GetHealth()*1000));
		UIProgressBarSatiety.SetProgressPos(s16(pEntityAlive->conditions().GetSatiety()*1000));
		UIProgressBarPsyHealth.SetProgressPos(s16(pEntityAlive->conditions().GetPsyHealth()*1000));
		UIProgressBarRadiation.SetProgressPos(s16(pEntityAlive->conditions().GetRadiation()*1000));

		
		//������ ������ drag&drop ��� ��� �������������� ����
		for(u32 i = 0; i <m_vDragDropItems.size()/*m_iUsedItems*/; i++) 
		{
			CInventoryItem* pItem = (CInventoryItem*)m_vDragDropItems[i]->GetData();
			if(pItem && !pItem->Useful())
			{
				if( m_vDragDropItems[i]->GetParent() )
					m_vDragDropItems[i]->GetParent()->DetachChild(m_vDragDropItems[i]);
				m_vDragDropItems[i]->SetData(NULL);
				m_vDragDropItems[i]->SetCustomDraw(NULL);
				
				if(m_pCurrentItem == pItem)
				{	
					SetCurrentItem(NULL);
					m_pCurrentDragDropItem = NULL;
				}

				xr_delete(m_vDragDropItems[i]);
				m_vDragDropItems.erase(m_vDragDropItems.begin()+i);
			}
		}

		UpdateWeight(UIBagWnd, true);

		// update money
		CInventoryOwner* pOurInvOwner = smart_cast<CInventoryOwner*>(pEntityAlive);
		char sMoney[50];
		int  iMoney(pOurInvOwner->m_dwMoney);
		itoa(iMoney, sMoney, 10);
		strcat(sMoney,"$");	
		UIMoneyWnd.SetText(sMoney);

		// update outfit parameters
		CCustomOutfit* outfit = (CCustomOutfit*)pOurInvOwner->inventory().m_slots[OUTFIT_SLOT].m_pIItem;		
		UIOutfitInfo.Update(*outfit);		
	}

	// Update time indicator
	UITimeWnd.Update();


	CUIWindow::Update();
}

void CUIInventoryWnd::Show() 
{ 
	InitInventory();
	inherited::Show();
	m_pCurrentDragDropItem = NULL;

	if (GameID() != GAME_SINGLE)
	{
		CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor) return;

        pActor->HideCurrentWeapon(GEG_PLAYER_INVENTORYMENU_OPEN);//, false);
	}

	//���� ����������� �������� ����������, ��� ����� ����� � ����
	SendInfoToActor("ui_inventory");

	Update();
}

void CUIInventoryWnd::Hide()
{
	inherited::Hide();

	SendInfoToActor("ui_inventory_hide");


	//������� ���� � �������� ����
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor && m_iCurrentActiveSlot != NO_ACTIVE_SLOT && 
		pActor->inventory().m_slots[m_iCurrentActiveSlot].m_pIItem)
	{
		pActor->inventory().Activate(m_iCurrentActiveSlot);
		m_iCurrentActiveSlot = NO_ACTIVE_SLOT;
	}

	if (GameID() != GAME_SINGLE)
	{
		CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor) return;

		pActor->RestoreHidedWeapon(GEG_PLAYER_INVENTORYMENU_CLOSE);
		/*
		NET_Packet	P;
		pActor->u_EventGen(P, GEG_PLAYER_INVENTORYMENU_CLOSE, pActor->ID());
		pActor->u_EventSend(P);
		*/
	}
}


bool CUIInventoryWnd::ToSlot()
{
	u32 item_slot = m_pCurrentItem->GetSlot();
	// ���� ������� ���� - ���� � �������, �� ��������� ������ ������� ������
	if (OUTFIT_SLOT == item_slot) UndressOutfit();

	// ������� ������� ���� � �����, ���� ��� �� ������, � ������� ���� � ���� � ����� �� ���� � �� ��
	if (OUTFIT_SLOT != item_slot && item_slot<SLOTS_NUM )
	{
		DRAG_DROP_LIST &DDList = UITopList[item_slot].GetDragDropItemsList();

		if (!DDList.empty() &&
			m_pCurrentDragDropItem != DDList.front())
		{
			CUIDragDropItem* slot_dditem = DDList.front();
			PIItem slot_iitem = (PIItem)( slot_dditem->GetData() );
			// ����� ������� ���� � �����...
			slot_dditem->MoveOnNextDrop();
			// ...� �������� ����� ��������� ����������� ��� ���� � ����
			
			if( GetInventory()->CanPutInBelt(slot_iitem) ){
				UIBeltList.SendMessage(
					slot_dditem, 
					DRAG_DROP_ITEM_DROP,
					NULL);
			}else{
				UIBagList.SendMessage(
					slot_dditem, 
					DRAG_DROP_ITEM_DROP,
					NULL);
			}
		}
	}

	// ����� �� �������� ����� ���� � �������������� ��� ��� ����?
	if(!GetInventory()->CanPutInSlot(m_pCurrentItem)) return false;

	//���������� �������� ������� � ��������������� ����
	bool result = GetInventory()->Slot(m_pCurrentItem);

	if(!result) return false;

	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);

	if(item_slot == OUTFIT_SLOT)
		UIOutfitSlot.AttachChild(m_pCurrentDragDropItem);
	else
		UITopList[item_slot].AttachChild(m_pCurrentDragDropItem);
			
	m_pMouseCapturer = NULL;

	//---------------------------------------------------------------------------
	SendEvent_Item2Slot(m_pCurrentItem);
	//---------------------------------------------------------------------------
	return true;
}

bool CUIInventoryWnd::ToBag()
{
	if(!GetInventory()->CanPutInRuck(m_pCurrentItem)) return false;

	//���������� �������� ������� � ��������������� ����
	bool result = GetInventory()->Ruck(m_pCurrentItem);

	if(!result) return false;

	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	UIBagList.AttachChild(m_pCurrentDragDropItem);
			
	m_pMouseCapturer = NULL;

	//---------------------------------------------------------------------------
	SendEvent_Item2Ruck(m_pCurrentItem);
	//---------------------------------------------------------------------------
	return true;
}

bool CUIInventoryWnd::ToBelt()
{
	if(!GetInventory()->CanPutInBelt(m_pCurrentItem)) return false;
	
	bool result = GetInventory()->Belt(m_pCurrentItem);

	if(!result) return false;

	((CUIDragDropList*)m_pCurrentDragDropItem->GetParent())->DetachChild(m_pCurrentDragDropItem);
	UIBeltList.AttachChild(m_pCurrentDragDropItem);
			
	m_pMouseCapturer = NULL;
	//---------------------------------------------------------------------------
	SendEvent_Item2Belt(m_pCurrentItem);
	//---------------------------------------------------------------------------
	return true;
}

//������ � ��������� ���� ������ � �����������
void CUIInventoryWnd::StartArtefactMerger()
{
	UIArtefactMergerWnd.InitArtefactMerger(smart_cast<CArtefactMerger*>(m_pCurrentItem));
	UIArtefactMergerWnd.Show();
}
void CUIInventoryWnd::StopArtefactMerger()
{
	UIArtefactMergerWnd.Hide();
	
	//������� ��� �������� �� ���������� ���������� � ������
	for(DRAG_DROP_LIST_it it = UIArtefactMergerWnd.UIArtefactList.GetDragDropItemsList().begin(); 
 						  UIArtefactMergerWnd.UIArtefactList.GetDragDropItemsList().end() != it;
						  ++it)
	{
		CUIDragDropItem* pDragDropItem = *it;
		UIBagList.AttachChild(pDragDropItem);
	}

	//((CUIDragDropList*)pDragDropItem->GetParent())->DetachChild(pDragDropItem);
	UIArtefactMergerWnd.UIArtefactList.DropAll();
}

//��� ������ � ����������� ���������� �����
void CUIInventoryWnd::AddArtefactToMerger(CArtefact* pArtefact)
{
	DD_ITEMS_VECTOR_IT it = std::find(m_vDragDropItems.begin(), m_vDragDropItems.end(),m_pCurrentDragDropItem);

	
	CUIDragDropItem& UIDragDropItem = *(*it);

//	m_vDragDropItems.push_back(xr_new<CUIWpnDragDropItem>());
//	CUIDragDropItem& UIDragDropItem = *m_vDragDropItems.back();

	UIDragDropItem.CUIStatic::Init(0, 0, INV_GRID_WIDTH, INV_GRID_HEIGHT);
	UIDragDropItem.SetShader(GetEquipmentIconsShader());
	UIDragDropItem.SetGridHeight(pArtefact->GetGridHeight());
	UIDragDropItem.SetGridWidth(pArtefact->GetGridWidth());
	UIDragDropItem.GetUIStaticItem().SetOriginalRect(
										pArtefact->GetXPos()*INV_GRID_WIDTH,
										pArtefact->GetYPos()*INV_GRID_HEIGHT,
										pArtefact->GetGridWidth()*INV_GRID_WIDTH,
										pArtefact->GetGridHeight()*INV_GRID_HEIGHT);
	UIDragDropItem.SetData(pArtefact);
	UIArtefactMergerWnd.UIArtefactList.AttachChild(&UIDragDropItem);

}

void CUIInventoryWnd::AddItemToBag(PIItem pItem)
{
	DD_ITEMS_VECTOR_IT it = std::find(m_vDragDropItems.begin(), m_vDragDropItems.end(),m_pCurrentDragDropItem);
	if (it != m_vDragDropItems.end()) return;

	m_vDragDropItems.push_back(xr_new<CUIWpnDragDropItem>());
	CUIDragDropItem& UIDragDropItem = *m_vDragDropItems.back();

	UIDragDropItem.CUIStatic::Init(0, 0, INV_GRID_WIDTH, INV_GRID_HEIGHT);
	UIDragDropItem.SetShader(GetEquipmentIconsShader());
	UIDragDropItem.SetGridHeight(pItem->GetGridHeight());
	UIDragDropItem.SetGridWidth(pItem->GetGridWidth());
	UIDragDropItem.GetUIStaticItem().SetOriginalRect(
										pItem->GetXPos()*INV_GRID_WIDTH,
										pItem->GetYPos()*INV_GRID_HEIGHT,
										pItem->GetGridWidth()*INV_GRID_WIDTH,
										pItem->GetGridHeight()*INV_GRID_HEIGHT);
	if(m_pInv->Get(pItem->object().ID(), false))
        UIBeltList.AttachChild(&UIDragDropItem);
	else if(m_pInv->Get(pItem->object().ID(), true))
		UIBagList.AttachChild(&UIDragDropItem);

	
	UIDragDropItem.SetData(pItem);
}

void CUIInventoryWnd::AttachAddon()
{
	if (OnClient())
	{
		NET_Packet P;
		m_pItemToUpgrade->object().u_EventGen(P, GE_ADDON_ATTACH, m_pItemToUpgrade->object().ID());
		P.w_u32(m_pCurrentItem->object().ID());
		m_pItemToUpgrade->object().u_EventSend(P);
	};

	R_ASSERT(m_pItemToUpgrade);
	m_pItemToUpgrade->Attach(m_pCurrentItem);


	//�������� ���� �� ��������� ����� � ��������� �� ����� ������ �������
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor && m_pItemToUpgrade == pActor->inventory().ActiveItem())
	{
					m_iCurrentActiveSlot = pActor->inventory().GetActiveSlot();
					pActor->inventory().Activate(NO_ACTIVE_SLOT);
//		m_iCurrentActiveSlot = pActor->HideActiveItem();
	}

	(smart_cast<CUIDragDropList*>(m_pCurrentDragDropItem->GetParent()))->
									DetachChild(m_pCurrentDragDropItem);
	SetCurrentItem(NULL);
	m_pCurrentDragDropItem = NULL;

	m_pItemToUpgrade = NULL;
}
void CUIInventoryWnd::DetachAddon(const char* addon_name)
{
	if (OnClient())
	{
		NET_Packet P;
		m_pCurrentItem->object().u_EventGen(P, GE_ADDON_DETACH, m_pCurrentItem->object().ID());
		P.w_stringZ(addon_name);
		m_pCurrentItem->object().u_EventSend(P);
	};
	m_pCurrentItem->Detach(addon_name);

	//�������� ���� �� ��������� ����� � ��������� �� ����� ������ �������
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor && m_pCurrentItem == pActor->inventory().ActiveItem())
	{
			m_iCurrentActiveSlot = pActor->inventory().GetActiveSlot();
			pActor->inventory().Activate(NO_ACTIVE_SLOT);
//			m_iCurrentActiveSlot = pActor->HideActiveItem();
	}
}

void CUIInventoryWnd::SetCurrentItem(CInventoryItem* pItem)
{
	m_pCurrentItem = pItem;
	const int offset = -10;

	UIItemInfo.InitItem(m_pCurrentItem);
	UIItemInfo.AlignRight(UIItemInfo.UIWeight, offset);
	UIItemInfo.AlignRight(UIItemInfo.UICost, offset);
	UIItemInfo.AlignRight(UIItemInfo.UICondition, offset);
}

//-----------------------------------------------------------------------------/
//  ����� ������ ���� ����
//-----------------------------------------------------------------------------/
bool CUIInventoryWnd::UndressOutfit()
{
	bool status = true;
	// �������� ������� ������
	CUIDragDropItem *pDDItem = UIOutfitSlot.GetCurrentOutfit();
	if (pDDItem)
	{
		// ����������� ��� � �����
		PIItem pInvItem = (PIItem)(UIOutfitSlot.GetCurrentOutfit())->GetData();

		// ���������� ������� �������� �������
		PIItem pCurrentItem = m_pCurrentItem;
		CUIDragDropItem *pCurrDDItem = m_pCurrentDragDropItem;

		SetCurrentItem(pInvItem);
		m_pCurrentDragDropItem = UIOutfitSlot.GetCurrentOutfit();

		status = ToBag();
		m_pCurrentDragDropItem->Rescale(UIBagList.GetItemsScaleX(),UIBagList.GetItemsScaleY());

		// �������������� ���������� ���������
		SetCurrentItem(pCurrentItem);
		m_pCurrentDragDropItem = pCurrDDItem;
	}
	return status;
}

//-----------------------------------------------------------------------------/
//	��������� �������������� ���� � ������� ����� � ����������� ���������� ��� 
//	��� �����, ���������� ������� ���� � ����� � �����
//-----------------------------------------------------------------------------/
bool CUIInventoryWnd::SlotToBag(PIItem pItem, CUIDragDropList *pList, const u32 SlotNum)
{
	// ��������� ����������� �����������  ������ � ������ ����
	if (SlotNum != pItem->GetSlot()) return false;
	// ���������� ���� �� ����� ���� ����.
	if (!pList->GetDragDropItemsList().empty())
	{
		// ����� ������� ������ � �����...
		(*pList->GetDragDropItemsList().begin())->MoveOnNextDrop();
		// ...� �������� ��� ��������� ������������� � �����
		GetBag()->SendMessage((*pList->GetDragDropItemsList().begin()), 
			DRAG_DROP_ITEM_DROP, NULL);
	}
	return true;
}

void	CUIInventoryWnd::SendEvent_Item2Slot			(PIItem	pItem)
{
	NET_Packet	P;
	pItem->object().u_EventGen(P, GEG_PLAYER_ITEM2SLOT, pItem->object().H_Parent()->ID());
	P.w_u16		(pItem->object().ID());
	pItem->object().u_EventSend(P);
};

void	CUIInventoryWnd::SendEvent_Item2Belt			(PIItem	pItem)
{
	NET_Packet	P;
	pItem->object().u_EventGen(P, GEG_PLAYER_ITEM2BELT, pItem->object().H_Parent()->ID());
	P.w_u16		(pItem->object().ID());
	pItem->object().u_EventSend(P);

};

void	CUIInventoryWnd::SendEvent_Item2Ruck			(PIItem	pItem)
{
	NET_Packet	P;
	pItem->object().u_EventGen(P, GEG_PLAYER_ITEM2RUCK, pItem->object().H_Parent()->ID());
	P.w_u16		(pItem->object().ID());
	pItem->object().u_EventSend(P);
};

void	CUIInventoryWnd::SendEvent_ItemDrop			(PIItem	pItem)
{
	NET_Packet	P;
	pItem->object().u_EventGen(P, GEG_PLAYER_ITEMDROP, pItem->object().H_Parent()->ID());
	P.w_u16		(pItem->object().ID());
	pItem->object().u_EventSend(P);
};

void	CUIInventoryWnd::SendEvent_Item_Eat			(PIItem	pItem)
{
	NET_Packet	P;
	pItem->object().u_EventGen(P, GEG_PLAYER_ITEM_EAT, pItem->object().H_Parent()->ID());
	P.w_u16		(pItem->object().ID());
	pItem->object().u_EventSend(P);
};


#include "../xr_level_controller.h"

bool CUIInventoryWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if( inherited::OnKeyboard(dik,keyboard_action) )return true;


	if(keyboard_action==WINDOW_KEY_PRESSED || keyboard_action==WINDOW_KEY_RELEASED){

		if(key_binding[dik]==kINVENTORY&&keyboard_action==WINDOW_KEY_PRESSED){
			GetHolder()->StartStopMenu(this);
		}

		CObject* O = Level().CurrentEntity();
		if( O ){
			IInputReceiver*		IR	= smart_cast<IInputReceiver*>( smart_cast<CGameObject*>(O) );

			if(keyboard_action==WINDOW_KEY_PRESSED)
					IR->IR_OnKeyboardPress(key_binding[dik]);
			else	
					IR->IR_OnKeyboardRelease(key_binding[dik]);
		
			return true;
		}
	}
	return false;
}
