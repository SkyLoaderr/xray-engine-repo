#include "stdafx.h"
#pragma hdrstop

#include "PropertiesGlow.h"
#include "glow.h"
#include "texture.h"
#include "ChoseForm.h"
#include "xr_trims.h"
#include "ui_main.h"
#include "PropertiesList.h"
#include "ImageThumbnail.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "mxPlacemnt"
#pragma resource "*.dfm"
TfrmPropertiesGlow *frmPropertiesGlow=0;
//---------------------------------------------------------------------------
void frmPropertiesGlowRun(ObjectList* pObjects, bool& bChange){
	frmPropertiesGlow = new TfrmPropertiesGlow(0);
    frmPropertiesGlow->Run(pObjects,bChange);
    _DELETE(frmPropertiesGlow);
}
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesGlow::TfrmPropertiesGlow(TComponent* Owner)
    : TForm(Owner)
{
	m_Props = TProperties::CreateForm(paProps,alClient,OnModified,OnItemFocused);
    DEFINE_INI(fsStorage);
	m_Thumbnail	= 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::Run(ObjectList* pObjects, bool& bChange)
{
    m_Objects = pObjects;
	GetObjectsInfo();
    bChange = (ShowModal()==mrOk);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	if (m_Props->IsFocused()) return;
	if (Key==VK_ESCAPE) ebCancel->Click();
	if (Key==VK_RETURN) ebOk->Click();
}
//----------------------------------------------------

void __fastcall TfrmPropertiesGlow::OnItemFocused(TElTreeItem* item)
{
	_DELETE(m_Thumbnail);
	if (item&&item->Tag){
    	EPropType type		= TProperties::GetItemType(item);
    	switch (type){
        	case PROP_A_TEXTURE:{
            	LPCSTR nm = TProperties::GetItemColumn(item,0);
            	if (nm&&nm[0]){
	                m_Thumbnail = new EImageThumbnail(nm,EImageThumbnail::EITTexture);
    	            if (m_Thumbnail->_Width()!=m_Thumbnail->_Height()) paImage->Repaint();
        	        pbImagePaint(item);
                }
            }break;
        }
    }
    if (!m_Thumbnail){
        paImage->Repaint();
    }
}
//----------------------------------------------------

void __fastcall	TfrmPropertiesGlow::OnShaderChange(PropValue* prop)
{
	ObjectIt _F 		= m_Objects->begin();
	for(;_F!=m_Objects->end();_F++) (*_F)->OnDeviceDestroy();
}

void TfrmPropertiesGlow::GetObjectsInfo()
{
	VERIFY( !m_Objects->empty() );

    PropValueVec values;
	ObjectIt _F 		= m_Objects->begin();
    LPCSTR pref			= GetClassNameByClassID((*_F)->ClassID);
	for(;_F!=m_Objects->end();_F++){
		CGlow *P		= dynamic_cast<CGlow*>(*_F); R_ASSERT(P);
        P->FillProp		(pref,values);
	}
    PropValue* T 		= PHelper.FindProp(values,pref,"Texture"); R_ASSERT(T);
    PropValue* S 		= PHelper.FindProp(values,pref,"Shader"); R_ASSERT(S);
    T->OnChange			= OnShaderChange;
    S->OnChange			= OnShaderChange;
	m_Props->AssignValues(values,true);
}
//---------------------------------------------------------------------------

void TfrmPropertiesGlow::CancelObjectsInfo()
{
	m_Props->ResetValues();
	ObjectIt _F 		= m_Objects->begin();
	for(;_F!=m_Objects->end();_F++) (*_F)->OnDeviceDestroy();
}
//---------------------------------------------------------------------------

void TfrmPropertiesGlow::ApplyObjectsInfo()
{
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::ebOkClick(TObject *Sender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::ebCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::pbImagePaint(TObject *Sender)
{
	if (m_Thumbnail) m_Thumbnail->Draw(paImage,pbImage,true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::OnModified()
{
	ebOk->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    switch (ModalResult){
    case mrOk: 		ApplyObjectsInfo();		break;
    case mrCancel: 	CancelObjectsInfo();	break;
    default: THROW2("Invalid case");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::fsStorageRestorePlacement(
      TObject *Sender)
{
	m_Props->RestoreColumnWidth(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::fsStorageSavePlacement(TObject *Sender)
{
	m_Props->SaveColumnWidth(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesGlow::FormDestroy(TObject *Sender)
{
	_DELETE(m_Thumbnail);
	TProperties::DestroyForm(m_Props);
}
//---------------------------------------------------------------------------

