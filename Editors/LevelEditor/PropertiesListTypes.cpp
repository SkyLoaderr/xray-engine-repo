#include "stdafx.h"
#pragma hdrstop

#include "PropertiesListTypes.h"

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

