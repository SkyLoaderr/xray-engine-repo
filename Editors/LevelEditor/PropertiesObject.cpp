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
#include "ChoseForm.h"
#include "ImageThumbnail.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElTreeStdEditors"
#pragma resource "*.dfm"

TfrmPropertiesObject* 	TfrmPropertiesObject::form 				= 0;
CEditableObject*		TfrmPropertiesObject::m_LibObject		= 0;
EImageThumbnail*		TfrmPropertiesObject::m_Thumbnail		= 0;
SurfInstVec				TfrmPropertiesObject::m_EditSurfaces;

void TfrmPropertiesObject::SetCurrent(CEditableObject* object){
    if (form){
	    form->ApplyObjectsInfo();
	    m_LibObject 	= object;
    	form->GetObjectsInfo();
	    GetSurfacesFromObject();
		form->pcObjects->ActivePage->OnShow(0);
    }else{
	    m_LibObject = object;
	    GetSurfacesFromObject();
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
    DEFINE_INI(fsStorage);
    bLoadMode = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::FormShow(TObject *Sender)
{
    pcObjects->ActivePage = tsMainOptions;
    ebOk->Enabled       = false;
    ebApply->Enabled    = false;
    tvMeshes->Enabled   = true;
    tsMainOptionsShow   (Sender);
    ebDropper->Enabled	= false;
    ebDropper->Down		= false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
    form = 0;
    _DELETE(m_Thumbnail);
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
//    ebOk->Enabled 		= false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebCancelClick(TObject *Sender)
{
//    if (m_LibObject&&IsModified()){
//    	Lib.ReloadObject(m_EditObject->GetName());
//        m_LibObject = 0;
//		TfrmEditLibrary::RefreshSelected();
//    }
    Close();
}
//--------------------------------------------------------------------------------------------------

void TfrmPropertiesObject::GetObjectsInfo(){
	bLoadMode = true;
	if (m_LibObject){
    	pcObjects->Enabled = true;

        mmScript->Text = m_LibObject->GetClassScript();
        cbMakeDynamic->ObjFirstInit( TCheckBoxState(m_LibObject->IsDynamic()) );

        Caption = AnsiString("Object properties - [")+AnsiString(m_LibObject->GetName())+AnsiString("]");
    }else{
    	pcObjects->Enabled = false;
        Caption = "Object properties: []";
    }
    ebOk->Enabled 		= false;
    ebApply->Enabled    = false;
	bLoadMode = false;
}

void TfrmPropertiesObject::ApplyObjectsInfo(){
	if (m_LibObject&&IsModified()){
        // dynamic flag
        int c=m_LibObject->IsDynamic();
        cbMakeDynamic->ObjApply(c);
        m_LibObject->SetFlag(c);
		// class script
        m_LibObject->GetClassScript() 	= mmScript->Text;
        // modify transformation
	    m_LibObject->t_vPosition.set	(sePositionX->Value,sePositionY->Value,	sePositionZ->Value);
		m_LibObject->t_vRotate.set		(deg2rad(seRotateX->Value),deg2rad(seRotateY->Value),deg2rad(seRotateZ->Value));
	    m_LibObject->t_vScale.set		(seScaleX->Value,	seScaleY->Value,	seScaleZ->Value);
        m_LibObject->Modified			();
        // set "Modify" library flag
	    TfrmEditLibrary::OnModified		();
        // apply surfaces
        SetSurfacesToObject				();
	}
    UI.RedrawScene();
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
	VERIFY(m_LibObject);
    tsScript->TabVisible 		= cbMakeDynamic->Checked;
    OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tsMainOptionsShow(TObject *Sender)
{
	if (!m_LibObject) return;
    mmScript->Enabled 		= true;
    gbTemplates->Enabled 	= true;

    tsScript->TabVisible 	= cbMakeDynamic->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::pcObjectsChange(TObject *Sender)
{
    ebDropper->Down		= false;
	if (((pcObjects->ActivePage==tsMeshes)||(pcObjects->ActivePage==tsSurfaces))&&m_LibObject)
	    ebDropper->Enabled	= true;
    else
	    ebDropper->Enabled	= false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::OnPick(const SRayPickInfo& pinf){
	R_ASSERT(pinf.e_mesh);
	if (form&&form->ebDropper->Down&&m_LibObject){
        if (form->pcObjects->ActivePage==form->tsMeshes){
            form->tvMeshes->Selected = form->tvMeshes->Items->LookForItem(0,pinf.e_mesh->GetName(),0,0,false,true,false,false,true);
            form->tvMeshes->EnsureVisible(form->tvMeshes->Selected);
        }else if (form->pcObjects->ActivePage==form->tsSurfaces){
            UI.RedrawScene();
            CSurface* surf=pinf.e_mesh->GetSurfaceByFaceID(pinf.inf.id);
            form->tvSurfaces->Selected = form->tvSurfaces->Items->LookForItem(0,surf->_Name(),0,0,false,true,false,false,true);
            form->tvSurfaces->EnsureVisible(form->tvSurfaces->Selected);
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// surfaces
//---------------------------------------------------------------------------
void TfrmPropertiesObject::GetSurfacesFromObject()
{
	if (!m_LibObject) return;
    m_EditSurfaces.resize(m_LibObject->m_Surfaces.size());
    SurfInstIt e_it = m_EditSurfaces.begin();
	for (SurfaceIt l_it=m_LibObject->m_Surfaces.begin(); l_it!=m_LibObject->m_Surfaces.end(); l_it++,e_it++)
		e_it->CopyFrom(*l_it);
}
//---------------------------------------------------------------------------
void TfrmPropertiesObject::SetSurfacesToObject()
{
	VERIFY(m_LibObject);
	VERIFY(m_LibObject->m_Surfaces.size()==m_EditSurfaces.size());
    m_EditSurfaces.resize(m_LibObject->m_Surfaces.size());
    SurfaceIt l_it = m_LibObject->m_Surfaces.begin();
	for (SurfInstIt e_it=m_EditSurfaces.begin(); e_it!=m_EditSurfaces.end(); e_it++,l_it++){
    	bool bChangeShader=(((*l_it)->_ShaderName()==e_it->_ShaderName())||((*l_it)->_Texture()==e_it->_Texture()));
		// remove shader
        if (bChangeShader&&(*l_it)->_Shader())
        	Device.Shader.Delete((*l_it)->_Shader());
        // copy new data
        **l_it = *e_it;
        // create shader
        if (bChangeShader)
        	(*l_it)->SetShader((*l_it)->_ShaderName(), Device.Shader.Create((*l_it)->_ShaderName(),(*l_it)->_Texture()));
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tsSurfacesShow(TObject *Sender)
{
	if (!m_LibObject) return;
// Set up surfaces&textures
    bool bEq;
    tvSurfaces->IsUpdating = true;
    tvSurfaces->Items->Clear();

    // create root tree node (object name)
    TElTreeItem* first_node=0;
    for (SurfInstIt e_it=m_EditSurfaces.begin(); e_it!=m_EditSurfaces.end(); e_it++){
        TElTreeItem* pNode = tvSurfaces->Items->AddChildObject(0,e_it->_Name(),(TObject*)(e_it));
        if (!first_node) first_node=pNode;
		tvSurfaces->Items->AddChild(pNode,e_it->_Texture());
    }
    tvSurfaces->Sort(true);
    tvSurfaces->FullExpand();
    tvSurfaces->IsUpdating = false;
	tvSurfaces->Selected=first_node;
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::ebSelectShaderClick(TObject *Sender)
{
	VERIFY(tvSurfaces->Selected->Level==0);
    if (!tvSurfaces->Selected) return;
	CSurface* surf = (CSurface*)tvSurfaces->Selected->Data;
	if (!surf) return;
    LPCSTR src = surf->_ShaderName();
	LPCSTR S = TfrmChoseItem::SelectShader(src&&src[0]?surf->_ShaderName():0);
    if (S&&(strcmp(surf->_ShaderName(),S)!=0)){
		lbShader->Caption = S;
    	surf->ED_SetShader(S);
        OnModified(Sender);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::ebSelectShaderXRLCClick(
      TObject *Sender)
{
	VERIFY(tvSurfaces->Selected->Level==0);
    if (!tvSurfaces->Selected) return;
	CSurface* surf = (CSurface*)tvSurfaces->Selected->Data;
	if (!surf) return;
	LPCSTR S = TfrmChoseItem::SelectShaderXRLC(surf->_ShaderXRLCName()?surf->_ShaderXRLCName():0);
	if (S&&(strcmp(surf->_ShaderXRLCName(),S)!=0)){
		lbShaderXRLC->Caption = S;
		surf->SetShaderXRLC(S);
        OnModified(Sender);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::ebSelectTextureClick(TObject *Sender)
{
	VERIFY(tvSurfaces->Selected->Level==0);
    if (!tvSurfaces->Selected) return;
	CSurface* surf = (CSurface*)tvSurfaces->Selected->Data;
	if (!surf) return;
    AnsiString temp = surf->_Texture();
	LPCSTR S = TfrmChoseItem::SelectTexture(false,temp.c_str());
	if (S&&(strcmp(surf->_Texture(),S)!=0)){
        lbTexture->Caption = S;
        surf->SetTexture(S);
		OnModified(Sender);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tvSurfacesItemFocused( TObject *Sender)
{
	TElTreeItem* Item = tvSurfaces->Selected;
    if (Item){
        paSurface->Visible 				= Item->Level==0;
        paTexture->Visible 				= Item->Level==1;
        switch (Item->Level){
        case 0:{
			CSurface* surf 				= (CSurface*)Item->Data;
            lbSurfFaces->Caption 		= m_LibObject->GetSurfFaceCount(surf->_Name());
            lbSurfSideFlag->Caption 	= (surf->_2Sided())?"yes":"no";
            lbShader->Caption			= surf->_ShaderName();
            lbShaderXRLC->Caption		= surf->_ShaderXRLCName();
            lbTexture->Caption			= surf->_Texture();
            // FVF
            lbSurfFVF_XYZ->Caption		= (surf->_FVF()&D3DFVF_XYZ)?"yes":"no";
            lbSurfFVF_Normal->Caption  	= (surf->_FVF()&D3DFVF_NORMAL)?"yes":"no";
            lbSurfFVF_Diffuse->Caption	= (surf->_FVF()&D3DFVF_DIFFUSE)?"yes":"no";
            lbSurfFVF_Specular->Caption	= (surf->_FVF()&D3DFVF_SPECULAR)?"yes":"no";
            lbSurfFVF_XYZRHW->Caption	= (surf->_FVF()&D3DFVF_XYZRHW)?"yes":"no";
            lbSurfFVF_TCCount->Caption 	= ((surf->_FVF()&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
        }break;
        case 1:
            _DELETE(m_Thumbnail);
            m_Thumbnail = new EImageThumbnail(AnsiString(Item->Text).c_str(),EImageThumbnail::EITTexture);
            lbWidth->Caption = m_Thumbnail->_Width();
            lbHeight->Caption = m_Thumbnail->_Height();
            lbAlpha->Caption = (m_Thumbnail->_Alpha())?"present":"absent";
            if (m_Thumbnail->_Width()!=m_Thumbnail->_Height()) paImage->Repaint();
            pbImagePaint(Sender);
        break;
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::pbImagePaint(TObject *Sender)
{
	if (m_Thumbnail) m_Thumbnail->Draw(paImage,pbImage,true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::cbSurfSideFlagClick(TObject *Sender)
{
	TCheckBox* cb = dynamic_cast<TCheckBox*>(Sender);
    cb->State = TCheckBoxState(!cb->Checked);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ExtBtn1Click(TObject *Sender)
{
	tvSurfaces->FullExpand();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ExtBtn2Click(TObject *Sender)
{
	tvSurfaces->FullCollapse();
}
//---------------------------------------------------------------------------



