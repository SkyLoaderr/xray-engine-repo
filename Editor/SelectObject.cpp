#include "pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "SelectObject.h"
#include "scene.h"
#include "sceneobject.h"
#include "sceneclasslist.h"
#include "sobject2.h"
#include "propertiesobject.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma resource "*.dfm"
TfraSelectObject *fraSelectObject;
//---------------------------------------------------------------------------
__fastcall TfraSelectObject::TfraSelectObject(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools->AddFrame(etObject,eaSelect,this);
}
//---------------------------------------------------------------------------
void __fastcall TfraSelectObject::PaneMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectObject::sbInvertClick(TObject *Sender)
{
	Scene.InvertSelection( OBJCLASS_SOBJECT2 );
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectObject::sbSelectAllClick(TObject *Sender)
{
	Scene.SelectObjects( true, OBJCLASS_SOBJECT2 );
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectObject::sbDeselectAllClick(TObject *Sender)
{
	Scene.SelectObjects( false, OBJCLASS_SOBJECT2 );
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectObject::sbSelectByClassClick(TObject *Sender)
{
	SelByClassObject( true );
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectObject::sbDeselectByClassClick(TObject *Sender)
{
	SelByClassObject( false );
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectObject::sbSelectByRefsClick(TObject *Sender)
{
	SelByRefObject( true );
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectObject::sbDeselectByRefsClick(TObject *Sender)
{
	SelByRefObject( false );
}

void TfraSelectObject::SelByRefObject( bool flag ){
	SceneObject *o = UI.SelectSObject();
	if( o ){
		_ASSERTE( o->ClassID() == OBJCLASS_SOBJECT2 );
		ObjectIt _F = Scene.FirstObj();
		for(;_F!=Scene.LastObj();_F++){
			if( (*_F)->ClassID() == OBJCLASS_SOBJECT2 ){
				SObject2 *_O = (SObject2 *)(*_F);
				if( _O->RefCompare( (SObject2*)o ) ){
					_O->Select( flag );
				}
			}
		}
	}
}

void TfraSelectObject::SelByClassObject( bool flag ){
	SceneObject *o = UI.SelectSObject();
	if( o ){
		_ASSERTE( o->ClassID() == OBJCLASS_SOBJECT2 );
		ObjectIt _F = Scene.FirstObj();
		for(;_F!=Scene.LastObj();_F++){
			if( (*_F)->ClassID() == OBJCLASS_SOBJECT2 ){
				SObject2 *_O = (SObject2 *)(*_F);
				if( _O->ClassCompare( (SObject2*)o ) ){
					_O->Select( flag );
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TfraSelectObject::sbResolveClick(TObject *Sender)
{
    Scene.UndoPrepare();
    ResolveObjects();
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectObject::sbPropertiesClick(TObject *Sender)
{
    RunEditor();
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectObject::sbRemoveClick(TObject *Sender)
{
    Scene.UndoPrepare();
    Scene.RemoveSelection( OBJCLASS_SOBJECT2 );
}
//---------------------------------------------------------------------------
void TfraSelectObject::RunEditor(){
    Scene.UndoPrepare();
	ObjectList objset;
	ObjectIt _F = Scene.FirstObj();
	for(;_F!=Scene.LastObj();_F++)
		if( (*_F)->ClassID() == OBJCLASS_SOBJECT2 && (*_F)->Selected() && (*_F)->Visible())
			objset.push_back( (*_F) );
	if( !objset.empty() ) frmPropertiesObject->ChangeProperties(&objset);
}

void TfraSelectObject::ResolveObjects(){
	ObjectIt _F = Scene.FirstObj();
	for(;_F!=Scene.LastObj();_F++)
		if( (*_F)->ClassID() == OBJCLASS_SOBJECT2 && (*_F)->Selected() ){
			SObject2 *sobj = (SObject2 *)(*_F);
			sobj->ResolveReference(); }
}

//----------------------------------------------------

void __fastcall TfraSelectObject::Label2Click(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

