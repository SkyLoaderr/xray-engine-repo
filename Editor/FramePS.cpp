#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FramePS.h"
#include "PSLibrary.h"
#include "Scene.h"
#include "PSObject.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma link "RXCtrls"
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

void __fastcall TfraPS::SelByRef( bool flag ){
	PS::SDef *PS = (PS::SDef*)PSLib->ChoosePS(false);
	if(PS){
		ObjectIt _F = Scene->FirstObj(OBJCLASS_PS);
        ObjectIt _E = Scene->LastObj(OBJCLASS_PS);
		for(;_F!=_E;_F++){
			if( (*_F)->Visible() ){
				CPSObject *_O = (CPSObject *)(*_F);
				if(_O->RefCompare(PS)) _O->Select( flag );
			}
		}
	}
}
//---------------------------------------------------------------------------

//----------------------------------------------------
void __fastcall TfraPS::OutCurrentName(){
	PS::SDef* PS = PSLib->GetCurrentPS();
    if (PS) lbSelectObjectName->Caption = PS->m_Name;
    else	lbSelectObjectName->Caption = "...";
}

//---------------------------------------------------------------------------
// Add new
//---------------------------------------------------------------------------
void __fastcall TfraPS::ebSelectClick(TObject *Sender)
{
	PSLib->ChoosePS();
    OutCurrentName();
}
//---------------------------------------------------------------------------



