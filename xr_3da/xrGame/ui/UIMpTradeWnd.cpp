#include "stdafx.h"
#include "UIMpTradeWnd.h"
#include "UIXmlInit.h"

#include "UIDragDropListEx.h"
#include "UIMpItemsStoreWnd.h"
#include "restrictions.h"
#include "UITabButton.h"
#include "UITabButtonMP.h"
#include "UITabControl.h"
#include "../UIDialogHolder.h"
#include "../object_broker.h"
#include "../level.h"
#include "../game_cl_deathmatch.h"


LPCSTR _list_names[]= {
		"lst_pistol",
		"lst_pistol_ammo",
		"lst_rifle",
		"lst_rifle_ammo",
		"lst_outfit",
		"lst_medkit",
		"lst_granade",
		"lst_others",
		"lst_shop",
		"lst_player_bag"
};

CUIMpTradeWnd::CUIMpTradeWnd()
{
	m_money								= 0;
	g_mp_restrictions.InitGroups		();
}

CUIMpTradeWnd::~CUIMpTradeWnd()
{
	m_root_tab_control->RemoveAll		();
	delete_data							(m_store_hierarchy);
	delete_data							(m_list[e_shop]);
	delete_data							(m_all_items);
	delete_data							(m_item_mngr);
	delete_data							(m_preset_mngr);
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

	u32 root_cnt						= m_store_hierarchy->GetRoot().ChildCount();
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

	m_btn_ok							= xr_new<CUI3tButton>();AttachChild(m_btn_ok);					m_btn_ok->SetAutoDelete			(true);
	m_btn_cancel						= xr_new<CUI3tButton>();AttachChild(m_btn_cancel);				m_btn_cancel->SetAutoDelete		(true);
	m_btn_shop_back						= xr_new<CUI3tButton>();AttachChild(m_btn_shop_back);			m_btn_shop_back->SetAutoDelete	(true);
	m_btn_preset_1						= xr_new<CUI3tButton>();AttachChild(m_btn_preset_1	);			m_btn_preset_1->SetAutoDelete	(true);
	m_btn_preset_2						= xr_new<CUI3tButton>();AttachChild(m_btn_preset_2	);			m_btn_preset_2->SetAutoDelete	(true);
	m_btn_preset_3						= xr_new<CUI3tButton>();AttachChild(m_btn_preset_3	);			m_btn_preset_3->SetAutoDelete	(true);
	m_btn_last_set						= xr_new<CUI3tButton>();AttachChild(m_btn_last_set	);			m_btn_last_set->SetAutoDelete	(true);
	m_btn_save_preset					= xr_new<CUI3tButton>();AttachChild(m_btn_save_preset);			m_btn_save_preset->SetAutoDelete(true);
	m_btn_reset							= xr_new<CUI3tButton>();AttachChild(m_btn_reset		);			m_btn_reset->SetAutoDelete		(true);
	m_btn_sell							= xr_new<CUI3tButton>();AttachChild(m_btn_sell		);			m_btn_sell->SetAutoDelete		(true);

	CUIXmlInit::Init3tButton			(xml_doc, "btn_ok",				0, m_btn_ok				);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_cancel",			0, m_btn_cancel			);
	CUIXmlInit::Init3tButton			(xml_doc, "shop_back_btn",		0, m_btn_shop_back		);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_preset_1",		0, m_btn_preset_1		);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_preset_2",		0, m_btn_preset_2		);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_preset_3",		0, m_btn_preset_3		);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_last_set",		0, m_btn_last_set		);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_save_preset",	0, m_btn_save_preset	);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_reset",			0, m_btn_reset			);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_sell",			0, m_btn_sell			);

	Register							(m_btn_ok);
	Register							(m_btn_cancel);
	Register							(m_btn_shop_back);
	Register							(m_btn_preset_1		);
	Register							(m_btn_preset_2		);
	Register							(m_btn_preset_3		);
	Register							(m_btn_last_set		);
	Register							(m_btn_save_preset	);
	Register							(m_btn_reset		);
	Register							(m_btn_sell			);

	AddCallback							("btn_ok",			BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnOkClicked));
	AddCallback							("btn_cancel",		BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnCancelClicked));
	AddCallback							("btn_shop_back",	BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnShopBackClicked));
	AddCallback							("sub_btn",			TAB_CHANGED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnSubLevelBtnClicked));
	AddCallback							("sub_btn",			BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnSubLevelBtnClicked));
	AddCallback							("btn_preset_1",	BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnPreset1Clicked		));
	AddCallback							("btn_preset_2",	BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnPreset2Clicked		));
	AddCallback							("btn_preset_3",	BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnPreset3Clicked		));
	AddCallback							("btn_last_set",	BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnLastSetClicked		));
	AddCallback							("btn_save_preset",	BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnSavePresetClicked	));
	AddCallback							("btn_reset",		BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnResetClicked		));
	AddCallback							("btn_sell",		BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnSellClicked		));


	for(int idx = e_first; idx<e_total_lists; ++idx)
	{
		CUIDragDropListEx* lst			= xr_new<CUIDragDropListEx>();
		m_list[idx]						= lst;
		if(idx!=e_shop)
		{
			AttachChild					(lst);
			lst->SetAutoDelete			(true);
		}
		CUIXmlInit::InitDragDropListEx	(xml_doc, _list_names[idx], 0, lst);
		BindDragDropListEvents			(lst, true);
	}

	m_static_money						= xr_new<CUIStatic>(); AttachChild(m_static_money); m_static_money->SetAutoDelete(true);
	CUIXmlInit::InitStatic				(xml_doc, "static_money",					0, m_static_money);

	m_static_current_item				= xr_new<CUIStatic>(); AttachChild(m_static_current_item); m_static_current_item->SetAutoDelete(true);
	CUIXmlInit::InitStatic				(xml_doc, "static_current_item",			0, m_static_current_item);

	m_static_rank						= xr_new<CUIStatic>(); AttachChild(m_static_rank); m_static_rank->SetAutoDelete(true);
	CUIXmlInit::InitStatic				(xml_doc, "static_rank",					0, m_static_rank);

	m_static_information				= xr_new<CUIStatic>(); AttachChild(m_static_information); m_static_information->SetAutoDelete(true);
	CUIXmlInit::InitStatic				(xml_doc, "static_info",					0, m_static_information);

	m_item_mngr							= xr_new<CItemMgr>();
	m_item_mngr->Load					(sectionPrice);
	m_item_mngr->Dump					();
	m_preset_mngr						= xr_new<CPresetMgr>();
	UpdateShop							();
	SetCurrentItem						(NULL);
}

