#include "stdafx.h"
#pragma hdrstop

#include "ObjectList.h"
#include "ELight.h"
#include "UI_Tools.h"
#include "Scene.h"
#include "leftbar.h"
#include "ui_main.h"
#include "folderlib.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmObjectList *frmObjectList=0;
//---------------------------------------------------------------------------
void frmObjectListShow(){
	if (frmObjectList){
    	frmObjectList->SetFocus();
    }else{
		frmObjectList = new TfrmObjectList(0);
    	frmObjectList->Show();
    }
}
//---------------------------------------------------------------------------
__fastcall TfrmObjectList::TfrmObjectList(TComponent* Owner)
    : TForm(Owner)
{
}

void __fastcall TfrmObjectList::sbCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::FormShow(TObject *Sender)
{
	Left = 0; Top = 45;
    tmRefreshSelection->Enabled = true;
    tmRefreshList->Enabled = true;
    obj_count = 0;
    cur_cls = OBJCLASS_DUMMY;
	tvItems->FilteredVisibility = ((rgSO->ItemIndex==1)||(rgSO->ItemIndex==2));
    InitListBox();
	// check window position
	UI.CheckWindowPos(this);
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmObjectList::FindFolderByType(int type)
{
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext())
        if (!node->Parent && (node->Data == (void*)type)) return node;
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmObjectList::FindObjectByType(int type, void *obj)
{
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext())
        if (!node->Parent && (node->Data == (void*)type)){
            for (TElTreeItem* chield = node->GetFirstChild(); chield; chield = node->GetNextChild(chield))
                if (chield->Data==obj) return chield;
            break;
        }
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmObjectList::AddFolder(int type)
{
    AnsiString name;
    name.sprintf("%ss",GetNameByClassID((EObjClass)type));
    TElTreeItem* node = tvItems->Items->AddObject(0,name,(void*)type);
    node->ParentStyle = false;
    node->Bold = true;
    return node;
}

TElTreeItem* TfrmObjectList::AddObject(TElTreeItem* node, LPSTR name, void* obj)
{
    return tvItems->Items->AddChildObject(node, name, obj);
}

void __fastcall TfrmObjectList::InitListBox()
{
    tvItems->IsUpdating = true;
    tvItems->Items->Clear();
    cur_cls = Tools.CurrentClassID();
    for(ObjectPairIt it=Scene.FirstClass(); it!=Scene.LastClass(); it++){
        ObjectList& lst = it->second;
        if ((cur_cls==OBJCLASS_DUMMY)||(it->first==cur_cls)){
            if (IsObjectListClassID(it->first)){
                TElTreeItem* node = FindFolderByType(it->first);
                if (!node) node = AddFolder(it->first);
                VERIFY(node);
                for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
                    AddObject(node,(*_F)->Name,(void*)(*_F));
            }
        }
    }
    tvItems->Sort(true);
    tvItems->IsUpdating = false;

    UpdateState();
    tvItems->FullExpand();
    obj_count 	= Scene.ObjCount();
}

void TfrmObjectList::UpdateState()
{
    tvItems->IsUpdating = true;

	tvItems->OnItemSelectedChange = 0;

    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext()){
        if (node&&(node->Level>0)){
        	CCustomObject* O = (CCustomObject*)node->Data;
            node->ParentStyle = false;
            node->StrikeOut = !O->Visible();
            if (rgSO->ItemIndex==1) 	 node->Hidden = !O->Visible();
            else if (rgSO->ItemIndex==2) node->Hidden = O->Visible();
            if (O->Visible())			 node->Selected=O->Selected();
        }
    }

/*    for(ObjectPairIt it=Scene.FirstClass(); it!=Scene.LastClass(); it++){
        ObjectList& lst = (*it).second;
        if (IsObjectListClassID((*it).first))
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
                TElTreeItem* node = FindObjectByType((*_F)->ClassID(), (void*)(*_F));
                if (node){
                    node->ParentStyle = false;
                    node->StrikeOut = !(*_F)->Visible();
                    if (rgSO->ItemIndex==1) 	 node->Hidden = !(*_F)->Visible();
                    else if (rgSO->ItemIndex==2) node->Hidden = (*_F)->Visible();
                    if ((*_F)->Visible())		 node->Selected=(*_F)->Selected();
                }
            }
    }
*/
    tvItems->IsUpdating = false;
}

void TfrmObjectList::UpdateSelection()
{
	Scene.SelectObjects( false, (EObjClass)cur_cls );
    for (TElTreeItem* node = tvItems->GetNextSelected(0); node; node=tvItems->GetNextSelected(node))
        if (node->Parent) ((CCustomObject*)(node->Data))->Select(true);
    UI.RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::tvItemsItemSelectedChange(
      TObject *Sender, TElTreeItem *Item)
{
    UpdateSelection();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::ebHideSelClick(TObject *Sender)
{
    for (TElTreeItem* node = tvItems->GetNextSelected(0); node; node=tvItems->GetNextSelected(node))
        if (node->Parent) ((CCustomObject*)(node->Data))->Show(false);
    UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::ebShowSelClick(TObject *Sender)
{
    for (TElTreeItem* node = tvItems->GetNextSelected(0); node; node=tvItems->GetNextSelected(node))
        if (node->Parent){
            ((CCustomObject*)(node->Data))->Show(true);
            ((CCustomObject*)(node->Data))->Select(true);
        }
    UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::sbRefreshListClick(TObject *Sender)
{
    if ((Scene.ObjCount()!=obj_count)||(cur_cls!=Tools.CurrentClassID()))
	    InitListBox();
    else
    	UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::tmRefreshListTimer(TObject *Sender)
{
    UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::tmRefreshSelectionTimer(TObject *Sender)
{
    // disable UpdateSelection
    tvItems->OnItemSelectedChange = 0;

    if ((Scene.ObjCount()!=obj_count)||(cur_cls!=Tools.CurrentClassID()))
		InitListBox();
//    UpdateListBox();

    // enable UpdateSelection
    tvItems->OnItemSelectedChange = tvItemsItemSelectedChange;
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	Action = caFree;
	frmObjectList = 0;

    tmRefreshSelection->Enabled = false;
    tmRefreshList->Enabled = false;

    tvItems->OnItemSelectedChange = 0;
    tvItems->Items->Clear();
    tvItems->OnItemSelectedChange = tvItemsItemSelectedChange;
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key==VK_ESCAPE) sbClose->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::rgSOClick(TObject *Sender)
{
	UpdateState();
	tvItems->FilteredVisibility = ((rgSO->ItemIndex==1)||(rgSO->ItemIndex==2));
}
//---------------------------------------------------------------------------

extern bool __fastcall LookupFunc(TElTreeItem* Item, void* SearchDetails);
//---------------------------------------------------------------------------
void __fastcall TfrmObjectList::tvItemsKeyPress(TObject *Sender,
      char &Key)
{
	TElTreeItem* node = tvItems->Items->LookForItemEx(tvItems->Selected,-1,false,false,false,&Key,LookupFunc);
    if (!node) node = tvItems->Items->LookForItemEx(0,-1,false,false,false,&Key,LookupFunc);
    FHelper.RestoreSelection(tvItems,node);
}
//---------------------------------------------------------------------------

