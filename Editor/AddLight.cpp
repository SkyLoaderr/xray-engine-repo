#include "pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "AddLight.h"
#include "ui_main.h"
#include "sceneobject.h"
#include "scene.h"
#include "light.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma resource "*.dfm"
TfraAddLight *fraAddLight;
//---------------------------------------------------------------------------
__fastcall TfraAddLight::TfraAddLight(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools->AddFrame(etLight,eaAdd,this);
}
//---------------------------------------------------------------------------
void __fastcall TfraAddLight::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraAddLight::Label2Click(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

