#include "stdafx.h"
#pragma hdrstop

#include "PropertiesObject.h"
#include "SceneClassList.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "Texture.h"
#include "ui_main.h"
#include "scene.h"
#include "main.h"
#include "motion.h"
#include "EditLibrary.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TfrmPropertiesObject* 	TfrmPropertiesObject::form 				= 0;
CEditableObject*		TfrmPropertiesObject::m_EditObject		= 0;

void TfrmPropertiesObject::SetCurrent(CEditableObject* object){
    if (form){
	    form->ApplyObjectsInfo();
	    m_EditObject = object;
    	form->GetObjectsInfo();
		form->pcObjects->ActivePage->OnShow(0);
    }else{
	    m_EditObject = object;
    }
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
    sgBB->Cells[0][0]="X"; sgBB->Cells[0][1]="Y"; sgBB->Cells[0][2]="Z";
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
    bLoadMode = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::FormShow(TObject *Sender)
{
    pcObjects->ActivePage = tsMainOptions;
    ebOk->Enabled       = false;
    ebApply->Enabled    = false;
    tvMeshes->Enabled   = true;
    edName->Hint 		= "Warning!!! Be careful before edit item library name.";
    tsMainOptionsShow   (Sender);
    ebDropper->Enabled	= false;
    ebDropper->Down		= false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	Action = caFree;
    form = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::OnModified(TObject *Sender)
{
    if (bLoadMode) return;
    ebOk->Enabled 		= true;
    ebApply->Enabled    = true;
    UI.RedrawScene();
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
    ebApply->Enabled 	= false;
    ebOk->Enabled 		= false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebCancelClick(TObject *Sender)
{
    Close();
    if (m_EditObject){
    	Lib->UnloadObject(m_EditObject->GetName());
        m_EditObject = 0;
    }
}
//--------------------------------------------------------------------------------------------------

void TfrmPropertiesObject::GetObjectsInfo(){
	bLoadMode = true;
	if (m_EditObject){
    	pcObjects->Enabled = true;

        mmScript->Text = m_EditObject->GetClassScript();
        edName->Text   = m_EditObject->GetName();
        cbMakeDynamic->ObjFirstInit( TCheckBoxState(m_EditObject->IsDynamic()) );

        Caption = AnsiString("Object properties - [")+edName->Text+AnsiString("]");
    }else{
    	pcObjects->Enabled = false;
        Caption = "Object properties: []";
    }
    ebOk->Enabled 		= false;
    ebApply->Enabled    = false;
	bLoadMode = false;
}

void TfrmPropertiesObject::ApplyObjectsInfo(){
	if (m_EditObject&&IsModified()){
        if (!edName->Text.Length()){
            ELog.DlgMsg(mtError,"Enter Object Name!");
            return;
        }
        // dynamic flag
        cbMakeDynamic->ObjApply( m_EditObject->m_DynamicObject );
		// class script
        m_EditObject->GetClassScript() = mmScript->Text;
        // modify transformation
	    m_EditObject->t_vPosition.set	(sePositionX->Value,sePositionY->Value,	sePositionZ->Value);
		m_EditObject->t_vRotate.set		(seRotateX->Value,	seRotateY->Value,	seRotateZ->Value);
	    m_EditObject->t_vScale.set		(seScaleX->Value,	seScaleY->Value,	seScaleZ->Value);
        // set "Modify" library flag
	    TfrmEditLibrary::OnModified();
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
	VERIFY(m_EditObject);
    tsScript->TabVisible 		= cbMakeDynamic->Checked;
    tsOAnimation->TabVisible 	= cbMakeDynamic->Checked;
    tsSAnimation->TabVisible 	= cbMakeDynamic->Checked&&m_EditObject->IsSkeleton();
    OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tsMainOptionsShow(TObject *Sender)
{
	if (!m_EditObject) return;
    edName->Enabled 		= true;
    mmScript->Enabled 		= true;
    gbTemplates->Enabled 	= true;

    tsScript->TabVisible 	= cbMakeDynamic->Checked;
    tsOAnimation->TabVisible= cbMakeDynamic->Checked;
    tsSAnimation->TabVisible= cbMakeDynamic->Checked&&m_EditObject->IsSkeleton();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::pcObjectsChange(TObject *Sender)
{
    ebDropper->Down		= false;
	if (((pcObjects->ActivePage==tsMeshes)||(pcObjects->ActivePage==tsSurfaces))&&m_EditObject)
	    ebDropper->Enabled	= true;
    else
	    ebDropper->Enabled	= false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::Pick(const SRayPickInfo& pinf){
	if (form&&form->ebDropper->Down&&m_EditObject){
		if ((form->pcObjects->ActivePage==form->tsMeshes)||(form->pcObjects->ActivePage==form->tsSurfaces)){
            if (form->pcObjects->ActivePage==form->tsMeshes){
                form->tvMeshes->Selected = form->tvMeshes->Items->LookForItem(0,pinf.e_mesh->GetName(),0,0,false,true,false,false,true);
                form->tvMeshes->EnsureVisible(form->tvMeshes->Selected);
            }else if (form->pcObjects->ActivePage==form->tsSurfaces){
                UI.RedrawScene();
                CSurface* surf=pinf.e_mesh->GetSurfaceByFaceID(pinf.rp_inf.id);
                form->tvSurfaces->Selected = form->tvSurfaces->Items->LookForItem(0,surf->_Name(),0,0,false,true,false,false,true);
                form->tvSurfaces->EnsureVisible(form->tvSurfaces->Selected);
            }
        }
    }
}
//---------------------------------------------------------------------------



