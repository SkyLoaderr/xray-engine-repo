#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FrameObject.h"
#include "scene.h"
#include "sceneobject.h"
#include "sceneclasslist.h"
#include "EditObject.h"
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
#pragma link "CloseBtn"
#pragma link "RXCtrls"
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

void TfraObject::SelByRefObject( bool flag ){
    ObjectList objlist;
    LPCSTR sel_name=0;
    if (Scene->GetQueryObjects(objlist,OBJCLASS_EDITOBJECT,1,1,-1))
        sel_name = ((CEditObject*)objlist.front())->GetRefName();
	LPCSTR N = TfrmChoseItem::SelectObject(false,true,0,sel_name);
    if (!N) return;
    CLibObject *o = Lib->SearchObject(N);
	if( o && o->IsLoaded() ){
    	CEditObject* _C = o->GetReference();
		ObjectIt _F = Scene->FirstObj(OBJCLASS_EDITOBJECT);
    	ObjectIt _E = Scene->LastObj(OBJCLASS_EDITOBJECT);
        for(;_F!=_E;_F++){
			if( (*_F)->Visible() ){
				CEditObject *_O = (CEditObject *)(*_F);
				if( _O->RefCompare( _C ) )
					_O->Select( flag );
			}
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TfraObject::ebResolveClick(TObject *Sender)
{
	if (Scene->SelectionCount(true,OBJCLASS_EDITOBJECT)){
    	if(Log->DlgMsg(mtConfirmation,"This irreversible operation.\nAre you sure to resolve reference?")==mrYes){
    		ResolveObjects(true);
	    	Scene->UndoSave();
        }
    }else{
        Log->DlgMsg(mtInformation,"Select reference objects and try again.");
    }
}
//---------------------------------------------------------------------------

void TfraObject::ResolveObjects(bool bMsg){
	ObjectList objset;
	int cnt=Scene->GetQueryObjects(objset, OBJCLASS_EDITOBJECT, 1, 1, 0);
	if (cnt){
		for (ObjectIt _F=objset.begin(); _F!=objset.end(); _F++){
			CEditObject *sobj = (CEditObject *)(*_F);
            if (sobj->IsReference()){
            	sobj->ResolveReference();
            }
        }
    }
    if (bMsg){
	    if (cnt) Log->DlgMsg(mtInformation,"Resolve %d object(s)",cnt);
        else 	 Log->DlgMsg(mtInformation,"Select reference objects and try again.");
    }
}

//----------------------------------------------------

//---------------------------------------------------------------------------
// Add new
//---------------------------------------------------------------------------
void __fastcall TfraObject::ebCurObjClick(TObject *Sender)
{
	LPCSTR N = TfrmChoseItem::SelectObject(false,true,0,0);
    if (!N) return;
    Lib->SetCurrentObject(N);
    // set current object
    OutCurrentName();
}
//---------------------------------------------------------------------------
void __fastcall TfraObject::OutCurrentName(){
    if (Lib->GetCurrentObject()){
        ebCurObj->Caption = AnsiString(Lib->GetCurrentObject()->GetRefName());
    }else{
		ebCurObj->Caption = "<none>";
    }
}
//---------------------------------------------------------------------------

