#include "stdafx.h"
#pragma hdrstop

#include "FolderLib.h"
#include "xr_trims.h"
#include "PropertiesListTypes.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

static TElTreeItem*			DragItem;

void FOLDER::ShowPPMenu(TMxPopupMenu* M, TExtBtn* B){
    POINT pt;
    GetCursorPos(&pt);
	M->Popup(pt.x,pt.y-10);
    if (B) B->MouseManualUp();
}
//---------------------------------------------------------------------------

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
        name = node->Text;
		node = node->Parent;
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

TElTreeItem* FOLDER::FindItemInFolder(DWORD type, TElTree* tv, TElTreeItem* start_folder, const AnsiString& name, bool bIgnoreExt)
{
	if (bIgnoreExt){
        if (start_folder){
            for (TElTreeItem* node=start_folder->GetFirstChild(); node; node=start_folder->GetNextChild(node)){
                AnsiString nm = ChangeFileExt(node->Text,"");
                if (type==((DWORD)(node->Data))&&(nm==name)) return node;
            }
        }else{
            for (TElTreeItem* node=tv->Items->GetFirstNode(); node; node=node->GetNextSibling()){
                AnsiString nm = ChangeFileExt(node->Text,"");
                if (type==((DWORD)(node->Data))&&(nm==name)) return node;
            }
        }
    }else{
       if (start_folder){
            for (TElTreeItem* node=start_folder->GetFirstChild(); node; node=start_folder->GetNextChild(node))
                if (type==((DWORD)(node->Data))&&(node->Text==name)) return node;
        }else{
            for (TElTreeItem* node=tv->Items->GetFirstNode(); node; node=node->GetNextSibling())
                if (type==((DWORD)(node->Data))&&(node->Text==name)) return node;
        }
    }

    return 0;
}
//---------------------------------------------------------------------------

