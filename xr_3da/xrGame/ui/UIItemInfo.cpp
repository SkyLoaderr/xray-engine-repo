// UIItemInfo.cpp:  окошко, для отображения информации о вещи
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "uiiteminfo.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"


#define ROTATION_SPEED -1.0f

CUIItemInfo::CUIItemInfo()
{
	y_rotate_angle = 0;
}

CUIItemInfo::~CUIItemInfo()
{
}

void CUIItemInfo::Init(int x, int y, int width, int height, const char* xml_name)
{
	inherited::Init(x, y, width, height);

	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$",xml_name);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit xml_init;

	AttachChild(&UIName);
	xml_init.InitStatic(uiXml, "static", 0, &UIName);
	AttachChild(&UIWeight);
	xml_init.InitStatic(uiXml, "static", 1, &UIWeight);
	AttachChild(&UIDesc);
	xml_init.InitStatic(uiXml, "static", 2, &UIDesc);

	AttachChild(&UI3dStatic);
	if(uiXml.NavigateToNode("static_3d",0))	
	{
		xml_init.InitWindow(uiXml, "static_3d", 0, &UI3dStatic);
		UI3dStatic.Show(true);
		UI3dStatic.Enable(true);
	}
	else
	{
		UI3dStatic.Show(false);
		UI3dStatic.Enable(false);
	}
}
void CUIItemInfo::InitItem(CInventoryItem* pInvItem)
{
	if(pInvItem)
	{
		string256 str;
		sprintf(str, "name: %s", pInvItem->cName());
		UIName.SetText(str);

		sprintf(str, "weight: %3.2f", pInvItem->Weight());
		UIWeight.SetText(str);

		sprintf(str, "cost:%d\\ncondition:%3.2f", 
					pInvItem->Cost(), 
					pInvItem->GetCondition());
		UIDesc.SetText(str);

		UI3dStatic.SetGameObject(pInvItem);
	}
	else
	{
		UIName.SetText(NULL);
		UIWeight.SetText(NULL);
		UIDesc.SetText(NULL);
		UI3dStatic.SetGameObject(NULL);
	}
}

void CUIItemInfo::Draw()
{
	y_rotate_angle += ROTATION_SPEED*(float)Device.dwTimeDelta/1000.f;
   	if(y_rotate_angle>2*PI) y_rotate_angle = 0;
	UI3dStatic.SetRotate(0,y_rotate_angle,0);
	
	inherited::Draw();
}