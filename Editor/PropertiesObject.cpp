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
#pragma resource "*.dfm"

TfrmPropertiesObject* 	TfrmPropertiesObject::form 				= 0;
CEditableObject*		TfrmPropertiesObject::m_EditObject		= 0;
EImageThumbnail*		TfrmPropertiesObject::m_Thumbnail		= 0;

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
    _DELETE(m_Thumbnail);
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
    if (m_EditObject&&IsModified()){
    	Lib.UnloadObject(m_EditObject->GetName());
        m_EditObject = 0;
		TfrmEditLibrary::RefreshSelected();
    }
}
//--------------------------------------------------------------------------------------------------

void TfrmPropertiesObject::GetObjectsInfo(){
	bLoadMode = true;
	if (m_EditObject){
    	pcObjects->Enabled = true;

        mmScript->Text = m_EditObject->GetClassScript();
        cbMakeDynamic->ObjFirstInit( TCheckBoxState(m_EditObject->IsDynamic()) );

        Caption = AnsiString("Object properties - [")+AnsiString(m_EditObject->GetName())+AnsiString("]");
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
        // dynamic flag
        cbMakeDynamic->ObjApply( m_EditObject->m_DynamicObject );
		// class script
        m_EditObject->GetClassScript() = mmScript->Text;
        // modify transformation
	    m_EditObject->t_vPosition.set	(sePositionX->Value,sePositionY->Value,	sePositionZ->Value);
		m_EditObject->t_vRotate.set		(seRotateX->Value,	seRotateY->Value,	seRotateZ->Value);
	    m_EditObject->t_vScale.set		(seScaleX->Value,	seScaleY->Value,	seScaleZ->Value);
        m_EditObject->Modified();
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
    mmScript->Enabled 		= true;
    gbTemplates->Enabled 	= true;

    tsScript->TabVisible 	= cbMakeDynamic->Checked;
    tsOAnimation->TabVisible= cbMakeDynamic->Checked;
    tsSAnimation->TabVisible= cbMakeDynamic->Checked&&m_EditObject->IsSkeleton();
}
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

void __fastcall TfrmPropertiesObject::OnPick(const SRayPickInfo& pinf){
	R_ASSERT(pinf.e_mesh);
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

//---------------------------------------------------------------------------
// surfaces
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tsSurfacesShow(TObject *Sender)
{
	if (!m_EditObject) return;
// Set up surfaces&textures
    bool bEq;
    tvSurfaces->IsUpdating = true;
    tvSurfaces->Items->Clear();

    // create root tree node (object name)
    for (SurfaceIt s_it=m_EditObject->m_Surfaces.begin(); s_it!=m_EditObject->m_Surfaces.end(); s_it++){
        TElTreeItem* pNode = tvSurfaces->Items->AddChildObject(0,(*s_it)->_Name(),(TObject*)(*s_it));
		tvSurfaces->Items->AddChild(pNode,(*s_it)->_Texture());
    }
    tvSurfaces->Sort(true);
    tvSurfaces->FullExpand();
    tvSurfaces->IsUpdating = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::ebSelectShaderClick(TObject *Sender)
{
	if (!m_Surf) return;
	LPCSTR S = TfrmChoseItem::SelectShader(true,0,m_Surf->_Shader()?m_Surf->_ShaderName():0);
    if (S){
        lbShader->Caption = S;
    	if (m_Surf->_Shader()){
        	if(strcmp(m_Surf->_ShaderName(),S)!=0){
		    	Device.Shader.Delete(m_Surf->_Shader());
    		    m_Surf->SetShader(S, Device.Shader.Create(S,m_Surf->_Texture()));
        		OnModified(Sender);
            }
        }else{
            m_Surf->SetShader(S, Device.Shader.Create(S,m_Surf->_Texture()));
            OnModified(Sender);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::ebSelectShaderXRLCClick(
      TObject *Sender)
{
	if (!m_Surf) return;
	LPCSTR S = TfrmChoseItem::SelectShaderXRLC(0,m_Surf->_ShaderXRLCName()?m_Surf->_ShaderXRLCName():0);
    if (S){
        lbShaderXRLC->Caption = S;
		m_Surf->SetShaderXRLC(S);
        OnModified(Sender);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::ebSelectTextureClick(TObject *Sender)
{
	if (!m_Surf) return;
    AnsiString temp = m_Surf->_Texture();
	LPCSTR S = TfrmChoseItem::SelectTexture(false,temp.c_str());
    if (S){
        lbTexture->Caption = S;
        m_Surf->SetTexture(S);
    	if (m_Surf->_Shader()){
			Device.Shader.Delete(m_Surf->_Shader());
    		m_Surf->SetShader(m_Surf->_ShaderName(), Device.Shader.Create(lbShader->Caption.c_str(),m_Surf->_Texture()));
			OnModified(Sender);
        }else{
            m_Surf->SetShader(m_Surf->_ShaderName(), Device.Shader.Create(lbShader->Caption.c_str(),m_Surf->_Texture()));
            OnModified(Sender);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tvSurfacesItemFocused( TObject *Sender)
{
	TElTreeItem* Item = tvSurfaces->Selected;
    if (Item){
        m_Surf		 					= 0;
        paSurface->Visible 				= Item->Level==0;
        paTexture->Visible 				= Item->Level==1;
        switch (Item->Level){
        case 0:{
            CSurface* surf				= (CSurface*)(Item->Data);
            lbSurfFaces->Caption 		= m_EditObject->GetSurfFaceCount(surf->_Name());
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
            m_Surf 						= surf;
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




