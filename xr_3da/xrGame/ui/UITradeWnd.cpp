// UITradeWnd.cpp:  ������ ��������
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UITradeWnd.h"

#include "..\\..\\XRXMLParser\\xrXMLParser.h"
#include "UIXmlInit.h"


#include "..\\HUDManager.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUITradeWnd::CUITradeWnd()
{
	Init();

	Show(false);
	Enable(false);

	SetFont(HUD().pFontHeaderEurope);
}

CUITradeWnd::~CUITradeWnd()
{
}

void CUITradeWnd::Init()
{
	CUIWindow::Init(0,0, Device.dwWidth, Device.dwHeight);

	AttachChild(&UIFrameWnd);
	UIFrameWnd.Init("ui\\ui_frame", 100,100, 600, 400);

	UIFrameWnd.AttachChild(&UIEditBox);
	UIEditBox.Init(10,10,500,300);
	

/*	CUIXml uiXml;
	uiXml.Init("x:\\inventory.xml");
	
	CUIXmlInit xml_init;

	CUIWindow::Init(0,0, Device.dwWidth, Device.dwHeight);


	AttachChild(&UIStaticTop);
	UIStaticTop.Init("ui\\ui_inv_quick_slots", 0,0,1024,128);
	AttachChild(&UIStaticBottom);
	UIStaticBottom.Init("ui\\ui_bottom_background", 0,Device.dwHeight-32,1024,32);

//	AttachChild(&UIStaticBelt);
//	UIStaticBelt.Init("ui\\ui_inv_belt", 0,140,1024,128);

	AttachChild(&UIStaticBelt);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticBelt);
	
	AttachChild(&UIBagWnd);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIBagWnd);


	AttachChild(&UIDescWnd);
	UIDescWnd.Init("ui\\ui_frame", 440, 260, 267, 500);
	UIDescWnd.InitLeftTop("ui\\ui_inv_info_over_lt", 5,10);
	
	UIDescWnd.AttachChild(&UIStaticDesc);
	UIStaticDesc.Init("ui\\ui_inv_info_over_b", 
						5, UIDescWnd.GetHeight() - 310 ,260,310);
	
	//UIStaticDesc.SetText("\\n\\n\\nYou say %c99,99,255Yes \\n  %c9,255,9I say no\\n%c255,255,255You say stop And I say\\nGo    go    go...................\\nYou say goodbye and I say hello..");


	UIDescWnd.AttachChild(&UIStaticText);
	UIStaticText.Init(30, 197 ,250,50);

	UIDescWnd.AttachChild(&UI3dStatic);
	UI3dStatic.Init(5,10, 250,190);

//	AttachChild(&UI3dStatic);
//	UI3dStatic.Init(800, 160, 260, 130);


	//UIStaticText.SetText("Weapon Description");

	AttachChild(&UIPersonalWnd);
	UIPersonalWnd.Init("ui\\ui_frame", 730, 260, 268, 490);
	UIPersonalWnd.InitLeftTop("ui\\ui_inv_personal_over_t", 5,10);

	//������ ���������
	UIPersonalWnd.AttachChild(&UIProgressBar1);
	xml_init.InitProgressBar(uiXml, "progress_bar", 0, &UIProgressBar1);

	UIPersonalWnd.AttachChild(&UIProgressBar2);
	xml_init.InitProgressBar(uiXml, "progress_bar", 1, &UIProgressBar2);

	UIPersonalWnd.AttachChild(&UIProgressBar3);
	xml_init.InitProgressBar(uiXml, "progress_bar", 2, &UIProgressBar3);

	UIPersonalWnd.AttachChild(&UIProgressBar4);
	xml_init.InitProgressBar(uiXml, "progress_bar", 3, &UIProgressBar4);


	UIPersonalWnd.AttachChild(&UIStaticPersonal);
	UIStaticPersonal.Init("ui\\ui_inv_personal_over_b", 
						-3,UIPersonalWnd.GetHeight() - 209 ,260,260);


	//������ �����
	AttachChild(&UIButton1);
	xml_init.InitButton(uiXml, "button", 0, &UIButton1);
	
	AttachChild(&UIButton2);
	xml_init.InitButton(uiXml, "button", 1, &UIButton2);
	

	//������ Drag&Drop
	AttachChild(&UIBeltList);
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 0, &UIBeltList);
		
	UIBagWnd.AttachChild(&UIBagList);	
	xml_init.InitDragDropList(uiXml, "dragdrop_list", 1, &UIBagList);

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
	

	UITopList[0].SetCheckProc(SlotProc0);
	UITopList[1].SetCheckProc(SlotProc1);
	UITopList[2].SetCheckProc(SlotProc2);
	UITopList[3].SetCheckProc(SlotProc3);
	UITopList[4].SetCheckProc(SlotProc4);

	UIBeltList.SetCheckProc(BeltProc);
	UIBagList.SetCheckProc(BagProc);*/
}

