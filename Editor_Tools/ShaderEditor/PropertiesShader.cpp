#include "stdafx.h"
#pragma hdrstop

#include "PropertiesShader.h"
#include <ElVCLUtils.hpp>
#include <ElTools.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElTree"
#pragma link "ElXPThemedControl"
#pragma link "ElTreeStdEditors"
#pragma link "ElTreeComboBox"
#pragma link "ElTreeDTPickEdit"
#pragma link "ElTreeModalEdit"
#pragma link "ElTreeAdvEdit"
#pragma link "ElTreeBtnEdit"
#pragma link "ElTreeCheckBoxEdit"
#pragma link "ElTreeCurrEdit"
#pragma link "ElTreeMaskEdit"
#pragma link "ElTreeMemoEdit"
#pragma link "ElTreeSpinEdit"
#pragma link "ElEdits"
#pragma link "ElSpin"
#pragma link "ElBtnEdit"
#pragma link "ElACtrls"
#pragma link "ElClrCmb"
#pragma link "ElTreeTreeComboEdit"
#pragma link "ElTreeCombo"
#pragma link "ElMenus"
#pragma link "ElCombos"
#pragma link "MxMenus"
#pragma link "FrameProperties"
#pragma resource "*.dfm"

TfrmShaderProperties* TfrmShaderProperties::form=0;
//---------------------------------------------------------------------------
__fastcall TfrmShaderProperties::TfrmShaderProperties(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderProperties::ShowProperties(){
	if (!form){
    	form = new TfrmShaderProperties(0);
    }
	form->Show();
}

void __fastcall TfrmShaderProperties::HideProperties(){
	if (form) form->Close();
}

void __fastcall TfrmShaderProperties::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	form = 0;
	Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderProperties::FormShow(TObject *Sender)
{
    fraProperties->ShowProperties(0);
}
//---------------------------------------------------------------------------

