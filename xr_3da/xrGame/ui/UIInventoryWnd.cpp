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
	UIBagWnd.Init("ui\\ui_frame", 20, 260, 400, 460);
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


	//элементы Drag&Drop
	UIBagWnd.AttachChild(&UIBagList);
	UIBagList.Init("ui\\ui_frame", 0,0, UIBagWnd.GetWidth(),UIBagWnd.GetHeight());

/*	UIBagList.SetCellWidth(50);
	UIBagList.SetCellHeight(50);
	UIBagList.InitGrid(8,7);


	UIDragDropItem1.Init("ui\\ui_button_02", 10,10, 150,50);
	UIDragDropItem1.SetGridHeight(1);
	UIDragDropItem1.SetGridWidth(3);
	UIBagList.AttachChild(&UIDragDropItem1);


	UIDragDropItem2.SetGridHeight(1);
	UIDragDropItem2.SetGridWidth(3);
	UIDragDropItem2.Init("ui\\ui_button_02", 10,10, 150,50);
	UIBagList.AttachChild(&UIDragDropItem2);*/
}
