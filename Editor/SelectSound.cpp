#include "pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "SelectSound.h"
#include "UI_Main.h"
#include "scene.h"
#include "sceneClassList.h"
#include "PropertiesSound.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma resource "*.dfm"
TfraSelectSound *fraSelectSound;
//---------------------------------------------------------------------------
__fastcall TfraSelectSound::TfraSelectSound(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools->AddFrame(etSound,eaSelect,this);
}
//---------------------------------------------------------------------------
void __fastcall TfraSelectSound::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectSound::Label2Click(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectSound::sbPropertiesClick(TObject *Sender)
{
    RunEditor();
}
//---------------------------------------------------------------------------
void __fastcall TfraSelectSound::RunEditor()
{
    Scene.UndoPrepare();
	ObjectList objset;
	ObjectIt _F = Scene.FirstObj();
    for(;_F!=Scene.LastObj();_F++)
		if( (*_F)->ClassID() == OBJCLASS_SOUND && (*_F)->Selected() && (*_F)->Visible())
			objset.push_back( (*_F) );
	if( !objset.empty() ) frmPropertiesSound->Run(&objset);
}

void __fastcall TfraSelectSound::sbInvertClick(TObject *Sender)
{
	Scene.InvertSelection( OBJCLASS_SOUND );
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectSound::sbSelectAllClick(TObject *Sender)
{
	Scene.SelectObjects( true, OBJCLASS_SOUND );
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectSound::sbDeselectAllClick(TObject *Sender)
{
	Scene.SelectObjects( false, OBJCLASS_SOUND );
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectSound::sbRemoveClick(TObject *Sender)
{
    Scene.UndoPrepare();
    Scene.RemoveSelection( OBJCLASS_SOUND );
}
//---------------------------------------------------------------------------

