#include "stdafx.h"
#pragma hdrstop

#include "PropertiesObject.h"
#include "SceneClassList.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "ImageThumbnail.h"
#include "ui_main.h"
#include "scene.h"
#include "main.h"
#include "ChoseForm.h"
#include "xr_trims.h"
//---------------------------------------------------------------------------
// surfaces
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tsSurfacesShow(TObject *Sender)
{
	if (!m_EditObject) return;
// Set up surfaces&textures
    bool bEq;
    tvSurfaces->Items->Clear();

    // create root tree node (object name)
    for (SurfaceIt s_it=m_EditObject->m_Surfaces.begin(); s_it!=m_EditObject->m_Surfaces.end(); s_it++){
        TElTreeItem* pNode = tvSurfaces->Items->AddChildObject(0,(*s_it)->_Name(),(TObject*)(*s_it));
        pNode->ParentStyle = false;
        pNode->Bold = true;
		tvSurfaces->Items->AddChild(pNode,(*s_it)->_Texture());
    }
    tvSurfaces->FullExpand();

    tvSurfaces->Sort(true);
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
        lbShader->Caption = S;
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
    _DELETE(m_Thm);
    m_Surf		 					= 0;
    paSurface->Visible 				= Item->Level==1;
	paTexture->Visible 				= Item->Level==2;
    switch (Item->Level){
    case 0:{
    	CSurface* surf				= (CSurface*)(Item->Data);
        lbSurfFaces->Caption 		= m_EditObject->GetSurfFaceCount(surf->_Name());
        lbSurfSideFlag->Caption 	= (surf->_2Sided())?"yes":"no";
        lbShader->Caption			= surf->_Name();
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
    	_DELETE(m_Thm);
        m_Thm = new EImageThumbnail(AnsiString(Item->Text).c_str(),EImageThumbnail::EITTexture);
		lbWidth->Caption = m_Thm->_Width();
        lbHeight->Caption = m_Thm->_Height();
        lbAlpha->Caption = (m_Thm->_Alpha())?"present":"absent";
        if (m_Thm->_Width()!=m_Thm->_Height()) paImage->Repaint();
        pbImagePaint(Sender);
    break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::pbImagePaint(TObject *Sender)
{
	if (m_Thm) m_Thm->Draw(paImage,pbImage,true);
}

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


