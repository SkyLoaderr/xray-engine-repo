#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FrameOccluder.h"
#include "ui_main.h"
#include "scene.h"
#include "sceneclasslist.h"
#include "PropertiesOccluder.h"
#include "occluder.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TfraOccluder::TfraOccluder(TComponent* Owner)
        : TFrame(Owner)
{
    DEFINE_INI(fsStorage);
}
//---------------------------------------------------------------------------
void __fastcall TfraOccluder::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraOccluder::TopClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraOccluder::ebOptimizeClick(TObject *Sender)
{
	ObjectList lst;
    if (Scene.GetQueryObjects(lst,OBJCLASS_OCCLUDER)){
    	for (ObjectIt it=lst.begin(); it!=lst.end(); it++)
        	((COccluder*)*it)->Optimize();
    }
    Scene.UndoSave();
}
//---------------------------------------------------------------------------

