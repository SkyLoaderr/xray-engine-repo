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
	LPCSTR PE = ::Render->PSystems.ChoosePED();
	if(PE){
		ObjectIt _F = Scene.FirstObj(OBJCLASS_PS);
        ObjectIt _E = Scene.LastObj(OBJCLASS_PS);
		for(;_F!=_E;_F++){
			if( (*_F)->Visible() ){
				EParticlesObject *_O = (EParticlesObject *)(*_F);
				if(_O->RefCompare(PE)) _O->Select( flag );
			}
		}
	}
}
//---------------------------------------------------------------------------

//----------------------------------------------------
void __fastcall TfraPS::OutCurrentName()
{
	LPCSTR PE = ::Render->PSystems.GetCurrentPED(false);
	ebCurObj->Caption = (PE)?PE:NONE_CAPTION;
}

//---------------------------------------------------------------------------
// Add new
//---------------------------------------------------------------------------
void __fastcall TfraPS::ebCurObjClick(TObject *Sender)
{
	::Render->PSystems.ChoosePED();
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

