#include "stdafx.h"
#pragma hdrstop

#include "FolderLib.h"
#include "xr_trims.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

LPCSTR FOLDER::GetFolderName(const AnsiString& full_name, AnsiString& dest)
{
    for (int i=full_name.Length(); i>=1; i--)
    	if (full_name[i]=='\\'){
        	dest=full_name.SubString(1,i);
            break;
        }
    return dest.c_str();
}

LPCSTR FOLDER::GetObjectName(const AnsiString& full_name, AnsiString& dest)
{
    for (int i=full_name.Length(); i>=1; i--)
    	if (full_name[i]=='\\'){
        	dest=full_name.SubString(i+1,full_name.Length());
            break;
        }
    return dest.c_str();
}

// собирает имя от стартового итема до конечного
// может включать либо не включать имя объекта
bool FOLDER::MakeName(TElTreeItem* begin_item, TElTreeItem* end_item, AnsiString& name, bool bOnlyFolder)
{
	if (begin_item){
    	TElTreeItem* node = (DWORD(begin_item->Data)==TYPE_OBJECT)?begin_item->Parent:begin_item;
        name = "";
        while (node){
			name.Insert(node->Text+AnsiString('\\'),0);
        	if (node==end_item) break;
            node=node->Parent;
        }
        if (!bOnlyFolder){
        	if (DWORD(begin_item->Data)==TYPE_OBJECT) name+=begin_item->Text;
            else return false;
        }
        return true;
    }else{
		name = "";
        return false;
    }
}
//---------------------------------------------------------------------------

bool FOLDER::MakeFullName(TElTreeItem* begin_item, TElTreeItem* end_item, AnsiString& name)
{
	if (begin_item){
    	TElTreeItem* node = begin_item;
        name = "";
        while (node){
			name.Insert(node->Text+AnsiString('\\'),0);
        	if (node==end_item) break;
            node=node->Parent;
        }
        return true;
    }else{
		name = "";
        return false;
    }
}
//---------------------------------------------------------------------------

TElTreeItem* FOLDER::FindItemInFolder(DWORD type, TElTree* tv, TElTreeItem* start_folder, const AnsiString& name)
{
	if (start_folder){
    	for (TElTreeItem* node=start_folder->GetFirstChild(); node; node=start_folder->GetNextChild(node))
        	if (type==((DWORD)(node->Data))&&(node->Text==name)) return node;
    }else{
    	for (TElTreeItem* node=tv->Items->GetFirstNode(); node; node=node->GetNextSibling())
        	if (type==((DWORD)(node->Data))&&(node->Text==name)) return node;
    }
    return 0;
}
//---------------------------------------------------------------------------

TElTreeItem* FOLDER::FindItemInFolder(TElTree* tv, TElTreeItem* start_folder, const AnsiString& name)
{
	if (start_folder){
    	for (TElTreeItem* node=start_folder->GetFirstChild(); node; node=start_folder->GetNextChild(node))
        	if (node->Text==name) return node;
    }else{
    	for (TElTreeItem* node=tv->Items->GetFirstNode(); node; node=node->GetNextSibling())
        	if (node->Text==name) return node;
    }
    return 0;
}
//---------------------------------------------------------------------------

TElTreeItem* FOLDER::FindItem(TElTree* tv, LPCSTR full_name, TElTreeItem** last_valid_node, int* last_valid_idx)
{
	int cnt = _GetItemCount(full_name,'\\');
    if (cnt<=0) return 0;

    // find folder item
    int itm = 0;
	char fld[64];
	TElTreeItem* node = 0;
    TElTreeItem* last_node = 0;
    do{
    	_GetItem(full_name,itm++,fld,'\\');
        last_node = node;
        node = FindItemInFolder(tv,node,fld);
    }while (node&&(itm<cnt));

    if(!node){
		if (last_valid_node) *last_valid_node=last_node;
        if (last_valid_idx) *last_valid_idx=--itm;
    }
    return node;
}
//---------------------------------------------------------------------------

