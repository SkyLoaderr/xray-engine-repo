#include "stdafx.h"
#include "UIMainIngameWnd.h"
#include "UICarPanel.h"

void CUICarPanel::Init(CUIMainIngameWnd* wnd, CUIXml& uiXml,CUIXmlInit& xml_init)
{

	////////////////////////////////////////////////////////////////////
	wnd->AttachChild(&UIStaticCarHealth);
	xml_init.InitStatic(uiXml, "car_health_static", 0, &UIStaticCarHealth);

	UIStaticCarHealth.AttachChild(&UICarHealthBar);
	xml_init.InitProgressBar(uiXml, "car_health_progress_bar", 0, &UICarHealthBar);
	ShowCarHealth(false);
	SetCarHealth(1.0f);
}

void CUICarPanel::ShowCarHealth(bool on)
{
	UIStaticCarHealth.Show(on);
}

//////////////////////////////////////////////////////////////////////////

void CUICarPanel::SetCarHealth(float value)
{
	s16 pos = static_cast<s16>(value * 100);
	clamp<s16>(pos, 0, 100);
	UICarHealthBar.SetProgressPos(pos);
}