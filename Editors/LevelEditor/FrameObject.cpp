#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FrameObject.h"
#include "scene.h"
#include "CustomObject.h"
#include "sceneclasslist.h"
#include "EditObject.h"
#include "SceneObject.h"
#include "propertiesobject.h"
#include "ui_main.h"
#include "library.h"
//#include "texturizer.h"
#include "EditMesh.h"
#include "texture.h"
//#include "FrameObjectTexturing.h"
#include "NumericVector.h"
#include "ChoseForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraObject::TfraObject(TComponent* Owner)
        : TFrame(Owner)
{
    OutCurrentName();
}
//---------------------------------------------------------------------------
void __fastcall TfraObject::PaneMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraObject::ExpandClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Selecting
//---------------------------------------------------------------------------

void __fastcall TfraObject::ebSelectByRefsClick(TObject *Sender)
{
	SelByRefObject( true );
}
//---------------------------------------------------------------------------

void __fastcall TfraObject::ebDeselectByRefsClick(TObject *Sender)
{
	SelByRefObject( false );
}
//---------------------------------------------------------------------------

void TfraObject::SelByRefObject( bool flag ){
    ObjectList objlist;
    LPCSTR sel_name=0;
    if (Scene.GetQueryObjects(objlist,OBJCLASS_SCENEOBJECT,1,1,-1))
        sel_name = ((CSceneObject*)objlist.front())->GetRefName();
	LPCSTR N = TfrmChoseItem::SelectObject(false,0,sel_name);
    if (!N) return;
    ObjectIt _F = Scene.FirstObj(OBJCLASS_SCENEOBJECT);
    ObjectIt _E = Scene.LastObj(OBJCLASS_SCENEOBJECT);
    for(;_F!=_E;_F++)
    {
        if((*_F)->Visible() )
        {
            CSceneObject *_O = (CSceneObject *)(*_F);
            if(_O->RefCompare(N)) _O->Select( flag );
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Add new
//---------------------------------------------------------------------------
void __fastcall TfraObject::ebCurObjClick(TObject *Sender)
{
	LPCSTR N = TfrmChoseItem::SelectObject(false,0,(ebCurObj->Caption!=NONE_CAPTION)?ebCurObj->Caption.c_str():0);
    if (!N) return;
    Lib.SetCurrentObject(N);
    // set current object
    OutCurrentName();
}
//---------------------------------------------------------------------------
void __fastcall TfraObject::OutCurrentName(){
	LPCSTR N = Lib.GetCurrentObject();
	ebCurObj->Caption = (N&&N[0])?N:NONE_CAPTION;
}
//---------------------------------------------------------------------------

