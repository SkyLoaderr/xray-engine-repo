#include "stdafx.h"
#include "UIMpTradeWnd.h"
#include "UIXmlInit.h"

#include "UIDragDropListEx.h"
#include "UIMpItemsStoreWnd.h"
#include "restrictions.h"
#include "UITabButton.h"
#include "UITabButtonMP.h"
#include "UITabControl.h"
#include "../object_broker.h"

CUIMpTradeWnd::CUIMpTradeWnd()
{
	m_rank			= 0;
	m_money			= 0;
	g_mp_restrictions.InitGroups	();
}

CUIMpTradeWnd::~CUIMpTradeWnd()
{
	m_root_tab_control->RemoveAll	();
	delete_data						(m_store_hierarchy);
	delete_data						(m_list[e_bag]);
}

void CUIMpTradeWnd::Init(const shared_str& sectionName, const shared_str& sectionPrice)
{
	m_sectionName						= sectionName;
	m_sectionPrice						= sectionPrice;

	CUIXml								xml_doc;
	R_ASSERT							(xml_doc.Init(CONFIG_PATH, UI_PATH, "mp_buy_menu.xml"));

	m_store_hierarchy					= xr_new<CStoreHierarchy>();
 	m_store_hierarchy->Init				(xml_doc, "items_hierarchy");
	m_store_hierarchy->InitItemsInGroup	(m_sectionName);

	CUIXmlInit::InitWindow				(xml_doc, "main",						0, this);

	m_root_tab_control					= xr_new<CUITabControl>(); AttachChild(m_root_tab_control); m_root_tab_control->SetAutoDelete(true);
	CUIXmlInit::InitTabControl			(xml_doc, "tab_control",				0, m_root_tab_control);
	Register							(m_root_tab_control);
	AddCallback							("tab_control",	TAB_CHANGED,		CUIWndCallback::void_function	(this, &CUIMpTradeWnd::OnRootTabChanged));

	u32 root_cnt = m_store_hierarchy->GetRoot().ChildCount();
	for(u32 i=0; i<root_cnt; ++i)
	{
		const CStoreHierarchy::item& it	= m_store_hierarchy->GetRoot().ChildAt(i);

		CUITabButtonMP* btn				= it.m_button;	
		m_root_tab_control->AddItem		(btn);
		btn->SetAutoDelete				(false);
	}
	m_root_tab_control->ResetTab		();

	m_shop_wnd							= xr_new<CUIWindow>();	AttachChild(m_shop_wnd);		m_shop_wnd->SetAutoDelete(true);
	CUIXmlInit::InitWindow				(xml_doc, "shop_wnd",	0, m_shop_wnd);

	m_btn_ok							= xr_new<CUI3tButton>();AttachChild(m_btn_ok);		m_btn_ok->SetAutoDelete(true);
	m_btn_cancel						= xr_new<CUI3tButton>();AttachChild(m_btn_cancel);	m_btn_cancel->SetAutoDelete(true);
	m_btn_shop_back						= xr_new<CUI3tButton>();AttachChild(m_btn_shop_back);m_btn_shop_back->SetAutoDelete(true);

	CUIXmlInit::Init3tButton			(xml_doc, "btn_ok",					0, m_btn_ok);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_cancel",				0, m_btn_cancel);
	CUIXmlInit::Init3tButton			(xml_doc, "shop_back_btn",			0, m_btn_shop_back);

	Register							(m_btn_ok);
	AddCallback							("btn_ok",			BUTTON_CLICKED,		CUIWndCallback::void_function	(this, &CUIMpTradeWnd::OnBtnOkClicked));
	Register							(m_btn_cancel);
	AddCallback							("btn_cancel",		BUTTON_CLICKED,		CUIWndCallback::void_function	(this,&CUIMpTradeWnd::OnBtnCancelClicked));
	Register							(m_btn_shop_back);
	AddCallback							("btn_shop_back",	BUTTON_CLICKED,		CUIWndCallback::void_function	(this, &CUIMpTradeWnd::OnBtnShopBackClicked));
	AddCallback							("sub_btn",			TAB_CHANGED,		CUIWndCallback::void_function	(this, &CUIMpTradeWnd::OnSubLevelBtnClicked));


	LPCSTR _list_names[]= {
			"lst_pistol",
			"lst_pistol_ammo",
			"lst_rifle",
			"lst_rifle_ammo",
			"lst_outfit",
			"lst_medkit",
			"lst_granade",
			"lst_others",
			"lst_bag"
	};
	for(int idx = e_first; idx<e_total_lists; ++idx)
	{
		CUIDragDropListEx* lst				= xr_new<CUIDragDropListEx>();
		m_list[idx]							= lst;
		if(idx!=e_bag)
		{
			AttachChild						(lst);
			lst->SetAutoDelete				(true);
		}
		CUIXmlInit::InitDragDropListEx		(xml_doc, _list_names[idx], 0, lst);
	}

	m_static_money							= xr_new<CUIStatic>(); AttachChild(m_static_money); m_static_money->SetAutoDelete(true);
	CUIXmlInit::InitStatic					(xml_doc, "static_money",					0, m_static_money);
	UpdateShop								();
}

