//---------------------------------------------------------------------------

#include <vcl.h>
#include "pch.h"
#include "ui_tools.h"
#pragma hdrstop

#include "AddHMap.h"
#include "main.h"
#include "ui_main.h"
#include "sceneobject.h"
#include "scene.h"
#include "light.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfraAddHMap *fraAddHMap;
//---------------------------------------------------------------------------
__fastcall TfraAddHMap::TfraAddHMap(TComponent* Owner)
        : TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfraAddHMap::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraAddHMap::sbLoadClick(TObject *Sender)
{
    if (opd->Execute()){
        lbName->Caption = ExtractFileName(opd->FileName);
        imHMap->Picture->LoadFromFile(opd->FileName);
        if (imHMap->Picture->Bitmap->PixelFormat != pf8bit){
            MessageDlg("Incorrect pixel format! Must be 8 bit grayscale.", mtError, TMsgDlgButtons() << mbOK, 0);
            sbClearClick(NULL);
            return;
        }
//        if (imHMap->Picture->Bitmap->Palette != NULL){
//            MessageDlg("Palette ...!", mtError, TMsgDlgButtons() << mbOK, 0);
//            sbClearClick(NULL);
//            return;
//        }
//        imHMap->Picture->Bitmap->Monochrome = true;
        lbSize->Caption = IntToStr(imHMap->Picture->Width)+"x"+IntToStr(imHMap->Picture->Height);
        //..
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraAddHMap::sbClearClick(TObject *Sender)
{
    if (lbName->Caption != "none"){
        lbName->Caption = "none";
        imHMap->Picture->Bitmap->Dormant();
        imHMap->Picture->Bitmap->FreeImage();
        imHMap->Picture->Assign(0);
        lbSize->Caption = "???x???";
    }
}
//---------------------------------------------------------------------------

