#ifndef xrEPropsH
#define xrEPropsH

#ifdef XR_EPROPS_EXPORTS
	#define XR_EPROPS_API __declspec(dllexport)
#else
	#define XR_EPROPS_API __declspec(dllimport)
#endif

#include "PropertiesListTypes.h"
#include "ItemListTypes.h"
                                        
enum EItemType{
	TYPE_INVALID= -1,
	TYPE_FOLDER	= 0,
	TYPE_OBJECT	= 1
};

typedef void 	__stdcall  (__closure *TOnILItemsFocused)	(ListItemsVec& items);
typedef void 	__stdcall  (__closure *TOnILCloseEvent)		(void);

typedef void	__stdcall  (__closure *TOnItemRename)		(LPCSTR p0, LPCSTR p1, EItemType type);
typedef BOOL 	__stdcall  (__closure *TOnItemRemove)		(LPCSTR p0, EItemType type);  
typedef void 	__stdcall  (__closure *TOnItemAfterRemove)	();

#ifdef __BORLANDC__
#	include "mxPlacemnt.hpp"
	DEFINE_VECTOR	(TElTreeItem*,ElItemsVec,ElItemsIt);
    typedef void 	__stdcall  (__closure *TOnILItemFocused)(TElTreeItem* item);
#endif

