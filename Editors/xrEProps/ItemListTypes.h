//---------------------------------------------------------------------------
#ifndef ItemListTypesH
#define ItemListTypesH

//---------------------------------------------------------------------------

class XR_EPROPS_API ListItem{
	friend class		CListHelper;
    friend class		TItemList;
    ref_str				key;
    int					type;
	void*				item;
public:
    typedef void 	__fastcall (__closure *TOnListItemFocused)	(ListItem* sender);
    typedef void 	__fastcall (__closure *TOnClick)			(ListItem* sender);
    typedef bool 	__fastcall (__closure *TOnDrawThumbnail)	(ListItem* sender, TCanvas *Surface, TRect &R);

    TOnClick			OnClickEvent;
    TOnListItemFocused	OnItemFocused;
    TOnDrawThumbnail	OnDrawThumbnail;
public:
    int 				tag;
    LPVOID				m_Object;
    int					icon_index;
    u32					prop_color;
public:
    enum{
    	flShowCB		= (1<<0),
    	flCBChecked		= (1<<1),
		flDrawThumbnail	= (1<<2),
		flDrawCanvas	= (1<<3),
        flSorted		= (1<<4),
    };
    Flags32				m_Flags;
public:
						ListItem		(int _type):type(_type),prop_color(clBlack),item(0),key(0),tag(0),icon_index(-1),OnDrawThumbnail(0),OnItemFocused(0),m_Object(0){m_Flags.zero();}
	virtual 			~ListItem		(){};
    void				SetName			(LPCSTR _key){key=_key;}

    IC int				Type			(){return type;}
	IC void*			Item			(){return item;}
	IC LPCSTR			Key				(){return *key;}
    IC void				SetIcon			(int index){icon_index=index;}
};

DEFINE_VECTOR			(ListItem*,ListItemsVec,ListItemsIt);
//---------------------------------------------------------------------------
#endif




