#pragma once

#include "UIWindow.h"
#include "UIStatic.h"

class CUIXml;
class CUIColorAnimatorWrapper;

class CUIMoneyIndicator: public CUIWindow {
public:
	CUIMoneyIndicator();
	virtual ~CUIMoneyIndicator();
	virtual void Update();
			void InitFromXML(CUIXml& xml_doc);
			void SetMoneyAmount(LPCSTR money);
			void SetMoneyChange(LPCSTR money);
			void SetMoneyBonus(LPCSTR money);
protected:
	CUIStatic	m_back;
	CUIStatic	m_money_amount;
	CUIStatic	m_money_change;
	CUIStatic	m_money_bonus;

	CUIColorAnimatorWrapper* m_pAnimChange;
	CUIColorAnimatorWrapper* m_pAnimBonus;
};