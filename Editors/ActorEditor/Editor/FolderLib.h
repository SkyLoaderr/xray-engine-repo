//---------------------------------------------------------------------------

#ifndef FolderLibH
#define FolderLibH
//---------------------------------------------------------------------------
#include "ElTree.hpp"
#include "ExtBtn.hpp"
#include "MxMenus.hpp"

enum EItemType{
	TYPE_INVALID= -1,
	TYPE_FOLDER	= 0,
	TYPE_OBJECT	= 1
};
class CFolderHelper{
    IC AnsiString		FolderAppend	(LPCSTR val)
    {
    	if (val&&val[0]) return AnsiString(val)+"\\";
        return   		"";
    }
    IC TElTreeItem*		LL_CreateFolder	(TElTree* tv, TElTreeItem* parent, const AnsiString& name)
    {
    	TElTreeItem* N	= tv->Items->AddChildObject(parent,name,(void*)TYPE_FOLDER);
        N->ForceButtons	= true;
        return N;
    }
    IC TElTreeItem*		LL_CreateObject	(TElTree* tv, TElTreeItem* parent, const AnsiString& name)
    {
    	TElTreeItem* N	= tv->Items->AddChildObject(parent,name,(void*)TYPE_OBJECT);
        return N;
    }
public:
    IC AnsiString		PrepareKey		(LPCSTR pref, LPCSTR key)
    {
        R_ASSERT(key);
        return FolderAppend(pref)+AnsiString(key);
    }
    IC AnsiString		PrepareKey		(LPCSTR pref0, LPCSTR pref1, LPCSTR key)
    {
        R_ASSERT(key);
        return FolderAppend(pref0)+FolderAppend(pref1)+AnsiString(key);
    }
    IC AnsiString		PrepareKey		(LPCSTR pref0, LPCSTR pref1, LPCSTR pref2, LPCSTR key)
    {
        R_ASSERT(key);
        return FolderAppend(pref0)+FolderAppend(pref1)+FolderAppend(pref2)+AnsiString(key);
    }
public:
	typedef void 		__fastcall (__closure *TOnItemRename)(LPCSTR p0, LPCSTR p1, EItemType type);
	typedef BOOL 		__fastcall (__closure *TOnItemRemove)(LPCSTR p0, EItemType type);  
	typedef void 		__fastcall (__closure *TOnItemAfterRemove)();
    IC bool				IsFolder			(TElTreeItem* node){return node?(TYPE_FOLDER==(u32)node->Data):TYPE_INVALID;}
    IC bool				IsObject			(TElTreeItem* node){return node?(TYPE_OBJECT==(u32)node->Data):TYPE_INVALID;}

    bool 			 	MakeFullName		(TElTreeItem* begin_item, TElTreeItem* end_item, AnsiString& folder);
    bool 			 	MakeName			(TElTreeItem* begin_item, TElTreeItem* end_item, AnsiString& folder, bool bOnlyFolder);
	TElTreeItem* 		FindItemInFolder	(TElTree* tv, TElTreeItem* start_folder, const AnsiString& name, bool bIgnoreExt=false);
	TElTreeItem* 		FindItemInFolder	(EItemType type, TElTree* tv, TElTreeItem* start_folder, const AnsiString& name, bool bIgnoreExt=false);
    TElTreeItem* 		AppendFolder		(TElTree* tv, AnsiString full_name);
	TElTreeItem*		AppendObject		(TElTree* tv, AnsiString full_name, bool allow_duplicate=false);
    TElTreeItem* 		FindObject			(TElTree* tv, AnsiString full_name, TElTreeItem** last_valid_node=0, int* last_valid_idx=0, bool bIgnoreExt=false);
    TElTreeItem* 		FindFolder			(TElTree* tv, AnsiString full_name, TElTreeItem** last_valid_node=0, int* last_valid_idx=0);
    TElTreeItem* 		FindItem			(TElTree* tv, AnsiString full_name, TElTreeItem** last_valid_node=0, int* last_valid_idx=0); 
    void 				GenerateFolderName	(TElTree* tv, TElTreeItem* node,AnsiString& name,AnsiString pref="folder", bool num_first=false);
	void 				GenerateObjectName	(TElTree* tv, TElTreeItem* node, AnsiString& name,AnsiString pref="object", bool num_first=false);
	AnsiString	 		GetFolderName		(const AnsiString& full_name, AnsiString& dest);
	AnsiString	 		GetObjectName		(const AnsiString& full_name, AnsiString& dest);
    AnsiString			ReplacePart			(AnsiString old_name, AnsiString ren_part, int level, LPSTR dest);
    bool 				RenameItem          (TElTree* tv, TElTreeItem* node, AnsiString& new_text, TOnItemRename OnRenameItem);
    void 				CreateNewFolder		(TElTree* tv, bool bEditAfterCreate);
    BOOL				RemoveItem			(TElTree* tv, TElTreeItem* pNode, TOnItemRemove OnRemoveItem, TOnItemAfterRemove OnAfterRemoveItem=0);
    // drag'n'drop
	void __fastcall		DragDrop			(TObject *Sender, TObject *Source, int X, int Y, TOnItemRename after_drag);
	void __fastcall		DragOver			(TObject *Sender, TObject *Source, int X, int Y, TDragState State, bool &Accept);
	void __fastcall		StartDrag			(TObject *Sender, TDragObject *&DragObject);
	void __fastcall		StartDragNoFolder	(TObject *Sender, TDragObject *&DragObject);
    // popup menu
    void				ShowPPMenu			(TMxPopupMenu* M, TExtBtn* B=0);
    // name edit
	bool 				NameAfterEdit		(TElTreeItem* node, AnsiString value, AnsiString& N);
    // last selection
    TElTreeItem*		RestoreSelection	(TElTree* tv, TElTreeItem* node, bool bLeaveSel);
    TElTreeItem*		RestoreSelection	(TElTree* tv, AnsiString full_name, bool bLeaveSel);
    TElTreeItem*		ExpandItem			(TElTree* tv, TElTreeItem* node);
    TElTreeItem*		ExpandItem			(TElTree* tv, AnsiString full_name);

	bool 				DrawThumbnail		(TCanvas *Surface, TRect &R, LPCSTR fname, u32 thm_type);
//------------------------------------------------------------------------------
};

extern CFolderHelper FHelper;
#endif
