//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

#include "ChoseObject.h"
#include "Library.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "propertiesobject.h"
#include "scene.h"
#include "ui_tools.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElTree"
#pragma link "CloseBtn"
#pragma resource "*.dfm"
TfrmChoseObject *frmChoseObject;
//---------------------------------------------------------------------------
__fastcall TfrmChoseObject::TfrmChoseObject(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmChoseObject::AddObject(LPSTR name, void* obj)
{
    return tvObjects->Items->AddChildObject(0, name, obj);
}
//---------------------------------------------------------------------------
void TfrmChoseObject::InitObjectFolder( )
{
    tvObjects->Items->Clear();
    ObjectIt _F = Lib.m_Objects.begin();
    for(;_F!=Lib.m_Objects.end();_F++){
//        TElTreeItem* obj_node =
        AddObject((*_F)->GetName(),(void*)(*_F));
    }
    tvObjects->FullExpand();
}
//---------------------------------------------------------------------------
void __fastcall TfrmChoseObject::sbSelectClick(TObject *Sender)
{
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfrmChoseObject::sbCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
bool __fastcall TfrmChoseObject::SelectObject(SceneObject** obj)
{
    if (ShowModal()==mrOk){
        TElTreeItem* pNode = tvObjects->Selected;
        if (pNode){
            (*obj) = (SceneObject*) pNode->Data;
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
SceneObject *TUI::SelectSObject(){
	SceneObject *_T = Lib.o();
    if (frmChoseObject->SelectObject(&_T)) return _T;
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmChoseObject::FormShow(TObject *Sender)
{
    InitObjectFolder();
}
//---------------------------------------------------------------------------
void __fastcall TfrmChoseObject::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key==VK_ESCAPE) sbCancel->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfrmChoseObject::tvObjectsDblClick(TObject *Sender)
{
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

