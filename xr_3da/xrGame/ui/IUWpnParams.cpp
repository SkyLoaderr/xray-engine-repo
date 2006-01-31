#include "StdAfx.h"

#include "UIWpnParams.h"
#include "UIXmlInit.h"
#include "xrXmlParser.h"
#include "../level.h"
#include "../game_base_space.h"
#include "../ai_space.h"
#include "../script_engine.h"

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

	bool	functor_exists;
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetRPM" ,m_functorRPM);			VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetDamage" ,m_functorDamage);		VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetDamageMP" ,m_functorDamageMP);	VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetHandling" ,m_functorHandling);	VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetAccuracy" ,m_functorAccuracy);	VERIFY(functor_exists);
}

void CUIWpnParams::SetInfo(const char* wpn_section){

	m_progressRPM.SetProgressPos(static_cast<s16>(m_functorRPM(wpn_section)));
	m_progressAccuracy.SetProgressPos(static_cast<s16>(m_functorAccuracy(wpn_section)));
	if (GameID() == GAME_SINGLE)
        m_progressDamage.SetProgressPos(static_cast<s16>(m_functorDamage(wpn_section)));
	else
		m_progressDamage.SetProgressPos(static_cast<s16>(m_functorDamageMP(wpn_section)));
	m_progressHandling.SetProgressPos(static_cast<s16>(m_functorHandling(wpn_section)));

	//// RPM
	//float rpm = pSettings->r_float(wpn_section,"rpm");

	//float val = rpm > 200 ? pow((rpm - 200.0f)/720.0f, 2.0f)*100.0f + 15 : rpm/14;
	//if (val > 100)
	//	val = 100;

	//m_progressRPM.SetProgressPos(static_cast<s16>(iFloor(val)));

	//// DAMAGE
	//val = pSettings->r_float(wpn_section,"hit_power");
	//if (GameID() == GAME_SINGLE)
	//{
	//	if (val < 150)
	//		val = val/2;
	//	else
	//		val = 75 + pow(val/400,2)*25;
	//}
	//else
	//{
	//	val = 75*(1 + log(val/100));
	//}
	//if (val < 0)
	//	val = 1;
	//if (val > 100)
	//	val = 100;

	//m_progressDamage.SetProgressPos(static_cast<s16>(iFloor(val)));
	//////////////////////////////////////////////////////////////////	

	//// ACCURACY
	//float cam_dispersion_frac;
	//if (pSettings->line_exist(wpn_section, "cam_dispertion_frac"))
 //       cam_dispersion_frac = pSettings->r_float(wpn_section,"cam_dispertion_frac");
	//else 
	//	cam_dispersion_frac = 0.7f;
	//float fire_dispersion_base = pSettings->r_float(wpn_section,"fire_dispersion_base");
	//float cam_dispersion_inc = pSettings->r_float(wpn_section,"cam_dispersion_inc");
	//float cam_dispersion = pSettings->r_float(wpn_section,"cam_dispersion");
	//float cam_step_angle_horz = pSettings->r_float(wpn_section,"cam_step_angle_horz");
	//char ammo[128];
	//_GetItem(pSettings->r_string(wpn_section,"ammo_class"), 0, ammo);
	//float k_disp =  pSettings->r_float(ammo, "k_disp");

 //   val = (0.05f*rpm*log(1/(cam_dispersion/10.0f + cam_dispersion_inc)) 
	//	+ 30.0f*log(1/(cam_dispersion*cam_step_angle_horz))
	//	)*pow(cam_dispersion_frac,2.0f) 
	//	+ 50.0f*log(1/(fire_dispersion_base*k_disp));

	//m_progressAccuracy.SetProgressPos(static_cast<s16>(iFloor(val)));



	//// HANDLING
	//float control_inertion_factor;
	//if (pSettings->line_exist(wpn_section, "control_inertion_factor"))
	//	control_inertion_factor = pSettings->r_float(wpn_section,"control_inertion_factor");
	//else
	//	control_inertion_factor = 1;

	//float PDM_disp_base = pSettings->r_float(wpn_section,"PDM_disp_base");
	//float PDM_disp_vel_factor = pSettings->r_float(wpn_section,"PDM_disp_vel_factor");	

	//if (control_inertion_factor < 2 && PDM_disp_base < 1.5){
	//	val = (1.2f - pow(PDM_disp_vel_factor - 0.5f, 2.0f) + pow(2.0f - control_inertion_factor,2.0f))*50.0f;
	//}
	//else{
	//	val = 15.0f - (PDM_disp_base + control_inertion_factor);
	//}

	//if (val > 100)
	//	val = 100;
	//if (val < 1 )
	//	val = 1;


	//m_progressHandling.SetProgressPos(static_cast<s16>(iFloor(val)));
	
}