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
	m_bNeedRereshShaders = false;
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

void __fastcall TfrmPropertiesEObject::RotateOnAfterEdit(PropValue* sender, LPVOID edit_val)
{
	Fvector* V = (Fvector*)edit_val;
	V->x = deg2rad(V->x);
	V->y = deg2rad(V->y);
	V->z = deg2rad(V->z);
	UI.RedrawScene();
}

void __fastcall TfrmPropertiesEObject::RotateOnBeforeEdit(PropValue* sender, LPVOID edit_val)
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

	    FILL_PROP(values, "Reference Name",			(LPVOID)S->GetRefName(),PROP::CreateMarker());
	    FILL_PROP(values, "Flags\\Dynamic",			&O->m_dwFlags, 			PROP::CreateFlag(CEditableObject::eoDynamic));
	    FILL_PROP(values, "Flags\\HOM",	   			&O->m_dwFlags, 			PROP::CreateFlag(CEditableObject::eoHOM));
	    FILL_PROP(values, "Flags\\Use LOD",			&O->m_dwFlags, 			PROP::CreateFlag(CEditableObject::eoUsingLOD));

	    FILL_PROP(values, "Transform\\Position",	&S->FPosition, 	PROP::CreateVector(-10000,	10000,0.01,2,0,0,0,OnChangeTransform));
    	FILL_PROP(values, "Transform\\Rotation",	&S->FRotation, 	PROP::CreateVector(-10000,	10000,0.1,1,RotateOnAfterEdit,RotateOnBeforeEdit,RotateOnDraw));
	    FILL_PROP(values, "Transform\\Scale",		&S->FScale, 	PROP::CreateVector(0.01,	10000,0.01,2,0,0,0,OnChangeTransform));

		O->FillPropSummary(0,values);

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
    PropValueVec values;
    if (S->GetReference()){
    	CEditableObject* 	O = S->GetReference();
        O->FillPropSurf		(0,values,OnChangeShader);
    }
    m_SurfProp->AssignValues(values,true);
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
	if (item&&item->Tag){
    	EPropType type		= TProperties::GetItemType(item);
    	switch (type){
        	case PROP_A_TEXTURE:{
            	LPCSTR nm = TProperties::GetItemColumn(item,0);
            	if (nm&&nm[0]){
	                m_Thumbnail = new EImageThumbnail(nm,EImageThumbnail::EITTexture);
                    lbWidth->Caption 	= m_Thumbnail->_Width();
                    lbHeight->Caption 	= m_Thumbnail->_Height();
                    lbAlpha->Caption 	= (m_Thumbnail->_Alpha())?"present":"absent";
                    if (m_Thumbnail->_Width()!=m_Thumbnail->_Height()) paImage->Repaint();
                    pbImagePaint(item);
                }
            }break;
        }
    }
    if (!m_Thumbnail){
        lbWidth->Caption 		= "...";
        lbHeight->Caption 		= "...";
        lbAlpha->Caption 		= "...";
        paImage->Repaint();
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
        AnsiString s_name = AnsiString("Surfaces\\")+AnsiString(surf->_Name());
        FOLDER::RestoreSelection(m_SurfProp->tvProperties,s_name.c_str());
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEObject::OnChangeShader(PropValue* sender)
{
	RefreshShaders();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEObject::OnChangeTransform(PropValue* sender)
{
	UI.RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEObject::tmIdleTimer(TObject *Sender)
{
	if (m_bNeedRereshShaders){
	    m_pEditObject->GetReference()->OnDeviceDestroy();
        m_bNeedRereshShaders = false;
        UI.RedrawScene();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEObject::FormShow(TObject *Sender)
{
	tmIdle->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEObject::FormHide(TObject *Sender)
{
	tmIdle->Enabled = false;
}
//---------------------------------------------------------------------------

