#include "stdafx.h"
#pragma hdrstop

#include "FramePortal.h"
#include "scene.h"
#include "portal.h"
#include "ui_tools.h"
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
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfraPortal::TopClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraPortal::ebComputeAllPortalsClick(TObject *Sender)
{
	int cnt=PortalUtils.CalculateAllPortals();
	if (cnt) ELog.DlgMsg(mtInformation,"Calculated '%d' portal(s).",cnt);
}
//---------------------------------------------------------------------------

void __fastcall TfraPortal::ebComputeClick(TObject *Sender)
{
	PortalUtils.CalculateSelectedPortals();
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

