//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

#include "ObjectList.h"
#include "Light.h"
#include "SceneClassList.h"
#include "ui_main.h"
#include "selectobject.h"
#include "selectlight.h"
#include "selectsound.h"
#include "UI_Tools.h"
#include "Scene.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RXCtrls"
#pragma link "multiobj"
#pragma link "ElTree"
#pragma link "CloseBtn"
#pragma resource "*.dfm"
TfrmObjectList *frmObjectList;
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
    tmRefreshSelection->Enabled = true;
    obj_count = 0;
    tvObjects->FilteredVisibility = !cbShowHidden->Checked;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmObjectList::FindFolderByType(DWORD type)
{
    for ( TElTreeItem* node = tvObjects->Items->GetFirstNode(); node; node = node->GetNext())
        if (!node->Parent && (node->Data == (void*)type)) return node;
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmObjectList::FindObjectByType(DWORD type, void *obj)
{
    for ( TElTreeItem* node = tvObjects->Items->GetFirstNode(); node; node = node->GetNext())
        if (!node->Parent && (node->Data == (void*)type)){
            for (TElTreeItem* chield = node->GetFirstChild(); chield; chield = node->GetNextChild(chield))
                if (chield->Data==obj) return chield;
        }
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmObjectList::AddFolder(DWORD type)
{
    AnsiString name;
    name.sprintf("%ss",GetNameByClassID(type));
    TElTreeItem* node = tvObjects->Items->AddObject(0,name,(void*)type);
    node->ParentStyle = false;
    node->Bold = true;
    return node;
}

TElTreeItem* TfrmObjectList::AddObject(TElTreeItem* node, LPSTR name, void* obj)
{
    return tvObjects->Items->AddChildObject(node, name, obj);
}

void __fastcall TfrmObjectList::InitListBox()
{
    tvObjects->Items->Clear();
    ObjectIt _F = Scene.FirstObj();
    for(;_F!=Scene.LastObj();_F++){
        TElTreeItem* node = FindFolderByType((*_F)->ClassID());
        if (!node) node = AddFolder((*_F)->ClassID());
        if (node)
//            TElTreeItem* obj_node =
            AddObject(node,(*_F)->GetName(),(void*)(*_F));
    }
    UpdateState();
    tvObjects->FullExpand();
    obj_count = Scene.ObjCount();
}

void TfrmObjectList::UpdateState()
{
    ObjectIt _F = Scene.FirstObj();
    for(;_F!=Scene.LastObj();_F++){
        TElTreeItem* node = FindObjectByType((*_F)->ClassID(), (void*)(*_F));
        if (node){
            node->ParentStyle = false;
            node->StrikeOut = !(*_F)->Visible();
            node->Hidden    = !(*_F)->Visible();
            node->Selected  = (*_F)->Selected();
        }
    }
    UI.UpdateScene();
}

void __fastcall  TfrmObjectList::UpdateListBox()
{
    TElTreeItem* node = tvObjects->Items->GetFirstNode();
    while (node){
        if (node->Parent && node->Data){
            SceneObject* obj = (SceneObject*)(node->Data);
            node->ParentStyle = false;
            node->StrikeOut = !obj->Visible();
            if (!node->StrikeOut)
                node->Selected = obj->Selected();
        }
        node = node->GetNext();
    }
}

void TfrmObjectList::UpdateSelection()
{
	Scene.SelectObjects( false, OBJCLASS_NONE );
    for (TElTreeItem* node = tvObjects->GetNextSelected(0); node; node=tvObjects->GetNextSelected(node))
        if (node->Parent) ((SceneObject*)(node->Data))->Select(true);
    UI.UpdateScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::tvObjectsTryEdit(TObject *Sender,
      TElTreeItem *Item, TElHeaderSection *Section, TFieldTypes &CellType,
      bool &CanEdit)
{
    CanEdit = false;
}
//---------------------------------------------------------------------------


void __fastcall TfrmObjectList::tvObjectsItemSelectedChange(
      TObject *Sender, TElTreeItem *Item)
{
    UpdateSelection();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::sbHideSelClick(TObject *Sender)
{
    for (TElTreeItem* node = tvObjects->GetNextSelected(0); node; node=tvObjects->GetNextSelected(node))
        if (node->Parent) ((SceneObject*)(node->Data))->Show(false);
    UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::sbShowSelClick(TObject *Sender)
{
    for (TElTreeItem* node = tvObjects->GetNextSelected(0); node; node=tvObjects->GetNextSelected(node))
        if (node->Parent){
            ((SceneObject*)(node->Data))->Show(true);
            ((SceneObject*)(node->Data))->Select(true);
        }
    UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::sbHideGroupClick(TObject *Sender)
{
    TElTreeItem* parent_node = tvObjects->GetNextSelected(0);
    if (parent_node->Parent) parent_node = parent_node->Parent;

    for (TElTreeItem* node = parent_node->GetFirstChild(); node; node=parent_node->GetNextChild(node))
        if (node->Parent) ((SceneObject*)(node->Data))->Show(false);

    UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::sbShowGroupClick(TObject *Sender)
{
    TElTreeItem* parent_node = tvObjects->GetNextSelected(0);
    if (parent_node->Parent) parent_node = parent_node->Parent;

    for (TElTreeItem* node = parent_node->GetFirstChild(); node; node=parent_node->GetNextChild(node))
        if (node->Parent){
            ((SceneObject*)(node->Data))->Show(true);
            ((SceneObject*)(node->Data))->Select(true);
        }

    UpdateState();
}
//---------------------------------------------------------------------------
void __fastcall TfrmObjectList::sbHideAllClick(TObject *Sender)
{
	Scene.ShowObjects( false, OBJCLASS_NONE );
    UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::sbShowAllClick(TObject *Sender)
{
	Scene.ShowObjects( true, OBJCLASS_NONE );
    UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::sbPropertiesClick(TObject *Sender)
{
//    "Different type selected. Properties unavailable."
    fraSelectLight->RunEditor();
    fraSelectObject->RunEditor();
    fraSelectSound->RunEditor();
    UpdateState();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::sbRefreshListClick(TObject *Sender)
{
    InitListBox();
    tvObjects->FullExpand();
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
    tvObjects->OnItemSelectedChange = 0;

    if (Scene.ObjCount()!=obj_count) InitListBox();
    UpdateListBox();

    // enable UpdateSelection
    tvObjects->OnItemSelectedChange = tvObjectsItemSelectedChange;
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    tmRefreshSelection->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjectList::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key==VK_ESCAPE) sbClose->Click();
}
//---------------------------------------------------------------------------


void __fastcall TfrmObjectList::cbShowHiddenClick(TObject *Sender)
{
    tvObjects->FilteredVisibility = !cbShowHidden->Checked;
}
//---------------------------------------------------------------------------


void __fastcall TfrmObjectList::tvObjectsDblClick(TObject *Sender)
{
    sbPropertiesClick(Sender);
}
//---------------------------------------------------------------------------