TElTreeItem* FOLDER::FindFolder(TElTree* tv, LPCSTR full_name, TElTreeItem** last_valid_node, int* last_valid_idx)
{
	int cnt = _GetItemCount(full_name,'\\');
    if (cnt<=0) return 0;

    // find folder item
    int itm = 0;
	char fld[64];
	TElTreeItem* node = 0;
    TElTreeItem* last_node = 0;
    do{
    	_GetItem(full_name,itm++,fld,'\\');
        last_node = node;
        node = FindItemInFolder(TYPE_FOLDER,tv,node,fld);
    }while (node&&(itm<cnt));

    if(!node){
		if (last_valid_node) *last_valid_node=last_node;
        if (last_valid_idx) *last_valid_idx=--itm;
    }
    return node;
}
//---------------------------------------------------------------------------

TElTreeItem* FOLDER::FindObject(TElTree* tv, LPCSTR full_name, TElTreeItem** last_valid_node, int* last_valid_idx)
{
	int cnt = _GetItemCount(full_name,'\\'); cnt--;
    if (cnt<0) return 0;

    // find folder item
    int itm = 0;
    char fld[64];
    TElTreeItem* node = 0;
    TElTreeItem* last_node = 0;
    if (cnt){
        do{
            _GetItem(full_name,itm++,fld,'\\');
            last_node = node;
            node = FindItemInFolder(TYPE_FOLDER,tv,node,fld);
        }while (node&&(itm<cnt));
    }

    if(cnt&&!node){
		if (last_valid_node) *last_valid_node=last_node;
        if (last_valid_idx) *last_valid_idx=--itm;
    }else{
    	// find object item if needed
        char obj[64];
        _GetItem(full_name,cnt,obj,'\\');
        last_node = node;
        node = FindItemInFolder(TYPE_OBJECT,tv,node,obj);
        if (!node){
            if (last_valid_node) *last_valid_node=last_node;
            if (last_valid_idx) *last_valid_idx=itm;
        }
    }

    return node;
}
//---------------------------------------------------------------------------

TElTreeItem* FOLDER::AppendFolder(TElTree* tv, LPCSTR full_name)
{
    int idx=0;
	TElTreeItem* last_node=0;
    TElTreeItem* node = FindFolder(tv,full_name,&last_node,&idx);

    if (node) return node;

	char fld[64];
	int cnt = _GetItemCount(full_name,'\\');
	node = last_node;
    for (int itm=idx; itm<cnt; itm++){
    	_GetItem(full_name,itm,fld,'\\');
		node=tv->Items->AddChildObject(node,fld,(void*)TYPE_FOLDER);
    }
	return node;
}
//---------------------------------------------------------------------------

TElTreeItem* FOLDER::AppendObject(TElTree* tv, LPCSTR full_name)
{
    int idx=0;
	TElTreeItem* last_node=0;
	char fld[128];
	int fld_cnt = _GetItemCount(full_name,'\\')-1;
    _GetItems(full_name,0,fld_cnt,fld,'\\');
    TElTreeItem* fld_node = fld[0]?FindFolder(tv,fld,&last_node,&idx):0;

    if (!fld_node){
	    fld_node = last_node;
    	for (int itm=idx; itm<fld_cnt; itm++){
    		_GetItem(full_name,itm,fld,'\\');
			fld_node=tv->Items->AddChildObject(fld_node,fld,(void*)TYPE_FOLDER);
    	}
    }
	char obj[64];
	_GetItem(full_name,fld_cnt,obj,'\\');
    if (FindItemInFolder(TYPE_OBJECT,tv,fld_node,obj)) return 0;
	return tv->Items->AddChildObject(fld_node,obj,(void*)TYPE_OBJECT);
}
//---------------------------------------------------------------------------

void FOLDER::GenerateFolderName(TElTree* tv, TElTreeItem* node, AnsiString& name,LPCSTR pref)
{
	name = pref;
    int cnt = 0;
    while (FindItemInFolder(TYPE_FOLDER,tv,node,name))
    	name.sprintf("%s_%02d",pref,cnt++);
}
//---------------------------------------------------------------------------

void FOLDER::GenerateObjectName(TElTree* tv, TElTreeItem* node, AnsiString& name,LPCSTR pref)
{
	name = pref;
    int cnt = 0;
    while (FindItemInFolder(TYPE_OBJECT,tv,node,name))
    	name.sprintf("%s_%02d",pref,cnt++);
}
//---------------------------------------------------------------------------

