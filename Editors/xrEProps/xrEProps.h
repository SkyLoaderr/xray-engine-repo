#ifndef xrEPropsH
#define xrEPropsH

#ifdef XR_EPROPS_EXPORTS
	#define XR_EPROPS_API __declspec(dllexport)
#else
	#define XR_EPROPS_API __declspec(dllimport)
#endif

#include "FolderLib.h"                 
#include "PropertiesListTypes.h"
#include "PropertiesListHelper.h"
#include "ItemListTypes.h"
#include "ItemListHelper.h"

typedef void 	__fastcall (__closure *TOnILItemsFocused)	(ListItemsVec& items);
typedef void 	__fastcall (__closure *TOnILCloseEvent)		(void);

#ifdef __BORLANDC__
#	include "mxPlacemnt.hpp"
	DEFINE_VECTOR	(TElTreeItem*,ElItemsVec,ElItemsIt);
    typedef void 	__fastcall (__closure *TOnILItemFocused)(TElTreeItem* item);
#endif

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

    virtual void 		RemoveSelItems			(CFolderHelper::TOnItemRemove on_remove=0)=0;
    virtual void 		RenameSelItem			()=0;
    virtual void		FireOnItemFocused		()=0;

    virtual void		GetFolders				(RStrVec& folders)=0;

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

    virtual void  		SaveParams				(TFormStorage* fs)=0;
    virtual void  		LoadParams				(TFormStorage* fs)=0;
#endif

	virtual void 		GenerateObjectName		(ref_str name, LPCSTR start_node, LPCSTR pref="object", bool num_first=false)=0;

    virtual void		SetOnItemsFocusedEvent	(TOnILItemsFocused)=0;
    virtual void		SetOnCloseEvent			(TOnILCloseEvent)=0;
    virtual void		SetOnItemRenameEvent	(CFolderHelper::TOnItemRename e)=0;
    virtual void		SetOnItemRemoveEvent	(CFolderHelper::TOnItemRemove e)=0;
    virtual void		SetOnModifiedEvent		(TOnModifiedEvent e)=0;
};

//---------------------------------------------------------------------------
void XR_EPROPS_API CheckWindowPos(TForm* form);
//---------------------------------------------------------------------------

#endif
 