#include "stdafx.h"
#include "UIMainIngameWnd.h"
#include "UIMotionIcon.h"
const LPCSTR MOTION_ICON_XML = "motion_icon.xml";

CUIMotionIcon::CUIMotionIcon()
{
	m_curren_state=stLast;
}

CUIMotionIcon::~CUIMotionIcon()
{

}
void CUIMotionIcon::Init			(int x, int y, int width, int height)
{
	CUIXml uiXml;
	bool result = uiXml.Init(CONFIG_PATH, UI_PATH, MOTION_ICON_XML);
	R_ASSERT3(result, "xml file not found", MOTION_ICON_XML);

	CUIXmlInit	xml_init;
	////////////////////////////////////////////////////////////////////
//	AttachChild(&m_states[stNormal]);
//	xml_init.InitStatic(uiXml, "normal", 0, &m_states[stNormal]);

	/*m_states[stNormal].*/AttachChild(&m_PowerBar[stNormal]);
	xml_init.InitProgressBar(uiXml, "power_bar_normal", 0, &m_PowerBar[stNormal]);	
	////////////////////////////////////////////////////////////////////
//	AttachChild(&m_states[stCrouch]);
//	xml_init.InitStatic(uiXml, "crouch", 0, &m_states[stCrouch]);

	/*m_states[stCrouch].*/AttachChild(&m_PowerBar[stCrouch]);
	xml_init.InitProgressBar(uiXml, "power_bar_crouch", 0, &m_PowerBar[stCrouch]);
	////////////////////////////////////////////////////////////////////
//	AttachChild(&m_states[stCreep]);
//	xml_init.InitStatic(uiXml, "creep", 0, &m_states[stCreep]);

	/*m_states[stCreep].*/AttachChild(&m_PowerBar[stCreep]);
	xml_init.InitProgressBar(uiXml, "power_bar_creep", 0, &m_PowerBar[stCreep]);
	////////////////////////////////////////////////////////////////////
//	AttachChild(&m_states[stClimb]);
//	xml_init.InitStatic(uiXml, "climb", 0, &m_states[stClimb]);

	/*m_states[stClimb].*/AttachChild(&m_PowerBar[stClimb]);
	xml_init.InitProgressBar(uiXml, "power_bar_climb", 0, &m_PowerBar[stClimb]);
	////////////////////////////////////////////////////////////////////

	Show(true);
	Enable(true);
	inherited::Init(x,y, width, height);
	
	for(int i=stNormal;i!=stLast;++i)
	{
//		m_states[i].Show(false);
//		m_states[i].Enable(false);
		m_PowerBar[i].Show(false);
		m_PowerBar[i].Enable(false);
	}
	ShowState(stNormal);
}

void CUIMotionIcon::ShowState(EState state)
{
	if(m_curren_state==state)return;
	if(m_curren_state!=stLast)
	{
	
		/*m_states*/m_PowerBar[m_curren_state].Show(false);
		/*m_states*/m_PowerBar[m_curren_state].Enable(false);
	}
	/*m_states*/m_PowerBar[state].Show(true);
	/*m_states*/m_PowerBar[state].Enable(true);

	m_curren_state=state;
}

void CUIMotionIcon::SetProgressPos(s16 Pos)
{
	m_PowerBar[m_curren_state].SetProgressPos(Pos);
}