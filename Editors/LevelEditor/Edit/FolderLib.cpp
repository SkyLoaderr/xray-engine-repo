#include "stdafx.h"
#pragma hdrstop

#include "FolderLib.h"
#include "PropertiesListHelper.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

CFolderHelper FHelper;

static TElTreeItem*			DragItem;

void CFolderHelper::ShowPPMenu(TMxPopupMenu* M, TExtBtn* B){
    POINT pt;
    GetCursorPos(&pt);
	M->Popup(pt.x,pt.y-10);
    if (B) B->MouseManualUp();
}
//---------------------------------------------------------------------------

AnsiString CFolderHelper::GetFolderName(const AnsiString& full_name, AnsiString& dest)
{
    for (int i=full_name.Length(); i>=1; i--)
    	if (full_name[i]=='\\'){
        	dest=full_name.SubString(1,i);
            break;
        }
    return dest.c_str();
}

AnsiString CFolderHelper::GetObjectName(const AnsiString& full_name, AnsiString& dest)
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
bool CFolderHelper::MakeName(TElTreeItem* begin_item, TElTreeItem* end_item, AnsiString& name, bool bOnlyFolder)
{
	if (begin_item){
    	TElTreeItem* node = (u32(begin_item->Data)==TYPE_OBJECT)?begin_item->Parent:begin_item;
        name = "";
        while (node){
			name.Insert(node->Text+AnsiString('\\'),0);
        	if (node==end_item) break;
            node=node->Parent;
        }
        if (!bOnlyFolder){
        	if (u32(begin_item->Data)==TYPE_OBJECT) name+=begin_item->Text;
            else return false;
        }
        return true;
    }else{
		name = "";
        return false;
    }
}
//---------------------------------------------------------------------------

bool CFolderHelper::MakeFullName(TElTreeItem* begin_item, TElTreeItem* end_item, AnsiString& name)
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

TElTreeItem* CFolderHelper::FindItemInFolder(u32 type, TElTree* tv, TElTreeItem* start_folder, const AnsiString& name, bool bIgnoreExt)
{
	if (bIgnoreExt){
        if (start_folder){
            for (TElTreeItem* node=start_folder->GetFirstChild(); node; node=start_folder->GetNextChild(node)){
                AnsiString nm = ChangeFileExt(node->Text,"");
                if (type==((u32)(node->Data))&&(nm==name)) return node;
            }
        }else{
            for (TElTreeItem* node=tv->Items->GetFirstNode(); node; node=node->GetNextSibling()){
                AnsiString nm = ChangeFileExt(node->Text,"");
                if (type==((u32)(node->Data))&&(nm==name)) return node;
            }
        }
    }else{
       if (start_folder){
            for (TElTreeItem* node=start_folder->GetFirstChild(); node; node=start_folder->GetNextChild(node))
                if (type==((u32)(node->Data))&&(node->Text==name)) return node;
        }else{
            for (TElTreeItem* node=tv->Items->GetFirstNode(); node; node=node->GetNextSibling())
                if (type==((u32)(node->Data))&&(node->Text==name)) return node;
        }
    }

    return 0;
}
//---------------------------------------------------------------------------

TElTreeItem* CFolderHelper::FindItemInFolder(TElTree* tv, TElTreeItem* start_folder, const AnsiString& name, bool bIgnoreExt)
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

TElTreeItem* CFolderHelper::FindItem(TElTree* tv, AnsiString full_name, TElTreeItem** last_valid_node, int* last_valid_idx)
{
	if (!full_name.IsEmpty()){
        int cnt = _GetItemCount(full_name.c_str(),'\\');
        if (cnt<=0) return 0;

        // find folder item
        int itm = 0;
        AnsiString fld;
        TElTreeItem* node = 0;
        TElTreeItem* last_node = 0;
        do{
            _GetItem(full_name.c_str(),itm++,fld,'\\');
            last_node = node;
            node = FindItemInFolder(tv,node,fld);
        }while (node&&(itm<cnt));

        if(!node){
            if (last_valid_node) *last_valid_node=last_node;
            if (last_valid_idx) *last_valid_idx=--itm;
        }else{
            if (last_valid_node) *last_valid_node=node;
            if (last_valid_idx) *last_valid_idx=--itm;
        }
        return node;
    }else
    	return 0;
}
//---------------------------------------------------------------------------

