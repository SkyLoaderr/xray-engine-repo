#include "stdafx.h"
#include "UIMainIngameWnd.h"
#include "UICarPanel.h"

const LPCSTR CAR_PANEL_XML = "car_panel.xml";
const LPCSTR POINTER_ARROW_TEX = "ui\\hud_map_arrow";

void CUICarPanel::Init			(int x, int y, int width, int height)
{
	CUIXml uiXml;
	bool result = uiXml.Init("$game_data$", CAR_PANEL_XML);
	R_ASSERT3(result, "xml file not found", CAR_PANEL_XML);

	CUIXmlInit	xml_init;
	////////////////////////////////////////////////////////////////////
	AttachChild(&UIStaticCarHealth);
	xml_init.InitStatic(uiXml, "car_health_static", 0, &UIStaticCarHealth);

	UIStaticCarHealth.AttachChild(&UICarHealthBar);
	xml_init.InitProgressBar(uiXml, "car_health_progress_bar", 0, &UICarHealthBar);
	SetCarHealth(1.0f);


	AttachChild(&UIPointerGage);
	xml_init.InitStatic(uiXml, "speedometer", 0, &UIPointerGage);
	UIPointerGage.InitPointer(POINTER_ARROW_TEX, 0, 0, 0, 120.f);
	SetSpeed(0.3f);


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
	UIPointerGage.SetValue(speed);
}

//////////////////////////////////////////////////////////////////////////

void CUICarPanel::SetRPM(float rpm)
{
}