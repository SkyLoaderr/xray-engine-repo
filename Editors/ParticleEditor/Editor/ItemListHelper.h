//---------------------------------------------------------------------------
#ifndef ItemListHelperH
#define ItemListHelperH

#include "ItemListTypes.h"
#include "FolderLib.h"

//---------------------------------------------------------------------------
class CListHelper{
//------------------------------------------------------------------------------
public:
    IC ListItem* 		FindItem		(ListItemsVec& items,	LPCSTR key, int type)
    {
    	for (ListItemsIt it=items.begin(); it!=items.end(); it++)
        	if (((*it)->type==type)&&((*it)->key==key)) return *it;
        return 0;
    }
 	bool 				NameAfterEdit	(ListItem* sender, AnsiString value, AnsiString& edit_val);
public:
	IC ListItem*		CreateItem		(ListItemsVec& items, LPCSTR key, int type, u32 item_flags=0, void* object=0)
    {
        ListItem* item	= xr_new<ListItem>	(type);
        item->SetName	(key);
        item->m_Object	= object;
        item->m_Flags.set(item_flags,TRUE);
        items.push_back	(item);
        return			item;
    }

	void 				DrawThumbnail	(TCanvas *Surface, TRect &R, LPCSTR fname);
};
//---------------------------------------------------------------------------
extern CListHelper LHelper;
//---------------------------------------------------------------------------
#endif
