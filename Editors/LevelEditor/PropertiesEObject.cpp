#include "stdafx.h"
#pragma hdrstop

#include "PropertiesEObject.h"
#include "SceneObject.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "ImageThumbnail.h"
#include "FolderLib.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElXPThemedControl"
#pragma link "ExtBtn"
#pragma link "mxPlacemnt"
#pragma link "ExtBtn"
#pragma link "ElPgCtl"
#pragma link "ElXPThemedControl"
#pragma link "MXCtrls"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesEObject::TfrmPropertiesEObject(TComponent* Owner)
    : TForm(Owner)
{
	m_Thumbnail	= 0;
    m_pEditObject=0;
}
//---------------------------------------------------------------------------


TfrmPropertiesEObject* TfrmPropertiesEObject::CreateProperties(TWinControl* parent, TAlign align, TOnModifiedEvent modif)
{
	TfrmPropertiesEObject* props = new TfrmPropertiesEObject(parent);
    props->OnModifiedEvent = modif;
    if (parent){
		props->Parent 	= parent;
    	props->Align 	= align;
	    props->BorderStyle = bsNone;
        props->ShowProperties();
    }
    props->m_Basic 		= TProperties::CreateForm(props->paBasic,alClient,props->OnModifiedEvent);
    props->m_Surfaces 	= TProperties::CreateForm(props->paSurfaces,alClient,props->OnModifiedEvent,props->OnSurfaceFocused);
	return props;
}
//---------------------------------------------------------------------------

void TfrmPropertiesEObject::DestroyProperties(TfrmPropertiesEObject*& props)
{
	VERIFY(props);
	props->Close();
    _DELETE(props);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEObject::RotateOnAfterEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	Fvector* V = (Fvector*)edit_val;
	V->x = deg2rad(V->x);
	V->y = deg2rad(V->y);
	V->z = deg2rad(V->z);
	UI.RedrawScene();
}

void __fastcall TfrmPropertiesEObject::RotateOnBeforeEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	Fvector* V = (Fvector*)edit_val;
	V->x = rad2deg(V->x);
	V->y = rad2deg(V->y);
	V->z = rad2deg(V->z);
}

void __fastcall TfrmPropertiesEObject::RotateOnDraw(PropValue* sender, LPVOID draw_val)
{
	Fvector* V = (Fvector*)draw_val;
	V->x = rad2deg(V->x);
	V->y = rad2deg(V->y);
	V->z = rad2deg(V->z);
}

void TfrmPropertiesEObject::FillBasicProps()
{
	// basic
	CSceneObject* 		S = m_pEditObject;
	m_Basic->BeginFillMode();
    if (S->GetReference()){
    	CEditableObject* 	O = S->GetReference();
		m_Basic->AddMarkerItem	(0,"Name",		S->GetRefName());
		m_Basic->AddFlagItem	(0,"Dynamic",	&O->m_dwFlags,CEditableObject::eoDynamic);
		m_Basic->AddFlagItem	(0,"HOM",		&O->m_dwFlags,CEditableObject::eoHOM);
        m_Basic->AddFlagItem	(0,"Use LOD", 	&O->m_dwFlags,CEditableObject::eoUsingLOD);
        TElTreeItem* M;
		M=m_Basic->AddMarkerItem(0,"Transformation")->item;
		m_Basic->AddVectorItem	(M,"Position",	&S->FPosition,	-10000,	10000,0.01,2,OnAfterTransformation);
		m_Basic->AddVectorItem	(M,"Rotation",	&S->FRotation,	-10000,	10000,0.1,1,RotateOnAfterEdit,RotateOnBeforeEdit,RotateOnDraw);
		m_Basic->AddVectorItem	(M,"Scale",		&S->FScale,		0.01,	10000,0.01,2,OnAfterTransformation);
		M=m_Basic->AddMarkerItem(0,"Summary")->item;
        AnsiString t; t.sprintf("V: %d, F: %d",	S->GetVertexCount(),S->GetFaceCount());
		m_Basic->AddMarkerItem	(M,"Object",   	t.c_str());
		M=m_Basic->AddMarkerItem(M,"Meshes")->item;
        for (EditMeshIt m_it=O->FirstMesh(); m_it!=O->LastMesh(); m_it++){
        	CEditableMesh* MESH=*m_it;
	        t.sprintf("V: %d, F: %d",MESH->GetVertexCount(),MESH->GetFaceCount());
			m_Basic->AddMarkerItem(M,MESH->GetName(),t.c_str());
        }
		M=m_Basic->AddMarkerItem(0,"Game options")->item;
		m_Basic->AddAnsiTextItem(M,"Script",	&O->m_ClassScript);
    }
    m_Basic->EndFillMode(true);
}
//---------------------------------------------------------------------------

