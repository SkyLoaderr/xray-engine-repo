#include "stdafx.h"
#pragma hdrstop

#include "PropertiesListTypes.h"
#include "xr_tokens.h"

AnsiString prop_draw_text;

//------------------------------------------------------------------------------
LPCSTR ATextValue::GetText(TOnDrawEvent OnDraw){
    prop_draw_text=GetValue();
    if (OnDraw)OnDraw(this, &prop_draw_text);
    return prop_draw_text.c_str();
}
//------------------------------------------------------------------------------

LPCSTR TextValue::GetText(TOnDrawEvent OnDraw)
{
    prop_draw_text=value;
    if (OnDraw)OnDraw(this, &prop_draw_text);
    return prop_draw_text.c_str();
}
//------------------------------------------------------------------------------

LPCSTR 	TokenValue::GetText(TOnDrawEvent OnDraw)
{
	u32 draw_val 	= GetValue();
    if (OnDraw)OnDraw(this, &draw_val);
	for(int i=0; token[i].name; i++) if (0==memcmp(&token[i].id,&draw_val,p_size)) return token[i].name;
    return 0;
}
//------------------------------------------------------------------------------

LPCSTR 	TokenValue2::GetText(TOnDrawEvent OnDraw)
{
	u32 draw_val 	= GetValue();
    if (OnDraw)OnDraw(this, &draw_val);
    if (draw_val>=items.size()) return 0;
	return items[draw_val].c_str();
}
//------------------------------------------------------------------------------

LPCSTR TokenValue3::GetText(TOnDrawEvent OnDraw)
{
	VERIFY(GetValue()<cnt);
	u32 draw_val 	= GetValue();
    if (OnDraw)OnDraw(this, &draw_val);
	for(u32 i=0; i<cnt; i++) if (items[i].ID==draw_val) return items[i].str;
    return 0;
}
//------------------------------------------------------------------------------

LPCSTR TokenValue4::GetText(TOnDrawEvent OnDraw)
{
	VERIFY(GetValue()<items->size());
	u32 draw_val 	= GetValue();
    if (OnDraw)OnDraw(this, &draw_val);
	for (ItemVec::const_iterator it=items->begin(); it!=items->end(); it++)
    	if (draw_val==it->ID) return it->str.c_str();
    return 0;
}
//------------------------------------------------------------------------------

LPCSTR	ListValue::GetText(TOnDrawEvent OnDraw)
{
    prop_draw_text=GetValue();
    if (OnDraw)OnDraw(this, &prop_draw_text);
    return prop_draw_text.c_str();
}
//------------------------------------------------------------------------------


