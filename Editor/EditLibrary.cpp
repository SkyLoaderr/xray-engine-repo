//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

#include "EditLibrary.h"
#include "Library.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "propertiesobject.h"
#include "scene.h"
#include "library.h"
#include "ui_tools.h"
#include "SObject2.h"
//#include "ObjectPreview.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElTree"
#pragma link "CloseBtn"
#pragma link "ElHeader"
#pragma link "RXCtrls"
#pragma resource "*.dfm"
TfrmEditLibrary *frmEditLibrary;
//---------------------------------------------------------------------------
__fastcall TfrmEditLibrary::TfrmEditLibrary(TComponent* Owner)
    : TForm(Owner)
{
//    preview = 0;
    InitObjectFolder(0);
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmEditLibrary::FindFolderByType(DWORD type)
{
    for ( TElTreeItem* node = tvObjects->Items->GetFirstNode(); node; node = node->GetNext())
        if (!node->Parent && (node->Data == (void*)type)) return node;
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmEditLibrary::FindObjectByType(DWORD type, void *obj)
{
    for ( TElTreeItem* node = tvObjects->Items->GetFirstNode(); node; node = node->GetNext())
        if (!node->Parent && (node->Data == (void*)type)){
            for (TElTreeItem* chield = node->GetFirstChild(); chield; chield = node->GetNextChild(chield))
                if (chield->Data==obj) return chield;
        }
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmEditLibrary::AddFolder(DWORD type)
{
    AnsiString name;
    name.sprintf("%ss",GetNameByClassID(type));
    TElTreeItem* node = tvObjects->Items->AddObject(0,name,(void*)type);
    node->ParentStyle = false;
    node->Bold = true;
    return node;
}

TElTreeItem* TfrmEditLibrary::AddObject(TElTreeItem* node, LPSTR name, void* obj)
{
    return tvObjects->Items->AddChildObject(node, name, obj);
}

void TfrmEditLibrary::InitObjectFolder( SObject2* obj )
{
    tvObjects->Items->Clear();
    ObjectIt _F = Lib.m_Objects.begin();
    for(;_F!=Lib.m_Objects.end();_F++){
        TElTreeItem* node = FindFolderByType((*_F)->ClassID());
        if (!node) node = AddFolder((*_F)->ClassID());
        if (node)
            AddObject(node,(*_F)->GetName(),(void*)(*_F));
    }
    tvObjects->FullExpand();
    if (obj)
        for (int i=0; i<tvObjects->Items->Count; i++){
            TElTreeItem* node = tvObjects->Items->Item[i];
            if (node->Data==obj){
                node->Selected = true;
                node->Focused = true;
                break;
            }
        }

}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::EditLibrary()
{
    ShowModal();
    Lib.m_Current = 0;
}

void __fastcall TfrmEditLibrary::FormShow(TObject *Sender)
{
    sbSave->Enabled = false;
//    preview = new TObjectPreview(false, paPreview);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::tvObjectsItemSelectedChange(
      TObject *Sender, TElTreeItem *Item)
{
//    if (preview && Item->Data) preview->SelectObject((SceneObject*)(Item->Data));
// change mmScript lines
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::FormClose(TObject *Sender,
      TCloseAction &Action)
{
//    if (preview) preview->Terminate();
//    _DELETE(preview);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key==VK_ESCAPE) sbCancel->Click();
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditLibrary::sbEditClick(TObject *Sender)
{
    Scene.UndoPrepare();
    SObject2* _pT;
    TElTreeItem* pNode = tvObjects->Selected;
    if (pNode && !IsClassID(int(pNode->Data))){
        _pT = (SObject2*) pNode->Data;
        if (_pT){
            if (frmPropertiesObject->EditObject(_pT)==mrOk){
                sbSave->Enabled = true;
                InitObjectFolder(_pT);
            }
        }else{
            MessageDlg("Select object to edit.", mtConfirmation, TMsgDlgButtons() << mbOK, 0);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::sbSaveClick(TObject *Sender)
{
    sbSave->Enabled = false;
    Lib.SaveLibrary();
//    Lib.ReloadLibrary();
    SObject2* _pT = 0;
    TElTreeItem* pNode = tvObjects->Selected;
    if (pNode && !IsClassID(int(pNode->Data))) _pT = (SObject2*) pNode->Data;
    InitObjectFolder(_pT);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::sbNewObjectClick(TObject *Sender)
{
    SObject2* obj = new SObject2(true);
    if (frmPropertiesObject->AddNewObject(obj)==mrOk){
        Lib.m_Objects.push_back(obj);
        InitObjectFolder(obj);
        sbSave->Enabled = true;
    }else{
        _DELETE(obj);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::sbDeleteObjectClick(TObject *Sender)
{
    TElTreeItem* pNode = tvObjects->Selected;
    if (pNode && !IsClassID(int(pNode->Data))){
        SObject2* _pT;
        _pT = (SObject2*) pNode->Data;
        if (_pT){
        	Lib.m_Objects.remove( _pT );
            _DELETE(_pT);
            sbSave->Enabled = true;
            InitObjectFolder(_pT);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditLibrary::sbCancelClick(TObject *Sender)
{
    Close();
    if (sbSave->Enabled)
        if (MessageDlg("Library was change. Do you want save?", mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0) == mrYes){
            sbSaveClick(Sender);
        }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditLibrary::tvObjectsDblClick(TObject *Sender)
{
    sbEditClick(Sender);
}
//---------------------------------------------------------------------------

