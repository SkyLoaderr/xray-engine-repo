//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ImageEditor.h"
#include "UI_Main.h"
#include "scene.h"
#include "ImageThumbnail.h"
#include "ImageManager.h"
#include "PropertiesList.h"
#include "FolderLib.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmImageLib* TfrmImageLib::form = 0;
AStringVec TfrmImageLib::check_tex_list;
AStringVec TfrmImageLib::modif_tex_list;
//---------------------------------------------------------------------------
__fastcall TfrmImageLib::TfrmImageLib(TComponent* Owner)
    : TForm(Owner)
{
    DEFINE_INI(fsStorage);
    bCheckMode = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::EditImageLib(AnsiString& title, bool bCheck){
	if (!form){
        form = new TfrmImageLib(0);
        form->Caption = title;
	    form->bCheckMode = bCheck;

        if (form->bCheckMode) form->ebClose->Caption = "Update&&Close";
		form->modif_tex_list.clear();
        form->m_Thm = 0;
        form->m_SelectedName = "";

        // scene locking
        Scene.lock();
    }

    form->Show();
    UI.RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::CheckImageLib(){
	check_tex_list.clear();
    if (ImageManager.GetModifiedFiles(check_tex_list))
    	EditImageLib(AnsiString("Check image params"),true);
}

bool __fastcall TfrmImageLib::HideImageLib(){
	if (form){
    	form->Close();
    	check_tex_list.clear();
		modif_tex_list.clear();
    }
    return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::FormShow(TObject *Sender)
{
	ImageProps = TfrmProperties::CreateProperties(paProperties,alClient);
    ImageProps->tvProperties->HeaderSections->Item[0]->Width = 101;
    ImageProps->tvProperties->HeaderSections->Item[1]->Width = 65;

    InitItemsList();
    UI.BeginEState(esEditImages);

    if (bCheckMode&&check_tex_list.size()){
	    // select first item
    	TElTreeItem* node = tvItems->Items->LookForItem(0,check_tex_list[0],0,0,false,true,false,true,true);
	    if (node){
    		tvItems->Selected = node;
			tvItems->EnsureVisible(node);
    	}
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::FormClose(TObject *Sender, TCloseAction &Action)
{
	if (!form) return;
    form->Enabled = false;

    SaveTextureParams();
    if (bCheckMode&&!check_tex_list.empty()){
        AnsiString info;
        UI.ProgressStart(check_tex_list.size(),"");
        for (AStringIt s_it=check_tex_list.begin(); s_it!=check_tex_list.end(); s_it++){
            info.sprintf("Making '%s'...",s_it->c_str());
            if (UI.NeedAbort()) break;
            ImageManager.Synchronize(s_it->c_str());
            UI.ProgressInc();
        }
        UI.ProgressEnd();
    }else{
	    // save game textures
        UI.ProgressStart(modif_tex_list.size(),"Save modified textures...");
    	for (AStringIt it=modif_tex_list.begin(); it!=modif_tex_list.end(); it++){
			ImageManager.CreateGameTexture(it->c_str());
            UI.ProgressInc();
        }
        UI.ProgressEnd();
	}

    UI.Command(COMMAND_REFRESH_TEXTURES,false);

	_DELETE(m_Thm);
    m_SelectedName = "";

    TfrmProperties::DestroyProperties(ImageProps);

	form = 0;
	Action = caFree;

	Scene.unlock();
    UI.EndEState(esEditImages);
}
//---------------------------------------------------------------------------
void TfrmImageLib::InitItemsList(const char* nm)
{
	tvItems->IsUpdating = true;

    tvItems->Selected = 0;
    tvItems->Items->Clear();
    // fill
    if (bCheckMode||check_tex_list.size()){
    	for (AStringIt it=check_tex_list.begin(); it!=check_tex_list.end(); it++)
        	FOLDER::AppendObject(tvItems,it->c_str());
//            form->AddItem(0,s->c_str());
    }else{
    	AStringVec lst;
    	ImageManager.GetFiles(lst);
        for (AStringIt it=lst.begin(); it!=lst.end(); it++)
        	FOLDER::AppendObject(tvItems,it->c_str());
//            form->AddItem(0,it->c_str());
    }

    // redraw
    if (nm)
		tvItems->Selected = tvItems->Items->LookForItem(0,nm,0,0,false,true,false,false,true);

	tvItems->IsUpdating = false;
}

//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key==VK_ESCAPE) 		ebClose->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ebCloseClick(TObject *Sender)
{
    HideImageLib();
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::SaveTextureParams(){
	if (m_Thm&&ImageProps->IsModified()){
        m_Thm->Save();
        if (find(modif_tex_list.begin(),modif_tex_list.end(),m_SelectedName)==modif_tex_list.end())
        	modif_tex_list.push_back(m_SelectedName);
    }
}

void __fastcall TfrmImageLib::tvItemsItemFocused(TObject *Sender)
{
	TElTreeItem* Item = tvItems->Selected;
	if (Item&&Item->Data){
		// save previous data
        SaveTextureParams();
        // make new name
    	FOLDER::MakeName(Item,0,m_SelectedName,false);
		_DELETE(m_Thm);
        // get new texture
        m_Thm = new EImageThumbnail(m_SelectedName.c_str(),EImageThumbnail::EITTexture);
        if (!m_Thm->Valid())	pbImage->Repaint();
        else	 				pbImagePaint(Sender);
        lbFileName->Caption 	= "\""+ChangeFileExt(m_SelectedName,"")+"\"";
		AnsiString temp; 		temp.sprintf("%d x %d x %s",m_Thm->_Width(),m_Thm->_Height(),m_Thm->_Format().HasAlphaChannel()?"32b":"24b");
        lbInfo->Caption			= temp;
        // set UI
        STextureParams& fmt 	= m_Thm->_Format();

        char key[255];
        TElTreeItem* M=0;

        ImageProps->BeginFillMode("Image properties");
        ImageProps->AddItem(0,PROP_TOKEN,"Format",			ImageProps->MakeTokenValue(&fmt.fmt,tfmt_token));
        M = ImageProps->AddItem(0,PROP_MARKER,"Flags");
        ImageProps->AddItem(M,PROP_FLAG,"Generate MipMaps",	ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flGenerateMipMaps));
        ImageProps->AddItem(M,PROP_FLAG,"Binary Alpha",		ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flBinaryAlpha));
        ImageProps->AddItem(M,PROP_FLAG,"Normal Map",		ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flNormalMap));
        ImageProps->AddItem(M,PROP_FLAG,"Du Dv Map",		ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flDuDvMap));
        ImageProps->AddItem(M,PROP_FLAG,"Border",			ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flColorBorder));
        ImageProps->AddItem(M,PROP_FLAG,"Alpha Border",		ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flAlphaBorder));
        ImageProps->AddItem(M,PROP_FLAG,"Fade To Color",	ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flFadeToColor));
        ImageProps->AddItem(M,PROP_FLAG,"Fade To Alpha",	ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flFadeToAlpha));
        ImageProps->AddItem(M,PROP_FLAG,"Dither",			ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flDitherColor));
        ImageProps->AddItem(M,PROP_FLAG,"Dither Each MIP",	ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flDitherEachMIPLevel));
        ImageProps->AddItem(M,PROP_FLAG,"Grayscale",		ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flGreyScale));
        ImageProps->AddItem(M,PROP_FLAG,"Implicit Lighted",	ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flImplicitLighted));
        ImageProps->AddItem(0,PROP_TOKEN,"Mip Filter",		ImageProps->MakeTokenValue(&fmt.mip_filter,tparam_token));
        ImageProps->AddItem(0,PROP_COLOR,"Border Color",	&fmt.border_color);
        ImageProps->AddItem(0,PROP_INTEGER,"Fade Amount",	ImageProps->MakeIntValue(&fmt.fade_amount,0,100,1));
        ImageProps->AddItem(0,PROP_COLOR,"Fade Color",		&fmt.fade_color);
        ImageProps->EndFillMode();
    }else{
		ImageProps->ClearProperties();
		_DELETE(m_Thm);
        m_SelectedName = "";
		lbFileName->Caption	= "...";
		lbInfo->Caption		= "...";
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::pbImagePaint(TObject *Sender)
{
    if (m_Thm) m_Thm->Draw(paImage,pbImage,true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ebConvertClick(TObject *Sender)
{
/*	if (!sel_tex) return;
	SaveExportParams();
	AnsiString fn = sel_tex->name();
	if(FS.GetSaveName(&FS.m_GameTextures,fn)){
    	UI.ProgressStart(2,"Convert texture...");
        UI.ProgressInc();
    	if(!sel_tex->SaveAsDDS(fn.c_str())){
        	ELog.DlgMsg(mtError,"Can't save picture.");
        }else{
	        UI.ProgressInc();
        	ELog.DlgMsg(mtInformation,"Picture %s succesfully converted.",sel_tex->name());
        }
        UI.ProgressEnd();
    }
*/
}
//---------------------------------------------------------------------------

extern bool __fastcall LookupFunc(TElTreeItem* Item, void* SearchDetails);

void __fastcall TfrmImageLib::tvItemsKeyPress(TObject *Sender, char &Key)
{
	TElTreeItem* node = tvItems->Items->LookForItemEx(tvItems->Selected,-1,false,false,false,&Key,LookupFunc);
    if (!node) node = tvItems->Items->LookForItemEx(0,-1,false,false,false,&Key,LookupFunc);
    if (node){
    	tvItems->Selected = node;
		tvItems->EnsureVisible(node);
    }
}
//---------------------------------------------------------------------------


