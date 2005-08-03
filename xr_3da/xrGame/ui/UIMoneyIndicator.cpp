#include "stdafx.h"
#include "UIMoneyIndicator.h"
#include "UIColorAnimatorWrapper.h"
#include "xrXmlParser.h"
#include "UIXmlInit.h"

CUIMoneyIndicator::CUIMoneyIndicator(){
	AttachChild(&m_back);
	AttachChild(&m_money_amount);
	AttachChild(&m_money_change);
	AttachChild(&m_money_bonus);
	m_pAnimChange = xr_new<CUIColorAnimatorWrapper>("ui_mp_chat");
	m_pAnimChange->Cyclic(false);
	m_pAnimChange->SetDone(true);
	m_pAnimBonus = xr_new<CUIColorAnimatorWrapper>("ui_mp_chat");
	m_pAnimBonus->Cyclic(false);
	m_pAnimBonus->SetDone(true);
}

CUIMoneyIndicator::~CUIMoneyIndicator(){

}

void CUIMoneyIndicator::InitFromXML(CUIXml& xml_doc){
	CUIXmlInit::InitWindow(xml_doc, "money_wnd", 0,	this);
	CUIXmlInit::InitStatic(xml_doc, "money_wnd:money_indicator",0, &m_back);
	CUIXmlInit::InitStatic(xml_doc, "money_wnd:money_indicator:total_money",0, &m_money_amount);
	CUIXmlInit::InitStatic(xml_doc, "money_wnd:money_change", 0, &m_money_change);
	CUIXmlInit::InitStatic(xml_doc, "money_wnd:money_bonus", 0, &m_money_bonus);
	m_money_change.SetVisible(false);
	m_money_bonus.SetVisible(false);
}

void CUIMoneyIndicator::SetMoneyAmount(LPCSTR money){
	m_money_amount.SetText(money);
}

void CUIMoneyIndicator::SetMoneyChange(LPCSTR money){
	m_money_change.SetText(money);
	m_money_change.SetVisible(true);
	m_pAnimChange->Reset();
}

void CUIMoneyIndicator::SetMoneyBonus(LPCSTR money){
	m_money_bonus.SetText(money);
	m_money_bonus.SetVisible(true);
	m_pAnimBonus->Reset();
}

void CUIMoneyIndicator::Update(){
	if (m_money_change.GetVisible())
        if (!m_pAnimChange->Done())
		{
			m_pAnimChange->Update();
			m_money_change.SetTextColor(subst_alpha(m_money_change.GetTextColor(), m_pAnimChange->GetColor()));
		}
		else
			m_money_change.SetVisible(false);

	if (m_money_bonus.GetVisible())
		if (!m_pAnimBonus->Done())
		{
			m_pAnimBonus->Update();
			m_money_bonus.SetTextColor(subst_alpha(m_money_change.GetTextColor(), m_pAnimBonus->GetColor()));
		}
		else
			m_money_bonus.SetVisible(false);
}

