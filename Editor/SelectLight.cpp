#include "pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "SelectLight.h"
#include "sceneobject.h"
#include "scene.h"
#include "sceneclasslist.h"
#include "light.h"
#include "propertieslight.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma resource "*.dfm"
TfraSelectLight *fraSelectLight;
//---------------------------------------------------------------------------
__fastcall TfraSelectLight::TfraSelectLight(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools->AddFrame(etLight,eaSelect,this);
}
//---------------------------------------------------------------------------
void __fastcall TfraSelectLight::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfraSelectLight::sbInvertClick(TObject *Sender)
{
	Scene.InvertSelection( OBJCLASS_LIGHT );
}
//---------------------------------------------------------------------------
void __fastcall TfraSelectLight::sbSelectAllClick(TObject *Sender)
{
	Scene.SelectObjects( true, OBJCLASS_LIGHT );
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectLight::sbDeselectAllClick(TObject *Sender)
{
	Scene.SelectObjects( false, OBJCLASS_LIGHT );
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectLight::sbRemoveClick(TObject *Sender)
{
    Scene.UndoPrepare();
    Scene.RemoveSelection( OBJCLASS_LIGHT );
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectLight::RunEditor()
{
    Scene.UndoPrepare();
	ObjectList objset;
	ObjectIt _F = Scene.FirstObj();
    for(;_F!=Scene.LastObj();_F++)
		if( (*_F)->ClassID() == OBJCLASS_LIGHT && (*_F)->Selected() && (*_F)->Visible())
			objset.push_back( (*_F) );
	if( !objset.empty() ) frmPropertiesLight->Run(&objset);
}
//---------------------------------------------------------------------------


void __fastcall TfraSelectLight::Label4Click(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectLight::sbPropertiesClick(TObject *Sender)
{
    RunEditor();
}
//---------------------------------------------------------------------------

