#include "stdafx.h"
#pragma hdrstop

#include "PropertiesPSDef.h"
#include "SceneClassList.h"
#include "SceneObject.h"
#include "EditParticles.h"
#include "ParticleSystem.h"
#include "FileSystem.h"
#include "ChoseForm.h"
#include "ShaderFunction.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "multi_edit"
#pragma link "RXCtrls"
#pragma link "Placemnt"
#pragma link "multi_color"
#pragma link "Gradient"
#pragma link "multi_check"
#pragma link "FrameEmitter"
#pragma resource "*.dfm"

TfrmPropertiesPSDef* TfrmPropertiesPSDef::form = 0;
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesPSDef::TfrmPropertiesPSDef(TComponent* Owner)
    : TForm(Owner)
{
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
    bSetMode = false;
}
//---------------------------------------------------------------------------


void TfrmPropertiesPSDef::GetObjectsInfo(){
	bSetMode							= true;
	if (m_PS){
    	// base
        edName->Text					= m_PS->m_Name;
        lbCreator->Caption				= m_PS->m_Computer;
        lbShader->Caption				= m_PS->m_ShaderName[0]?m_PS->m_ShaderName:"...";
        lbTexture->Caption				= m_PS->m_TextureName[0]?m_PS->m_TextureName:"...";
        // params
		seLife->ObjFirstInit			(m_PS->m_fLife);
		seSpeedStart->ObjFirstInit		(m_PS->m_Speed.start);
		seSpeedEnd->ObjFirstInit		(m_PS->m_Speed.end);
        seSizeStart->ObjFirstInit		(m_PS->m_Size.start);
        seSizeEnd->ObjFirstInit			(m_PS->m_Size.end);
        seASpeedStart->ObjFirstInit		(m_PS->m_ASpeed.start);
        seASpeedEnd->ObjFirstInit		(m_PS->m_ASpeed.end);
        seGravityStartX->ObjFirstInit	(m_PS->m_Gravity.start.x);
        seGravityStartY->ObjFirstInit	(m_PS->m_Gravity.start.y);
        seGravityStartZ->ObjFirstInit	(m_PS->m_Gravity.start.z);
        seGravityEndX->ObjFirstInit		(m_PS->m_Gravity.end.x);
        seGravityEndY->ObjFirstInit		(m_PS->m_Gravity.end.y);
        seGravityEndZ->ObjFirstInit		(m_PS->m_Gravity.end.z);
        seColorStartR->ObjFirstInit		(m_PS->m_Color.start.r);
        seColorStartG->ObjFirstInit		(m_PS->m_Color.start.g);
        seColorStartB->ObjFirstInit		(m_PS->m_Color.start.b);
        seColorStartA->ObjFirstInit		(m_PS->m_Color.start.a);
        seColorEndR->ObjFirstInit		(m_PS->m_Color.end.r);
        seColorEndG->ObjFirstInit		(m_PS->m_Color.end.g);
        seColorEndB->ObjFirstInit		(m_PS->m_Color.end.b);
        seColorEndA->ObjFirstInit		(m_PS->m_Color.end.a);

        Fcolor C; 
        C.set			   				(m_PS->m_Color.start.r/255,m_PS->m_Color.start.g/255,m_PS->m_Color.start.b/255,0);
		grColor->BeginColor				= TColor(C.get_windows());
        C.set							(m_PS->m_Color.end.r/255,m_PS->m_Color.end.g/255,m_PS->m_Color.end.b/255,0);
		grColor->EndColor				= TColor(C.get_windows());

        // variable
		seLifeVar->ObjFirstInit			(m_PS->m_fLifeVar);
		seSpeedVar->ObjFirstInit		(m_PS->m_fSpeedVar);
        seSizeVar->ObjFirstInit			(m_PS->m_fSizeVar);
		seASpeedVar->ObjFirstInit		(m_PS->m_fASpeedVar);
        seColorVarR->ObjFirstInit		(m_PS->m_ColorVar.r);
        seColorVarG->ObjFirstInit		(m_PS->m_ColorVar.g);
        seColorVarB->ObjFirstInit		(m_PS->m_ColorVar.b);
        seColorVarA->ObjFirstInit		(m_PS->m_ColorVar.a);

        // draw
        seBlurTimeStart->ObjFirstInit	(m_PS->m_BlurTime.start);
        seBlurTimeEnd->ObjFirstInit		(m_PS->m_BlurTime.end);
        seBlurSubdivStart->ObjFirstInit	(m_PS->m_BlurSubdiv.start);
        seBlurSubdivEnd->ObjFirstInit  	(m_PS->m_BlurSubdiv.end);

        // emitter
        fraEmitter->GetInfoFirst		(m_PS->m_DefaultEmitter);

        // animation
        cbAnimEnabled->Checked			= m_PS->m_dwFlag&PS_FRAME_ENABLED;
        cbAnimAnimate->Checked			= m_PS->m_dwFlag&PS_FRAME_ANIMATE;
        cbAnimRandomInitialFrame->Checked=m_PS->m_dwFlag&PS_FRAME_RND_INIT;
        cbAnimRandomPlaybackDir->Checked= m_PS->m_dwFlag&PS_FRAME_RND_PLAYBACK_DIR;
        seAnimFrameCount->ObjFirstInit	(m_PS->m_Animation.m_iFrameCount);
        seAnimSpeed->ObjFirstInit		(m_PS->m_Animation.m_fSpeed);
        seAnimSpeedVar->ObjFirstInit	(m_PS->m_Animation.m_fSpeedVar);

        seAnimFrameWidth->ObjFirstInit	(m_PS->m_Animation.m_FrameSize.x);
        seAnimFrameHeight->ObjFirstInit	(m_PS->m_Animation.m_FrameSize.y);

        seAnimTexWidth->ObjFirstInit	((1.f/m_PS->m_Animation.m_TexSize.x)*m_PS->m_Animation.m_FrameSize.x);
        seAnimTexHeight->ObjFirstInit	((1.f/m_PS->m_Animation.m_TexSize.y)*m_PS->m_Animation.m_FrameSize.y);

        // flags
        cbMotionBlur->Checked			= m_PS->m_dwFlag&PS_MOTIONBLUR;
        cbAlignToPath->Checked			= m_PS->m_dwFlag&PS_ALIGNTOPATH;
        cbRandomInitAngle->Checked		= m_PS->m_dwFlag&PS_RND_INIT_ANGLE;

        pcPS->Enabled					= true;
//        gbBase->Enabled					= true;
//        gbParams->Enabled	  			= true;
//        gbVariability->Enabled			= true;
    }else{
        pcPS->Enabled					= false;
//        gbBase->Enabled					= false;
//        gbParams->Enabled	  			= false;
//        gbVariability->Enabled			= false;
    }
	bSetMode							= false;
}

