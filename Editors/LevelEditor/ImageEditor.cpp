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
#include "xr_tokens.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElTree"
#pragma link "ElXPThemedControl"
#pragma link "ExtBtn"
#pragma link "mxPlacemnt"
#pragma resource "*.dfm"
TfrmImageLib* TfrmImageLib::form = 0;
FileMap TfrmImageLib::texture_map;
FileMap TfrmImageLib::modif_map;
FileMap TfrmImageLib::compl_map;
AnsiString TfrmImageLib::m_LastSelection="";
//---------------------------------------------------------------------------
__fastcall TfrmImageLib::TfrmImageLib(TComponent* Owner)
    : TForm(Owner)
{
    DEFINE_INI(fsStorage);
    bCheckMode = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::FormCreate(TObject *Sender)
{
	ImageProps = TfrmProperties::CreateProperties(paProperties,alClient);
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::EditImageLib(AnsiString& title, bool bCheck){
	if (!form){
        form = new TfrmImageLib(0);
        form->Caption = title;
	    form->bCheckMode = bCheck;
        compl_map.clear();

        if (form->bCheckMode) 	form->ebClose->Caption = "Update&&Close";
    	else        			ImageManager.GetTextures(texture_map);
		form->modif_map.clear();
        form->m_Thm = 0;
        form->m_SelectedName = "";

        form->paTextureCount->Caption = AnsiString(" Images in list: ")+AnsiString(texture_map.size());

        // scene locking
        Scene.lock();
    }

    form->Show();
    UI.RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::CheckImageLib(){
	texture_map.clear();
    if (ImageManager.GetModifiedTextures(texture_map)){
//		ImageManager.SynchronizeTextures(true,false,false,&texture_map,0);
    	EditImageLib(AnsiString("Check image params"),true);
    }
}

bool __fastcall TfrmImageLib::HideImageLib(){
	if (form){
    	form->Close();
    	texture_map.clear();
		modif_map.clear();
    }
    return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::FormShow(TObject *Sender)
{
    InitItemsList();
    UI.BeginEState(esEditImages);

    TElTreeItem *node=0;
    if (m_LastSelection.IsEmpty()) node = tvItems->Items->GetFirstNode();
    else node = FOLDER::FindObject(tvItems,m_LastSelection.c_str());
    if (node){
        tvItems->Selected = node;
        tvItems->EnsureVisible(node);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::FormClose(TObject *Sender, TCloseAction &Action)
{
	if (!form) return;
    form->Enabled = false;

    SaveTextureParams();
    if (bCheckMode&&!texture_map.empty()){
    	LPSTRVec modif;
		ImageManager.SynchronizeTextures(true,true,true,&texture_map,&modif);
    	Device.RefreshTextures(&modif);
		ImageManager.FreeModifVec(modif);
    }else{
	    // save game textures
        if (modif_map.size()){
            LPSTRVec modif;
            ImageManager.SynchronizeTextures(true,true,true,&modif_map,&modif);
            Device.RefreshTextures(&modif);
            ImageManager.FreeModifVec(modif);
        }
	}

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
	FilePairIt it = texture_map.begin();
	FilePairIt _E = texture_map.end();
    for (; it!=_E; it++)
        FOLDER::AppendObject(tvItems,it->first.c_str());

    // redraw
    if (nm)
		tvItems->Selected = tvItems->Items->LookForItem(0,nm,0,0,false,true,false,false,true);

	tvItems->IsUpdating = false;
}

//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key==VK_ESCAPE){
    	ebClose->Click();
    	Key = 0; // :-) нужно для того чтобы AccessVoilation не вылазил по ESCAPE
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmImageLib::ebCloseClick(TObject *Sender)
{
    HideImageLib();
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::SaveTextureParams(){
	if (m_Thm&&(ImageProps->IsModified()||bCheckMode)){
        m_Thm->Save();
	    FilePairIt it=texture_map.find(m_SelectedName); R_ASSERT(it!=texture_map.end());
        modif_map.insert(*it);
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
        m_Thm = new EImageThumbnail(m_SelectedName.c_str(),EImageThumbnail::EITTexture,true,bCheckMode);
        if (!m_Thm->Valid()){
        	pbImage->Repaint();
            ImageProps->ClearProperties();
            lbFileName->Caption 	= "/Error/";
            lbInfo->Caption			= "/Error/";
        }else{
        	pbImagePaint(Sender);
            lbFileName->Caption 	= "\""+ChangeFileExt(m_SelectedName,"")+"\"";
            AnsiString temp; 		temp.sprintf("%d x %d x %s",m_Thm->_Width(),m_Thm->_Height(),m_Thm->_Format().HasAlpha()?"32b":"24b");
            if (!compl_map.empty()){
				FilePairIt it = compl_map.find(m_SelectedName);
                if (it!=compl_map.end()){
	                int A			= it->second/1000;
    	            int M			= it->second-A*1000;
	    	        AnsiString t2; 	t2.sprintf(" [A:%d%% M:%d%%]",A,M);
            		temp			+= t2;
                }
            }
            lbInfo->Caption			= temp;
            // set UI
            STextureParams& fmt 	= m_Thm->_Format();

            char key[255];
            TElTreeItem* M=0;

            ImageProps->BeginFillMode("Image properties");
            ImageProps->AddItem(0,PROP_TOKEN,"Format",			ImageProps->MakeTokenValue(&fmt.fmt,tfmt_token));
            M = ImageProps->AddItem(0,PROP_MARKER,"MipMaps");
            ImageProps->AddItem(M,PROP_FLAG,"Enabled",			ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flGenerateMipMaps));
            ImageProps->AddItem(M,PROP_TOKEN,"Filter",			ImageProps->MakeTokenValue(&fmt.mip_filter,tparam_token));
            M = ImageProps->AddItem(0,PROP_MARKER,"Fade");
            ImageProps->AddItem(M,PROP_FLAG,"Color Enabled",	ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flFadeToColor));
            ImageProps->AddItem(M,PROP_FLAG,"Alpha Enabled",	ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flFadeToAlpha));
            ImageProps->AddItem(M,PROP_INTEGER,"Amount",		ImageProps->MakeIntValue(&fmt.fade_amount,0,1000,1));
            ImageProps->AddItem(M,PROP_COLOR,"Color",			&fmt.fade_color);
            M = ImageProps->AddItem(0,PROP_MARKER,"Border");
            ImageProps->AddItem(M,PROP_FLAG,"Color Enabled",	ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flColorBorder));
            ImageProps->AddItem(M,PROP_FLAG,"Alpha Enabled",	ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flAlphaBorder));
            ImageProps->AddItem(M,PROP_COLOR,"Color",			&fmt.border_color);
            M = ImageProps->AddItem(0,PROP_MARKER,"Flags");
            ImageProps->AddItem(M,PROP_FLAG,"Binary Alpha",		ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flBinaryAlpha));
            ImageProps->AddItem(M,PROP_FLAG,"Normal Map",		ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flNormalMap));
            ImageProps->AddItem(M,PROP_FLAG,"Du Dv Map",		ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flDuDvMap));
            ImageProps->AddItem(M,PROP_FLAG,"Dither",			ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flDitherColor));
            ImageProps->AddItem(M,PROP_FLAG,"Dither Each MIP",	ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flDitherEachMIPLevel));
            ImageProps->AddItem(M,PROP_FLAG,"Grayscale",		ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flGreyScale));
            ImageProps->AddItem(M,PROP_FLAG,"Implicit Lighted",	ImageProps->MakeFlagValue(&fmt.flag,STextureParams::flImplicitLighted));
            ImageProps->EndFillMode();

            m_LastSelection = m_SelectedName;
        }
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

void __fastcall TfrmImageLib::fsStorageRestorePlacement(TObject *Sender)
{
	ImageProps->RestoreColumnWidth(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::fsStorageSavePlacement(TObject *Sender)
{
	ImageProps->SaveColumnWidth(fsStorage);
}
//---------------------------------------------------------------------------


void __fastcall TfrmImageLib::tvItemsKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
//
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ebCheckAllComplianceClick(TObject *Sender)
{
    ImageManager.CheckCompliance(texture_map,compl_map);
	tvItemsItemFocused(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ebCheckSelComplianceClick(TObject *Sender)
{
	if (tvItems->Selected){
    	int compl=0;
        AnsiString 	fname=m_SelectedName;
        Engine.FS.m_Textures.Update(fname);
	    if (ImageManager.CheckCompliance(fname.c_str(),compl)){
	    	compl_map[m_SelectedName.c_str()] = compl;
            tvItemsItemFocused(Sender);
        }else{
	    	ELog.DlgMsg(mtError,"Some error found in check.");
        }
    }else{
    	ELog.DlgMsg(mtError,"At first select item!");
    }
}
//---------------------------------------------------------------------------