//------------------------------------------------------------------------------
// Properties
//------------------------------------------------------------------------------
class XR_EPROPS_API IPropHelper{
public:
    virtual PropItem* 			__stdcall	FindItem			(PropItemVec& items, ref_str key, EPropType type=PROP_UNDEF)=0;
public:
//------------------------------------------------------------------------------
// predefind event routines
    virtual void 				__stdcall 	FvectorRDOnAfterEdit(PropValue* sender, Fvector& edit_val, bool& accepted)=0;
    virtual void 				__stdcall 	FvectorRDOnBeforeEdit(PropValue* sender,Fvector& edit_val)=0;
    virtual void 				__stdcall 	FvectorRDOnDraw		(PropValue* sender, ref_str& draw_val)=0;
    virtual void 				__stdcall 	floatRDOnAfterEdit	(PropValue* sender,	float& edit_val, bool& accepted)=0;
    virtual void 				__stdcall 	floatRDOnBeforeEdit	(PropValue* sender,	float& edit_val)=0;
    virtual void 				__stdcall 	floatRDOnDraw		(PropValue* sender, ref_str& draw_val)=0;
// name edit
    virtual void 				__stdcall  	NameAfterEdit		(PropValue* sender,	ref_str& edit_val, bool& accepted)=0;
    virtual void				__stdcall  	NameBeforeEdit		(PropValue* sender,	ref_str& edit_val)=0;
    virtual void 				__stdcall  	NameDraw			(PropValue* sender, ref_str& draw_val)=0;
public:
    virtual CaptionValue*  		__stdcall	CreateCaption	    (PropItemVec& items, ref_str key, ref_str val)=0;
    virtual CanvasValue*		__stdcall	CreateCanvas	    (PropItemVec& items, ref_str key, ref_str val, int height)=0;
    virtual ButtonValue*		__stdcall	CreateButton	    (PropItemVec& items, ref_str key, ref_str val, u32 flags)=0;
    virtual ChooseValue*		__stdcall	CreateChoose	    (PropItemVec& items, ref_str key, ref_str* val, u32 mode, LPCSTR path=0)=0;
    virtual S8Value* 			__stdcall	CreateS8		    (PropItemVec& items, ref_str key, s8* val, s8 mn=0, s8 mx=100, s8 inc=1)=0;
    virtual S16Value* 			__stdcall	CreateS16		    (PropItemVec& items, ref_str key, s16* val, s16 mn=0, s16 mx=100, s16 inc=1)=0;
    virtual S32Value* 	 		__stdcall	CreateS32		    (PropItemVec& items, ref_str key, s32* val, s32 mn=0, s32 mx=100, s32 inc=1)=0;
    virtual U8Value* 			__stdcall	CreateU8		    (PropItemVec& items, ref_str key, u8* val, u8 mn=0, u8 mx=100, u8 inc=1)=0;
    virtual U16Value* 			__stdcall	CreateU16		    (PropItemVec& items, ref_str key, u16* val, u16 mn=0, u16 mx=100, u16 inc=1)=0;
    virtual U32Value* 	  		__stdcall	CreateU32		    (PropItemVec& items, ref_str key, u32* val, u32 mn=0, u32 mx=100, u32 inc=1)=0;
    virtual FloatValue* 		__stdcall	CreateFloat		    (PropItemVec& items, ref_str key, float* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2)=0;
    virtual BOOLValue* 	  		__stdcall	CreateBOOL		    (PropItemVec& items, ref_str key, BOOL* val)=0;
    virtual VectorValue*  	 	__stdcall	CreateVector	    (PropItemVec& items, ref_str key, Fvector* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2)=0;
    virtual Flag8Value*			__stdcall	CreateFlag8		    (PropItemVec& items, ref_str key, Flags8* val, u8 mask, LPCSTR c0=0, LPCSTR c1=0, u32 flags=0)=0;
    virtual Flag16Value*		__stdcall	CreateFlag16	    (PropItemVec& items, ref_str key, Flags16* val, u16 mask, LPCSTR c0=0, LPCSTR c1=0, u32 flags=0)=0;
    virtual Flag32Value*		__stdcall	CreateFlag32	    (PropItemVec& items, ref_str key, Flags32* val, u32 mask, LPCSTR c0=0, LPCSTR c1=0, u32 flags=0)=0;
	virtual Token8Value*		__stdcall	CreateToken8	    (PropItemVec& items, ref_str key, u8* val, xr_token* token)=0;
	virtual Token16Value*		__stdcall	CreateToken16	    (PropItemVec& items, ref_str key, u16* val, xr_token* token)=0;
	virtual Token32Value*		__stdcall	CreateToken32	    (PropItemVec& items, ref_str key, u32* val, xr_token* token)=0;
	virtual RToken8Value* 		__stdcall	CreateRToken8	    (PropItemVec& items, ref_str key, u8* val, RTokenVec* token)=0;
	virtual RToken16Value* 		__stdcall	CreateRToken16	    (PropItemVec& items, ref_str key, u16* val, RTokenVec* token)=0;
	virtual RToken32Value* 		__stdcall	CreateRToken32	    (PropItemVec& items, ref_str key, u32* val, RTokenVec* token)=0;
	virtual TokenValueSH*   	__stdcall	CreateTokenSH	    (PropItemVec& items, ref_str key, u32* val, u32 cnt, const TokenValueSH::Item* lst)=0;
	virtual ListValue* 	 		__stdcall	CreateList		    (PropItemVec& items, ref_str key, ref_str* val, RStringVec* lst)=0;
    virtual U32Value*  			__stdcall	CreateColor		    (PropItemVec& items, ref_str key, u32* val)=0;
    virtual ColorValue*			__stdcall	CreateFColor	    (PropItemVec& items, ref_str key, Fcolor* val)=0;
    virtual VectorValue*		__stdcall	CreateVColor	    (PropItemVec& items, ref_str key, Fvector* val)=0;
	virtual RTextValue* 		__stdcall	CreateRText		    (PropItemVec& items, ref_str key, ref_str* val)=0;
	virtual WaveValue* 			__stdcall	CreateWave		    (PropItemVec& items, ref_str key, WaveForm* val)=0;
    virtual FloatValue* 		__stdcall	CreateTime		    (PropItemVec& items, ref_str key, float* val, float mn=0.f, float mx=86400.f)=0;

    virtual FloatValue* 		__stdcall	CreateAngle		    (PropItemVec& items, ref_str key, float* val, float mn=0.f, float mx=PI_MUL_2, float inc=0.01f, int decim=2)=0;
    virtual VectorValue* 		__stdcall	CreateAngle3	    (PropItemVec& items, ref_str key, Fvector* val, float mn=0.f, float mx=PI_MUL_2, float inc=0.01f, int decim=2)=0;
    virtual RTextValue* 		__stdcall	CreateName		    (PropItemVec& items, ref_str key, ref_str* val, ListItem* owner)=0;  
	virtual RTextValue* 		__stdcall	CreateNameCB		(PropItemVec& items, ref_str key, ref_str* val, TOnDrawTextEvent=0, void __stdcall (__closure* before) (PropValue*, ref_str&)=0, void __stdcall (__closure* after) (PropValue*, ref_str&, bool&)=0)=0;