void TfrmPropertiesEObject::FillSurfProps()
{
	// surfaces
	m_Surfaces->BeginFillMode();
	CSceneObject* 		S = m_pEditObject;
    if (S->GetReference()){
    	CEditableObject* 	O = S->GetReference();
        TElTreeItem* M;
        for (SurfaceIt s_it=O->FirstSurface(); s_it!=O->LastSurface(); s_it++){
        	CSurface* SURF=*s_it;
			M=m_Surfaces->AddMarkerItem(0,SURF->_Name())->item;
			m_Surfaces->AddAnsiEShaderItem(M,"Shader",	&SURF->m_ShaderName, 	OnAfterShaderEdit);
			m_Surfaces->AddAnsiCShaderItem(M,"Compile",	&SURF->m_ShaderXRLCName);
            m_Surfaces->AddAnsiTextureItem(M,"Texture",	&SURF->m_Texture,		OnAfterTextureEdit);
        }
    }
    m_Surfaces->EndFillMode(true);
}
//---------------------------------------------------------------------------

void TfrmPropertiesEObject::UpdateProperties(CSceneObject* S)
{
	m_pEditObject = S;
	FillBasicProps();
    FillSurfProps();
}

void __fastcall TfrmPropertiesEObject::FormDestroy(TObject *Sender)
{
	_DELETE(m_Thumbnail);
    TProperties::DestroyForm(m_Basic);
    TProperties::DestroyForm(m_Surfaces);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEObject::fsStorageRestorePlacement(
      TObject *Sender)
{
	m_Basic->RestoreColumnWidth(fsStorage);
	m_Surfaces->RestoreColumnWidth(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEObject::fsStorageSavePlacement(
      TObject *Sender)
{
	m_Basic->SaveColumnWidth(fsStorage);
	m_Surfaces->SaveColumnWidth(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEObject::OnSurfaceFocused(TElTreeItem* item)
{
	_DELETE(m_Thumbnail);
	if (item){
    	EPropType type	= TProperties::GetItemType(item);
        TElTreeItem* parent	= item->Parent?item->Parent:item;
		CSurface* surf 		= (CSurface*)parent->Data;
    	switch (type){
        	case PROP_ANSI_TEXTURE:
                m_Thumbnail = new EImageThumbnail(TProperties::GetItemColumn(item,0),EImageThumbnail::EITTexture);
                lbWidth->Caption 	= m_Thumbnail->_Width();
                lbHeight->Caption 	= m_Thumbnail->_Height();
                lbAlpha->Caption 	= (m_Thumbnail->_Alpha())?"present":"absent";
                if (m_Thumbnail->_Width()!=m_Thumbnail->_Height()) paImage->Repaint();
                pbImagePaint(item);
            break;
            default:
			    ResetSurfInfo		(true,false);
        }
        if (surf){
            lbSurfFaces->Caption 	= m_pEditObject->GetSurfFaceCount(surf->_Name());
            lb2Sided->Caption 		= (surf->_2Sided())?"yes":"no";
            lbXYZ->Caption			= (surf->_FVF()&D3DFVF_XYZ)?"yes":"no";
            lbNormal->Caption  		= (surf->_FVF()&D3DFVF_NORMAL)?"yes":"no";
            lbDiffuse->Caption		= (surf->_FVF()&D3DFVF_DIFFUSE)?"yes":"no";
            lbUVs->Caption 			= ((surf->_FVF()&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
        }else{
			ResetSurfInfo			(false,true);
        }
    }else{
	    ResetSurfInfo				(true,true);
    }
}
//---------------------------------------------------------------------------

void TfrmPropertiesEObject::ResetSurfInfo(bool imageinfo, bool surfinfo)
{
	// image
    if (imageinfo){
	    lbWidth->Caption 		= "...";
    	lbHeight->Caption 		= "...";
	    lbAlpha->Caption 		= "...";
		paImage->Repaint();
    }
    // surface
    if (surfinfo){
	    lbSurfFaces->Caption 	= "...";
    	lb2Sided->Caption 		= "...";
	    lbXYZ->Caption			= "...";
    	lbNormal->Caption  		= "...";
	    lbDiffuse->Caption		= "...";
    	lbUVs->Caption 			= "...";
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEObject::pbImagePaint(TObject *Sender)
{
	if (m_Thumbnail) m_Thumbnail->Draw(paImage,pbImage,true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEObject::OnPick(const SRayPickInfo& pinf)
{
	R_ASSERT(pinf.e_mesh);
	if (ebDropper->Down&&m_pEditObject){
        CSurface* surf=pinf.e_mesh->GetSurfaceByFaceID(pinf.inf.id);
        m_Surfaces->tvProperties->Selected = FOLDER::FindItemInFolder(m_Surfaces->tvProperties,0,surf->_Name());
        m_Surfaces->tvProperties->EnsureVisible(m_Surfaces->tvProperties->Selected);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEObject::OnAfterShaderEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	AnsiString new_name = *(AnsiString*)edit_val;
	TElTreeItem* parent	= item->Parent; VERIFY(parent);
	CSurface* surf 		= (CSurface*)parent->Data;	VERIFY(surf);
    surf->DeleteShader	();
    surf->ED_SetShader	(new_name.c_str());
    surf->CreateShader	();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEObject::OnAfterTextureEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	AnsiString new_name = *(AnsiString*)edit_val;
	TElTreeItem* parent	= item->Parent; VERIFY(parent);
	CSurface* surf 		= (CSurface*)parent->Data; 	VERIFY(surf);
    surf->DeleteShader	();
    surf->SetTexture	(new_name.c_str());
    surf->CreateShader	();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEObject::OnAfterTransformation(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	UI.RedrawScene();
}
//---------------------------------------------------------------------------


