// UIItemInfo.cpp:  окошко, для отображения информации о вещи
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "uiiteminfo.h"
#include "../string_table.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

//////////////////////////////////////////////////////////////////////////

const char * const 		fieldsCaptionColor		= "%c216,186,140";

//////////////////////////////////////////////////////////////////////////

CUIItemInfo::CUIItemInfo()
{
//	y_rotate_angle = 0;
}

//////////////////////////////////////////////////////////////////////////

CUIItemInfo::~CUIItemInfo()
{
}

//////////////////////////////////////////////////////////////////////////

void CUIItemInfo::Init(int x, int y, int width, int height, const char* xml_name)
{
	inherited::Init(x, y, width, height);

	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$",xml_name);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit xml_init;

	AttachChild(&UIName);
	xml_init.InitStatic(uiXml, "static", 0, &UIName);
	UIName.Enable(false);
	UIName.SetElipsis(CUIStatic::eepEnd, 3);
	AttachChild(&UIWeight);
	xml_init.InitStatic(uiXml, "static", 1, &UIWeight);
	UIWeight.Enable(false);
	AttachChild(&UICost);
	xml_init.InitStatic(uiXml, "static", 2, &UICost);
	UICost.Enable(false);
	AttachChild(&UICondition);
	xml_init.InitStatic(uiXml, "static", 3, &UICondition);
	UICondition.Enable(false);

	AttachChild(&UIDesc);
	xml_init.InitListWnd(uiXml, "descr_list", 0, &UIDesc);
	UIDesc.EnableScrollBar(true);
	UIDesc.ActivateList(false);

	if (uiXml.NavigateToNode("image_static", 0))
	{
		AttachChild(&UIItemImage);
		xml_init.InitStatic(uiXml, "image_static", 0, &UIItemImage);
		UIItemImage.TextureAvailable(true);
	}
	UIItemImage.TextureOff();
	UIItemImage.ClipperOn();
}

//////////////////////////////////////////////////////////////////////////

void CUIItemInfo::InitItem(CInventoryItem* pInvItem)
{
	if(pInvItem)
	{
		CStringTable stbl;

		string256 str;
		sprintf(str, "%s", pInvItem->Name());
		UIName.SetText(str);

		sprintf(str, "%s%s: %%cdefault%3.2f", fieldsCaptionColor, *stbl("weight"), pInvItem->Weight());
		UIWeight.SetText(str);

		sprintf(str, "%s%s: %%cdefault%d", fieldsCaptionColor, *stbl("base cost"), pInvItem->Cost());
		UICost.SetText(str);

		sprintf(str, "%s%s: %%cdefault%3.2f", fieldsCaptionColor, *stbl("condition"), pInvItem->GetCondition());
		UICondition.SetText(str);

		UIDesc.RemoveAll();
		// Добавляем текст
		CUIString str2;
		str2.SetText(*pInvItem->ItemDescription());
		CUIStatic::PreprocessText(str2.m_str, UIDesc.GetItemWidth() - 5, UIDesc.GetFont());
		UIDesc.AddParsedItem<CUIListItem>(str2, 0, UIDesc.GetTextColor());

		// Загружаем картинку
		UIItemImage.SetShader(InventoryUtilities::GetEquipmentIconsShader());

		int m_iGridWidth	= pSettings->r_u32(pInvItem->cNameSect(), "inv_grid_width");
		int m_iGridHeight	= pSettings->r_u32(pInvItem->cNameSect(), "inv_grid_height");

		int m_iXPos			= pSettings->r_u32(pInvItem->cNameSect(), "inv_grid_x");
		int m_iYPos			= pSettings->r_u32(pInvItem->cNameSect(), "inv_grid_y");

		UIItemImage.GetUIStaticItem().SetOriginalRect(
			m_iXPos * INV_GRID_WIDTH,
			m_iYPos * INV_GRID_HEIGHT,
			m_iGridWidth * INV_GRID_WIDTH,
			m_iGridHeight * INV_GRID_HEIGHT);
		UIItemImage.TextureOn();
		UIItemImage.ClipperOn();

		// Разместить текстуру по центру статика
		Irect	r	= UIItemImage.GetUIStaticItem().GetOriginalRect();
		RECT	r2	= UIItemImage.GetWndRect();

		if ((r2.right - r2.left >= r.width()) && (r2.bottom - r2.top >= r.height()))
		{
			UIItemImage.SetTextureOffset((r2.right - r2.left - r.width()) / 2, (r2.bottom - r2.top - r.height()) / 2);
			UIItemImage.SetTextureScale(1.0f);
		}
		else
		{
			float xFactor = (r2.right - r2.left) / static_cast<float>(r.width()) ;
			float yFactor = (r2.bottom - r2.top) / static_cast<float>(r.height());
			float scale = std::min(xFactor, yFactor);

			int xOffset = 0, yOffset = 0;

			if (xFactor > yFactor)
			{
				xOffset = (r2.right - r2.left - r.width()) / 2;
			}
			else
			{
				yOffset = (r2.bottom - r2.top - r.height()) / 2;
			}

			UIItemImage.SetTextureOffset(xOffset, yOffset);
			UIItemImage.SetTextureScale(scale);
		}

//		UIItemImage.SetTextureOffset((r.right - r.left - m_iGridWidth * INV_GRID_WIDTH) / 2,
//									 (r.bottom - r.top - m_iGridHeight* INV_GRID_HEIGHT) / 2);
	}
	else
	{
		UIName.SetText(NULL);
		UIWeight.SetText(NULL);
		UICost.SetText(NULL);
		UICondition.SetText(NULL);
		UIDesc.RemoveAll();
		UIItemImage.TextureOff();
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIItemInfo::Draw()
{
//	y_rotate_angle += ROTATION_SPEED*(float)Device.dwTimeDelta/1000.f;
//   	if(y_rotate_angle>2*PI) y_rotate_angle = 0;
//	UI3dStatic.SetRotate(0,y_rotate_angle,0);
	
	inherited::Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUIItemInfo::AlignRight(CUIStatic &Item, int offset)
{
	if (Item.GetFont())
	{
		float	s	= Item.GetFont()->SizeOf(Item.GetText());
				s	-= Item.GetFont()->SizeOf(fieldsCaptionColor);
				s	-= Item.GetFont()->SizeOf("%cdefault");
		Item.MoveWindow(static_cast<int>(GetWndRect().right - s) + offset, Item.GetWndRect().top);
	}
}
