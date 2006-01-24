#include "StdAfx.h"

#include "UIWpnParams.h"
#include "UIXmlInit.h"
#include "xrXmlParser.h"

CUIWpnParams::CUIWpnParams(){
	AttachChild(&m_textAccuracy);
	AttachChild(&m_textDamage);
	AttachChild(&m_textDistance);
	AttachChild(&m_textRPM);

	AttachChild(&m_progressAccuracy);
	AttachChild(&m_progressDamage);
	AttachChild(&m_progressDistance);
	AttachChild(&m_progressRPM);
}

CUIWpnParams::~CUIWpnParams(){

}

void CUIWpnParams::InitFromXml(CUIXml& xml_doc){
	CUIXmlInit::InitWindow(xml_doc, "wpn_params", 0, this);

	CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_accuracy",	0, &m_textAccuracy);
	CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_damage",	0, &m_textDamage);
	CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_distance",	0, &m_textDistance);
	CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_rpm",		0, &m_textRPM);

	CUIXmlInit::InitProgressBar2(xml_doc, "wpn_params:progress_accuracy",	0, &m_progressAccuracy);
	CUIXmlInit::InitProgressBar2(xml_doc, "wpn_params:progress_damage",		0, &m_progressDamage);
	CUIXmlInit::InitProgressBar2(xml_doc, "wpn_params:progress_distance",	0, &m_progressDistance);
	CUIXmlInit::InitProgressBar2(xml_doc, "wpn_params:progress_rpm",		0, &m_progressRPM);

	m_progressAccuracy.SetRange(0, 100);
	m_progressDamage.SetRange(0, 100);
	m_progressDistance.SetRange(0, 100);
	m_progressRPM.SetRange(0, 100);
}

void CUIWpnParams::SetInfo(const char* wpn_section){
	return;

	// RPM
	float min = pSettings->r_float("weapon_params_info", "min_rpm");
	float max = pSettings->r_float("weapon_params_info", "max_rpm");
	float val = pSettings->r_float(wpn_section,"rpm");
	float r = max - min;
	m_progressRPM.SetProgressPos(static_cast<s16>(100 - iFloor(((max - val)/r)*100)));

	// damage
	min = pSettings->r_float("weapon_params_info","min_damage");
	max = pSettings->r_float("weapon_params_info","max_damage");
	val = pSettings->r_float(wpn_section,"hit_power");
	r = max - min;
	m_progressDamage.SetProgressPos(static_cast<s16>(100 - iFloor(((max - val)/r)*100)));

	// accuracy
	min = pSettings->r_float("weapon_params_info","min_dispersion");
	max = pSettings->r_float("weapon_params_info","max_dispersion");
	val = pSettings->r_float(wpn_section,"fire_dispersion_base");
	r = max - min;
	m_progressAccuracy.SetProgressPos(static_cast<s16>(iFloor(((max - val)/r)*100)));

	
//	m_progressDamage.SetProgressPos(100);
	m_progressDistance.SetProgressPos(60);
	
}