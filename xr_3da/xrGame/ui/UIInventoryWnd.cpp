// UIInventoryWnd.h:  диалог инвентаря
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIInventoryWnd.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIInventoryWnd::CUIInventoryWnd()
{
}

CUIInventoryWnd::~CUIInventoryWnd()
{
}

void CUIInventoryWnd::Init()
{
	
	/*CUIFrameWindow		UIBagWnd;
	CUIFrameWindow		UIDescWnd;
	CUIFrameWindow		UIPersonalWnd;

	CUIButton			UIButton1;
	CUIButton			UIButton2;

	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBelt;
	CUIStatic			UIStaticBottom;
	
	CUIStatic			UIStaticDesc;
	CUIStatic			UIStaticPersonal;

	CUIDragDropList		UITopList1; 
	CUIDragDropList		UITopList2; 
	CUIDragDropList		UITopList3;
	CUIDragDropList		UITopList4;
	CUIDragDropList		UITopList5;
	CUIDragDropList		UIBagList;
	CUIDragDropList		UIBeltList;*/

	CUIWindow::Init(0,0, Device.dwWidth, Device.dwHeight);


	AttachChild(&UIStaticTop);
	UIStaticTop.Init("ui\\ui_inv_quick_slots", 0,0,1024,128);
	AttachChild(&UIStaticBottom);
	UIStaticBottom.Init("ui\\ui_bottom_background", 0,Device.dwHeight-32,1024,32);

	AttachChild(&UIStaticBelt);
	UIStaticBelt.Init("ui\\ui_inv_belt", 0,140,1024,128);

	
	AttachChild(&UIBagWnd);
	UIBagWnd.Init("ui\\ui_frame", 20, 260, 400, 480);
	UIBagWnd.InitLeftTop("ui\\ui_inv_list_over_lt", 15,20);
	UIBagWnd.InitLeftBottom("ui\\ui_frame_over_lb", 19,155);


	AttachChild(&UIDescWnd);
	UIDescWnd.Init("ui\\ui_frame", 440, 260, 267, 460);
	UIDescWnd.InitLeftTop("ui\\ui_inv_info_over_lt", 5,10);
	
	UIDescWnd.AttachChild(&UIStaticDesc);
	UIStaticDesc.Init("ui\\ui_inv_info_over_b", 
						5, UIDescWnd.GetHeight() - 310 ,260,260);




	AttachChild(&UIPersonalWnd);
	UIPersonalWnd.Init("ui\\ui_frame", 730, 260, 268, 460);
	UIPersonalWnd.InitLeftTop("ui\\ui_inv_personal_over_t", 5,10);

	UIPersonalWnd.AttachChild(&UIStaticPersonal);
	UIStaticPersonal.Init("ui\\ui_inv_personal_over_b", 
						-3,UIPersonalWnd.GetHeight() - 209 ,260,260);


	//кнопки внизу
	AttachChild(&UIButton1);
	UIButton1.Init("ui\\ui_button_01", 20, Device.dwHeight-50,150,50);
	UIButton1.SetText("Inventory");
	
	AttachChild(&UIButton2);
	UIButton2.Init("ui\\ui_button_02", 200, Device.dwHeight-50,150,50);
	UIButton2.SetText("Map");


	//Списки Drag&Drop
	UIBagWnd.AttachChild(&UIBagList);
	UIBagList.Init(0+20,0+30, UIBagWnd.GetWidth(),UIBagWnd.GetHeight());

	UIBagList.SetCellWidth(50);
	UIBagList.SetCellHeight(50);
	UIBagList.InitGrid(8,7);


	AttachChild(&UIBeltList);
	UIBeltList.Init( 0+22,140+34,1024,128);

	UIBeltList.SetCellWidth(53);
	UIBeltList.SetCellHeight(50);
	UIBeltList.InitGrid(1,18);

	AttachChild(&UITopList1);
	UITopList1.Init( 23,14,120,120);
	UITopList1.SetCellWidth(53);
	UITopList1.SetCellHeight(53);
	UITopList1.InitGrid(2,2);

	AttachChild(&UITopList2);
	UITopList2.Init( 168,14,120,120);
	UITopList2.SetCellWidth(53);
	UITopList2.SetCellHeight(53);
	UITopList2.InitGrid(2,2);
		
	AttachChild(&UITopList3);
	UITopList3.Init(313,14,320,120);
	UITopList3.SetCellWidth(53);
	UITopList3.SetCellHeight(53);
	UITopList3.InitGrid(2,6);

	AttachChild(&UITopList4);
	UITopList4.Init( 670,14,120,120);
	UITopList4.SetCellWidth(53);
	UITopList4.SetCellHeight(53);
	UITopList4.InitGrid(2,2);

	AttachChild(&UITopList5);
	UITopList5.Init( 818,14,120,170);
	UITopList5.SetCellWidth(53);
	UITopList5.SetCellHeight(53);
	UITopList5.InitGrid(2,3);

	//элементы Drag&Drop
	UIDragDropItem1.Init("ui\\ui_button_02", 10,10, 150,50);
	UIDragDropItem1.SetGridHeight(1);
	UIDragDropItem1.SetGridWidth(3);
	UIBagList.AttachChild(&UIDragDropItem1);


	UIDragDropItem2.SetGridHeight(1);
	UIDragDropItem2.SetGridWidth(3);
	UIDragDropItem2.Init("ui\\ui_button_02", 10,10, 150,50);
	UIBagList.AttachChild(&UIDragDropItem2);

	UIDragDropItem3.Init("ui\\ui_button_02", 10,10, 150,50);
	UIDragDropItem3.SetGridHeight(1);
	UIDragDropItem3.SetGridWidth(3);
	UIBagList.AttachChild(&UIDragDropItem3);


	UIDragDropItem4.SetGridHeight(1);
	UIDragDropItem4.SetGridWidth(3);
	UIDragDropItem4.Init("ui\\ui_button_02", 10,10, 150,50);
	UIBagList.AttachChild(&UIDragDropItem4);

	UIDragDropItem5.Init("ui\\hud_health_back", 10,10,100,100);
	UIDragDropItem5.SetGridHeight(2);
	UIDragDropItem5.SetGridWidth(2);
	UIBagList.AttachChild(&UIDragDropItem5);

	UIDragDropItem6.Init("ui\\hud_health_back", 10,10, 100,100);
	UIDragDropItem6.SetGridHeight(2);
	UIDragDropItem6.SetGridWidth(2);
	UIBagList.AttachChild(&UIDragDropItem6);


}
