#include "stdafx.h"
#pragma hdrstop

#include "Splash.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "MXCtrls"
#pragma resource "*.dfm"
TSplashForm* TSplashForm::form=0;
//---------------------------------------------------------------------------
__fastcall TSplashForm::TSplashForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TSplashForm::CreateSplash(u32 inst, LPCSTR res_splash_name)
{
	form 					= xr_new<TSplashForm>((TComponent*)(NULL));
    form->Image1->Picture->Bitmap->LoadFromResourceName(inst,res_splash_name);
}
//---------------------------------------------------------------------------

void TSplashForm::DestroySplash()
{
	xr_delete				(form);
}
//---------------------------------------------------------------------------

