//---------------------------------------------------------------------------
#ifndef ItemListHelperH
#define ItemListHelperH

#include "ItemListTypes.h"
#include "FolderLib.h"

//---------------------------------------------------------------------------
class CListHelper{
//------------------------------------------------------------------------------
	static AnsiString XKey;
    IC AnsiString		FolderAppend	(LPCSTR val)
    {
    	if (val&&val[0]) return AnsiString(val)+"\\";
        return   		"";
    }
public:
    IC LPCSTR			PrepareKey		(LPCSTR pref, LPCSTR key)
    {
        R_ASSERT(key);
    	XKey			= FolderAppend(pref)+AnsiString(key);
        return XKey.c_str();
    }
    IC LPCSTR			PrepareKey		(LPCSTR pref0, LPCSTR pref1, LPCSTR key)
    {
        R_ASSERT(key);
    	XKey			= FolderAppend(pref0)+FolderAppend(pref1)+AnsiString(key);
        return XKey.c_str();
    }
    IC LPCSTR			PrepareKey		(LPCSTR pref0, LPCSTR pref1, LPCSTR pref2, LPCSTR key)
    {
        R_ASSERT(key);
    	XKey			= FolderAppend(pref0)+FolderAppend(pref1)+FolderAppend(pref2)+AnsiString(key);
        return XKey.c_str();
    }
    IC ListItem* 		FindItem		(ListItemsVec& items,	LPCSTR key, int type)
    {
    	for (ListItemsIt it=items.begin(); it!=items.end(); it++)
        	if (((*it)->type==type)&&((*it)->key==key)) return *it;
        return 0;
    }
public:
	IC ListItem*		CreateItem		(ListItemsVec& items, LPCSTR pref, LPCSTR name, int type, u32 item_flags=0)
    {
        ListItem* item	= xr_new<ListItem>	(type);
        item->SetName	(pref,name);
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
