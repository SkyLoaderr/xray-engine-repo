//---------------------------------------------------------------------------

#ifndef FolderLibH
#define FolderLibH
//---------------------------------------------------------------------------
#include "ElTree.hpp"
#include "ExtBtn.hpp"
#include "MxMenus.hpp"

namespace FOLDER{
	const DWORD TYPE_INVALID=-1;
	const DWORD TYPE_FOLDER=0;
	const DWORD TYPE_OBJECT=1;

	typedef void 		__fastcall (__closure *TOnAfterDrag)(LPVOID p0, LPVOID p1);
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
    // drag'n'drop
	void 				DragDrop			(TObject *Sender, TObject *Source, int X, int Y);
	void 				DragOver			(TObject *Sender, TObject *Source, int X, int Y, TDragState State, bool &Accept);
	void 				StartDrag			(TObject *Sender, TDragObject *&DragObject, TOnAfterDrag after_drag);
    // popup menu
    void				ShowPPMenu			(TMxPopupMenu* M, TExtBtn* B=0);
}
#endif
