#include "stdafx.h"
#pragma hdrstop

#include "PropertiesSound.h"
#include "UI_Main.h"
#include "Sound.h"
#include "SceneClassList.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RXSpin"
#pragma link "RXCtrls"
#pragma link "CloseBtn"
#pragma link "multi_edit"
#pragma resource "*.dfm"
TfrmPropertiesSound *frmPropertiesSound=0;
//---------------------------------------------------------------------------
void frmPropertiesSoundRun(ObjectList* pObjects, bool& bChange){
	frmPropertiesSound = new TfrmPropertiesSound(0);
    frmPropertiesSound->Run(pObjects,bChange);
    _DELETE(frmPropertiesSound);
}
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesSound::TfrmPropertiesSound(TComponent* Owner)
    : TForm(Owner)
{
}
//----------------------------------------------------
void __fastcall TfrmPropertiesSound::Run(ObjectList* pObjects, bool& bChange)
{
    m_Objects = pObjects;
	GetObjectsInfo();
    bChange = (ShowModal()==mrOk);
}
//---------------------------------------------------------------------------

void TfrmPropertiesSound::GetObjectsInfo(){
	VERIFY( !m_Objects->empty() );

	CSound *_L = 0;
	ObjectIt _F = m_Objects->begin();

	VERIFY( (*_F)->ClassID()==OBJCLASS_SOUND );
	_L = (CSound *)(*_F);

	seRange->ObjFirstInit( _L->m_Range );
	_F++;
    edName->Text = _L->m_fName;
    edName->Enabled = true;
    ebLink->Enabled = true;
    ebUnLink->Enabled = true;

    bMultipleSelection = false;
	for(;_F!=m_Objects->end();_F++){

		VERIFY( (*_F)->ClassID()==OBJCLASS_SOUND );
		_L = (CSound *)(*_F);

        bMultipleSelection = true;
		seRange->ObjNextInit( _L->m_Range );
	}
    if (bMultipleSelection){
        edName->Text = "<Multiple selection>";
        edName->Enabled = false;
        ebLink->Enabled = false;
        ebUnLink->Enabled = false;
    }
}

void TfrmPropertiesSound::ApplyObjectsInfo(){
	VERIFY( !m_Objects->empty() );

	CSound *_L = 0;
	ObjectIt _F = m_Objects->begin();

	for(;_F!=m_Objects->end();_F++){

		VERIFY( (*_F)->ClassID()==OBJCLASS_SOUND );
		_L = (CSound *)(*_F);
        if (!bMultipleSelection) strcpy(_L->m_fName, edName->Text.c_str());

		seRange->ObjApplyFloat( _L->m_Range );
	}
    UI->UpdateScene();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesSound::btApplyClick(TObject *Sender)
{
    ApplyObjectsInfo();
    GetObjectsInfo();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSound::btOkClick(TObject *Sender)
{
    ApplyObjectsInfo();
}

void __fastcall TfrmPropertiesSound::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) btCancel->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSound::ebLinkClick(TObject *Sender)
{
    if (!bMultipleSelection){
    	char _FileName[MAX_PATH]="";
        if( FS.GetOpenName( &FS.m_GameSounds, _FileName ) ){
            VERIFY( _FileName );
            strlwr(_FileName);
            char* s=strstr(_FileName,FS.m_GameSounds.m_Path);
            if (s&&((s-_FileName)==0))
                edName->Text = _FileName+strlen(FS.m_GameSounds.m_Path)+1;
            else
                MessageDlg("You can't change folder.\nThe only valid folder for sounds is GAME\\DATA\\SOUNDS\\...", mtError, TMsgDlgButtons() << mbOK, 0);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSound::ebUnLinkClick(TObject *Sender)
{
    if (!bMultipleSelection) edName->Text = "";
}
//---------------------------------------------------------------------------

