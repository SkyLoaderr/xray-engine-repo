// UIInventoryWnd.h:  ������ ���������
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIInventoryWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"


#include "..\\actor.h"
#include "..\\WeaponAmmo.h"
#include "..\\hudmanager.h"






//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIInventoryWnd::CUIInventoryWnd()
{
	m_pCurrentItem = NULL;
	m_pCurrentDragDropItem = NULL;
}

CUIInventoryWnd::~CUIInventoryWnd()
{
//	m_vDragDropItems.erase(m_vDragDropItems.begin(), m_vDragDropItems.end());
}

void CUIInventoryWnd::Init()
{
	CUIXml uiXml;
	//uiXml.Init("x:\\inventory.xml");
	uiXml.Init("$game_data$","inventory.xml");
	
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
	UIPersonalWnd.AttachChild(&UIProgressBarHealth);
	xml_init.InitProgressBar(uiXml, "progress_bar", 0, &UIProgressBarHealth);
	
	UIPersonalWnd.AttachChild(&UIProgressBarSatiety);
	xml_init.InitProgressBar(uiXml, "progress_bar", 1, &UIProgressBarSatiety);

	UIPersonalWnd.AttachChild(&UIProgressBarPower);
	xml_init.InitProgressBar(uiXml, "progress_bar", 2, &UIProgressBarPower);

	UIPersonalWnd.AttachChild(&UIProgressBarRadiation);
	xml_init.InitProgressBar(uiXml, "progress_bar", 3, &UIProgressBarRadiation);


	UIPersonalWnd.AttachChild(&UIStaticPersonal);
	UIStaticPersonal.Init("ui\\ui_inv_personal_over_b", 
						-3,UIPersonalWnd.GetHeight() - 209 ,260,260);


	//������ �����
	AttachChild(&UIButton1);
	xml_init.InitButton(uiXml, "button", 0, &UIButton1);
	AttachChild(&UIButton2);
	xml_init.InitButton(uiXml, "button", 1, &UIButton2);
	AttachChild(&UIButton3);
	xml_init.InitButton(uiXml, "button", 2, &UIButton3);
	AttachChild(&UIButton4);
	xml_init.InitButton(uiXml, "button", 3, &UIButton4);
	AttachChild(&UIButton5);
	xml_init.InitButton(uiXml, "button", 4, &UIButton5);
	AttachChild(&UIButton6);
	xml_init.InitButton(uiXml, "button", 5, &UIButton6);



	

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
	UIBagList.SetCheckProc(BagProc);


	AttachChild(&UIPropertiesBox);
	UIPropertiesBox.Init("ui\\ui_frame",0,0,300,300);
	UIPropertiesBox.AddItem("Drop");
	UIPropertiesBox.AddItem("Eat");
	UIPropertiesBox.AddItem("Activate");
	UIPropertiesBox.Hide();


}

//���������� �������� �� ������������ ����������� ��� � �������
bool CUIInventoryWnd::GreaterRoomInRuck(PIItem item1, PIItem item2)
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

void CUIInventoryWnd::InitInventory(CInventory* pInv) 
{
	m_pMouseCapturer = NULL;

	m_pCurrentItem = NULL;
	UIStaticText.SetText(NULL);

	UIPropertiesBox.Hide();

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
	for( i = 0; i < /*pInv->m_slots.size()*/ SLOTS_NUM; i++) 
	{
			if(pInv->m_slots[i].m_pIItem) 
			{

				CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];		
				
				UIDragDropItem.Init(pInv->m_slots[i].m_pIItem->m_sIconTexture, 0,0, 50,50);
				UIDragDropItem.SetGridHeight(pInv->m_slots[i].m_pIItem->m_iGridHeight);
				UIDragDropItem.SetGridWidth(pInv->m_slots[i].m_pIItem->m_iGridWidth);

				UIDragDropItem.SetData(pInv->m_slots[i].m_pIItem);
				
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
				
				UIDragDropItem.Init((*it)->m_sIconTexture, 0,0, 50,50);
				UIDragDropItem.SetGridHeight((*it)->m_iGridHeight);
				UIDragDropItem.SetGridWidth((*it)->m_iGridWidth);

				UIDragDropItem.SetData((*it));

				CWeaponAmmo* pWeaponAmmo  = dynamic_cast<CWeaponAmmo*>((*it));
				if(pWeaponAmmo)	UIDragDropItem.SetCustomUpdate(AmmoUpdateProc);
		
				UIBeltList.AttachChild(&UIDragDropItem);
				m_iUsedItems++;
			}
	}


	ruck_list = pInv->m_ruck;
	ruck_list.sort(GreaterRoomInRuck);

	//������
	for(it =  ruck_list.begin(); it !=  ruck_list.end(); it++) 
	{
			if((*it)) 
			{
				CUIDragDropItem& UIDragDropItem = m_vDragDropItems[m_iUsedItems];		
				UIDragDropItem.Init((*it)->m_sIconTexture, 0,0, 50,50);
				UIDragDropItem.SetGridHeight((*it)->m_iGridHeight);
				UIDragDropItem.SetGridWidth((*it)->m_iGridWidth);

				UIDragDropItem.SetData((*it));

				CWeaponAmmo* pWeaponAmmo  = dynamic_cast<CWeaponAmmo*>((*it));
				if(pWeaponAmmo)	UIDragDropItem.SetCustomUpdate(AmmoUpdateProc);

		
				UIBagList.AttachChild(&UIDragDropItem);
				m_iUsedItems++;
			}
	}
}  


