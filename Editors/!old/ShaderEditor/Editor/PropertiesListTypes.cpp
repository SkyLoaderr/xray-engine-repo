#include "stdafx.h"
#pragma hdrstop

#include "PropertiesListTypes.h"
#pragma package(smart_init)

AnsiString prop_draw_text;

//------------------------------------------------------------------------------
LPCSTR RTextValue::GetText(TOnDrawTextEvent OnDrawText)
{
    prop_draw_text=*GetValue();
    if (OnDrawText)OnDrawText(this, &prop_draw_text);
    return prop_draw_text.c_str();
}
//------------------------------------------------------------------------------
LPCSTR ATextValue::GetText(TOnDrawTextEvent OnDrawText)
{
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

LPCSTR ButtonValue::GetText(TOnDrawTextEvent OnDrawText)
{
    prop_draw_text = _ListToSequence(value);
    return prop_draw_text.c_str();
}
//------------------------------------------------------------------------------

LPCSTR TokenValueSH::GetText(TOnDrawTextEvent OnDrawText)
{
	u32 draw_val 	= GetValue();
    if (OnDrawText)OnDrawText(this, &draw_val);
	for(u32 i=0; i<cnt; i++) if (items[i].ID==draw_val) return items[i].str;
    return 0;
}
//------------------------------------------------------------------------------

