#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FrameDPatch.h"
#include "sceneobject.h"
#include "scene.h"
#include "sceneclasslist.h"
#include "dpatch.h"
#include "propertiesglow.h"
#include "ui_main.h"
#include "texture.h"
#include "Cursor3D.h"
#include "Shader.h"
#include "Texture.h"
#include "ChoseForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma link "RXCtrls"
#pragma link "multi_edit"
#pragma link "Placemnt"
#pragma resource "*.dfm"

AnsiString TfraDPatch::sTexture = "...";
AnsiString TfraDPatch::sShader  = "...";
//---------------------------------------------------------------------------
__fastcall TfraDPatch::TfraDPatch(TComponent* Owner)
        : TFrame(Owner)
{
	lbDPTexture->Caption = sTexture;
    lbDPShader->Caption = sShader;
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
}
//---------------------------------------------------------------------------

void __fastcall TfraDPatch::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraDPatch::TopClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraDPatch::DrawTexture(ETextureCore* tex, TPaintBox* pb, TPanel* pa)
{
    if (tex){
        RECT r; r.left = 1; r.top = 1;
        float w, h;
        w = tex->width();
        h = tex->height();
        if (w>h){   r.right = pb->Width; r.bottom = h/w*pb->Height;
        }else{      r.right = h/w*pb->Width; r.bottom = pb->Height;}
        tex->StretchThumbnail(pa->Handle, &r);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraDPatch::pbImagePaint(TObject *Sender)
{
    if (sTexture!="..."){
        ETextureCore* tx = new ETextureCore(sTexture.c_str());
	    DrawTexture(tx,pbImage,paImage);
        _DELETE(tx);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraDPatch::ebSelectShaderClick(TObject *Sender)
{
    bool bValidShader 	= (sShader!="...");
	LPCSTR S = TfrmChoseItem::SelectShader(true,0,bValidShader?sShader.c_str():0);
    if (S){
    	sShader = S;
    	lbDPShader->Caption=S;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraDPatch::ebSelectTextureClick(TObject *Sender)
{
    bool bValidTex 		= (sTexture!="...");
	LPCSTR S = TfrmChoseItem::SelectTexture(false,bValidTex?0:sTexture.c_str());
    if (S){
        ETextureCore* new_tex = new ETextureCore(S);
        if (new_tex->Valid()){
            if ((new_tex->width()<=64)&&((new_tex->height()<=64))&&(new_tex->alpha())){
		    	sTexture=S;
                lbDPTexture->Caption = sTexture;
                pbImagePaint(Sender);
            }else{
                if (!new_tex->alpha()) Log->DlgMsg(mtError, "Texture must contains alpha channel.");
                else                   Log->DlgMsg(mtError, "Texture width and height must <= 64");
            }
        }else{
            Log->DlgMsg(mtError, "Texture can't load!");
			sTexture="...";
	    	lbDPTexture->Caption=sTexture;
        }
		_DELETE(new_tex);
    }
}
//---------------------------------------------------------------------------

