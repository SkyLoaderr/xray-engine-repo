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
	AttachChild(&m_states[stNormal]);
	xml_init.InitStatic(uiXml, "normal", 0, &m_states[stNormal]);
	
	////////////////////////////////////////////////////////////////////
	AttachChild(&m_states[stCrouch]);
	xml_init.InitStatic(uiXml, "crouch", 0, &m_states[stCrouch]);

	////////////////////////////////////////////////////////////////////
	AttachChild(&m_states[stCreep]);
	xml_init.InitStatic(uiXml, "creep", 0, &m_states[stCreep]);

	AttachChild(&m_states[stClimb]);
	xml_init.InitStatic(uiXml, "climb", 0, &m_states[stClimb]);

	Show(true);
	Enable(true);
	inherited::Init(x,y, width, height);
	
	for(int i=stNormal;i!=stLast;++i)
	{
		m_states[i].Show(false);
		m_states[i].Enable(false);
	}
	ShowState(stNormal);
}

void CUIMotionIcon::ShowState(EState state)
{
	if(m_curren_state==state)return;
	if(m_curren_state!=stLast)
	{
	
		m_states[m_curren_state].Show(false);
		m_states[m_curren_state].Enable(false);
	}
	m_states[state].Show(true);
	m_states[state].Enable(true);

	m_curren_state=state;
}