TElTreeItem* FOLDER::FindItemInFolder(TElTree* tv, TElTreeItem* start_folder, const AnsiString& name, bool bIgnoreExt)
{
	if (bIgnoreExt){
        if (start_folder){
            for (TElTreeItem* node=start_folder->GetFirstChild(); node; node=start_folder->GetNextChild(node)){
                AnsiString nm = ChangeFileExt(node->Text,"");
                if (nm==name) return node;
            }
        }else{
            for (TElTreeItem* node=tv->Items->GetFirstNode(); node; node=node->GetNextSibling()){
                AnsiString nm = ChangeFileExt(node->Text,"");
                if (nm==name) return node;
            }
        }
    }else{
        if (start_folder){
            for (TElTreeItem* node=start_folder->GetFirstChild(); node; node=start_folder->GetNextChild(node))
                if (node->Text==name) return node;
        }else{
            for (TElTreeItem* node=tv->Items->GetFirstNode(); node; node=node->GetNextSibling())
                if (node->Text==name) return node;
        }
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

TElTreeItem* FOLDER::FindObject(TElTree* tv, LPCSTR full_name, TElTreeItem** last_valid_node, int* last_valid_idx, bool bIgnoreExt)
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
        node = FindItemInFolder(TYPE_OBJECT,tv,node,obj,bIgnoreExt);
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

LPCSTR FOLDER::ReplacePart(LPCSTR old_name, LPCSTR ren_part, int level, LPSTR dest)
{
    VERIFY(level<_GetItemCount(old_name,'\\'));
    _ReplaceItem(old_name,level,ren_part,dest,'\\');
    return dest;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Drag'n'Drop
//---------------------------------------------------------------------------
void FOLDER::DragDrop(TObject *Sender, TObject *Source, int X, int Y, TOnRenameItem after_drag)
{
	R_ASSERT(after_drag);

	TElTree* tv = dynamic_cast<TElTree*>(Sender); VERIFY(Sender);
    TSTItemPart IP=(TSTItemPart)0;
    int 		hc=0;
	TElTreeItem* tgt_folder = tv->GetItemAt(X, Y, IP, hc);
    if (tgt_folder&&(IsObject(tgt_folder))) tgt_folder=tgt_folder->Parent;

    AnsiString base_name;
    MakeName(tgt_folder,0,base_name,true);
    AnsiString cur_fld_name=base_name;
    TElTreeItem* cur_folder=tgt_folder;

    int drg_level=DragItem->Level;

    TElTreeItem* item = DragItem;
    do{
    	DWORD type = DWORD(item->Data);
		TElTreeItem* pNode = FindItemInFolder(type,tv,cur_folder,item->Text);
		if (pNode&&IsObject(item)){
            item=item->GetNext();
        	continue;
        }

        if (!pNode) pNode = tv->Items->AddChildObject(cur_folder,item->Text,(TObject*)type);
		if (IsFolder(item)){
        	cur_folder = pNode;
		    MakeName(cur_folder,0,cur_fld_name,true);
            item=item->GetNext();
        }else{
        	// rename
		    AnsiString old_name, new_name;
		    MakeName(item,0,old_name,false);
		    MakeName(pNode,0,new_name,false);

            after_drag(old_name.c_str(),new_name.c_str());

            TElTreeItem* parent=item->Parent;
            // get next item && delete existance
            TElTreeItem* next=item->GetNext();
			item->Delete();

            if (parent&&((parent->GetLastChild()==item)||(0==parent->ChildrenCount))){
	            if (0==parent->ChildrenCount) parent->Delete();
	        	cur_folder = cur_folder?cur_folder->Parent:0;
            }

            item=next;
        }
    }while(item&&(item->Level>drg_level));
}
//---------------------------------------------------------------------------

void FOLDER::DragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
	TElTree* tv = dynamic_cast<TElTree*>(Sender); VERIFY(Sender);
	TElTreeItem* tgt;
    TElTreeItem* src=DragItem;
    TSTItemPart IP;
    int HCol;
    if (!DragItem) Accept = false;
  	else{
		tgt = tv->GetItemAt(X, Y, IP, HCol);
        if (tgt){
        	if (IsFolder(src)){
                bool b = true;
                for (TElTreeItem* itm=tgt->Parent; itm; itm=itm->Parent) if (itm==src){b=false; break;}
            	if (IsFolder(tgt)){
		        	Accept = b&&(tgt!=src)&&(src->Parent!=tgt);
                }else if (IsObject(tgt)){
		        	Accept = b&&(src!=tgt->Parent)&&(tgt!=src)&&(tgt->Parent!=src->Parent);
                }
            }else if (IsObject(src)){
            	if (IsFolder(tgt)){
		        	Accept = (tgt!=src)&&(src->Parent!=tgt);
                }else if (IsObject(tgt)){
		        	Accept = (tgt!=src)&&(src->Parent!=tgt->Parent);
                }
            }
        }else Accept = !!DragItem->Parent;
    }
}
//---------------------------------------------------------------------------

void FOLDER::StartDrag(TObject *Sender, TDragObject *&DragObject)
{
	TElTree* tv = dynamic_cast<TElTree*>(Sender); VERIFY(Sender);
	if (tv->ItemFocused) 	DragItem = tv->ItemFocused;
  	else					DragItem = 0;
}
//---------------------------------------------------------------------------

bool FOLDER::AfterTextEdit(TElTreeItem* node, LPCSTR value, AnsiString& N)
{
	N=N.LowerCase();
	int cnt=_GetItemCount(N.c_str(),'\\');
    if (cnt>1){ N=value; return false; }
    VERIFY(node);
    for (TElTreeItem* itm=node->GetFirstSibling(); itm; itm=itm->GetNextSibling()){
        if ((itm->Text==N)&&(itm!=node)){
	        N=value;
            return false;
        }
    }
    // all right
    node->Text=N;
	cnt=_GetItemCount(value,'\\');
    string256 new_name;
	_ReplaceItem(value,cnt-1,N.c_str(),new_name,'\\');
    N=new_name;
    return true;
}
//---------------------------------------------------------------------------

void FOLDER::BeforeTextEdit(LPCSTR value, AnsiString& N)
{
	int cnt=_GetItemCount(N.c_str(),'\\');
	N = _SetPos(N.c_str(),cnt-1,'\\');
}
//------------------------------------------------------------------------------
void FOLDER::TextDraw(LPCSTR value, AnsiString& N)
{
	int cnt=_GetItemCount(N.c_str(),'\\');
	N = _SetPos(N.c_str(),cnt-1,'\\');
}
//------------------------------------------------------------------------------

bool FOLDER::RenameItem(TElTree* tv, TElTreeItem* node, AnsiString& new_text, TOnRenameItem OnRename){
    new_text = new_text.LowerCase();

    // find item with some name
    for (TElTreeItem* item=node->GetFirstSibling(); item; item=item->GetNextSibling()){
        if ((item->Text==new_text)&&(item!=node))
            return false;
    }
    AnsiString full_name;
    if (FOLDER::IsFolder(node)){
    	// is folder - rename all folder items
        for (TElTreeItem* item=node->GetFirstChild(); item&&(item->Level>node->Level); item=item->GetNext()){
            if (FOLDER::IsObject(item)){
                FOLDER::MakeName(item,0,full_name,false);
                VERIFY(node->Level<_GetItemCount(full_name.c_str(),'\\'));
                string256 new_full_name;
                _ReplaceItem(full_name.c_str(),node->Level,new_text.c_str(),new_full_name,'\\');
                OnRename(full_name.c_str(),new_full_name);
            }
        }
    }else if (FOLDER::IsObject(node)){
    	// is object - rename only this item
        FOLDER::MakeName(node,0,full_name,false);
        VERIFY(node->Level<_GetItemCount(full_name.c_str(),'\\'));
        string256 new_full_name;
        _ReplaceItem(full_name.c_str(),node->Level,new_text.c_str(),new_full_name,'\\');
		OnRename(full_name.c_str(),new_full_name);
    }
    tv->Selected=node;
    return true;
}
//------------------------------------------------------------------------------

void FOLDER::CreateNewFolder(TElTree* tv, bool bEditAfterCreate)
{
	AnsiString folder;
    AnsiString start_folder;
    FOLDER::MakeName(tv->Selected,0,start_folder,true);
    FOLDER::GenerateFolderName(tv,tv->Selected,folder);
    folder = start_folder+folder;
	TElTreeItem* node = FOLDER::AppendFolder(tv,folder.c_str());
    if (tv->Selected) tv->Selected->Expand(false);
    if (bEditAfterCreate) tv->EditItem(node,-1);
}
//------------------------------------------------------------------------------

bool FOLDER::RemoveItem(TElTree* tv, TElTreeItem* pNode, TOnRemoveItem OnRemoveItem)
{
	bool bRes = false;
    if (pNode){
		tv->IsUpdating = true;
	    TElTreeItem* pSelNode = pNode->GetPrevSibling();
	    if (!pSelNode) pSelNode = pNode->GetNextSibling();
		AnsiString full_name;
    	if (FOLDER::IsFolder(pNode)){
	        if (ELog.DlgMsg(mtConfirmation, "Delete selected folder?") == mrYes){
		        for (TElTreeItem* item=pNode->GetFirstChild(); item&&(item->Level>pNode->Level); item=item->GetNext()){
                    FOLDER::MakeName(item,0,full_name,false);
                	if (FOLDER::IsObject(item)) OnRemoveItem(full_name.c_str());
                }
	            pNode->Delete();
                bRes = true;
        	}
        }
    	if (FOLDER::IsObject(pNode)){
	        if (ELog.DlgMsg(mtConfirmation, "Delete selected blender?") == mrYes){
				FOLDER::MakeName(pNode,0,full_name,false);
                OnRemoveItem(full_name.c_str());
	            pNode->Delete();
                bRes = true;
        	}
        }
        tv->Selected	= pSelNode;
		tv->IsUpdating 	= false;
        tv->SetFocus();
    }else{
		ELog.DlgMsg(mtInformation, "At first select item.");
    }
    return bRes;
}
TElTreeItem* FOLDER::RestoreSelection(TElTree* tv, TElTreeItem* node)
{
	if (node){
	    tv->Selected 		= node;
		tv->EnsureVisible	(node);
        TElTreeItem* folder	= node->Parent;
        while(folder){
			if (folder) folder->Expand(false);
        	if (folder->Parent){ 
            	folder = folder->Parent;
            }else				break;
        }
    }
    return node;
}
TElTreeItem* FOLDER::RestoreSelection(TElTree* tv, LPCSTR full_name)
{
	return RestoreSelection(tv,FindItem(tv,full_name));
}
//------------------------------------------------------------------------------

