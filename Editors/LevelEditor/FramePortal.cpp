#include "stdafx.h"
#pragma hdrstop

#include "FramePortal.h"
#include "scene.h"
#include "portal.h"
#include "ui_main.h"
#include "sector.h"
#include "scene.h"
#include "portalutils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraPortal::TfraPortal(TComponent* Owner)
        : TFrame(Owner)
{
	iPickSectors=0;
}
//---------------------------------------------------------------------------
void __fastcall TfraPortal::PanelMinClick(TObject *Sender)
{
    PanelMinMaxClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfraPortal::TopClick(TObject *Sender)
{
    PanelMaximizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraPortal::ebComputeAllPortalsClick(TObject *Sender)
{
	if (mrYes==ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Are you sure want to destroy all existing portals and compute them again?"))
    {
		int cnt=PortalUtils.CalculateAllPortals();
		if (cnt) ELog.DlgMsg(mtInformation,"Calculated '%d' portal(s).",cnt);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraPortal::ebComputeClick(TObject *Sender)
{
	if (mrYes==ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Are you sure want to destroy selected portals and compute them again?"))
    {
		int cnt = PortalUtils.CalculateSelectedPortals();
		if (cnt) ELog.DlgMsg(mtInformation,"Calculated '%d' portal(s).",cnt);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraPortal::ebInvertOrientClick(TObject *Sender)
{
	ObjectList lst;
    if (Scene.GetQueryObjects(lst,OBJCLASS_PORTAL,1,1,0)){
    	for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
			CPortal* _O = (CPortal*)*it;
		    _O->InvertOrientation();
        }
    }
}
//---------------------------------------------------------------------------

