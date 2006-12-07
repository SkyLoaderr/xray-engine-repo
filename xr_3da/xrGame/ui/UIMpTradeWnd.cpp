#include "stdafx.h"
#include "UIMpTradeWnd.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "UIXmlInit.h"
#include "UIBuyWeaponTab.h"
#include "UI3tButton.h"


CUIMpTradeWnd::CUIMpTradeWnd()
{
	m_rank			= 0;
	m_money			= 0;
}

CUIMpTradeWnd::~CUIMpTradeWnd()
{}

void CUIMpTradeWnd::Init(const shared_str& sectionName, const shared_str& sectionPrice)
{
	m_sectionName						= sectionName;
	m_sectionPrice						= sectionPrice;

	CUIXml								xml_doc;
	R_ASSERT							(xml_doc.Init(CONFIG_PATH, UI_PATH, "mp_buy_menu.xml"));

	CUIXmlInit::InitWindow				(xml_doc, "main",						0, this);

	m_tab_control						= xr_new<CUIBuyWeaponTab>(); AttachChild(m_tab_control); m_tab_control->SetAutoDelete(true);
	m_tab_control->Init					(&xml_doc, "tab_control");
	m_btn_ok							= xr_new<CUI3tButton>();AttachChild(m_btn_ok);		m_btn_ok->SetAutoDelete(true);
	m_btn_cancel						= xr_new<CUI3tButton>();AttachChild(m_btn_cancel);	m_btn_cancel->SetAutoDelete(true);

	CUIXmlInit::Init3tButton			(xml_doc, "btn_ok",					0, m_btn_ok);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_cancel",				0, m_btn_cancel);
	Register							(m_btn_ok);
	AddCallback							("btn_ok",	BUTTON_CLICKED,	CUIWndCallback::void_function(this, &CUIMpTradeWnd::OnBtnOkClicked));
	Register							(m_btn_cancel);
	AddCallback							("btn_cancel",BUTTON_CLICKED,CUIWndCallback::void_function(this,&CUIMpTradeWnd::OnBtnCancelClicked));

}

void CUIMpTradeWnd::OnBtnOkClicked(CUIWindow* w, void* d)
{}

void CUIMpTradeWnd::OnBtnCancelClicked(CUIWindow* w, void* d)
{}