void CUIMpTradeWnd::OnBtnOkClicked(CUIWindow* w, void* d)
{
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

void CUIMpTradeWnd::OnBtnPreset1Clicked(CUIWindow* w, void* d)
{
	bool b = m_btn_save_preset->GetCheck();
	if(b)
	{
		m_btn_save_preset->SetCheck	(false);
		m_preset_mngr->StorePreset	(1,this);
	}
}

void CUIMpTradeWnd::OnBtnPreset2Clicked(CUIWindow* w, void* d)
{
	bool b = m_btn_save_preset->GetCheck();
	if(b)
	{
		m_btn_save_preset->SetCheck	(false);
		m_preset_mngr->StorePreset	(2,this);
	}
}

void CUIMpTradeWnd::OnBtnPreset3Clicked(CUIWindow* w, void* d)
{
	bool b = m_btn_save_preset->GetCheck();
	if(b)
	{
		m_btn_save_preset->SetCheck	(false);
		m_preset_mngr->StorePreset	(3,this);
	}
}

void CUIMpTradeWnd::OnBtnLastSetClicked(CUIWindow* w, void* d)
{}

void CUIMpTradeWnd::OnBtnSavePresetClicked(CUIWindow* w, void* d)
{
	bool b = m_btn_save_preset->GetCheck();

	if(b)
		SetInfoString			("ui_st_select_preset_to_store_to");
	else
		SetInfoString			("ui_st_store_preset_canceled");
}

void CUIMpTradeWnd::OnBtnResetClicked(CUIWindow* w, void* d)
{
	ITEMS_vec_it it			= m_all_items.begin();
	ITEMS_vec_it it_e		= m_all_items.end();

	for(;it!=it_e;++it)
	{
		SBuyItemInfo* info = *it;
		Msg("[%s] - state[%s]",info->m_name_sect.c_str(),info->GetStateAsText());
	}

}

void CUIMpTradeWnd::OnBtnSellClicked(CUIWindow* w, void* d)
{}

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
	{	//show sub-levels
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
		const shared_str& sect			= curr_level.m_items_in_group	[idx];
		RenewShopItem					(sect, false);
//.		SBuyItemInfo* pitem				= CreateItem	(sect, SBuyItemInfo::e_shop);
//.		pList->SetItem					(pitem->m_cell_item);
	}
}

