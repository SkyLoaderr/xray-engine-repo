#include "stdafx.h"
#pragma hdrstop

#include "ESceneGlowTools.h"
#include "PropertiesListHelper.h"

void ESceneGlowTools::CreateControls()
{
	inherited::CreateControls();
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

