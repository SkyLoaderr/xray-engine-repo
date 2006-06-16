#include "stdafx.h"

#include "UIWpnParams.h"
#include "uiiteminfo.h"
#include "uistatic.h"
#include "UIXmlInit.h"

#include "UIListWnd.h"
#include "UIProgressBar.h"
#include "UIScrollView.h"

#include "../string_table.h"
#include "../Inventory_Item.h"
#include "UIInventoryUtilities.h"
#include "../PhysicsShellHolder.h"

CUIItemInfo::CUIItemInfo()
{
	UIItemImageSize.set			(0.0f,0.0f);
	UICondProgresBar			= NULL;
	UICondition					= NULL;
	UIWpnParams					= NULL;
	m_pInvItem					= NULL;
	m_b_force_drawing			= false;
}

CUIItemInfo::~CUIItemInfo()
{
	xr_delete(UIWpnParams); //UIWpnParams must be without "autodelete"
}

void CUIItemInfo::Init(LPCSTR xml_name){
	UIName			= xr_new<CUIStatic>();	 AttachChild(UIName);		UIName->SetAutoDelete(true);
	UIWeight		= xr_new<CUIStatic>();	 AttachChild(UIWeight);		UIWeight->SetAutoDelete(true);
	UICost			= xr_new<CUIStatic>();	 AttachChild(UICost);		UICost->SetAutoDelete(true);
	UIItemImage		= xr_new<CUIStatic>();	 AttachChild(UIItemImage);	UIItemImage->SetAutoDelete(true);

	UIWpnParams		= xr_new<CUIWpnParams>();//must be without "autodelete"
	UIDesc			= xr_new<CUIScrollView>(); AttachChild(UIDesc);		UIDesc->SetAutoDelete(true);

		CUIXml uiXml;
	bool xml_result			= uiXml.Init(CONFIG_PATH, UI_PATH, xml_name);
	R_ASSERT2				(xml_result, "xml file not found");

	CUIXmlInit					xml_init;

	UIWpnParams->InitFromXml(uiXml);

	xml_init.InitStatic			(uiXml, "static_name", 0,			UIName);
	xml_init.InitStatic			(uiXml, "static_weight", 0,			UIWeight);
	xml_init.InitStatic			(uiXml, "static_cost", 0,			UICost);

	if(uiXml.NavigateToNode("static_condition",0)){
		UICondition					= xr_new<CUIStatic>();	 
		AttachChild(UICondition);
		UICondition->SetAutoDelete(true);
		xml_init.InitStatic			(uiXml, "static_condition", 0,		UICondition);
	}

	if(uiXml.NavigateToNode("condition_progress",0)){
		UICondProgresBar= xr_new<CUIProgressBar>(); AttachChild(UICondProgresBar);UICondProgresBar->SetAutoDelete(true);
		xml_init.InitProgressBar(uiXml, "condition_progress", 0, UICondProgresBar);
	}

	xml_init.InitScrollView			(uiXml, "descr_list", 0, UIDesc);

	if (uiXml.NavigateToNode("image_static", 0))
	{	
		xml_init.InitStatic			(uiXml, "image_static", 0, UIItemImage);
		UIItemImage->TextureAvailable(true);
	}
	UIItemImage->TextureOff			();
	UIItemImage->ClipperOn			();
	UIItemImageSize.set				(UIItemImage->GetWidth(),UIItemImage->GetHeight());

	xml_init.InitAutoStaticGroup	(uiXml, "auto", this);
}

void CUIItemInfo::Init(float x, float y, float width, float height, LPCSTR xml_name)
{
	inherited::Init(x, y, width, height);
    Init(xml_name);
}

void CUIItemInfo::InitItem(CInventoryItem* pInvItem)
{
	m_pInvItem			= pInvItem;
	if(!m_pInvItem)		return;

	string256			str;
	UIName->SetText		(pInvItem->Name());

	sprintf				(str, "%3.2f kg", pInvItem->Weight());
	UIWeight->SetText	(str);

	sprintf				(str, "%d RU", pInvItem->Cost());
	UICost->SetText		(str);
	
	float cond = pInvItem->GetCondition();

	//sprintf				(str, "%3.2f", cond);
	//
	//if(UICondition)
	//	UICondition->SetText(str);

	if(UICondProgresBar){
		UICondProgresBar->Show				(true);
		UICondProgresBar->SetProgressPos	( s16(iFloor(cond*100.0f+1.0f-EPS)) );
	}

	UIDesc->Clear						();
	VERIFY(0==UIDesc->GetSize());
	TryAddWpnInfo(*pInvItem->object().cNameSect());
	CUIStatic* pItem					= xr_new<CUIStatic>();
	pItem->SetWidth						(UIDesc->GetDesiredChildWidth());
	pItem->SetText						(*pInvItem->ItemDescription());
	pItem->AdjustHeightToText			();
	UIDesc->AddWindow					(pItem, true);
	UIDesc->ScrollToBegin				();

	// Загружаем картинку
	UIItemImage->SetShader				(InventoryUtilities::GetEquipmentIconsShader());

	int iGridWidth						= pInvItem->GetGridWidth();
	int iGridHeight						= pInvItem->GetGridHeight();
	int iXPos							= pInvItem->GetXPos();
	int iYPos							= pInvItem->GetYPos();

	UIItemImage->GetUIStaticItem().SetOriginalRect(	float(iXPos*INV_GRID_WIDTH), float(iYPos*INV_GRID_HEIGHT),
													float(iGridWidth*INV_GRID_WIDTH),	float(iGridHeight*INV_GRID_HEIGHT));
	UIItemImage->TextureOn	();
	UIItemImage->ClipperOn	();
	UIItemImage->SetStretchTexture(true);
	Frect v_r				= {0.0f, 0.0f, float(iGridWidth*INV_GRID_WIDTH),	float(iGridHeight*INV_GRID_HEIGHT)};
	UIItemImage->GetUIStaticItem().SetRect(v_r);
	UIItemImage->SetWidth	(_min(v_r.width(),	UIItemImageSize.x));
	UIItemImage->SetHeight	(_min(v_r.height(),	UIItemImageSize.y));
//	UIItemImage->InitTexture("ui\\ui_debug_red_n_black");
	
}

void CUIItemInfo::TryAddWpnInfo (LPCSTR wpn_section){
	if (UIWpnParams->Check(wpn_section))
	{
		UIWpnParams->SetInfo(wpn_section);
		UIDesc->AddWindow(UIWpnParams,false);
	}
}

void CUIItemInfo::Draw()
{
	if(m_pInvItem || m_b_force_drawing)
		inherited::Draw();
}
