//---------------------------------------------------------------------------
#ifndef ItemListTypesH
#define ItemListTypesH

#include "ElTree.hpp"
//---------------------------------------------------------------------------

class ListItem{
	friend class		CListHelper;
    friend class		TItemList;
    AnsiString			key;
    int					type;
	TElTreeItem*		item;
public:
    typedef void 	__fastcall (__closure *TOnListItemFocused)	(ListItem* sender);
    typedef void 	__fastcall (__closure *TOnClick)			(ListItem* sender);

    TOnClick			OnClickEvent;
    TOnListItemFocused	OnItemFocused;
public:
    int 				tag;
    LPVOID				m_Object;
public:
    enum{
    	flShowCB		= (1<<0),
    	flCBChecked		= (1<<1),
        flDrawThumbnail	= (1<<2),
        flDrawCanvas	= (1<<3),
    };
    Flags32				m_Flags;
public:
						ListItem		(int _type):type(_type),item(0),key(0),tag(0),OnItemFocused(0),m_Object(0){m_Flags.zero();}
	virtual 			~ListItem		(){};
    void				SetItemHeight	(int height){item->OwnerHeight=false; item->Height=height;}
    void				SetName			(LPCSTR _key){key=_key;}

    IC int				Type			(){return type;}
	IC TElTreeItem*		Item			(){return item;}
	IC LPCSTR			Key				(){return key.c_str();}
};

DEFINE_VECTOR			(ListItem*,ListItemsVec,ListItemsIt);
//---------------------------------------------------------------------------
#endif




