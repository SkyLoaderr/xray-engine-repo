#include "stdafx.h"
#include "UIMainIngameWnd.h"
#include "UICarPanel.h"

const LPCSTR CAR_PANEL_XML = "car_panel.xml";
const LPCSTR POINTER_ARROW_TEX = "ui\\hud_map_arrow";

void CUICarPanel::Init			(int x, int y, int width, int height)
{
	CUIXml uiXml;
	bool result = uiXml.Init(CONFIG_PATH, UI_PATH, CAR_PANEL_XML);
	R_ASSERT3(result, "xml file not found", CAR_PANEL_XML);

	CUIXmlInit	xml_init;
	////////////////////////////////////////////////////////////////////
	AttachChild(&UIStaticCarHealth);
	xml_init.InitStatic(uiXml, "car_health_static", 0, &UIStaticCarHealth);

	UIStaticCarHealth.AttachChild(&UICarHealthBar);
	xml_init.InitProgressBar(uiXml, "car_health_progress_bar", 0, &UICarHealthBar);
	SetCarHealth(1.0f);

/*
	AttachChild(&UISpeedometer);
	xml_init.InitStatic(uiXml, "speedometer", 0, &UISpeedometer);
	UISpeedometer.InitPointer(POINTER_ARROW_TEX, 0, 0, M_PI*1.f/3.f, -M_PI*1.f/3.f);
	SetSpeed(0.3f);

	AttachChild(&UITachometer);
	xml_init.InitStatic(uiXml, "tachometer", 0, &UITachometer);
	UITachometer.InitPointer(POINTER_ARROW_TEX,  0, 0, M_PI*1.f/3.f, -M_PI*1.f/3.f);
	SetSpeed(0.3f);
*/
	Show(false);
	Enable(false);



	inherited::Init(x,y, width, height);
}

//////////////////////////////////////////////////////////////////////////

void CUICarPanel::SetCarHealth(float value)
{
	s16 pos = static_cast<s16>(value * 100);
	clamp<s16>(pos, 0, 100);
	UICarHealthBar.SetProgressPos(pos);
}

//////////////////////////////////////////////////////////////////////////

void CUICarPanel::SetSpeed(float speed)
{
//	clamp(speed,0.f,1.f);
//	UISpeedometer.SetValue(speed);
}

//////////////////////////////////////////////////////////////////////////

void CUICarPanel::SetRPM(float rpm)
{
//	clamp(rpm,0.f,1.f);
//	UITachometer.SetValue(rpm);
}