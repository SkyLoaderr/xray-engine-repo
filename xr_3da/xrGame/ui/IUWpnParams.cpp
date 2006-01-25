#include "StdAfx.h"

#include "UIWpnParams.h"
#include "UIXmlInit.h"
#include "xrXmlParser.h"

CUIWpnParams::CUIWpnParams(){
	AttachChild(&m_textAccuracy);
	AttachChild(&m_textDamage);
	AttachChild(&m_textHandling);
	AttachChild(&m_textRPM);

	AttachChild(&m_progressAccuracy);
	AttachChild(&m_progressDamage);
	AttachChild(&m_progressHandling);
	AttachChild(&m_progressRPM);
}

CUIWpnParams::~CUIWpnParams(){

}

void CUIWpnParams::InitFromXml(CUIXml& xml_doc){
	CUIXmlInit::InitWindow(xml_doc, "wpn_params", 0, this);

	CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_accuracy",	0, &m_textAccuracy);
	CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_damage",	0, &m_textDamage);
	CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_handling",	0, &m_textHandling);
	CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_rpm",		0, &m_textRPM);

	CUIXmlInit::InitProgressBar2(xml_doc, "wpn_params:progress_accuracy",	0, &m_progressAccuracy);
	CUIXmlInit::InitProgressBar2(xml_doc, "wpn_params:progress_damage",		0, &m_progressDamage);
	CUIXmlInit::InitProgressBar2(xml_doc, "wpn_params:progress_handling",	0, &m_progressHandling);
	CUIXmlInit::InitProgressBar2(xml_doc, "wpn_params:progress_rpm",		0, &m_progressRPM);

	m_progressAccuracy.SetRange(0, 100);
	m_progressDamage.SetRange(0, 100);
	m_progressHandling.SetRange(0, 100);
	m_progressRPM.SetRange(0, 100);
}

void CUIWpnParams::SetInfo(const char* wpn_section){
//	return;

	// RPM
//	float min = pSettings->r_float("weapon_params_info", "min_rpm");
//	float max = pSettings->r_float("weapon_params_info", "max_rpm");
	float val = pSettings->r_float(wpn_section,"rpm");

	val = val > 200 ? pow((val - 200.0f)/720.0f, 2.0f)*100.0f + 15 : pow(val,0.5f);
	if (val > 100)
		val = 100;

	m_progressRPM.SetProgressPos(static_cast<s16>(iFloor(val)));

	// damage
//	min = pSettings->r_float("weapon_params_info","min_damage");
//	max = pSettings->r_float("weapon_params_info","max_damage");
	val = pSettings->r_float(wpn_section,"hit_power");

	val/=3;
	if (val > 100)
		val = 100;
	
	m_progressDamage.SetProgressPos(static_cast<s16>(iFloor(val)));

	// accuracy
	float min = 0.1f; //pSettings->r_float("weapon_params_info","min_dispersion");
	float max = 2.0f; //pSettings->r_float("weapon_params_info","max_dispersion");
	val = pSettings->r_float(wpn_section,"fire_dispersion_base");
	float r = max - min;
	m_progressAccuracy.SetProgressPos(static_cast<s16>(iFloor(((max - val)/r)*100)));

	float control_inertion_factor;
	if (pSettings->line_exist(wpn_section, "control_inertion_factor"))
		control_inertion_factor = pSettings->r_float(wpn_section,"control_inertion_factor");
	else
		control_inertion_factor = 1;

	float PDM_disp_base = pSettings->r_float(wpn_section,"PDM_disp_base");
	float PDM_disp_vel_factor = pSettings->r_float(wpn_section,"PDM_disp_vel_factor");	

	if (control_inertion_factor < 2 && PDM_disp_base < 2){
		val = (1 - pow(PDM_disp_vel_factor - 0.55f, 2.0f) + pow(2 - control_inertion_factor,2))*50;
	}
	else{
		val = (25 - PDM_disp_base*control_inertion_factor);
	}

	if (val > 100)
		val = 100;
	if (val < 1 )
		val = 1;


	m_progressHandling.SetProgressPos(static_cast<s16>(iFloor(val)));
	
}