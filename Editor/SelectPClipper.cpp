#include "pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "SelectPClipper.h"
#include "ui_main.h"
#include "scene.h"
#include "sceneclasslist.h"
#include "PropertiesPClipper.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma resource "*.dfm"
TfraSelectPClipper *fraSelectPClipper;
//---------------------------------------------------------------------------
__fastcall TfraSelectPClipper::TfraSelectPClipper(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools->AddFrame(etPClipper,eaSelect,this);
}
//---------------------------------------------------------------------------
void __fastcall TfraSelectPClipper::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectPClipper::Label4Click(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectPClipper::sbInvertClick(TObject *Sender)
{
	Scene.InvertSelection( OBJCLASS_PCLIPPER );
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectPClipper::sbSelectAllClick(TObject *Sender)
{
	Scene.SelectObjects( true, OBJCLASS_PCLIPPER );
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectPClipper::sbDeselectAllClick(TObject *Sender)
{
	Scene.SelectObjects( false, OBJCLASS_PCLIPPER );
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectPClipper::sbPropertiesClick(TObject *Sender)
{
    RunEditor();
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectPClipper::sbRemoveClick(TObject *Sender)
{
    Scene.UndoPrepare();
    Scene.RemoveSelection( OBJCLASS_PCLIPPER );
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectPClipper::RunEditor()
{
    Scene.UndoPrepare();
	ObjectList objset;
	ObjectIt _F = Scene.FirstObj();
    for(;_F!=Scene.LastObj();_F++)
		if( (*_F)->ClassID() == OBJCLASS_PCLIPPER && (*_F)->Selected() && (*_F)->Visible())
			objset.push_back( (*_F) );
	if( !objset.empty() ) frmPropertiesPClipper->Run(&objset);
}
//---------------------------------------------------------------------------

