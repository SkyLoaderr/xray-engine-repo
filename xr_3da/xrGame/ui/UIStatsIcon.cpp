#include "StdAfx.h"
#include "UIStatsIcon.h"

CUIStatsIcon::CUIStatsIcon(){
	SetStretchTexture(true);
}

void CUIStatsIcon::SetText(LPCSTR str){
		InitTexture(str);
}