#include "stdafx.h"
#pragma hdrstop

#include "PropertiesListTypes.h"

AnsiString prop_draw_text;

//------------------------------------------------------------------------------
LPCSTR ATextValue::GetText(TOnDrawTextEvent OnDrawText){
    prop_draw_text=GetValue();
    if (OnDrawText)OnDrawText(this, &prop_draw_text);
    return prop_draw_text.c_str();
}
//------------------------------------------------------------------------------

LPCSTR TextValue::GetText(TOnDrawTextEvent OnDrawText)
{
    prop_draw_text=GetValue();
    if (OnDrawText)OnDrawText(this, &prop_draw_text);
    return prop_draw_text.c_str();
}
//------------------------------------------------------------------------------

LPCSTR 	TokenValue::GetText(TOnDrawTextEvent OnDrawText)
{
	u32 draw_val 	= GetValue();
    if (OnDrawText)OnDrawText(this, &draw_val);
	for(int i=0; token[i].name; i++) if (0==memcmp(&token[i].id,&draw_val,p_size)) return token[i].name;
    return 0;
}
//------------------------------------------------------------------------------

LPCSTR 	TokenValue2::GetText(TOnDrawTextEvent OnDrawText)
{
	u32 draw_val 	= GetValue();
    if (OnDrawText)OnDrawText(this, &draw_val);
    if (draw_val>=items.size()) return 0;
	return items[draw_val].c_str();
}
//------------------------------------------------------------------------------

LPCSTR TokenValue3::GetText(TOnDrawTextEvent OnDrawText)
{
	u32 draw_val 	= GetValue();
    if (OnDrawText)OnDrawText(this, &draw_val);
	for(u32 i=0; i<cnt; i++) if (items[i].ID==draw_val) return items[i].str;
    return 0;
}
//------------------------------------------------------------------------------

LPCSTR TokenValue4::GetText(TOnDrawTextEvent OnDrawText)
{
	u32 draw_val 	= GetValue();
    if (OnDrawText)OnDrawText(this, &draw_val);
	for (ItemVec::const_iterator it=items->begin(); it!=items->end(); it++)
    	if (0==memcmp(&it->ID,&draw_val,p_size)) return it->str.c_str();
    return 0;
}
//------------------------------------------------------------------------------