TElTreeItem* CFolderHelper::FindFolder(TElTree* tv, AnsiString full_name, TElTreeItem** last_valid_node, int* last_valid_idx)
{
	int cnt = _GetItemCount(full_name.c_str(),'\\');
    if (cnt<=0) return 0;

    // find folder item
    int itm = 0;
	AnsiString fld;
	TElTreeItem* node = 0;
    TElTreeItem* last_node = 0;
    do{
    	_GetItem(full_name.c_str(),itm++,fld,'\\');
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

TElTreeItem* CFolderHelper::FindObject(TElTree* tv, AnsiString full_name, TElTreeItem** last_valid_node, int* last_valid_idx, bool bIgnoreExt)
{
	int cnt = _GetItemCount(full_name.c_str(),'\\'); cnt--;
    if (cnt<0) return 0;

    // find folder item
    int itm = 0;
    AnsiString fld;
    TElTreeItem* node = 0;
    TElTreeItem* last_node = 0;
    if (cnt){
        do{
            _GetItem(full_name.c_str(),itm++,fld,'\\');
            last_node = node;
            node = FindItemInFolder(TYPE_FOLDER,tv,node,fld);
        }while (node&&(itm<cnt));
    }

    if(cnt&&!node){
		if (last_valid_node) *last_valid_node=last_node;
        if (last_valid_idx) *last_valid_idx=--itm;
    }else{
    	// find object item if needed
        AnsiString obj;
        _GetItem(full_name.c_str(),cnt,obj,'\\');
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

TElTreeItem* CFolderHelper::AppendFolder(TElTree* tv, AnsiString full_name)
{
    int idx=0;
	TElTreeItem* last_node=0;
    TElTreeItem* node = FindFolder(tv,full_name,&last_node,&idx);

    if (node) return node;

	AnsiString fld;
	int cnt = _GetItemCount(full_name.c_str(),'\\');
	node = last_node;
    for (int itm=idx; itm<cnt; itm++){
    	_GetItem(full_name.c_str(),itm,fld,'\\');
		node=tv->Items->AddChildObject(node,fld,(void*)TYPE_FOLDER);
    }
	return node;
}
//---------------------------------------------------------------------------

TElTreeItem* CFolderHelper::AppendObject(TElTree* tv, AnsiString full_name)
{
    int idx=0;
	TElTreeItem* last_node=0;
	AnsiString fld;
	int fld_cnt = _GetItemCount(full_name.c_str(),'\\')-1;
    _GetItems(full_name.c_str(),0,fld_cnt,fld,'\\');
//.
    TElTreeItem* fld_node = !fld.IsEmpty()?FindItem/*FindFolder*/(tv,fld,&last_node,&idx):0;
//.
    if (!fld_node){
	    fld_node = last_node;
    	for (int itm=idx; itm<fld_cnt; itm++){
    		_GetItem(full_name.c_str(),itm,fld,'\\');
			fld_node=tv->Items->AddChildObject(fld_node,fld,(void*)TYPE_FOLDER);
    	}
    }
	AnsiString obj;
	_GetItem(full_name.c_str(),fld_cnt,obj,'\\');
    if (FindItemInFolder(TYPE_OBJECT,tv,fld_node,obj)) return 0;
	return tv->Items->AddChildObject(fld_node,obj,(void*)TYPE_OBJECT);
}
//---------------------------------------------------------------------------

void CFolderHelper::GenerateFolderName(TElTree* tv, TElTreeItem* node, AnsiString& name,AnsiString pref)
{
	name = pref;
    int cnt = 0;
    while (FindItemInFolder(TYPE_FOLDER,tv,node,name))
    	name.sprintf("%s_%02d",pref,cnt++);
}
//---------------------------------------------------------------------------

void CFolderHelper::GenerateObjectName(TElTree* tv, TElTreeItem* node, AnsiString& name,AnsiString pref)
{
	name = pref;
    int cnt = 0;
    while (FindItemInFolder(TYPE_OBJECT,tv,node,name))
    	name.sprintf("%s_%02d",pref,cnt++);
}
//---------------------------------------------------------------------------

AnsiString CFolderHelper::ReplacePart(AnsiString old_name, AnsiString ren_part, int level, LPSTR dest)
{
    VERIFY(level<_GetItemCount(old_name.c_str(),'\\'));
    _ReplaceItem(old_name.c_str(),level,ren_part.c_str(),dest,'\\');
    return dest;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Drag'n'Drop
//---------------------------------------------------------------------------
void CFolderHelper::DragDrop(TObject *Sender, TObject *Source, int X, int Y, TOnRenameItem after_drag)
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
    	u32 type = u32(item->Data);
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

void CFolderHelper::DragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State, bool &Accept)
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

void CFolderHelper::StartDrag(TObject *Sender, TDragObject *&DragObject)
{
	TElTree* tv = dynamic_cast<TElTree*>(Sender); VERIFY(Sender);
	if (tv->ItemFocused) 	DragItem = tv->ItemFocused;
  	else					DragItem = 0;
}
//---------------------------------------------------------------------------

bool CFolderHelper::NameAfterEdit(TElTreeItem* node, AnsiString value, AnsiString& N)
{
	VERIFY(node);
	N=N.LowerCase();
    if (N.IsEmpty()){ N=value; return false; }
	int cnt=_GetItemCount(N.c_str(),'\\');
    if (cnt>1)		{ N=value; return false; }
    VERIFY(node);

    for (TElTreeItem* itm=node->GetFirstSibling(); itm; itm=itm->GetNextSibling()){
        if ((itm->Text==N)&&(itm!=node)){
	        N=value;
            return false;
        }
    }
    // all right
    node->Text=N;
	cnt=_GetItemCount(value.c_str(),'\\');
    AnsiString new_name;
	_ReplaceItem(value.c_str(),cnt-1,N.c_str(),new_name,'\\');
    N=new_name;
    return true;
}
//---------------------------------------------------------------------------

bool CFolderHelper::RenameItem(TElTree* tv, TElTreeItem* node, AnsiString& new_text, TOnRenameItem OnRename){
    new_text = new_text.LowerCase();

    // find item with some name
    for (TElTreeItem* item=node->GetFirstSibling(); item; item=item->GetNextSibling()){
        if ((item->Text==new_text)&&(item!=node))
            return false;
    }
    AnsiString full_name;
    if (IsFolder(node)){
    	// is folder - rename all folder items
        for (TElTreeItem* item=node->GetFirstChild(); item&&(item->Level>node->Level); item=item->GetNext()){
            if (IsObject(item)){
                MakeName(item,0,full_name,false);
                VERIFY(node->Level<_GetItemCount(full_name.c_str(),'\\'));
                AnsiString new_full_name;
                _ReplaceItem(full_name.c_str(),node->Level,new_text.c_str(),new_full_name,'\\');
                OnRename(full_name.c_str(),new_full_name.c_str());
            }
        }
    }else if (IsObject(node)){
    	// is object - rename only this item
        MakeName(node,0,full_name,false);
        VERIFY(node->Level<_GetItemCount(full_name.c_str(),'\\'));
        AnsiString new_full_name;
        _ReplaceItem(full_name.c_str(),node->Level,new_text.c_str(),new_full_name,'\\');
		OnRename(full_name.c_str(),new_full_name.c_str());
    }
    tv->Selected=node;
    return true;
}
//------------------------------------------------------------------------------

void CFolderHelper::CreateNewFolder(TElTree* tv, bool bEditAfterCreate)
{
	AnsiString folder;
    AnsiString start_folder;
    MakeName(tv->Selected,0,start_folder,true);
    GenerateFolderName(tv,tv->Selected,folder);
    folder = start_folder+folder;
	TElTreeItem* node = AppendFolder(tv,folder.c_str());
    if (tv->Selected) tv->Selected->Expand(false);
    if (bEditAfterCreate) tv->EditItem(node,-1);
}
//------------------------------------------------------------------------------

BOOL CFolderHelper::RemoveItem(TElTree* tv, TElTreeItem* pNode, TOnRemoveItem OnRemoveItem, TOnAfterRemoveItem OnAfterRemoveItem)
{
	BOOL bRes = FALSE;
    R_ASSERT(OnRemoveItem);
    if (pNode){
		tv->IsUpdating = true;
	    TElTreeItem* pSelNode = pNode->GetPrevSibling();
	    if (!pSelNode) pSelNode = pNode->GetNextSibling();
		AnsiString full_name;
    	if (IsFolder(pNode)){
	        if (ELog.DlgMsg(mtConfirmation, TMsgDlgButtons() << mbYes << mbNo,"Delete selected folder?") == mrYes){
                bRes = TRUE;
		        for (TElTreeItem* item=pNode->GetFirstChild(); item&&(item->Level>pNode->Level); item=item->GetNext()){
                    MakeName(item,0,full_name,false);
                	if (IsObject(item))
                    	if (!OnRemoveItem(full_name.c_str())) bRes=FALSE;
                }
                if (bRes){
                	pNode->Delete();
                    if (OnAfterRemoveItem) OnAfterRemoveItem();
                }
        	}
        }
    	if (IsObject(pNode)){
	        if (ELog.DlgMsg(mtConfirmation, TMsgDlgButtons() << mbYes << mbNo,"Delete selected item?") == mrYes){
				MakeName(pNode,0,full_name,false);
                bRes = OnRemoveItem(full_name.c_str());
	            if (bRes){
                	pNode->Delete();
                    if (OnAfterRemoveItem) OnAfterRemoveItem();
                }
        	}
        }
        if (bRes) tv->Selected = pSelNode;
        tv->IsUpdating 	= false;
        tv->SetFocus();
    }else{
		ELog.DlgMsg(mtInformation, "At first select item.");
    }
    return bRes;
}
TElTreeItem* CFolderHelper::ExpandItem(TElTree* tv, TElTreeItem* node)
{
	if (node){
	    tv->IsUpdating 	= true;
        TElTreeItem* folder	= node;//->Parent;
        while(folder){
			if (folder) folder->Expand(false);
        	if (folder->Parent){
            	folder = folder->Parent;
            }else				break;
        }
	    tv->IsUpdating 	= false;
    }
    return node;
}
TElTreeItem* CFolderHelper::ExpandItem(TElTree* tv, AnsiString full_name)
{
	TElTreeItem* last_valid=0;
    FindItem(tv,full_name,&last_valid);
	return ExpandItem(tv,last_valid);
}
TElTreeItem* CFolderHelper::RestoreSelection(TElTree* tv, TElTreeItem* node, bool bLeaveSel)
{
	if (tv->MultiSelect){
        if (bLeaveSel){
            if (node) node->Selected = true;
        }else{
            tv->DeselectAll();
            if (node) node->Selected = true;
        }
		if (tv->OnAfterSelectionChange) tv->OnAfterSelectionChange(tv);
    }else{
		tv->Selected 		= node;
    }
	if (node){
		tv->EnsureVisible	(node);
    }
    return node;
}
TElTreeItem* CFolderHelper::RestoreSelection(TElTree* tv, AnsiString full_name, bool bLeaveSel)
{
	TElTreeItem* last_valid=0;
    FindItem(tv,full_name,&last_valid);
	return RestoreSelection(tv,last_valid,bLeaveSel);
}
//------------------------------------------------------------------------------

