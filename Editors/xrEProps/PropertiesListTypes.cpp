#include "stdafx.h"
#pragma hdrstop

#include "PropertiesListTypes.h"
#include "MxShortcut.hpp"
//------------------------------------------------------------------------------

std::string	ShortcutValue::GetDrawText		(TOnDrawTextEvent OnDrawText)
{
    std::string 	txt = MxShortCutToText(value->hotkey).c_str();
    if (!OnDrawText.empty())	OnDrawText(this, txt);
    return 			txt;
}

