#include "stdafx.h"
#pragma hdrstop

#include "ItemListHelper.h"
//---------------------------------------------------------------------------

CListHelper LHelper;

bool CListHelper::NameAfterEdit(ListItem* sender, AnsiString value, AnsiString& N)
{
	N=N.LowerCase();
    if (N.IsEmpty()){ N=value; return false; }
	int cnt=_GetItemCount(N.c_str(),'\\');
    if (cnt>1)		{ N=value; return false; }

	TElTreeItem* node    = sender->Item(); VERIFY(node);

    for (TElTreeItem* itm=node->GetFirstSibling(); itm; itm=itm->GetNextSibling()){
        if ((itm->Text==N)&&(itm!=node)){
        	// елемент с таким именем уже существует
	        N=value;
            return false;
        }
    }
    // all right
    node->Text = N;
    AnsiString tmp;
	_ReplaceItem(sender->key.c_str(),	_GetItemCount(sender->key.c_str(),'\\')-1,	N.c_str(),tmp,'\\');	sender->key=tmp;
    // Имя объекта может быть составным из a\\b\\name
	_ReplaceItem(value.c_str(),			_GetItemCount(value.c_str(),'\\')-1,		N.c_str(),tmp,'\\');	N=tmp;
    
    return true;
}
//---------------------------------------------------------------------------


