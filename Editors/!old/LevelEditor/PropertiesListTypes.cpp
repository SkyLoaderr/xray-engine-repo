#include "stdafx.h"
#pragma hdrstop

#include "PropertiesListTypes.h"
//------------------------------------------------------------------------------

void __stdcall	PropValue::SetSubItemCount(u32 cnt)
{
	Owner()->subitem=cnt;
}                        

