#include "stdafx.h"
#pragma hdrstop

#include "ESceneGlowTools.h"
#include "PropertiesListHelper.h"

void ESceneGlowTools::CreateControls()
{
	inherited::CreateControls();
}
//----------------------------------------------------
 
void ESceneGlowTools::RemoveControls()
{
}
//----------------------------------------------------

void ESceneGlowTools::FillProp(LPCSTR pref, PropItemVec& items)
{
    PHelper.CreateFlag32(items, FHelper.PrepareKey(pref,"Common\\Test Visibility"),		&m_Flags,			flTestVisibility);
	inherited::FillProp(pref, items);
}
//------------------------------------------------------------------------------

