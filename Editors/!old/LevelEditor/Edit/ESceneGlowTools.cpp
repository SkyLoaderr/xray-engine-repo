#include "stdafx.h"
#pragma hdrstop

#include "ESceneGlowTools.h"
#include "PropertiesListHelper.h"
#include "UI_LevelTools.h"

void ESceneGlowTools::CreateControls()
{
	inherited::CreateDefaultControls(estDefault);
    m_Flags.zero	();
}
//----------------------------------------------------
 
void ESceneGlowTools::RemoveControls()
{
	inherited::RemoveControls();
}
//----------------------------------------------------

void ESceneGlowTools::FillProp(LPCSTR pref, PropItemVec& items)
{                      
    PHelper().CreateFlag32(items, PrepareKey(pref,"Common\\Test Visibility"),		&m_Flags,			flTestVisibility);
	inherited::FillProp	(pref, items);
}
//------------------------------------------------------------------------------