bool TfrmPropertiesPSDef::ApplyObjectsInfo(){
	if (bSetMode) return false;
	if (m_PS){
    	// base
        {
        	AnsiString name 			= m_PS->m_Name;
            PS::SDef* F 				= TfrmEditParticles::FindPS(edName->Text.c_str());
            if (F&&(m_PS!=F)){
            	Log->DlgMsg				(mtError,"Specify name already present in library. Change name and try again.");
            }else
		        if (m_PS->SetName		(edName->Text.c_str()))
				    TfrmEditParticles::OnNameUpdate();
        }
        m_PS->SetShader					(lbShader->Caption=="..."?"":lbShader->Caption.c_str());
        m_PS->SetTexture				(lbTexture->Caption=="..."?"":lbTexture->Caption.c_str());
        // params
		seLife->ObjApplyFloat 			(m_PS->m_fLife);
		seSpeedStart->ObjApplyFloat		(m_PS->m_Speed.start);
		seSpeedEnd->ObjApplyFloat		(m_PS->m_Speed.end);
        seSizeStart->ObjApplyFloat		(m_PS->m_Size.start);
        seSizeEnd->ObjApplyFloat		(m_PS->m_Size.end);
        seASpeedStart->ObjApplyFloat	(m_PS->m_ASpeed.start);
        seASpeedEnd->ObjApplyFloat		(m_PS->m_ASpeed.end);
        seGravityStartX->ObjApplyFloat	(m_PS->m_Gravity.start.x);
        seGravityStartY->ObjApplyFloat	(m_PS->m_Gravity.start.y);
        seGravityStartZ->ObjApplyFloat	(m_PS->m_Gravity.start.z);
        seGravityEndX->ObjApplyFloat	(m_PS->m_Gravity.end.x);
        seGravityEndY->ObjApplyFloat	(m_PS->m_Gravity.end.y);
        seGravityEndZ->ObjApplyFloat	(m_PS->m_Gravity.end.z);
        m_PS->m_Color.start.set			(seColorStartR->Value,seColorStartG->Value,seColorStartB->Value,seColorStartA->Value);
        m_PS->m_Color.end.set			(seColorEndR->Value,seColorEndG->Value,seColorEndB->Value,seColorEndA->Value);
        // variable
		seLifeVar->ObjApplyFloat		(m_PS->m_fLifeVar);
		seSpeedVar->ObjApplyFloat		(m_PS->m_fSpeedVar);
        seSizeVar->ObjApplyFloat		(m_PS->m_fSizeVar);
		seASpeedVar->ObjApplyFloat		(m_PS->m_fASpeedVar);
        seColorVarR->ObjApplyFloat		(m_PS->m_ColorVar.r);
        seColorVarG->ObjApplyFloat		(m_PS->m_ColorVar.g);
        seColorVarB->ObjApplyFloat		(m_PS->m_ColorVar.b);
        seColorVarA->ObjApplyFloat		(m_PS->m_ColorVar.a);
        // draw
        seBlurTimeStart->ObjApplyFloat	(m_PS->m_BlurTime.start);
        seBlurTimeEnd->ObjApplyFloat	(m_PS->m_BlurTime.end);
        m_PS->m_BlurSubdiv.start		= seBlurSubdivStart->Value;
        m_PS->m_BlurSubdiv.end			= seBlurSubdivEnd->Value;

        // flags
        m_PS->m_dwFlag					= 0|(cbMotionBlur->Checked?PS_MOTIONBLUR:0);
        m_PS->m_dwFlag					|= (cbAlignToPath->Checked?PS_ALIGNTOPATH:0);
		m_PS->m_dwFlag					|= (cbRandomInitAngle->Checked?PS_RND_INIT_ANGLE:0);

        // emitter
        fraEmitter->SetInfo				(m_PS->m_DefaultEmitter);

        // animation
        m_PS->m_Animation.SetTextureSize(seAnimTexWidth->Value, seAnimTexHeight->Value,
        								 seAnimFrameWidth->Value, seAnimFrameHeight->Value);
        seAnimFrameCount->ObjApplyInt	(m_PS->m_Animation.m_iFrameCount);
        seAnimSpeed->ObjApplyFloat		(m_PS->m_Animation.m_fSpeed);
        seAnimSpeedVar->ObjApplyFloat	(m_PS->m_Animation.m_fSpeedVar);
        m_PS->m_dwFlag					|= (cbAnimEnabled->Checked?PS_FRAME_ENABLED:0);
        m_PS->m_dwFlag					|= (cbAnimAnimate->Checked?PS_FRAME_ANIMATE:0);
        m_PS->m_dwFlag					|= (cbAnimRandomInitialFrame->Checked?PS_FRAME_RND_INIT:0);
        m_PS->m_dwFlag					|= (cbAnimRandomPlaybackDir->Checked?PS_FRAME_RND_PLAYBACK_DIR:0);

		TfrmEditParticles::Modified();

        GetObjectsInfo();
    }
    return true;
}

