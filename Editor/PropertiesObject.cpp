#include "stdafx.h"
#pragma hdrstop

#include "PropertiesObject.h"
#include "SceneClassList.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "Texture.h"
#include "xrShader.h"
#include "Shader.h"
#include "ui_main.h"
#include "scene.h"
#include "XRShaderDef.h"
#include "main.h"
#include "motion.h"
#include "EditLibrary.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma link "RXCtrls"
#pragma link "RXSpin"
#pragma link "CloseBtn"
#pragma link "ElHeader"
#pragma link "ElTree"
#pragma link "RxMenus"
#pragma link "multi_check"
#pragma link "multi_edit"
#pragma link "Placemnt"
#pragma resource "*.dfm"

TfrmPropertiesObject* 	TfrmPropertiesObject::form 				= 0;
CEditableObject* 		TfrmPropertiesObject::m_CurrentObject 	= 0;

void TfrmPropertiesObject::SetCurrent(CEditableObject* object){
    VERIFY(object);
    m_CurrentObject = object;
    if (form) form->GetObjectsInfo();
}

void __fastcall TfrmPropertiesObject::ShowProperties(){
    if (!form) form = new TfrmPropertiesObject(0);
    form->Show();
	form->GetObjectsInfo();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::HideProperties(){
	if (form) form->Close();
}

//---------------------------------------------------------------------------
__fastcall TfrmPropertiesObject::TfrmPropertiesObject(TComponent* Owner)
    : TForm(Owner)
{
    sgTransform->Cells[0][0]="Position"; sgTransform->Cells[0][1]="Rotation"; sgTransform->Cells[0][2]="Scale";
    sgBB->Cells[0][0]="X"; sgBB->Cells[0][1]="Y"; sgBB->Cells[0][2]="Z";
    tx_selected = 0;
    surf_selected = 0;
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::FormShow(TObject *Sender)
{
    pcObjects->ActivePage = tsMainOptions;
    ebOk->Enabled       = false;
    ebApply->Enabled	= false;
    tvMeshes->Enabled   = true;
    edName->Hint 		= "Warning!!! Be careful before edit item library name.";
    tsMainOptionsShow   (Sender);
    tsMeshesShow        (Sender);
    ebDropper->Enabled	= false;
    ebDropper->Down		= false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	Action = caFree;
	tvMeshes->Items->Clear();
    tvSurfaces->Items->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::OnModified(TObject *Sender)
{
    ebOk->Enabled = true;
    ebApply->Enabled = true;
    UI->RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebOkClick(TObject *Sender)
{
    ApplyObjectsInfo();
    Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebApplyClick(TObject *Sender)
{
    ApplyObjectsInfo();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
    // save shader names for cancel...
    int k=0;
    for (SurfaceIt s_it=m_CurrentObject->FirstSurface(); s_it!=m_CurrentObject->LastSurface(); s_it++){
        bool bCreateShader=false;
        if ((*s_it)->shader){
            if(strcmp(SH_Names[k].c_str(),(*s_it)->shader->shader->cName)!=0){
                Device.Shader.Delete((*s_it)->shader);
                bCreateShader=true;
            }
        }else{
            (*s_it)->shader = Device.Shader.Create(SHLib->FindShader(SH_Names[k])->cName,(*s_it)->textures);
            bCreateShader = true;
        }
        if (bCreateShader){
            SH_ShaderDef* sh_base=SHLib->FindShader(SH_Names[k]);
            VERIFY2(sh_base,"Can't find shader.");
            (*s_it)->shader = Device.Shader.Create(sh_base->cName,(*s_it)->textures);
            VERIFY((*s_it)->shader);
            if (sh_base->Passes_Count>0)
                (*s_it)->has_alpha = sh_base->Passes[0].Flags.bABlend;
        }
        k++;
    }
}
//--------------------------------------------------------------------------------------------------

void TfrmPropertiesObject::GetObjectsInfo(){
	if (m_CurrentObject){
    	Enabled = true;
        SH_Names.clear();
        // save shader names for cancel...
        for (SurfaceIt s_it=m_CurrentObject->FirstSurface(); s_it!=m_CurrentObject->LastSurface(); s_it++)
            SH_Names.push_back((*s_it)->shader->shader->cName);

        mmScript->Text = m_CurrentObject->GetClassScript();
        edName->Text   = m_CurrentObject->GetName();
        cbMakeDynamic->ObjFirstInit( TCheckBoxState(m_CurrentObject->IsDynamic()) );
    }else{
    	Enabled = false;
    }
}

void TfrmPropertiesObject::ApplyObjectsInfo(){
	if (m_CurrentObject){
        if (!edName->Text.Length()){
            ELog.DlgMsg(mtError,"Enter Object Name!");
            return;
        }
        if (!tvMeshes->Items->Count){
            ELog.DlgMsg(mtError,"Add mesh!");
            return;
        }
        cbMakeDynamic->ObjApply( m_CurrentObject->m_DynamicObject );
        m_CurrentObject->GetClassScript() = mmScript->Text;
        m_CurrentObject->SetName(edName->Text.c_str());

		m_CurrentObject->m_LibParent->Modified();
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
}

//----------------------------------------------------

void __fastcall TfrmPropertiesObject::cbMakeDynamicClick(TObject *Sender)
{
    tsScript->TabVisible 		= cbMakeDynamic->Checked;
    tsOAnimation->TabVisible 	= cbMakeDynamic->Checked;
    tsSAnimation->TabVisible 	= cbMakeDynamic->Checked&&m_CurrentObject->IsSkeleton();
    OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tsMainOptionsShow(TObject *Sender)
{
    edName->Enabled 		= true;
    mmScript->Enabled 		= true;
    gbTemplates->Enabled 	= true;
    tsScript->TabVisible 	= cbMakeDynamic->Checked;
    tsOAnimation->TabVisible= cbMakeDynamic->Checked;
    tsSAnimation->TabVisible= cbMakeDynamic->Checked&&m_CurrentObject->IsSkeleton();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::pcObjectsChange(TObject *Sender)
{
    ebDropper->Down		= false;
	if (((pcObjects->ActivePage==tsMeshes)||(pcObjects->ActivePage==tsSurfaces))&&m_CurrentObject)
	    ebDropper->Enabled	= true;
    else
	    ebDropper->Enabled	= false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::OnIdle(){
	if (form&&form->ebDropper->Down&&m_CurrentObject){
		if ((form->pcObjects->ActivePage==form->tsMeshes)||(form->pcObjects->ActivePage==form->tsSurfaces)){
        // check cursor position and define ...
            POINT pt,wpt;
            GetCursorPos( &pt );

            SHORT ks=GetAsyncKeyState(VK_LBUTTON);

			TWinControl* ctr = FindVCLWindow(pt);
            if (ctr!=frmMain) return;

            if (ks&0x0001){
                Fvector S, D;
                Ipoint CP;
                pt = UI->GetD3DWindow()->ScreenToClient(pt);
                CP.set(float(pt.x),float(pt.y));
                Device.m_Camera.MouseRayFromPoint(S, D, CP );

                float dist=flt_max;
                SRayPickInfo pinf;
                if (m_CurrentObject->RayPick(dist,S,D,precalc_identity,&pinf)){
					if (form->pcObjects->ActivePage==form->tsMeshes){
 	                   	form->tvMeshes->Selected = form->tvMeshes->Items->LookForItem(0,pinf.e_mesh->GetName(),0,0,false,true,false,false,true);
                        form->tvMeshes->EnsureVisible(form->tvMeshes->Selected);
	                }else if (form->pcObjects->ActivePage==form->tsSurfaces){
						UI->RedrawScene();
						st_Surface* surf=pinf.e_mesh->GetSurfaceByFaceID(pinf.rp_inf.id);
 	                   	form->tvSurfaces->Selected = form->tvSurfaces->Items->LookForItem(0,surf->name,0,0,false,true,false,false,true);
                        form->tvSurfaces->EnsureVisible(form->tvSurfaces->Selected);
    	            }
                }
            }
        }
    }
}
//---------------------------------------------------------------------------

