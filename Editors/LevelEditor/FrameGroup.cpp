#include "stdafx.h"
#pragma hdrstop

#include "ui_main.h"
#include "FrameGroup.h"
#include "GroupObject.h"
#include "Scene.h"
#include "escenegrouptools.h"
#include "ChoseForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "MXCtrls"
#pragma link "multi_edit"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraGroup::TfraGroup(TComponent* Owner, ESceneGroupTools* gt)
        : TForm(Owner)
{
	ParentTools = gt;	VERIFY(ParentTools);
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::PaneMinClick(TObject *Sender)
{
    PanelMinMaxClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ExpandClick(TObject *Sender)
{
    PanelMaximizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ebMakeGroupClick(TObject *Sender)
{
    ParentTools->GroupObjects	();
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ebUngroupClick(TObject *Sender)
{
    ParentTools->UngroupObjects	();
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ebOpenClick(TObject *Sender)
{
    ParentTools->OpenGroups	();
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ebCloseClick(TObject *Sender)
{
    ParentTools->CloseGroups();
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ebCenterToGroupClick(TObject *Sender)
{
    ParentTools->CenterToGroup	();
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ebAlignToObjectClick(TObject *Sender)
{
	ParentTools->AlignToObject();	
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ebSelectClick(TObject *Sender)
{
	LPCSTR nm;
    if (TfrmChoseItem::SelectItem(smGroup,nm,1))
        ParentTools->SetCurrentObject(nm);	
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ebSaveSelectedClick(TObject *Sender)
{
	ParentTools->SaveSelectedObject();	
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ExtBtn1Click(TObject *Sender)
{
	SelByRefObject( true );
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ExtBtn3Click(TObject *Sender)
{
	SelByRefObject( false );
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ExtBtn6Click(TObject *Sender)
{
	MultiSelByRefObject(true);
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ExtBtn7Click(TObject *Sender)
{
	MultiSelByRefObject(false);
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::seSelPercentKeyPress(TObject *Sender, char &Key)
{
	if (Key==VK_RETURN) ExecCommand(COMMAND_RENDER_FOCUS);
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::MultiSelByRefObject ( bool clear_prev )
{
    ObjectList 	objlist;
    LPU32Vec 	sellist;
    if (Scene->GetQueryObjects(objlist,OBJCLASS_GROUP,1,1,-1)){
    	for (ObjectIt it=objlist.begin(); it!=objlist.end(); it++){
	        LPCSTR N = ((CGroupObject*)*it)->GetRefName();
            ObjectIt _F = Scene->FirstObj(OBJCLASS_GROUP);
            ObjectIt _E = Scene->LastObj(OBJCLASS_GROUP);
            for(;_F!=_E;_F++){
	            CGroupObject *_O = (CGroupObject *)(*_F);
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
            CGroupObject *_O = (CGroupObject *)(*o_it);
            _O->Select( true );
        }
    }
}
//---------------------------------------------------------------------------

void TfraGroup::SelByRefObject( bool flag )
{
    ObjectList objlist;
	LPCSTR N=lbCurrent->Caption.c_str();
	if (N){
        ObjectIt _F = Scene->FirstObj(OBJCLASS_GROUP);
        ObjectIt _E = Scene->LastObj(OBJCLASS_GROUP);
        for(;_F!=_E;_F++){
            if((*_F)->Visible() ){
                CGroupObject *_O = (CGroupObject*)(*_F);
                if(_O->RefCompare(N)) _O->Select( flag );
            }
        }
    }
}
//---------------------------------------------------------------------------


