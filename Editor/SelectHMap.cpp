//---------------------------------------------------------------------------

#include "pch.h"
#include "ui_tools.h"
#pragma hdrstop

#include "SelectHMap.h"
#include "main.h"
#include "ui_main.h"
#include "sceneobject.h"
#include "scene.h"
#include "light.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RXSpin"
#pragma resource "*.dfm"
TfraSelectHMap *fraSelectHMap;
//---------------------------------------------------------------------------
__fastcall TfraSelectHMap::TfraSelectHMap(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools.AddFrame(etHMap,eaSelect,this);
//    seMCenterX->Value   = Scene.m_HMap.vOffset.x;
//    seMCenterY->Value   = Scene.m_HMap.vOffset.y;
//    seMCenterZ->Value   = Scene.m_HMap.vOffset.z;
//    seMDimAlt->Value    = Scene.m_HMap.fDimAlt;
//    seMSlotSize->Value  = Scene.m_HMap.fSlotSize ;
}
//---------------------------------------------------------------------------
void __fastcall TfraSelectHMap::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TfraSelectHMap::seMCenterXChange(TObject *Sender)
{
//    Scene.m_HMap.vOffset.x = seMCenterX->Value;
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectHMap::seMCenterYChange(TObject *Sender)
{
//    Scene.m_HMap.vOffset.y = seMCenterY->Value;
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectHMap::seMCenterZChange(TObject *Sender)
{
//    Scene.m_HMap.vOffset.z = seMCenterZ->Value;
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectHMap::seMDimAltChange(TObject *Sender)
{
//    Scene.m_HMap.fDimAlt = seMDimAlt->Value;
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectHMap::seMSlotSizeChange(TObject *Sender)
{
//    Scene.m_HMap.fSlotSize = seMSlotSize->Value;
}
//---------------------------------------------------------------------------

