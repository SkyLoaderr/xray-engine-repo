#include "stdafx.h"
#include "UIMainIngameWnd.h"
#include "UIMotionIcon.h"
#include "UIXmlInit.h"
const LPCSTR MOTION_ICON_XML = "motion_icon.xml";

CUIMotionIcon::CUIMotionIcon()
{
	m_curren_state=stLast;
}

CUIMotionIcon::~CUIMotionIcon()
{

}
void CUIMotionIcon::Init()
{
	CUIXml uiXml;
	bool result = uiXml.Init(CONFIG_PATH, UI_PATH, MOTION_ICON_XML);
	R_ASSERT3(result, "xml file not found", MOTION_ICON_XML);

	CUIXmlInit	xml_init;

	xml_init.InitStatic			(uiXml, "background", 0, this);	

	AttachChild					(&m_power_progress);
	xml_init.InitProgressBar	(uiXml, "power_progress", 0, &m_power_progress);	

	AttachChild					(&m_luminosity_progress);
	xml_init.InitProgressBar	(uiXml, "luminosity_progress", 0, &m_luminosity_progress);	

	AttachChild					(&m_noise_progress);
	xml_init.InitProgressBar	(uiXml, "noise_progress", 0, &m_noise_progress);	
	
	AttachChild					(&m_states[stNormal]);
	xml_init.InitStatic			(uiXml, "state_normal", 0, &m_states[stNormal]);
	m_states[stNormal].Show		(false);

	AttachChild					(&m_states[stCrouch]);
	xml_init.InitStatic			(uiXml, "state_crouch", 0, &m_states[stCrouch]);	
	m_states[stCrouch].Show		(false);

	AttachChild					(&m_states[stCreep]);
	xml_init.InitStatic			(uiXml, "state_creep", 0, &m_states[stCreep]);	
	m_states[stCreep].Show		(false);

	AttachChild					(&m_states[stClimb]);
	xml_init.InitStatic			(uiXml, "state_climb", 0, &m_states[stClimb]);	
	m_states[stClimb].Show		(false);

	AttachChild					(&m_states[stRun]);
	xml_init.InitStatic			(uiXml, "state_run", 0, &m_states[stRun]);	
	m_states[stRun].Show		(false);

	AttachChild					(&m_states[stSprint]);
	xml_init.InitStatic			(uiXml, "state_sprint", 0, &m_states[stSprint]);	
	m_states[stSprint].Show		(false);

	ShowState					(stNormal);
}

void CUIMotionIcon::ShowState(EState state)
{
	if(m_curren_state==state)			return;
	if(m_curren_state!=stLast)
	{
	
		m_states[m_curren_state].Show	(false);
		m_states[m_curren_state].Enable	(false);
	}
	m_states[state].Show				(true);
	m_states[state].Enable				(true);

	m_curren_state=state;
}

void CUIMotionIcon::SetPower(s16 Pos)
{
	m_power_progress.SetProgressPos(Pos);
}

void CUIMotionIcon::SetNoise(s16 Pos)
{
	Pos	= clampr(Pos, m_noise_progress.GetRange_min(), m_noise_progress.GetRange_max());
	m_noise_progress.SetProgressPos(Pos);
}

void CUIMotionIcon::SetLuminosity(s16 Pos)
{
	Pos	= clampr(Pos, m_luminosity_progress.GetRange_min(), m_luminosity_progress.GetRange_max());
	m_luminosity_progress.SetProgressPos(Pos);
}