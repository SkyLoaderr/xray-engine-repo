//---------------------------------------------------------------------------

#ifndef FolderLibH
#define FolderLibH
//---------------------------------------------------------------------------
#include "ElTree.hpp"

const DWORD TYPE_FOLDER=0;
const DWORD TYPE_OBJECT=1;

namespace FOLDER{
    bool 			 	MakeName			(TElTreeItem* begin_item, TElTreeItem* end_item, AnsiString& folder, bool bOnlyFolder);
	TElTreeItem* 		FindItemInFolder	(DWORD type, TElTree* tv, TElTreeItem* start_folder, const AnsiString& name);
    TElTreeItem* 		AppendFolder		(TElTree* tv, LPCSTR full_name);
	TElTreeItem*		AppendObject		(TElTree* tv, LPCSTR full_name);
    TElTreeItem* 		FindItem			(DWORD type, TElTree* tv, LPCSTR full_name, TElTreeItem** last_valid_node=0, int* last_valid_idx=0);
    void 				GenerateFolderName	(TElTree* tv, TElTreeItem* node,AnsiString& name);
}
#endif
