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
#include "ChoseForm.h"
#include "xr_trims.h"
//---------------------------------------------------------------------------
// surfaces
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tsSurfacesShow(TObject *Sender)
{
	if (!m_CurrentObject) return;
// Set up surfaces&textures
    bool bEq;
    tvSurfaces->Items->Clear();

    // create root tree node (object name)
    TElTreeItem* root = tvSurfaces->Items->Add(0,"Surfaces");
    root->ParentStyle = false;
    root->Bold = true;
    root->Underlined = true;

    for (SurfaceIt s_it=m_EditObject->m_Surfaces.begin(); s_it!=m_EditObject->m_Surfaces.end(); s_it++){
        TElTreeItem* pNode = tvSurfaces->Items->AddChildObject(root,(*s_it)->_Name(),(TObject*)(*s_it));
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
	if (!surf_selected) return;
	LPCSTR S = TfrmChoseItem::SelectShader(true,0,surf_selected->_Shader()?surf_selected->_ShaderName():0);
    if (S){
        lbShader->Caption = S;
    	if (surf_selected->_Shader()){
        	if(strcmp(surf_selected->_ShaderName(),S)!=0){
		    	Device.Shader.Delete(surf_selected->_Shader());
    		    surf_selected->SetShader(S, Device.Shader.Create(S,surf_selected->_Texture()));
        		OnModified(Sender);
            }
        }else{
            surf_selected->SetShader(S, Device.Shader.Create(S,surf_selected->_Texture()));
            OnModified(Sender);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::ebSelectShaderXRLCClick(
      TObject *Sender)
{
	if (!surf_selected) return;
	LPCSTR S = TfrmChoseItem::SelectShaderXRLC(0,surf_selected->_Shader()?surf_selected->_ShaderName():0);
    if (S){
        lbShader->Caption = S;
    	if (surf_selected->_Shader()){
        	if(strcmp(surf_selected->_ShaderName(),S)!=0){
		    	Device.Shader.Delete(surf_selected->_Shader());
    		    surf_selected->SetShader(S, Device.Shader.Create(S,surf_selected->_Texture()));
        		OnModified(Sender);
            }
        }else{
            surf_selected->SetShader(S, Device.Shader.Create(S,surf_selected->_Texture()));
            OnModified(Sender);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::ebSelectTextureClick(TObject *Sender)
{
	if (!surf_selected) return;
    AnsiString temp = surf_selected->_Texture();
	LPCSTR S = TfrmChoseItem::SelectTexture(false,temp.c_str());
    if (S){
        lbTexture->Caption = S;
        surf_selected->SetTexture(S);
    	if (surf_selected->_Shader()){
			Device.Shader.Delete(surf_selected->_Shader());
    		surf_selected->SetShader(surf_selected->_ShaderName(), Device.Shader.Create(lbShader->Caption.c_str(),surf_selected->_Texture()));
			OnModified(Sender);
        }else{
            surf_selected->SetShader(surf_selected->_ShaderName(), Device.Shader.Create(lbShader->Caption.c_str(),surf_selected->_Texture()));
            OnModified(Sender);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tvSurfacesItemSelectedChange(
      TObject *Sender, TElTreeItem *Item)
{
	if (Item==tvSurfaces->Selected) 	return;
    tx_selected 					= 0;
    surf_selected 					= 0;
    paSurface->Visible 				= Item->Level==1;
	paTexture->Visible 				= Item->Level==2;
    switch (Item->Level){
    case 0:
        paImage->Repaint();
    break;
    case 1:{
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
		surf_selected 				= surf;
    }break;
    case 2:
//S
/*
        tx_selected = Device.Shader.FindTexture(Item->Text.c_str());
        if (tx_selected){
            lbWidth->Caption = tx_selected->width();
            lbHeight->Caption = tx_selected->height();
            lbAlpha->Caption = (tx_selected->alpha())?"present":"absent";
            if (tx_selected->width()!=tx_selected->height()) paImage->Repaint();
            imPaint(Sender);
        }else{
        	ELog.DlgMsg(mtInformation,"Texture '%s' not used in current material.",Item->Text.c_str());
        }
*/    break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::imPaint(TObject *Sender)
{
    if (tx_selected){
//S
/*        RECT r; r.left = 1; r.top = 1;
        float w, h;
        w = tx_selected->width();
        h = tx_selected->height();
        if (w>h){
        	r.right = im->Width; r.bottom = h/w*im->Height;
        }else{
        	r.right = w/h*im->Width; r.bottom = im->Height;}
		tx_selected->StretchThumbnail(paImage->Handle, &r);
*/    }
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


