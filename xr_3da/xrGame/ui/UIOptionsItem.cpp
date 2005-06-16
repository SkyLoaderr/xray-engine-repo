////////////////////////////////////////////////////
//
///////////////////////////////////////////////////

#include "StdAfx.h"
#include "UIOptionsItem.h"
#include "UIOptionsManager.h"
#include "../MainUI.h"

void CUIOptionsItem::Register(const char* entry, const char* group){
	UI()->OptionsManager()->RegisterItem(this, group);
	m_entry = entry;	
}