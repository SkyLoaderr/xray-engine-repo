#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FrameObject.h"
#include "scene.h"
#include "ui_main.h"
#include "EditObject.h"
#include "SceneObject.h"
#include "library.h"
#include "ChoseForm.h"
#include "xr_trims.h"
#include "ImageThumbnail.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "multi_edit"
#pragma link "mxPlacemnt"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraObject::TfraObject(TComponent* Owner)
        : TFrame(Owner)
{
    DEFINE_INI(fsStorage);
    m_Current = 0;
}
//---------------------------------------------------------------------------
bool __fastcall TfraObject::OnDrawObjectThumbnail(ListItem* sender, TCanvas *Surface, TRect &R)
{
	return FHelper.DrawThumbnail(Surface,R,sender->Key(),EImageThumbnail::EITObject);
}
//---------------------------------------------------------------------------
void __fastcall TfraObject::OnEnter()
{
    m_Items 				= TItemList::CreateForm(paItems, alClient, 0);
    m_Items->OnItemsFocused	= OnItemFocused;
    m_Items->LoadSelection	(fsStorage);
    m_Items->LoadParams		(fsStorage);
    ListItemsVec items;
    
    FS_QueryMap lst;
    if (Lib.GetObjects(lst)){
	    FS_QueryPairIt	it	= lst.begin();
    	FS_QueryPairIt	_E	= lst.end();
	    for (; it!=_E; it++){
            AnsiString fn 	= ChangeFileExt(it->first.c_str(),".thm");
            FS.update_path	(_objects_,fn);
	    	ListItem* I=LHelper.CreateItem(items,it->first.c_str(),0,FS.exist(fn.c_str())?ListItem::flDrawThumbnail:0,0);
            if (I->m_Flags.is(ListItem::flDrawThumbnail)) I->OnDrawThumbnail= OnDrawObjectThumbnail;
        }
    }
    m_Items->AssignItems	(items,false,"Objects",true);
	fsStorage->RestoreFormPlacement();
}
//---------------------------------------------------------------------------
void __fastcall TfraObject::OnExit()
{
    m_Items->SaveSelection	(fsStorage);
    m_Items->SaveParams		(fsStorage);
	fsStorage->SaveFormPlacement();
    TItemList::DestroyForm	(m_Items);
}
//---------------------------------------------------------------------------
void __fastcall TfraObject::OnItemFocused(ListItemsVec& items)
{
	VERIFY(items.size()<=1);
    m_Current 			= 0;
    for (ListItemsIt it=items.begin(); it!=items.end(); it++)
        m_Current 		= (*it)->Key();
    UI.Command			(COMMAND_RENDER_FOCUS);
}
//------------------------------------------------------------------------------
void __fastcall TfraObject::PaneMinClick(TObject *Sender)
{
    PanelMinMaxClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraObject::ExpandClick(TObject *Sender)
{
    PanelMaximizeClick(Sender);
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

void __fastcall TfraObject::MultiSelByRefObject ( bool clear_prev )
{
    ObjectList 	objlist;
    LPU32Vec 	sellist;
    if (Scene.GetQueryObjects(objlist,OBJCLASS_SCENEOBJECT,1,1,-1)){
    	for (ObjectIt it=objlist.begin(); it!=objlist.end(); it++){
	        LPCSTR N = ((CSceneObject*)*it)->GetRefName();
            ObjectIt _F = Scene.FirstObj(OBJCLASS_SCENEOBJECT);
            ObjectIt _E = Scene.LastObj(OBJCLASS_SCENEOBJECT);
            for(;_F!=_E;_F++){
	            CSceneObject *_O = (CSceneObject *)(*_F);
                if((*_F)->Visible()&&_O->RefCompare(N)){
                	if (clear_prev){
                    	_O->Select( false );
	                    sellist.push_back((u32*)_O);
                    }else{
                    	if (!_O->Selected())
                        	sellist.push_back((u32*)_O);
                    }
                }
            }
        }
        std::sort			(sellist.begin(),sellist.end());
        sellist.erase		(std::unique(sellist.begin(),sellist.end()),sellist.end());
        std::random_shuffle	(sellist.begin(),sellist.end());
        int max_k		= iFloor(float(sellist.size())/100.f*float(seSelPercent->Value)+0.5f);
        int k			= 0;
        for (LPU32It o_it=sellist.begin(); k<max_k; o_it++,k++){
            CSceneObject *_O = (CSceneObject *)(*o_it);
            _O->Select( true );
        }
    }
}
//---------------------------------------------------------------------------


void TfraObject::SelByRefObject( bool flag )
{
    ObjectList objlist;
//    LPCSTR sel_name=0;
//    if (Scene.GetQueryObjects(objlist,OBJCLASS_SCENEOBJECT,1,1,-1))
//        sel_name = ((CSceneObject*)objlist.front())->GetRefName();
	LPCSTR N=Current();
//    if (!TfrmChoseItem::SelectItem(TfrmChoseItem::smObject,N,1,sel_name)) return;
	if (N){
        ObjectIt _F = Scene.FirstObj(OBJCLASS_SCENEOBJECT);
        ObjectIt _E = Scene.LastObj(OBJCLASS_SCENEOBJECT);
        for(;_F!=_E;_F++){
            if((*_F)->Visible() ){
                CSceneObject *_O = (CSceneObject *)(*_F);
                if(_O->RefCompare(N)) _O->Select( flag );
            }
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraObject::ebMultiAppendClick(TObject *Sender)
{
	LPCSTR N;
    if (TfrmChoseItem::SelectItem(TfrmChoseItem::smObject,N,32,0)){
    	Fvector pos={0.f,0.f,0.f};
    	Fvector up={0.f,1.f,0.f};
        Scene.SelectObjects(false,OBJCLASS_SCENEOBJECT);
	    AStringVec lst;
    	_SequenceToList(lst,N);
        UI.ProgressStart(lst.size(),"Append object: ");
        for (AStringIt it=lst.begin(); it!=lst.end(); it++){
            string256 namebuffer;
            Scene.GenObjectName(OBJCLASS_SCENEOBJECT, namebuffer, it->c_str());
            CSceneObject *obj = xr_new<CSceneObject>((LPVOID)0,namebuffer);
            CEditableObject* ref = obj->SetReference(it->c_str());
            if (!ref){
                ELog.DlgMsg(mtError,"TfraObject:: Can't load reference object.");
                xr_delete(obj);
                return;
            }
/*            if (ref->IsDynamic()){
                ELog.DlgMsg(mtError,"TfraObject:: Can't assign dynamic object.");
                xr_delete(obj);
                return;
            }
*/
            obj->MoveTo(pos,up);
            Scene.AppendObject( obj );
        }
        UI.ProgressEnd();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraObject::ebMultiSelectByRefMoveClick(TObject *Sender)
{
	MultiSelByRefObject(true);
}
//---------------------------------------------------------------------------

void __fastcall TfraObject::ebMultiSelectByRefAppendClick(TObject *Sender)
{
	MultiSelByRefObject(false);
}
//---------------------------------------------------------------------------


void __fastcall TfraObject::seSelPercentKeyPress(TObject *Sender,
      char &Key)
{
	if (Key==VK_RETURN) UI.Command(COMMAND_RENDER_FOCUS);
}
//---------------------------------------------------------------------------

void __fastcall TfraObject::ExtBtn4Click(TObject *Sender)
{
    if (TfrmChoseItem::SelectItem(TfrmChoseItem::smObject,m_Current,1,m_Current))
    	m_Items->SelectItem(m_Current,true,false,true);
}
//---------------------------------------------------------------------------

void __fastcall TfraObject::paCurrentObjectResize(TObject *Sender)
{
    if (m_Current) m_Items->SelectItem(m_Current,true,false,true);
}
//---------------------------------------------------------------------------



