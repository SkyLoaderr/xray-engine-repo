//---------------------------------------------------------------------------

#ifndef FolderLibH
#define FolderLibH
//---------------------------------------------------------------------------
#include "ElTree.hpp"

namespace FOLDER{
	const DWORD TYPE_FOLDER=0;
	const DWORD TYPE_OBJECT=1;

    IC bool				IsFolder			(TElTreeItem* node){return (TYPE_FOLDER==(DWORD)node->Data);}
    IC bool				IsObject			(TElTreeItem* node){return (TYPE_OBJECT==(DWORD)node->Data);}

    bool 			 	MakeName			(TElTreeItem* begin_item, TElTreeItem* end_item, AnsiString& folder, bool bOnlyFolder);
	TElTreeItem* 		FindItemInFolder	(DWORD type, TElTree* tv, TElTreeItem* start_folder, const AnsiString& name);
    TElTreeItem* 		AppendFolder		(TElTree* tv, LPCSTR full_name);
	TElTreeItem*		AppendObject		(TElTree* tv, LPCSTR full_name);
    TElTreeItem* 		FindObject			(TElTree* tv, LPCSTR full_name, TElTreeItem** last_valid_node=0, int* last_valid_idx=0);
    TElTreeItem* 		FindFolder			(TElTree* tv, LPCSTR full_name, TElTreeItem** last_valid_node=0, int* last_valid_idx=0);
    void 				GenerateFolderName	(TElTree* tv, TElTreeItem* node,AnsiString& name);
	LPCSTR		 		GetFolderName		(const AnsiString& full_name, AnsiString& dest);
	LPCSTR		 		GetObjectName		(const AnsiString& full_name, AnsiString& dest);
}
#endif