//��� ������ �������� ���������� ������� 
//�������� ����, ���� ����� ����� ������ ��
//��������� �������� CUIInventoryWnd � 
//�������� �������� GetInventory()

//�������� �� ��������� ��������� � �����
bool CUIInventoryWnd::SlotProc0(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	PIItem pInvItem = (PIItem)pItem->GetData();

	if( pInvItem->m_slot == 0)
		return ((CUIInventoryWnd*)(pList->GetParent()))->GetInventory()->Slot(pInvItem);
	else
		return false;
}

bool CUIInventoryWnd::SlotProc1(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	PIItem pInvItem = (PIItem)pItem->GetData();

	if(pInvItem->m_slot == 1)
		return ((CUIInventoryWnd*)(pList->GetParent()))->GetInventory()->Slot(pInvItem);
	else
		return false;
}
bool CUIInventoryWnd::SlotProc2(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	PIItem pInvItem = (PIItem)pItem->GetData();

	if( pInvItem->m_slot == 2)
		return ((CUIInventoryWnd*)(pList->GetParent()))->GetInventory()->Slot(pInvItem);
	else
		return false;
}
bool CUIInventoryWnd::SlotProc3(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	PIItem pInvItem = (PIItem)pItem->GetData();

	if( pInvItem->m_slot == 3)
		return ((CUIInventoryWnd*)(pList->GetParent()))->GetInventory()->Slot(pInvItem);
	else
		return false;
}
bool CUIInventoryWnd::SlotProc4(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	PIItem pInvItem = (PIItem)pItem->GetData();

	if(/*pList->GetChildNum() == 0 &&*/ pInvItem->m_slot == 4)
		return ((CUIInventoryWnd*)(pList->GetParent()))->GetInventory()->Slot(pInvItem);
	else
		return false;
}


//� ������
bool CUIInventoryWnd::BagProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	PIItem pInvItem = (PIItem)pItem->GetData();

	
	return ((CUIInventoryWnd*)(pList->GetParent()->GetParent()))->GetInventory()->Ruck(pInvItem);
}

//�� ����
bool CUIInventoryWnd::BeltProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	PIItem pInvItem = (PIItem)pItem->GetData();

	return ((CUIInventoryWnd*)(pList->GetParent()))->GetInventory()->Belt(pInvItem);
}

//------------------------------------------------
//��� ������ ������� �������, ������� ��� �������
void CUIInventoryWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(msg == CUIDragDropItem::ITEM_DRAG)
	{
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		UIStaticText.SetText(pInvItem->NameComplex());
		
		m_pCurrentItem = pInvItem;
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;
	}
	else if(msg == CUIDragDropItem::ITEM_DB_CLICK)
	{
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		UIStaticText.SetText(pInvItem->NameComplex());

		m_pCurrentItem = pInvItem;
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;

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
    }
	//�� ������� ������ ������ 
	else if(msg == CUIDragDropItem::ITEM_RBUTTON_CLICK)
	{
		PIItem pInvItem = (PIItem)((CUIDragDropItem*)pWnd)->GetData();
		UIStaticText.SetText(pInvItem->NameComplex());
		
		m_pCurrentItem = pInvItem;
		m_pCurrentDragDropItem = (CUIDragDropItem*)pWnd;

		int x,y;
		RECT rect = GetAbsoluteRect();
		HUD().GetUI()->GetCursor()->GetPos(x,y);
		
		UIPropertiesBox.BringAllToTop();
		UIPropertiesBox.Show(x-rect.left, y-rect.top);
	}
	//��������� �� ���� ����������� ������ �������
	else if(pWnd == &UIPropertiesBox &&
			msg == CUIPropertiesBox::PROPERTY_CLICKED)
	{
		switch(UIPropertiesBox.GetClickedIndex())
		{
		case 0:	//�������� ������
			DropItem();
			break;
		case 1:
			EatItem();
			break;
		}
	}
	else if(pWnd == &UIButton1 && msg == CUIButton::BUTTON_CLICKED)
	{
		CActor *l_pA = dynamic_cast<CActor*>(Level().CurrentEntity());
		
		if(l_pA)
		{
			l_pA->Sleep(1.f/6.f);
		}
	}
	else if(pWnd == &UIButton2 && msg == CUIButton::BUTTON_CLICKED)
	{
		CActor *l_pA = dynamic_cast<CActor*>(Level().CurrentEntity());
		
		if(l_pA)
		{
			l_pA->Sleep(1.f);
		}
	}
	else if(pWnd == &UIButton3 && msg == CUIButton::BUTTON_CLICKED)
	{
		CActor *l_pA = dynamic_cast<CActor*>(Level().CurrentEntity());
		
		if(l_pA)
		{
			l_pA->Sleep(1.f/60.f);
		}
	}
	else if(pWnd == &UIButton4 && msg == CUIButton::BUTTON_CLICKED)
	{
		CActor *l_pA = dynamic_cast<CActor*>(Level().CurrentEntity());
		
		if(l_pA)
		{
			l_pA->ChangeHealth(l_pA->m_fMedkit);
			l_pA->ChangeBleeding(l_pA->m_fMedkitWound);
		}
	}
	else if(pWnd == &UIButton5 && msg == CUIButton::BUTTON_CLICKED)
	{
		CActor *l_pA = dynamic_cast<CActor*>(Level().CurrentEntity());
		
		if(l_pA)
		{
			l_pA->ChangeRadiation(-l_pA->m_fAntirad);
		}
	}
	else if(pWnd == &UIButton6 && msg == CUIButton::BUTTON_CLICKED)
	{
		CActor *l_pA = dynamic_cast<CActor*>(Level().CurrentEntity());
		
		if(l_pA)
		{
			l_pA->ChangeSatiety(0.1f);
		}
	}


	CUIWindow::SendMessage(pWnd, msg, pData);
}


