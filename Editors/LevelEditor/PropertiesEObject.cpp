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
    props->m_BasicProp	= TProperties::CreateForm(props->paBasic,alClient,props->OnModifiedEvent);
    props->m_SurfProp 	= TProperties::CreateForm(props->paSurfaces,alClient,props->OnModifiedEvent,props->OnSurfaceFocused);
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
	m_BasicProp->BeginFillMode();
    if (S->GetReference()){
    	CEditableObject* 	O = S->GetReference();
        PropValueVec values;

	    FILL_PROP(values, "Reference Name",			(LPVOID)S->GetRefName(),PROP::CreateMarkerValue());
	    FILL_PROP(values, "Flags\\Dynamic",			&O->m_dwFlags, 			PROP::CreateFlagValue(CEditableObject::eoDynamic));
	    FILL_PROP(values, "Flags\\HOM",	   			&O->m_dwFlags, 			PROP::CreateFlagValue(CEditableObject::eoHOM));
	    FILL_PROP(values, "Flags\\Use LOD",			&O->m_dwFlags, 			PROP::CreateFlagValue(CEditableObject::eoUsingLOD));

		O->FillPropSummary(values);

	    FILL_PROP(values, "Transform\\Position",	&S->FPosition, 	PROP::CreateVectorValue(-10000,	10000,0.01,2,OnAfterTransformation));
    	FILL_PROP(values, "Transform\\Rotation",	&S->FRotation, 	PROP::CreateVectorValue(-10000,	10000,0.1,1,RotateOnAfterEdit,RotateOnBeforeEdit,RotateOnDraw));
	    FILL_PROP(values, "Transform\\Scale",		&S->FScale, 	PROP::CreateVectorValue(0.01,	10000,0.01,2,OnAfterTransformation));

        m_BasicProp->AssignValues(values,true);
    }else{
    	m_BasicProp->ClearProperties();
    }
    m_BasicProp->EndFillMode(true);
}
//---------------------------------------------------------------------------

void TfrmPropertiesEObject::FillSurfProps()
{
	// surfaces
	CSceneObject* 		S = m_pEditObject;
    if (S->GetReference()){
    	CEditableObject* 	O = S->GetReference();
        PropValueVec values;
        O->FillPropSurf		(values,OnAfterShaderEdit,OnAfterTextureEdit);
        m_SurfProp->AssignValues(values,true);
    }else{
    	m_SurfProp->ClearProperties();
    }
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
    TProperties::DestroyForm(m_BasicProp);
    TProperties::DestroyForm(m_SurfProp);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEObject::fsStorageRestorePlacement(
      TObject *Sender)
{
	m_BasicProp->RestoreColumnWidth(fsStorage);
	m_SurfProp->RestoreColumnWidth(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEObject::fsStorageSavePlacement(
      TObject *Sender)
{
	m_BasicProp->SaveColumnWidth(fsStorage);
	m_SurfProp->SaveColumnWidth(fsStorage);
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
        	case PROP_A_TEXTURE:
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
        FOLDER::RestoreSelection(m_SurfProp->tvProperties,surf->_Name());
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


