#include "stdafx.h"
#pragma hdrstop

#include "ItemListTypes.h"
#include "ItemListHelper.h"

void ListItem::SetName(LPCSTR pref, LPCSTR _name)
{
	name 	= _name;
	key		= _name?LHelper.PrepareKey(pref,_name):pref;
}

