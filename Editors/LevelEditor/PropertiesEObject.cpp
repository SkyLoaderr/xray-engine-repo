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

void TfrmPropertiesEObject::FillBasicProps()
{
	// basic
	CSceneObject* 		S = m_pEditObject;
	PropItemVec items;
    if (S->GetReference()){
    	CEditableObject* 	O = S->GetReference();
        O->FillBasicProps	(0,items);
    }
	m_BasicProp->AssignItems(items,true);
}
//---------------------------------------------------------------------------

void TfrmPropertiesEObject::FillSurfProps()
{
	// surfaces
	CSceneObject* 		S = m_pEditObject;
    PropItemVec values;
    if (S->GetReference()){
    	CEditableObject* 	O = S->GetReference();
        O->FillPropSurf		(0,values);
    }
    m_SurfProp->AssignItems(values,true);
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

void __fastcall TfrmPropertiesEObject::FormShow(TObject *Sender)
{
	// check window position
	UI.CheckWindowPos(this);
}
//---------------------------------------------------------------------------


