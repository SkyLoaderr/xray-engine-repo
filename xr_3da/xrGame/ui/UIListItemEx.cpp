#include "stdafx.h"
#include ".\uilistitemex.h"

CUIListItemEx::CUIListItemEx(void)
{
	this->InitTexture("ui\\hud_map_point");
	this->SetStretchTexture(true);
	this->SetColor(color_argb(0, 0, 0, 0));
}

CUIListItemEx::~CUIListItemEx(void)
{
}

void CUIListItemEx::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	//inherited::SendMessage(pWnd, msg, pData);

	switch (msg)
	{
	case LIST_ITEM_SELECT:
		this->SetColor(color_argb(255, 0, 0, 255));	
		this->Draw();
		break;
	case LIST_ITEM_UNSELECT:
		this->SetColor(color_argb(0, 0, 0, 0));
		this->Draw();
		break;
	}
}