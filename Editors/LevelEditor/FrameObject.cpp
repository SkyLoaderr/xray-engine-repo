#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FrameObject.h"
#include "scene.h"
#include "ui_main.h"
#include "EditObject.h"
#include "SceneObject.h"
#include "propertiesobject.h"
#include "library.h"
#include "ChoseForm.h"
#include "xr_trims.h"
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

void __fastcall TfraObject::ebMultiSelectByRefClick(TObject *Sender)
{
    ObjectList objlist;
    if (Scene.GetQueryObjects(objlist,OBJCLASS_SCENEOBJECT,1,1,-1)){
    	for (ObjectIt it=objlist.begin(); it!=objlist.end(); it++){
	        LPCSTR N = ((CSceneObject*)*it)->GetRefName();
            ObjectIt _F = Scene.FirstObj(OBJCLASS_SCENEOBJECT);
            ObjectIt _E = Scene.LastObj(OBJCLASS_SCENEOBJECT);
            for(;_F!=_E;_F++){
                if((*_F)->Visible()){
                    CSceneObject *_O = (CSceneObject *)(*_F);
                    if(_O->RefCompare(N)) _O->Select( true );
                }
            }
        }
    }
}
//---------------------------------------------------------------------------

void TfraObject::SelByRefObject( bool flag )
{
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
void __fastcall TfraObject::OutCurrentName()
{
	LPCSTR N = Lib.GetCurrentObject();
	ebCurObj->Caption = (N&&N[0])?N:NONE_CAPTION;
}
//---------------------------------------------------------------------------

void __fastcall TfraObject::ebMultiAppendClick(TObject *Sender)
{
	LPCSTR N = TfrmChoseItem::SelectObject(true,0,(ebCurObj->Caption!=NONE_CAPTION)?ebCurObj->Caption.c_str():0);
    if (N){
    	Fvector pos={0.f,0.f,0.f};
    	Fvector up={0.f,1.f,0.f};
        Scene.SelectObjects(false,OBJCLASS_SCENEOBJECT);
	    AStringVec lst;
    	SequenceToList(lst,N);
        UI.ProgressStart(lst.size(),"Append object: ");
        for (AStringIt it=lst.begin(); it!=lst.end(); it++){
            string256 namebuffer;
            Scene.GenObjectName(OBJCLASS_SCENEOBJECT, namebuffer, it->c_str());
            CSceneObject *obj = new CSceneObject(0,namebuffer);
            CEditableObject* ref = obj->SetReference(it->c_str());
            if (!ref){
                ELog.DlgMsg(mtError,"TfraObject:: Can't load reference object.");
                _DELETE(obj);
                return;
            }
            
            obj->MoveTo(pos,up);
            Scene.AddObject( obj );
        }
        UI.ProgressEnd();
    }
}
//---------------------------------------------------------------------------


