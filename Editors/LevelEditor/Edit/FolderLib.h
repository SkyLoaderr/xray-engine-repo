//---------------------------------------------------------------------------

#ifndef FolderLibH
#define FolderLibH
//---------------------------------------------------------------------------
#include "ElTree.hpp"
#include "ExtBtn.hpp"
#include "MxMenus.hpp"

// refs

const u32 TYPE_INVALID= -1;
const u32 TYPE_FOLDER	= 0;
const u32 TYPE_OBJECT	= 1;
class CFolderHelper{
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
public:
	typedef void 		__fastcall (__closure *TOnRenameItem)(LPCSTR p0, LPCSTR p1);
	typedef BOOL 		__fastcall (__closure *TOnRemoveItem)(LPCSTR p0);
	typedef void 		__fastcall (__closure *TOnAfterRemoveItem)();
    IC bool				IsFolder			(TElTreeItem* node){return node?(TYPE_FOLDER==(u32)node->Data):TYPE_INVALID;}
    IC bool				IsObject			(TElTreeItem* node){return node?(TYPE_OBJECT==(u32)node->Data):TYPE_INVALID;}

    bool 			 	MakeFullName		(TElTreeItem* begin_item, TElTreeItem* end_item, AnsiString& folder);
    bool 			 	MakeName			(TElTreeItem* begin_item, TElTreeItem* end_item, AnsiString& folder, bool bOnlyFolder);
	TElTreeItem* 		FindItemInFolder	(TElTree* tv, TElTreeItem* start_folder, const AnsiString& name, bool bIgnoreExt=false);
	TElTreeItem* 		FindItemInFolder	(u32 type, TElTree* tv, TElTreeItem* start_folder, const AnsiString& name, bool bIgnoreExt=false);
    TElTreeItem* 		AppendFolder		(TElTree* tv, AnsiString full_name);
	TElTreeItem*		AppendObject		(TElTree* tv, AnsiString full_name);
    TElTreeItem* 		FindObject			(TElTree* tv, AnsiString full_name, TElTreeItem** last_valid_node=0, int* last_valid_idx=0, bool bIgnoreExt=false);
    TElTreeItem* 		FindFolder			(TElTree* tv, AnsiString full_name, TElTreeItem** last_valid_node=0, int* last_valid_idx=0);
    TElTreeItem* 		FindItem			(TElTree* tv, AnsiString full_name, TElTreeItem** last_valid_node=0, int* last_valid_idx=0);
    void 				GenerateFolderName	(TElTree* tv, TElTreeItem* node,AnsiString& name,AnsiString pref="folder");
	void 				GenerateObjectName	(TElTree* tv, TElTreeItem* node, AnsiString& name,AnsiString pref="object");
	AnsiString	 		GetFolderName		(const AnsiString& full_name, AnsiString& dest);
	AnsiString	 		GetObjectName		(const AnsiString& full_name, AnsiString& dest);
    AnsiString			ReplacePart			(AnsiString old_name, AnsiString ren_part, int level, LPSTR dest);
    bool 				RenameItem          (TElTree* tv, TElTreeItem* node, AnsiString& new_text, TOnRenameItem OnRenameItem);
    void 				CreateNewFolder		(TElTree* tv, bool bEditAfterCreate);
    BOOL				RemoveItem			(TElTree* tv, TElTreeItem* pNode, TOnRemoveItem OnRemoveItem, TOnAfterRemoveItem OnAfterRemoveItem=0);
    // drag'n'drop
	void __fastcall		DragDrop			(TObject *Sender, TObject *Source, int X, int Y, TOnRenameItem after_drag);
	void __fastcall		DragOver			(TObject *Sender, TObject *Source, int X, int Y, TDragState State, bool &Accept);
	void __fastcall		StartDrag			(TObject *Sender, TDragObject *&DragObject);
    // popup menu
    void				ShowPPMenu			(TMxPopupMenu* M, TExtBtn* B=0);
    // last selection
    TElTreeItem*		RestoreSelection	(TElTree* tv, TElTreeItem* node, bool bLeaveSel);
    TElTreeItem*		RestoreSelection	(TElTree* tv, AnsiString full_name, bool bLeaveSel);
    TElTreeItem*		ExpandItem			(TElTree* tv, TElTreeItem* node);
    TElTreeItem*		ExpandItem			(TElTree* tv, AnsiString full_name);
//------------------------------------------------------------------------------
};
extern CFolderHelper FHelper;
#endif