void CUIMpTradeWnd::OnBtnOkClicked(CUIWindow* w, void* d)
{}

void CUIMpTradeWnd::OnBtnCancelClicked(CUIWindow* w, void* d)
{}

void CUIMpTradeWnd::OnBtnShopBackClicked(CUIWindow* w, void* d)
{
	m_store_hierarchy->MoveUp	();
	UpdateShop					();
}

void CUIMpTradeWnd::OnRootTabChanged(CUIWindow* w, void* d)
{
	int curr					= m_root_tab_control->GetActiveIndex();
	m_store_hierarchy->Reset	();
	m_store_hierarchy->MoveDown	(curr);
	
	UpdateShop					();
}

void CUIMpTradeWnd::OnSubLevelBtnClicked(CUIWindow* w, void* d)
{
	CUITabButtonMP* btn			= smart_cast<CUITabButtonMP*>(w);

	u32 curr					= btn->m_temp_index;
	m_store_hierarchy->MoveDown	(curr);
	
	UpdateShop					();
}

void CUIMpTradeWnd::UpdateShop()
{
	m_shop_wnd->DetachAll					();
	

	bool b_matched_root						= m_store_hierarchy->CurrentIsRoot();
	m_btn_shop_back->Enable					( !b_matched_root );
	if(b_matched_root)
		m_root_tab_control->ResetTab		();

	Msg("current level=[%s]",m_store_hierarchy->CurrentLevel().m_name.c_str());
	if(m_store_hierarchy->CurrentIsRoot())	return;

	if(m_store_hierarchy->CurrentLevel().HasSubLevels())
	{	//show sub-levels
		FillUpSubLevelButtons	();
	}else
	{//show items
		FillUpSubLevelItems		();
	}
}

void CUIMpTradeWnd::FillUpSubLevelButtons()
{
	u32 root_cnt = m_store_hierarchy->CurrentLevel().ChildCount();

	Fvector2		pos;
	pos.set			(40.0f,90.0f);

	for(u32 i=0; i<root_cnt; ++i)
	{
		const CStoreHierarchy::item& it		= m_store_hierarchy->CurrentLevel().ChildAt(i);
		CUITabButtonMP* btn					= it.m_button;
		btn->m_temp_index					= i;
		Register							(btn);
		btn->SetWndPos						(pos);
		pos.add								(btn->GetWndSize().y);
		pos.y								+= 40.0f;
		pos.x								= 40.0f;
		m_shop_wnd->AttachChild				(btn);
	}
}

void CUIMpTradeWnd::FillUpSubLevelItems()
{
	CUIDragDropListEx*	pList	= m_list[e_bag];
	m_shop_wnd->AttachChild		(pList);
	pList->ClearAll				(false);
}
