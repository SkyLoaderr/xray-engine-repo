// CUIInventoryWnd.h:  диалог инвентаря
// 
//////////////////////////////////////////////////////////////////////

#pragma once


#include "UIStatic.h"
#include "UIButton.h"
#include "UIDragDropItem.h"
#include "UIDragDropList.h"
#include "UIProgressBar.h"



class CUIInventoryWnd: public CUIWindow  
{
public:
	CUIInventoryWnd();
	virtual ~CUIInventoryWnd();

	virtual void Init();


protected:

	CUIFrameWindow		UIBagWnd;
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
	CUIDragDropList		UIBeltList;


	CUIDragDropItem		UIDragDropItem1;
	CUIDragDropItem		UIDragDropItem2;

	
};