    virtual ref_str  			__stdcall	PrepareKey			(LPCSTR pref, 	LPCSTR key)=0;
    virtual ref_str  			__stdcall	PrepareKey			(LPCSTR pref0, 	LPCSTR pref1,	LPCSTR key)=0;
    virtual ref_str  			__stdcall	PrepareKey			(LPCSTR pref0,	LPCSTR pref1, 	LPCSTR pref2,	LPCSTR key)=0;
};
//---------------------------------------------------------------------------
IPropHelper& XR_EPROPS_API PHelper ();
//---------------------------------------------------------------------------

//------------------------------------------------------------------------------
// List
//------------------------------------------------------------------------------
class XR_EPROPS_API IItemList
{
protected:                
    virtual void 		OnCreate				(LPCSTR title, TWinControl* parent, TAlign align, u32 flags)=0;
    virtual void 		OnDestroy				()=0;
public:
	enum{
    	// set
    	ilEditMenu		= (1<<0),              
        ilMultiSelect	= (1<<1),
        ilDragAllowed	= (1<<2),
        ilDragCustom	= (1<<3),
        ilFolderStore	= (1<<4),
        ilSuppressIcon 	= (1<<5),
        ilSuppressStatus= (1<<6),

        // internal
        ilRT_FullExpand	= (1<<30),
//        ilRT_UpdateLocked=(1<<31),
    };
public:
	static IItemList* 	CreateForm				(LPCSTR title, TWinControl* parent=0, TAlign align=alNone, u32 flags=ilMultiSelect|ilFolderStore);
	static IItemList* 	CreateModalForm			(LPCSTR title, u32 flags=ilMultiSelect|ilFolderStore);
	static void 		DestroyForm				(IItemList*& props);

    virtual void 		RemoveSelItems			(TOnItemRemove on_remove=0)=0;
    virtual void 		RenameSelItem			()=0;
    virtual void		FireOnItemFocused		()=0;

    virtual void		GetFolders				(RStringVec& folders)=0;

    virtual void 		ClearList				()=0;

    virtual void 		SelectItem				(LPCSTR full_name, bool bVal, bool bLeaveSel, bool bExpand)=0;
    virtual void 		AssignItems				(ListItemsVec& values, bool full_expand, bool full_sort=false)=0;

    virtual int  		GetSelected				(LPCSTR pref, ListItemsVec& items, bool bOnlyObject)=0;
    virtual ListItem*	FindItem				(LPCSTR full_name)=0;

    virtual void 		LockUpdating			()=0;
    virtual void 		UnlockUpdating			()=0;
    virtual bool		IsLocked				()=0;

    virtual void		SetImages				(TImageList* image_list)=0;

#ifdef __BORLANDC__
    virtual int  		GetSelected				(ElItemsVec& items)=0;
    virtual TElTreeItem*GetSelected				()=0;

    virtual void		LoadSelection			(TFormStorage* storage)=0;
    virtual void		SaveSelection			(TFormStorage* storage)=0;

    virtual void  		SaveParams				(TFormStorage* fs)=0;
    virtual void  		LoadParams				(TFormStorage* fs)=0;
#endif

	virtual void 		GenerateObjectName		(ref_str name, LPCSTR start_node, LPCSTR pref="object", bool num_first=false)=0;

    virtual void		SetOnItemFocusedEvent	(TOnILItemFocused)=0;
    virtual void		SetOnItemsFocusedEvent	(TOnILItemsFocused)=0;
    virtual void		SetOnCloseEvent			(TOnILCloseEvent)=0;
    virtual void		SetOnItemRenameEvent	(TOnItemRename e)=0;
    virtual void		SetOnItemRemoveEvent	(TOnItemRemove e)=0;
    virtual void		SetOnModifiedEvent		(TOnModifiedEvent e)=0;

    virtual void 		SetILFocus				()=0;
    
    virtual u32			GetFlags				()=0;	
    virtual void		SetFlags				(u32 mask)=0;
};

class XR_EPROPS_API IListHelper{
public:
    virtual ListItem* 			__stdcall	FindItem			(ListItemsVec& items,	LPCSTR key)=0;
 	virtual bool 				__stdcall	NameAfterEdit		(ListItem* sender, ref_str value, ref_str& edit_val)=0;
public:
	virtual ListItem*			__stdcall	CreateItem			(ListItemsVec& items, LPCSTR key, int type, u32 item_flags=0, void* object=0)=0;
};
//------------------------------------------------------------------------------
IListHelper& XR_EPROPS_API LHelper ();
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
void XR_EPROPS_API CheckWindowPos(TForm* form);
//---------------------------------------------------------------------------

#endif
 