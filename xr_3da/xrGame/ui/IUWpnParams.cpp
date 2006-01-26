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
	float val = pSettings->r_float(wpn_section,"rpm");

	val = val > 200 ? pow((val - 200.0f)/720.0f, 2.0f)*100.0f + 15 : val/14;
	if (val > 100)
		val = 100;

	m_progressRPM.SetProgressPos(static_cast<s16>(iFloor(val)));

	// DAMAGE
	val = pSettings->r_float(wpn_section,"hit_power");

	val/=3;
	if (val > 100)
		val = 100;
	
	m_progressDamage.SetProgressPos(static_cast<s16>(iFloor(val)));

	// ACCURACY
	float cam_dispersion_frac = pSettings->r_float(wpn_section,"cam_dispertion_frac");
	float fire_dispersion_base = pSettings->r_float(wpn_section,"fire_dispersion_base");
	float cam_dispersion_inc = pSettings->r_float(wpn_section,"cam_dispersion_inc");
	float cam_dispersion = pSettings->r_float(wpn_section,"cam_dispersion");
	float cam_step_angle_horz = pSettings->r_float(wpn_section,"cam_step_angle_horz");
	char ammo[128];
	_GetItem(pSettings->r_string(wpn_section,"ammo_class"), 0, ammo);
	float k_disp =  float(atof(ammo));

    val = 100.0f*pow(1.0f - cam_dispersion_frac, 1.5f) + 0.1f/pow(fire_dispersion_base*k_disp,2.0f) 
		- 30.0f*pow(fire_dispersion_base*k_disp,0.5f) + 50.0f*(cam_dispersion_inc + 0.5f*cam_dispersion 
		+ cam_dispersion*pow(cam_step_angle_horz,2.0f));
	m_progressAccuracy.SetProgressPos(static_cast<s16>(iFloor(val)));



	// HANDLING
	float control_inertion_factor;
	if (pSettings->line_exist(wpn_section, "control_inertion_factor"))
		control_inertion_factor = pSettings->r_float(wpn_section,"control_inertion_factor");
	else
		control_inertion_factor = 1;

	float PDM_disp_base = pSettings->r_float(wpn_section,"PDM_disp_base");
	float PDM_disp_vel_factor = pSettings->r_float(wpn_section,"PDM_disp_vel_factor");	

	if (control_inertion_factor < 2 && PDM_disp_base < 1.5){
		val = (1.2f - pow(PDM_disp_vel_factor - 0.5f, 2.0f) + pow(2.0f - control_inertion_factor,2.0f))*50.0f;
	}
	else{
		val = 15.0f - PDM_disp_base + control_inertion_factor*2.0f;
	}

	if (val > 100)
		val = 100;
	if (val < 1 )
		val = 1;


	m_progressHandling.SetProgressPos(static_cast<s16>(iFloor(val)));
	
}