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

void TfrmPropertiesEObject::FillBasicProps()
{
	// basic
	CSceneObject* 		S = m_pEditObject;
	m_BasicProp->BeginFillMode();
    if (S->GetReference()){
    	CEditableObject* 	O = S->GetReference();
        PropValueVec values;

	    FILL_PROP(values, "Reference Name",			(LPVOID)S->GetRefName(),PHelper.CreateMarker());
	    FILL_PROP(values, "Flags\\Dynamic",			&O->m_dwFlags, 			PHelper.CreateFlag(CEditableObject::eoDynamic));
	    FILL_PROP(values, "Flags\\HOM",	   			&O->m_dwFlags, 			PHelper.CreateFlag(CEditableObject::eoHOM));
	    FILL_PROP(values, "Flags\\Use LOD",			&O->m_dwFlags, 			PHelper.CreateFlag(CEditableObject::eoUsingLOD));

	    FILL_PROP(values, "Transform\\Position",	&S->FPosition, 			PHelper.CreateVector(-10000,	10000,0.01,2,0,0,0,	0,OnChangeTransform));
    	FILL_PROP(values, "Transform\\Rotation",	&S->FRotation, 			PHelper.CreateVector(-10000,	10000,0.1,1,		0,PHelper.FvectorRDOnAfterEdit,PHelper.FvectorRDOnBeforeEdit,PHelper.FvectorRDOnDraw,OnChangeTransform));
	    FILL_PROP(values, "Transform\\Scale",		&S->FScale, 			PHelper.CreateVector(0.01,		10000,0.01,2,0,0,0,	0,OnChangeTransform));
//	    FILL_PROP(values, "Transform\\Position",	&O->t_vPosition, 		PHelper.CreateVector(-10000,	10000,0.01,2,0,0,0,OnChangeTransform));
//    	FILL_PROP(values, "Transform\\Rotation",	&O->t_vRotate, 			PHelper.CreateVector(-10000,	10000,0.1,1,RotateOnAfterEdit,RotateOnBeforeEdit,RotateOnDraw,OnChangeTransform));
//		FILL_PROP(values, "Transform\\Scale",		&O->t_vScale, 			PHelper.CreateVector(0.01,	10000,0.01,2,0,0,0,OnChangeTransform));

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
        FHelper.RestoreSelection(m_SurfProp->tvProperties,s_name.c_str());
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
/*	// внести изменения в едит об для последующего сохранения
	CSceneObject* 		S = m_pEditObject;
	CEditableObject* 	O = S->GetReference(); R_ASSERT(O);
    S->FPosition.set	(O->t_vPosition);
    S->FRotation.set	(O->t_vRotate);
    S->FScale.set		(O->t_vScale);
*/
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
	// check window position
	UI.CheckWindowPos(this);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesEObject::FormHide(TObject *Sender)
{
	tmIdle->Enabled = false;
}
//---------------------------------------------------------------------------

