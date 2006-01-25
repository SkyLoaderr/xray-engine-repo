#pragma once
#include "UIWindow.h"

#include "UIProgressBar.h"

class CUIXml;

class CUIWpnParams : public CUIWindow {
public:
	CUIWpnParams();
	virtual ~CUIWpnParams();

	void InitFromXml(CUIXml& xml_doc);
	void SetInfo(const char* wnp_section);

protected:
	CUIProgressBar	m_progressAccuracy;
	CUIProgressBar	m_progressHandling;
	CUIProgressBar	m_progressDamage;
	CUIProgressBar	m_progressRPM;

	CUIStatic	m_textAccuracy;
	CUIStatic	m_textHandling;
	CUIStatic	m_textDamage;
	CUIStatic	m_textRPM;
};