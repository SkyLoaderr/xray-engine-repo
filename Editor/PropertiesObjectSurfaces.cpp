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
#include "ChoseForm.h"
#include "xr_trims.h"
//---------------------------------------------------------------------------
// surfaces
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tsSurfacesShow(TObject *Sender)
{
// Set up surfaces&textures
    bool bEq;
    AnsiString name;
    tvSurfaces->Items->Clear();
    if (!bMultiSelection){
    	CEditObject *_O = m_EditObject;
        if (m_EditObject->GetRef()) _O = m_EditObject->GetRef();

        // create root tree node (object name)
        name.sprintf("%s (%s)",m_EditObject->GetName(),_O->GetName());
        TElTreeItem* root = tvSurfaces->Items->AddObject(0,name,(TObject*)_O);
        root->ParentStyle = false;
        root->Bold = true;
		root->Underlined = true;

        for (SurfaceIt s_it=_O->m_Surfaces.begin(); s_it!=_O->m_Surfaces.end(); s_it++){
	        TElTreeItem* pNode = tvSurfaces->Items->AddChildObject(root,(*s_it)->name,(TObject*)(*s_it));
	        pNode->ParentStyle = false;
	        pNode->Bold = true;
        	for (AStringIt n_it=(*s_it)->textures.begin(); n_it!=(*s_it)->textures.end(); n_it++)
				tvSurfaces->Items->AddChild(pNode,*n_it);
        }
		root->Expand(false);
    }else{
    	VERIFY(!m_Objects.empty());

	    ObjectIt _F = m_Objects.begin();
    	CEditObject *_O = 0;
    	CEditObject *_OO = 0;
    	for(;_F!=m_Objects.end();_F++){
	    	VERIFY( (*_F)->ClassID()==OBJCLASS_EDITOBJECT );
        	_O = (CEditObject *)(*_F);

            // if item is LibRef get info from LibRef
            if (_O->m_LibRef) _OO = _O->m_LibRef; else _OO = _O;

	        // create root tree node (object name)
            name.sprintf("%s (%s)",_O->GetName(),_OO->GetName());
            TElTreeItem* root = tvSurfaces->Items->AddObject(0,name,(TObject*)_OO);
            root->ParentStyle = false;
            root->Bold = true;
            root->Underlined = true;

            for (SurfaceIt s_it=_OO->m_Surfaces.begin(); s_it!=_OO->m_Surfaces.end(); s_it++){
		        TElTreeItem* pNode = tvSurfaces->Items->AddChildObject(root,(*s_it)->name,(TObject*)(*s_it));
                pNode->ParentStyle = false;
                pNode->Bold = true;
                for (AStringIt n_it=(*s_it)->textures.begin(); n_it!=(*s_it)->textures.end(); n_it++)
                    tvSurfaces->Items->AddChild(pNode,*n_it);
            }
            root->Expand(false);
    	}
    }
//    tvTextures->FullExpand();
    tvSurfaces->Sort(true);
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::ebSelectShaderClick(TObject *Sender)
{
	if (!surf_selected) return;
	LPCSTR S = TfrmChoseItem::SelectShader(true,0,surf_selected->shader?surf_selected->shader->shader->cName:0);
    if (S){
        lbShader->Caption = S;
    	if (surf_selected->shader){
        	if(strcmp(surf_selected->shader->shader->cName,S)!=0){
		    	UI->Device.Shader.Delete(surf_selected->shader);
    		    surf_selected->shader = UI->Device.Shader.Create(S,surf_selected->textures);
        		OnModified(Sender);
            }
        }else{
            surf_selected->shader = UI->Device.Shader.Create(S,surf_selected->textures);
            OnModified(Sender);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::ebSelectTextureClick(TObject *Sender)
{
	if (!surf_selected) return;
    AnsiString temp;
    temp = ListToSequence(surf_selected->textures);
	LPCSTR S = TfrmChoseItem::SelectTexture(true,temp.c_str());
    if (S){
        lbTexture->Caption = S;
        SequenceToList(surf_selected->textures,S);
    	if (surf_selected->shader){
			UI->Device.Shader.Delete(surf_selected->shader);
    		surf_selected->shader = UI->Device.Shader.Create(lbShader->Caption.c_str(),surf_selected->textures);
			OnModified(Sender);
        }else{
            surf_selected->shader = UI->Device.Shader.Create(lbShader->Caption.c_str(),surf_selected->textures);
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
    	CEditObject* O				= (CEditObject*)Item->Parent->Data;
    	st_Surface* surf			= (st_Surface*)(Item->Data);
        lbSurfFaces->Caption 		= O->GetSurfFaceCount(surf->name);
        lbSurfSideFlag->Caption 	= (surf->sideflag)?"yes":"no";
        lbShader->Caption			= surf->shader->shader->cName;
        lbTexture->Caption			= ListToSequence(surf->textures);
        // FVF
        lbSurfFVF_XYZ->Caption		= (surf->dwFVF&D3DFVF_XYZ)?"yes":"no";
        lbSurfFVF_Normal->Caption  	= (surf->dwFVF&D3DFVF_NORMAL)?"yes":"no";
        lbSurfFVF_Diffuse->Caption	= (surf->dwFVF&D3DFVF_DIFFUSE)?"yes":"no";
        lbSurfFVF_Specular->Caption	= (surf->dwFVF&D3DFVF_SPECULAR)?"yes":"no";
        lbSurfFVF_XYZRHW->Caption	= (surf->dwFVF&D3DFVF_XYZRHW)?"yes":"no";
		lbSurfFVF_TCCount->Caption 	= ((surf->dwFVF&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
		surf_selected 				= surf;
    }break;
    case 2:
        tx_selected = UI->Device.Shader.FindTexture(Item->Text.c_str());
        if (tx_selected){
            lbWidth->Caption = tx_selected->width();
            lbHeight->Caption = tx_selected->height();
            lbAlpha->Caption = (tx_selected->alpha())?"present":"absent";
            if (tx_selected->width()!=tx_selected->height()) paImage->Repaint();
            imPaint(Sender);
        }else{
        	Log->DlgMsg(mtInformation,"Texture '%s' not used in current material.",Item->Text.c_str());
        }
    break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::imPaint(TObject *Sender)
{
    if (tx_selected){
        RECT r; r.left = 1; r.top = 1;
        float w, h;
        w = tx_selected->width();
        h = tx_selected->height();
        if (w>h){   
        	r.right = im->Width; r.bottom = h/w*im->Height;
        }else{
        	r.right = w/h*im->Width; r.bottom = im->Height;}
		tx_selected->StretchThumbnail(paImage->Handle, &r);
    }
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


