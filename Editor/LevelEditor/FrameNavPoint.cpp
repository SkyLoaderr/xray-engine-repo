#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FrameNavPoint.h"
#include "sceneobject.h"
#include "scene.h"
#include "sceneclasslist.h"
#include "NavPoint.h"
#include "propertiesnavpoint.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma link "multiobj"
#pragma link "RXCtrls"
#pragma link "APHeadLabel"
#pragma resource "*.dfm"
TfraNavPoint *fraNavPoint;
//---------------------------------------------------------------------------
__fastcall TfraNavPoint::TfraNavPoint(TComponent* Owner)
        : TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfraNavPoint::RunEditor(ObjectList& objset)
{
	frmPropertiesNavPoint->Run(&objset);
}
//---------------------------------------------------------------------------

void __fastcall TfraNavPoint::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraNavPoint::Label4Click(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

