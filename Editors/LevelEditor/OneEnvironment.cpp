#include "stdafx.h"
#pragma hdrstop

#include "OneEnvironment.h"
#include "scene.h"
#include "ColorPicker.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmOneEnvironment *frmOneEnvironment;
//---------------------------------------------------------------------------
__fastcall TfrmOneEnvironment::TfrmOneEnvironment(TComponent* Owner)
    : TForm(Owner)
{
	m_CurEnv = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneEnvironment::ShowEnv(st_Environment* env){
	VERIFY(env);
	m_CurEnv = env;

    bLoadMode = true;

    seViewDistance->Value	= env->m_ViewDist;
    seFogness->Value		= env->m_Fogness;
    mcFogColor->ObjFirstInit	( env->m_FogColor.get_windows() );
    mcAmbColor->ObjFirstInit	( env->m_AmbColor.get_windows() );
	// sky
    seSkyFogness->Value		= m_CurEnv->m_SkyColor.a;
    Fcolor C;    			C.set(m_CurEnv->m_SkyColor); C.a=0;
    mcSkyColor->ObjFirstInit	( C.get_windows() );

    bLoadMode = false;
    Visible=true;
}
//---------------------------------------------------------------------------
void TfrmOneEnvironment::UpdateEnvData(){
	if (bLoadMode) return;
    if (!m_CurEnv) return;
    seViewDistance->ObjApplyFloat	(m_CurEnv->m_ViewDist);
    seFogness->ObjApplyFloat  		(m_CurEnv->m_Fogness);
	int c;
    if (mcFogColor->ObjApply(c)) m_CurEnv->m_FogColor.set_windows(c);
    if (mcAmbColor->ObjApply(c)) m_CurEnv->m_AmbColor.set_windows(c);
    if (mcSkyColor->ObjApply(c)) m_CurEnv->m_SkyColor.set_windows(c);
    m_CurEnv->m_SkyColor.a	= seSkyFogness->Value;
}
//---------------------------------------------------------------------------
void __fastcall TfrmOneEnvironment::HideEnv(){
    Visible=false;
}

void __fastcall TfrmOneEnvironment::UpdateClick(TObject *Sender)
{
	UpdateEnvData();
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneEnvironment::mcAmbColorMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	DWORD color = ((TMultiObjColor*)Sender)->Brush->Color;
	if (SelectColorWin(&color)) ((TMultiObjColor*)Sender)->_Set(color);
    UpdateClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneEnvironment::seFognessLWChange(TObject *Sender,
      int Val)
{
	UpdateEnvData();
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneEnvironment::seFognessKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	if (Key==VK_RETURN) UpdateEnvData();
}
//---------------------------------------------------------------------------


