#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FramePS.h"
#include "PSLibrary.h"
#include "Scene.h"
#include "EParticlesObject.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraPS::TfraPS(TComponent* Owner)
        : TFrame(Owner)
{
    OutCurrentName();
}
//---------------------------------------------------------------------------
void __fastcall TfraPS::PaneMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraPS::ExpandClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Selecting
//---------------------------------------------------------------------------
void __fastcall TfraPS::ebSelectByRefsClick(TObject *Sender)
{
	SelByRef( true );
}
//---------------------------------------------------------------------------

void __fastcall TfraPS::ebDeselectByRefsClick(TObject *Sender)
{
	SelByRef( false );
}

void __fastcall TfraPS::SelByRef( bool flag )
{
	LPCSTR PG = ::Render->PSystems.ChoosePG();
	if(PG){
		ObjectIt _F = Scene.FirstObj(OBJCLASS_PS);
        ObjectIt _E = Scene.LastObj(OBJCLASS_PS);
		for(;_F!=_E;_F++){
			if( (*_F)->Visible() ){
				EParticlesObject *_O = (EParticlesObject *)(*_F);
				if(_O->RefCompare(PG)) _O->Select( flag );
			}
		}
	}
}
//---------------------------------------------------------------------------

//----------------------------------------------------
void __fastcall TfraPS::OutCurrentName()
{
	LPCSTR PG = ::Render->PSystems.GetCurrentPG(false);
	ebCurObj->Caption = (PG)?PG:NONE_CAPTION;
}

//---------------------------------------------------------------------------
// Add new
//---------------------------------------------------------------------------
void __fastcall TfraPS::ebCurObjClick(TObject *Sender)
{
	::Render->PSystems.ChoosePG();
    OutCurrentName();
}
//---------------------------------------------------------------------------

void __fastcall TfraPS::ebCurrentPSPlayClick(TObject *Sender)
{
    ObjectIt _F = Scene.FirstObj(OBJCLASS_PS);
    ObjectIt _E = Scene.LastObj(OBJCLASS_PS);
    for(;_F!=_E;_F++){
        if( (*_F)->Visible() && (*_F)->Selected())
            ((EParticlesObject *)(*_F))->Play();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraPS::ebCurrentPSStopClick(TObject *Sender)
{
    ObjectIt _F = Scene.FirstObj(OBJCLASS_PS);
    ObjectIt _E = Scene.LastObj(OBJCLASS_PS);
    for(;_F!=_E;_F++){
        if( (*_F)->Visible() && (*_F)->Selected())
            ((EParticlesObject *)(*_F))->Stop();
    }
}
//---------------------------------------------------------------------------

