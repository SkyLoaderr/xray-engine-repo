//---------------------------------------------------------------------------

#ifndef FolderLibH
#define FolderLibH
//---------------------------------------------------------------------------
#include "ElTree.hpp"
#include "ExtBtn.hpp"
#include "MxMenus.hpp"

// refs

const DWORD TYPE_INVALID= -1;
const DWORD TYPE_FOLDER	= 0;
const DWORD TYPE_OBJECT	= 1;
class CFolderHelper{
public:
	typedef void 		__fastcall (__closure *TOnRenameItem)(LPCSTR p0, LPCSTR p1);
	typedef BOOL 		__fastcall (__closure *TOnRemoveItem)(LPCSTR p0);
	typedef void 		__fastcall (__closure *TOnAfterRemoveItem)();
    IC bool				IsFolder			(TElTreeItem* node){return node?(TYPE_FOLDER==(DWORD)node->Data):TYPE_INVALID;}
    IC bool				IsObject			(TElTreeItem* node){return node?(TYPE_OBJECT==(DWORD)node->Data):TYPE_INVALID;}

    bool 			 	MakeFullName		(TElTreeItem* begin_item, TElTreeItem* end_item, AnsiString& folder);
    bool 			 	MakeName			(TElTreeItem* begin_item, TElTreeItem* end_item, AnsiString& folder, bool bOnlyFolder);
	TElTreeItem* 		FindItemInFolder	(TElTree* tv, TElTreeItem* start_folder, const AnsiString& name, bool bIgnoreExt=false);
	TElTreeItem* 		FindItemInFolder	(DWORD type, TElTree* tv, TElTreeItem* start_folder, const AnsiString& name, bool bIgnoreExt=false);
    TElTreeItem* 		AppendFolder		(TElTree* tv, LPCSTR full_name);
	TElTreeItem*		AppendObject		(TElTree* tv, LPCSTR full_name);
    TElTreeItem* 		FindObject			(TElTree* tv, LPCSTR full_name, TElTreeItem** last_valid_node=0, int* last_valid_idx=0, bool bIgnoreExt=false);
    TElTreeItem* 		FindFolder			(TElTree* tv, LPCSTR full_name, TElTreeItem** last_valid_node=0, int* last_valid_idx=0);
    TElTreeItem* 		FindItem			(TElTree* tv, LPCSTR full_name, TElTreeItem** last_valid_node=0, int* last_valid_idx=0);
    void 				GenerateFolderName	(TElTree* tv, TElTreeItem* node,AnsiString& name,LPCSTR pref="folder");
	void 				GenerateObjectName	(TElTree* tv, TElTreeItem* node, AnsiString& name,LPCSTR pref="object");
	LPCSTR		 		GetFolderName		(const AnsiString& full_name, AnsiString& dest);
	LPCSTR		 		GetObjectName		(const AnsiString& full_name, AnsiString& dest);
    LPCSTR				ReplacePart			(LPCSTR old_name, LPCSTR ren_part, int level, LPSTR dest);
    bool 				RenameItem          (TElTree* tv, TElTreeItem* node, AnsiString& new_text, TOnRenameItem OnRenameItem);
    void 				CreateNewFolder		(TElTree* tv, bool bEditAfterCreate);
    BOOL				RemoveItem			(TElTree* tv, TElTreeItem* pNode, TOnRemoveItem OnRemoveItem, TOnAfterRemoveItem OnAfterRemoveItem=0);
    // drag'n'drop
	void __fastcall		DragDrop			(TObject *Sender, TObject *Source, int X, int Y, TOnRenameItem after_drag);
	void __fastcall		DragOver			(TObject *Sender, TObject *Source, int X, int Y, TDragState State, bool &Accept);
	void __fastcall		StartDrag			(TObject *Sender, TDragObject *&DragObject);
    // popup menu
    void				ShowPPMenu			(TMxPopupMenu* M, TExtBtn* B=0);
    // folder text edit
    // name
 	bool 				NameAfterEdit		(TElTreeItem* node, LPCSTR value, AnsiString& edit_val);
    void __fastcall		NameAfterEdit		(PropValue* sender, LPVOID edit_val);
    void __fastcall		NameBeforeEdit		(PropValue* sender, LPVOID edit_val);
    void __fastcall		NameDraw			(PropValue* sender, LPVOID draw_val);
    // last selection
    TElTreeItem*		RestoreSelection	(TElTree* tv, TElTreeItem* node);
    TElTreeItem*		RestoreSelection	(TElTree* tv, LPCSTR full_name);
    TElTreeItem*		ExpandItem			(TElTree* tv, LPCSTR full_name);
    TElTreeItem*		ExpandItem			(TElTree* tv, TElTreeItem* node);
//------------------------------------------------------------------------------
};
extern CFolderHelper FHelper;
#endif
