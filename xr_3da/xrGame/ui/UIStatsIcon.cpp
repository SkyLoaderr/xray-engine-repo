#include "StdAfx.h"
#include "UIStatsIcon.h"

CUIStatsIcon::CUIStatsIcon(){
	SetStretchTexture(true);
}

void CUIStatsIcon::SetText(LPCSTR str){
	if (str[0] != 0)
	{
		InitTexture(str);
		SetVisible(true);
	}
	else
		SetVisible(false);
}