//=============================================================================
//  Filename:   UIJobItem.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Ёлемент листа дл€ задани€
//=============================================================================

#include "StdAfx.h"
#include "UIJobItem.h"
#include "UIXmlInit.h"

//////////////////////////////////////////////////////////////////////////

const char * const		JOB_ITEM_XML		= "job_item.xml";

//////////////////////////////////////////////////////////////////////////

CUIJobItem::CUIJobItem(int leftOffest)
	:	m_id				(0),
		articleTypeMsg		(PDA_OPEN_ENCYCLOPEDIA_ARTICLE)
{
	// Initialize internal structures
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", JOB_ITEM_XML);
	R_ASSERT3(xml_result, "xml file not found", JOB_ITEM_XML);

	CUIXmlInit xml_init;

	// Picture
	AttachChild(&UIPicture);
	xml_init.InitStatic(uiXml, "picture_static", 0, &UIPicture);
	UIPicture.MoveWindow(UIPicture.GetWndRect().left + leftOffest, UIPicture.GetWndRect().top);

	// Caption
	AttachChild(&UICaption);
	xml_init.InitStatic(uiXml, "caption_static", 0, &UICaption);
	UICaption.MoveWindow(UICaption.GetWndRect().left + leftOffest, UICaption.GetWndRect().top);

	// Description
	AttachChild(&UIDescription);
	xml_init.InitStatic(uiXml, "description_static", 0, &UIDescription);
	UIDescription.MoveWindow(UIDescription.GetWndRect().left + leftOffest, UIDescription.GetWndRect().top);

	// Description
	AttachChild(&UIAdditionalMaterials);
	xml_init.InitButton(uiXml, "materials_button", 0, &UIAdditionalMaterials);
	UIAdditionalMaterials.Show(false);
}

//////////////////////////////////////////////////////////////////////////

void CUIJobItem::Init(int x, int y, int width, int height)
{
	inherited::Init(x, y, width, height);
}

//////////////////////////////////////////////////////////////////////////

void CUIJobItem::SetPicture(LPCSTR texName, const Irect &originalRect, u32 color)
{
	UIPicture.InitTexture	(texName);
	UIPicture.SetColor		(color);

	if (0 == originalRect.x1 &&
		0 == originalRect.y1 &&
		0 == originalRect.x2 &&
		0 == originalRect.y2 )
		return;

	UIPicture.SetOriginalRect(originalRect.x1, originalRect.y1, originalRect.x2, originalRect.y2);
	UIPicture.ClipperOn();
	RECT r = UIPicture.GetWndRect();

	float m = 1.0f;
	if (r.right - r.left < originalRect.x2 || r.bottom - r.top < originalRect.y2)
	{
		m = std::min(static_cast<float>(r.right - r.left) / originalRect.x2, static_cast<float>(r.bottom - r.top) / originalRect.y2);
		UIPicture.SetTextureScale(m);
	}

	int offX = static_cast<int>(r.right - r.left - originalRect.x2 * m) / 2;
	int offY = static_cast<int>(r.bottom - r.top - originalRect.y2 * m) / 2;
	clamp<int>(offX, 0, r.right - r.left);
	clamp<int>(offY, 0, r.bottom - r.top);
	UIPicture.SetTextureOffset(offX, offY);
}

//////////////////////////////////////////////////////////////////////////

void CUIJobItem::SetCaption(LPCSTR caption)
{
	UICaption.SetText(caption);
}

//////////////////////////////////////////////////////////////////////////

void CUIJobItem::SetDescription(LPCSTR description)
{
	UIDescription.SetText(description);
}

//////////////////////////////////////////////////////////////////////////

void CUIJobItem::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if (&UIAdditionalMaterials == pWnd && BUTTON_CLICKED == msg)
	{
		GetTop()->SendMessage(this, static_cast<s16>(articleTypeMsg), NULL);
	}
}