void CItemMgr::Load(const shared_str& sect_cost)
{
	CInifile::Sect &sect = pSettings->r_section(sect_cost);

	u32 idx	=0;
	for (CInifile::SectIt it = sect.begin(); it != sect.end(); ++it,++idx)
	{
		_i&		val			= m_items[it->first]; 
		val.slot_idx		= 0xff;
		int c = sscanf		(it->second.c_str(),"%d,%d,%d,%d,%d",&val.cost[0],&val.cost[1],&val.cost[2],&val.cost[3],&val.cost[4]);
		VERIFY				(c>0);

		while(c<_RANK_COUNT)
		{
			val.cost[c]	= val.cost[c-1];
			++c;
		}
	}
	
	for(u8 i=CUIMpTradeWnd::e_first; i<CUIMpTradeWnd::e_total_lists; ++i)
	{
		const shared_str& buff		= pSettings->r_string("buy_menu_items_place", _list_names[i]);
		
		u32 cnt						= _GetItemCount(buff.c_str());
		string1024					_one;

		for(u32 c=0; c<cnt; ++c)
		{
			_GetItem				(buff.c_str(), c, _one);
			shared_str _one_str		= _one;
			COST_MAP_IT it			= m_items.find(_one_str);
			R_ASSERT				(it!=m_items.end());
			R_ASSERT3				(it->second.slot_idx==0xff,"item has duplicate record in [buy_menu_items_place] section ",_one);
			it->second.slot_idx		= i;
		}
	}
}

const u32	CItemMgr::GetItemCost(const shared_str& sect_name, u32 rank) const
{
	COST_MAP_CIT it		= m_items.find(sect_name);
	VERIFY				(it!=m_items.end());
	return				it->second.cost[rank];
}

const u8 CItemMgr::GetItemSlotIdx	(const shared_str& sect_name) const
{
	COST_MAP_CIT it		= m_items.find(sect_name);
	VERIFY				(it!=m_items.end());
	return				it->second.slot_idx;
}

void CItemMgr::Dump() const
{
	COST_MAP_CIT it		= m_items.begin();
	COST_MAP_CIT it_e	= m_items.end();

	Msg("--CItemMgr::Dump");
	for(;it!=it_e;++it)
	{
		const _i&		val		= it->second; 
		R_ASSERT3		(it->second.slot_idx!=0xff,"item has no record in [buy_menu_items_place] section ",it->first.c_str());
		Msg				("[%s] slot=[%d] cost= %d,%d,%d,%d,%d",it->first.c_str(),val.slot_idx,val.cost[0],val.cost[1],val.cost[2],val.cost[3],val.cost[4]);
	}

}

CPresetMgr::CPresetMgr()
{
}

const CPresetMgr::items& CPresetMgr::GetPreset(u32 idx)
{
	VERIFY			(idx<4); 
	return			m_storage[idx];
};

#include "../string_table.h"
void CPresetMgr::StorePreset(u32 idx, CUIMpTradeWnd* source)
{
	string512			buff;
	sprintf				(buff,"%s [%d]",CStringTable().translate("ui_st_preset_stored_to").c_str(), idx);
	source->SetInfoString(buff);
}