//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesPSDef::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) Close();
}

//----------------------------------------------------
void __fastcall TfrmPropertiesPSDef::FormShow(TObject *Sender)
{
	m_PS 				= 0;
    tmUpdate->Enabled	= true;
    bSetMode			= false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPSDef::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    tmUpdate->Enabled	= false;
	Action 				= caFree;
    form 				= 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPSDef::ShowProperties(){
    if (!form) form = new TfrmPropertiesPSDef(0);
    form->Show();
	form->m_PS = TfrmEditParticles::GetSelectedPS();
	form->GetObjectsInfo();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPSDef::HideProperties(){
	if (form) form->Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPSDef::tmUpdateTimer(TObject *Sender)
{
	if (form->m_PS!=TfrmEditParticles::GetSelectedPS()){
    	form->m_PS=TfrmEditParticles::GetSelectedPS();
		form->GetObjectsInfo();
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmPropertiesPSDef::ebSelectShaderClick(TObject *Sender)
{
	if (!m_PS) return;
	LPCSTR S = TfrmChoseItem::SelectShader(true,0,m_PS->m_ShaderName[0]?m_PS->m_ShaderName:0);
    if (S){lbShader->Caption=S; ApplyObjectsInfo(); TfrmEditParticles::UpdateCurrent();}
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPSDef::ebSelectTextureClick(TObject *Sender)
{
	if (!m_PS) return;
	LPCSTR S = TfrmChoseItem::SelectTexture(false,m_PS->m_TextureName[0]?m_PS->m_TextureName:0);
    if (S){lbTexture->Caption=S; ApplyObjectsInfo(); TfrmEditParticles::UpdateCurrent();}
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPSDef::clColorMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	bSetMode = true;
	if (X<grColor->Width/2){
		DWORD color = grColor->BeginColor;
		if (SelectColorWin(&color,&color)){
            grColor->BeginColor = color;
            Fcolor C; C.set_windows(grColor->BeginColor);
            seColorStartR->Value = C.r*255;
            seColorStartG->Value = C.g*255;
            seColorStartB->Value = C.b*255;
        }
    }else{
		DWORD color = grColor->EndColor;
		if (SelectColorWin(&color,&color)){
            grColor->EndColor = color;
            Fcolor C; C.set_windows(grColor->EndColor);
            seColorEndR->Value = C.r*255;
            seColorEndG->Value = C.g*255;
            seColorEndB->Value = C.b*255;
        }
    }
	bSetMode = false;
	ApplyObjectsInfo();
}
//---------------------------------------------------------------------------


void __fastcall TfrmPropertiesPSDef::seLWChange(TObject *Sender,
      int Val)
{
	ApplyObjectsInfo();
}
//---------------------------------------------------------------------------


void __fastcall TfrmPropertiesPSDef::seExit(TObject *Sender)
{
	ApplyObjectsInfo();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPSDef::seKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	if (Key==VK_RETURN) ApplyObjectsInfo();
}
//---------------------------------------------------------------------------


void __fastcall TfrmPropertiesPSDef::ExtBtn2Click(TObject *Sender){
	seSizeEnd->Value = seSizeStart->Value;
	ApplyObjectsInfo();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesPSDef::ExtBtn3Click(TObject *Sender){
	seASpeedEnd->Value = seASpeedStart->Value;
	ApplyObjectsInfo();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesPSDef::ExtBtn4Click(TObject *Sender){
	seGravityEndX->Value = seGravityStartX->Value;
	ApplyObjectsInfo();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesPSDef::ExtBtn5Click(TObject *Sender){
	seGravityEndY->Value = seGravityStartY->Value;
	ApplyObjectsInfo();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesPSDef::ExtBtn6Click(TObject *Sender){
	seGravityEndZ->Value = seGravityStartZ->Value;
	ApplyObjectsInfo();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesPSDef::ExtBtn7Click(TObject *Sender){
	seColorEndR->Value = seColorStartR->Value;
	ApplyObjectsInfo();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesPSDef::ExtBtn8Click(TObject *Sender){
	seColorEndG->Value = seColorStartG->Value;
	ApplyObjectsInfo();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesPSDef::ExtBtn9Click(TObject *Sender){
	seColorEndB->Value = seColorStartB->Value;
	ApplyObjectsInfo();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesPSDef::ExtBtn10Click(TObject *Sender){
	seColorEndA->Value = seColorStartA->Value;
	ApplyObjectsInfo();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesPSDef::ExtBtn1Click(TObject *Sender){
	seSpeedEnd->Value = seSpeedStart->Value;
	ApplyObjectsInfo();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPSDef::seEmitterExit(TObject *Sender)
{
	ApplyObjectsInfo();
    TfrmEditParticles::UpdateEmitter();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPSDef::seEmitterKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	if (Key==VK_RETURN){ 
    	ApplyObjectsInfo();
		TfrmEditParticles::UpdateEmitter();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPSDef::seEmitterLWChange(TObject *Sender,
      int Val)
{
    ApplyObjectsInfo();
    TfrmEditParticles::UpdateEmitter();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesPSDef::ebBirthFuncMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (!m_PS) return;
    if (bSetMode) return;
	if (Shift.Contains(ssShift)) fraEmitter->ebBirthFunc->Down = true;
    else{
		frmShaderFunctionRun(&m_PS->m_DefaultEmitter.m_BirthFunc);
    	fraEmitter->ebBirthFunc->Down = false;
    }
    ApplyObjectsInfo();
    TfrmEditParticles::UpdateEmitter();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPSDef::ebBirthFuncClick(TObject *Sender)
{
    ApplyObjectsInfo();
    TfrmEditParticles::UpdateEmitter();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPSDef::ExtBtn12Click(TObject *Sender){
	seBlurSubdivEnd->Value = seBlurSubdivStart->Value;
	ApplyObjectsInfo();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPSDef::ExtBtn11Click(TObject *Sender){
	seBlurTimeEnd->Value = seBlurTimeStart->Value;
	ApplyObjectsInfo();
}
//---------------------------------------------------------------------------