/*
//���������� �������� �� ������������ ����������� ��� � �������
bool CUITradeWnd::GreaterRoomInRuck(PIItem item1, PIItem item2)
{
	int item1_room = item1->m_iGridWidth*item1->m_iGridHeight;
	int item2_room = item2->m_iGridWidth*item2->m_iGridHeight;

	if(item1_room > item2_room)
		return true;
	else if (item1_room == item2_room)
	{
		if(item1->m_iGridWidth >= item2->m_iGridWidth)
			return true;
	}
   	return false;
}

void CUITradeWnd::InitInventory(CInventory* pInv) 
{
	m_pMouseCapturer = NULL;

	m_pCurrentItem = NULL;
	UIStaticText.SetText(NULL);


	m_pInv = pInv;

	//�������� ����� ����������� �������
	UITopList[0].DropAll();
	UITopList[1].DropAll();
	UITopList[2].DropAll();
	UITopList[3].DropAll();
	UITopList[4].DropAll();
		
	UIBeltList.DropAll();
	UIBagList.DropAll();


//	u32 i;
	for(u32 i = 0; i <MAX_ITEMS; i++) 
	{
		m_vDragDropItems[i].SetData(NULL);
		m_vDragDropItems[i].SetWndRect(0,0,0,0);
	}

		
	m_iUsedItems = 0;


	//Slots
	for( i = 0; i < SLOTS_NUM; i++) 
	{
			if(pInv->m_slots[i].m_pIItem) 
			{

				CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];		
				UIDragDropItem.SetData(pInv->m_slots[i].m_pIItem);
					
				UIDragDropItem.Init(pInv->m_slots[i].m_pIItem->m_sIconTexture, 0,0, 50,50);
				UIDragDropItem.SetGridHeight(pInv->m_slots[i].m_pIItem->m_iGridHeight);
				UIDragDropItem.SetGridWidth(pInv->m_slots[i].m_pIItem->m_iGridWidth);
				
				UITopList[i].AttachChild(&UIDragDropItem);
				m_iUsedItems++;
			}
	}


	//����
	for(PPIItem it =  pInv->m_belt.begin(); it !=  pInv->m_belt.end(); it++) 
	{
			if((*it)) 
			{

				CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];		
				UIDragDropItem.SetData((*it));

				UIDragDropItem.Init((*it)->m_sIconTexture, 0,0, 50,50);
				UIDragDropItem.SetGridHeight((*it)->m_iGridHeight);
				UIDragDropItem.SetGridWidth((*it)->m_iGridWidth);
		
				UIBeltList.AttachChild(&UIDragDropItem);
				m_iUsedItems++;
			}
	}


	TIItemList ruck_list(pInv->m_ruck);
	ruck_list.sort(GreaterRoomInRuck);

	//������
	for(it =  ruck_list.begin(); it !=  ruck_list.end(); it++) 
	{
			if((*it)) 
			{
				CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];		
				UIDragDropItem.SetData((*it));

				UIDragDropItem.Init((*it)->m_sIconTexture, 0,0, 50,50);
				UIDragDropItem.SetGridHeight((*it)->m_iGridHeight);
				UIDragDropItem.SetGridWidth((*it)->m_iGridWidth);

		
				UIBagList.AttachChild(&UIDragDropItem);
				m_iUsedItems++;
			}
	}
}  


//��� ������ �������� ���������� ������� 
//�������� ����, ���� ����� ����� ������ ��
//��������� �������� CUITradeWnd � 
//�������� �������� GetInventory()

//�������� �� ��������� ��������� � �����
bool CUITradeWnd::SlotProc0(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	PIItem pInvItem = (PIItem)pItem->GetData();

	if( pInvItem->m_slot == 0)
		return ((CUITradeWnd*)(pList->GetParent()))->GetInventory()->Slot(pInvItem);
	else
		return false;
}

bool CUITradeWnd::SlotProc1(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	PIItem pInvItem = (PIItem)pItem->GetData();

	if(pInvItem->m_slot == 1)
		return ((CUITradeWnd*)(pList->GetParent()))->GetInventory()->Slot(pInvItem);
	else
		return false;
}
bool CUITradeWnd::SlotProc2(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	PIItem pInvItem = (PIItem)pItem->GetData();

	if( pInvItem->m_slot == 2)
		return ((CUITradeWnd*)(pList->GetParent()))->GetInventory()->Slot(pInvItem);
	else
		return false;
}
bool CUITradeWnd::SlotProc3(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	PIItem pInvItem = (PIItem)pItem->GetData();

	if( pInvItem->m_slot == 3)
		return ((CUITradeWnd*)(pList->GetParent()))->GetInventory()->Slot(pInvItem);
	else
		return false;
}
bool CUITradeWnd::SlotProc4(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	PIItem pInvItem = (PIItem)pItem->GetData();

	if(pInvItem->m_slot == 4)
		return ((CUITradeWnd*)(pList->GetParent()))->GetInventory()->Slot(pInvItem);
	else
		return false;
}


//� ������
bool CUITradeWnd::BagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	PIItem pInvItem = (PIItem)pItem->GetData();

	
	return ((CUITradeWnd*)(pList->GetParent()->GetParent()))->GetInventory()->Ruck(pInvItem);
}

//�� ����
bool CUITradeWnd::BeltProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	PIItem pInvItem = (PIItem)pItem->GetData();

	return ((CUITradeWnd*)(pList->GetParent()))->GetInventory()->Belt(pInvItem);
}
*/

