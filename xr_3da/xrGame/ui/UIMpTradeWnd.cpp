#include "stdafx.h"
#include "UIMpTradeWnd.h"

#include "UIMpItemsStoreWnd.h"
#include "UITabButtonMP.h"
#include "UITabControl.h"
#include "UIDragDropListEx.h"
#include "UICellItem.h"

#include "../UIDialogHolder.h"
#include "../game_cl_deathmatch.h"


void CUIMpTradeWnd::OnBtnOkClicked(CUIWindow* w, void* d)
{
	StorePreset							(_preset_idx_last);
	GetHolder()->StartStopMenu			(this,true);
	game_cl_Deathmatch * dm				= smart_cast<game_cl_Deathmatch *>(&(Game()));
	dm->OnBuyMenu_Ok					();
}

void CUIMpTradeWnd::OnBtnCancelClicked(CUIWindow* w, void* d)
{
	GetHolder()->StartStopMenu			(this,true);
}

void CUIMpTradeWnd::OnBtnShopBackClicked(CUIWindow* w, void* d)
{
	m_store_hierarchy->MoveUp			();
	UpdateShop							();
}

#include "../../xr_input.h"
void CUIMpTradeWnd::OnBtnPreset1Clicked(CUIWindow* w, void* d)
{
	if(pInput->iGetAsyncKeyState(DIK_LSHIFT))
	{
		DumpPreset					(_preset_idx_1);
		return;
	}

	ApplyPreset					(_preset_idx_1);
}

void CUIMpTradeWnd::OnBtnPreset2Clicked(CUIWindow* w, void* d)
{
	if(pInput->iGetAsyncKeyState(DIK_LSHIFT))
	{
		DumpPreset					(_preset_idx_2);
		return;
	};
	ApplyPreset					(_preset_idx_2);
}

void CUIMpTradeWnd::OnBtnPreset3Clicked(CUIWindow* w, void* d)
{
	if(pInput->iGetAsyncKeyState(DIK_LSHIFT))
	{
		DumpPreset					(_preset_idx_3);
		return;
	};

	ApplyPreset					(_preset_idx_3);
}

void CUIMpTradeWnd::OnBtnLastSetClicked(CUIWindow* w, void* d)
{
	if(pInput->iGetAsyncKeyState(DIK_LSHIFT))
	{
		DumpPreset					(_preset_idx_last);
		return;
	};
		ApplyPreset					(_preset_idx_last);
}

void CUIMpTradeWnd::OnBtnSave1PresetClicked(CUIWindow* w, void* d)
{
	StorePreset					(_preset_idx_1);
}

void CUIMpTradeWnd::OnBtnSave2PresetClicked(CUIWindow* w, void* d)
{
	StorePreset					(_preset_idx_2);
}

void CUIMpTradeWnd::OnBtnSave3PresetClicked(CUIWindow* w, void* d)
{
	StorePreset					(_preset_idx_3);
}

void CUIMpTradeWnd::OnBtnResetClicked(CUIWindow* w, void* d)
{
	ApplyPreset				(_preset_idx_origin); //origin
}

void CUIMpTradeWnd::OnRootTabChanged(CUIWindow* w, void* d)
{
	int curr							= m_root_tab_control->GetActiveIndex();
	m_store_hierarchy->Reset			();
	m_store_hierarchy->MoveDown			(curr);
	
	UpdateShop							();
}

void CUIMpTradeWnd::OnSubLevelBtnClicked(CUIWindow* w, void* d)
{
	CUITabButtonMP* btn					= smart_cast<CUITabButtonMP*>(w);

	u32 curr							= btn->m_temp_index;
	m_store_hierarchy->MoveDown			(curr);
	
	UpdateShop							();
}

void CUIMpTradeWnd::UpdateShop()
{
	m_shop_wnd->DetachAll				();
	

	bool b_matched_root					= m_store_hierarchy->CurrentIsRoot();
	m_btn_shop_back->Enable				( !b_matched_root );
	if(b_matched_root)
		m_root_tab_control->ResetTab	();

	Msg									("current level=[%s]",m_store_hierarchy->CurrentLevel().m_name.c_str());
	if(m_store_hierarchy->CurrentIsRoot())	return;

	if(m_store_hierarchy->CurrentLevel().HasSubLevels())
	{//show sub-levels
		FillUpSubLevelButtons			();
	}else
	{//show items
		FillUpSubLevelItems				();
	}
}

void CUIMpTradeWnd::FillUpSubLevelButtons()
{
	u32 root_cnt						= m_store_hierarchy->CurrentLevel().ChildCount();

	Fvector2							pos;
	pos.set								(40.0f,90.0f);

	for(u32 i=0; i<root_cnt; ++i)
	{
		const CStoreHierarchy::item& it	= m_store_hierarchy->CurrentLevel().ChildAt(i);
		CUITabButtonMP* btn				= it.m_button;
		btn->m_temp_index				= i;
		Register						(btn);
		btn->SetWndPos					(pos);
		pos.add							(btn->GetWndSize().y);
		pos.y							+= 40.0f;
		pos.x							= 40.0f;
		m_shop_wnd->AttachChild			(btn);
	}
}

void CUIMpTradeWnd::FillUpSubLevelItems()
{
	CUIDragDropListEx*	pList			= m_list[e_shop];
	m_shop_wnd->AttachChild				(pList);
	pList->ClearAll						(false);
	
	const CStoreHierarchy::item& curr_level = m_store_hierarchy->CurrentLevel();

	for(u32 idx=0; idx<curr_level.m_items_in_group.size();++idx)
	{
		const shared_str& sect			= curr_level.m_items_in_group[idx];
		RenewShopItem					(sect, false);
	}
}

#include "../actor.h"
void CUIMpTradeWnd::Show()
{
	m_pMouseCapturer		= NULL;
	inherited::Show			();


	CActor *pActor			= smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor) 
		pActor->SetWeaponHideState(INV_STATE_BUY_MENU, true);
}

void CUIMpTradeWnd::Hide()
{
	inherited::Hide			();

	CActor *pActor			= smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor)
		pActor->SetWeaponHideState(INV_STATE_BUY_MENU, false);
}