void CUIInventoryWnd::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	//����� ��������������� ���� �� ������ ������
	if(mouse_action == RBUTTON_DOWN)
	{
		if(UIPropertiesBox.IsShown())
		/*{
			UIPropertiesBox.Show(x,y);
		}
		else*/
		{
			UIPropertiesBox.Hide();
		}
	}

	CUIWindow::OnMouse(x, y, mouse_action);
}

void CUIInventoryWnd::Draw()
{
	static float y_rotate_angle = 0;

	y_rotate_angle += -0.01f;
   	if(y_rotate_angle>2*PI) y_rotate_angle = 0;

	UI3dStatic.SetGameObject(m_pCurrentItem);
	UI3dStatic.SetRotate(0,y_rotate_angle,0);
	
	CUIWindow::Draw();
}


//��� �������� �� ������� � �������
void CUIInventoryWnd::AmmoUpdateProc(CUIDragDropItem* pItem)
{
	CWeaponAmmo* pAmmoItem = (CWeaponAmmo*)(pItem->GetData());
	RECT rect = pItem->GetAbsoluteRect();
	
		pItem->GetFont()->Out(float(rect.left), 
							float(rect.bottom - pItem->GetFont()->CurrentHeight()- 2),
							"%d",	pAmmoItem->m_boxCurr);
}
void CUIInventoryWnd::Update()
{
	CActor *l_pA = dynamic_cast<CActor*>(Level().CurrentEntity());

	if(l_pA) 
	{
		UIProgressBarHealth.SetProgressPos(s16(l_pA->GetHealth()*1000));
		UIProgressBarSatiety.SetProgressPos(s16(l_pA->GetSatiety()*1000));
		UIProgressBarPower.SetProgressPos(s16(l_pA->GetPower()*1000));
		UIProgressBarRadiation.SetProgressPos(s16(l_pA->GetRadiation()*1000));
	}

	CUIWindow::Update();
}

void CUIInventoryWnd::DropItem()
{
	CActor *l_pA = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(!l_pA) return;

	m_pCurrentItem->Drop();
	
	/*NET_Packet P;
	l_pA->u_EventGen(P,GE_OWNERSHIP_REJECT,l_pA->ID());
	P.w_u16(u16(m_pCurrentItem->ID()));
	l_pA->u_EventSend(P);*/

	
	m_pCurrentDragDropItem->GetParent()->DetachChild(m_pCurrentDragDropItem);

	m_pCurrentItem = NULL;
	m_pCurrentDragDropItem = NULL;
}

void CUIInventoryWnd::EatItem()
{
	CActor *l_pA = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(!l_pA) return;

	if(l_pA->m_inventory.Eat(m_pCurrentItem))
	{
		m_pCurrentDragDropItem->GetParent()->DetachChild(m_pCurrentDragDropItem);
		m_pCurrentItem = NULL;
		m_pCurrentDragDropItem = NULL;
	}

	/*m_pCurrentItem->Drop();
	
	NET_Packet P;
	l_pA->u_EventGen(P,GE_DESTROY,m_pCurrentItem->ID());
	P.w_u16(u16(m_pCurrentItem->ID()));
	l_pA->u_EventSend(P);*/

	
	

}