//------------------------------------------------
//��� ������ ������� �������, ������� ��� �������
void CUITradeWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
/*	if(msg == CUIDragDropItem::ITEM_DRAG)
	{
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		UIStaticText.SetText(pInvItem->NameComplex());
		
		m_pCurrentItem = pInvItem;
	}
	else if(msg == CUIDragDropItem::ITEM_DB_CLICK)
	{
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		UIStaticText.SetText(pInvItem->NameComplex());

		m_pCurrentItem = pInvItem;

		//���������� �������� ������� � ��������������� ����
		bool result = GetInventory()->Slot(pInvItem);

		CUIDragDropItem* pDragDropItem = (CUIDragDropItem*)pWnd;
		
		if(result)
		{
			((CUIDragDropList*)pDragDropItem->GetParent())->DetachChild(pDragDropItem);
			UITopList[pInvItem->m_slot].AttachChild(pDragDropItem);
			
			m_pMouseCapturer = NULL;
		}
		else
		{
			CUIDragDropList* pListToAttach = NULL;

			//���������� �������� �� ����,���� �� � �������
			if(((CUIDragDropList*)pDragDropItem->GetParent())== &UIBagList)
			{
				result = GetInventory()->Belt(pInvItem);
				pListToAttach = &UIBeltList;
			}
			//���� �� �����, �� � ������
			else if(((CUIDragDropList*)pDragDropItem->GetParent())== &UIBeltList)
			{
				result = GetInventory()->Ruck(pInvItem);
				pListToAttach = &UIBagList;
			}
			else
				result = false;

			if(result)
			{
				((CUIDragDropList*)pDragDropItem->GetParent())->DetachChild(pDragDropItem);
				pListToAttach->AttachChild(pDragDropItem);
				m_pMouseCapturer = NULL;
			}
			//���� � �� ���� ������, �� ������ ����������� ������� � ����� ������
			else
			{
				((CUIDragDropList*)pDragDropItem->GetParent())->DetachChild(pDragDropItem);
				((CUIDragDropList*)pDragDropItem->GetParent())->AttachChild(pDragDropItem);
			}
		}
    }*/


	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUITradeWnd::Draw()
{
	CUIWindow::Draw();
}

bool CUITradeWnd::IR_OnKeyboardPress(int dik)
{
	if(!IsEnabled()) return false;
			
	//mouse click
	if(dik==MOUSE_1)
	{
		OnMouse(HUD().GetUI()->GetCursor()->GetPos().x,
				HUD().GetUI()->GetCursor()->GetPos().y,
                CUIWindow::LBUTTON_DOWN);
		return true;
	}
	
	
	return 	OnKeyboard(dik,	CUIWindow::KEY_PRESSED);
}
bool CUITradeWnd::IR_OnKeyboardRelease(int dik)
{
	if(!IsEnabled()) return false;
			
	//mouse click
	if(dik==MOUSE_1)
	{
			OnMouse(HUD().GetUI()->GetCursor()->GetPos().x,
			 	    HUD().GetUI()->GetCursor()->GetPos().y,	
					CUIWindow::LBUTTON_UP);
		return true;
	}

	return 	OnKeyboard(dik,	CUIWindow::KEY_RELEASED);
}
bool CUITradeWnd::IR_OnMouseMove(int dx, int dy)
{
	if(!IsEnabled()) return false;
	
	if (HUD().GetUI()->GetCursor()->IsVisible())
	{ 
		HUD().GetUI()->GetCursor()->MoveBy(dx, dy);
		OnMouse(HUD().GetUI()->GetCursor()->GetPos().x, 
				HUD().GetUI()->GetCursor()->GetPos().y, 
				CUIWindow::MOUSE_MOVE);
	}

	return true;
}