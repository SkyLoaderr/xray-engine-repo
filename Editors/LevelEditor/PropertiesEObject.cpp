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
    props->m_Basic 		= TfrmProperties::CreateProperties(props->paBasic,alClient,props->OnModifiedEvent);
    props->m_Surfaces 	= TfrmProperties::CreateProperties(props->paSurfaces,alClient,props->OnModifiedEvent,props->OnSurfaceFocused);
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
		m_Basic->AddItem	(0,PROP_MARKER2,"Name",(LPVOID)S->GetRefName());
		m_Basic->AddItem	(0,PROP_FLAG,	"Dynamic",m_Basic->MakeFlagValue(&O->m_dwFlags,CEditableObject::eoDynamic));
        TElTreeItem* M;
		M=m_Basic->AddItem	(0,PROP_MARKER,	"Transformation");
		m_Basic->AddItem	(M,PROP_VECTOR,	"Position",	m_Basic->MakeVectorValue(&S->FPosition,	-10000,	10000,0.01,2,OnAfterTransformation));
		m_Basic->AddItem	(M,PROP_VECTOR,	"Rotation",	m_Basic->MakeVectorValue(&S->FRotation,	-10000,	10000,0.1,1,RotateOnAfterEdit,RotateOnBeforeEdit,RotateOnDraw));
		m_Basic->AddItem	(M,PROP_VECTOR,	"Scale",	m_Basic->MakeVectorValue(&S->FScale,	0.01,	10000,0.01,2,OnAfterTransformation));
		M=m_Basic->AddItem	(0,PROP_MARKER,	"Summary");
        AnsiString t; t.sprintf("V: %d, F: %d",S->GetVertexCount(),S->GetFaceCount());
		m_Basic->AddItem	(M,PROP_MARKER2,"Object",t.c_str());
		M=m_Basic->AddItem	(M,PROP_MARKER,	"Meshes");
        for (EditMeshIt m_it=O->FirstMesh(); m_it!=O->LastMesh(); m_it++){
        	CEditableMesh* MESH=*m_it;
	        t.sprintf("V: %d, F: %d",MESH->GetVertexCount(),MESH->GetFaceCount());
			m_Basic->AddItem(M,PROP_MARKER2,MESH->GetName(),t.c_str());
        }
		M=m_Basic->AddItem	(0,PROP_MARKER,	"Game options");
		m_Basic->AddItem	(0,PROP_ANSI_TEXT,"Script",	m_Basic->MakeAnsiTextValue(&O->m_ClassScript));
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
			M=m_Surfaces->AddItem(0,PROP_MARKER,SURF->_Name(),SURF);
			m_Surfaces->AddItem(M,PROP_ANSI_SH_ENGINE,	"Shader",	m_Surfaces->MakeAnsiTextValue(&SURF->m_ShaderName, 	OnAfterShaderEdit));
			m_Surfaces->AddItem(M,PROP_ANSI_SH_COMPILE,	"Compile",	m_Surfaces->MakeAnsiTextValue(&SURF->m_ShaderXRLCName));
            m_Surfaces->AddItem(M,PROP_ANSI_TEXTURE,	"Texture",	m_Surfaces->MakeAnsiTextValue(&SURF->m_Texture,		OnAfterTextureEdit));
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
    TfrmProperties::DestroyProperties(m_Basic);
    TfrmProperties::DestroyProperties(m_Surfaces);
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
    	EPropType type	= TfrmProperties::GetItemType(item);
        TElTreeItem* parent	= item->Parent?item->Parent:item;
		CSurface* surf 		= (CSurface*)parent->Data;
    	switch (type){
        	case PROP_ANSI_TEXTURE:
                m_Thumbnail = new EImageThumbnail(TfrmProperties::GetItemColumn(item,0),EImageThumbnail::EITTexture);
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


