//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ImageEditor.h"
#include "UI_Main.h"
#include "scene.h"
#include "main.h"
#include "texture.h"
#include "previewimage.h"
#include "ImageThumbnail.h"
#include "ImageManager.h"
#include "PropertiesList.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmImageLib* TfrmImageLib::form = 0;
AStringVec TfrmImageLib::check_tex_list;
//---------------------------------------------------------------------------
__fastcall TfrmImageLib::TfrmImageLib(TComponent* Owner)
    : TForm(Owner)
{
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
    bCheckMode = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::EditImageLib(AnsiString& title, bool bCheck){
	if (!form){
        form = new TfrmImageLib(0);
        form->Caption = title;
	    form->bCheckMode = bCheck;

        if (form->bCheckMode) form->ebClose->Caption = "Update&&Close";

        // scene locking
        Scene->lock();
    }

    form->Show();
    UI->RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::CheckImageLib(){
	check_tex_list.clear();

    int count = strlen(FS.m_Textures.m_Path);
    AStringVec& lst = FS.GetFiles(FS.m_Textures.m_Path);
    AnsiString thm=FS.m_TexturesThumbnail.m_Path;
    for (AStringIt it=lst.begin(); it!=lst.end(); it++){
        if (it->Pos(thm)==1) continue;
        AnsiString t = it->Delete(1,count);
		if (ImageManager.IfChanged(t.c_str())) check_tex_list.push_back(t);
    }
/*


    AnsiString mask = "*.tga";
    FS.m_Textures.Update(mask);
    LPCSTR T=0;
    if (T=FS.FindFirst(mask.c_str())){
        do{
        	if (ImageManager.IfChanged(T)) continue;
            check_tex_list.push_back(T);
        }while(T=FS.FindNext());
    }
    mask = "*.bmp";
    FS.m_Textures.Update(mask);
    if (T=FS.FindFirst(mask.c_str())){
        do{
        	if (ImageManager.IfChanged(T)) continue;
            check_tex_list.push_back(T);
        }while(T=FS.FindNext());
    }
*/    
    if (check_tex_list.size())
    	EditImageLib(AnsiString("Check image params"),true);
}

bool __fastcall TfrmImageLib::HideImageLib(){
	if (form){
    	form->Close();
    	check_tex_list.clear();
    }
    return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::FormShow(TObject *Sender)
{
    ImageProps = new TfrmProperties(paProperties);
	ImageProps->Parent = paProperties;
    ImageProps->Align = alClient;
    ImageProps->BorderStyle = bsNone;
    ImageProps->tvProperties->HeaderSections->Item[0]->Width = 101;
    ImageProps->tvProperties->HeaderSections->Item[1]->Width = 65;
    ImageProps->ShowProperties();

    InitItemsList();
    UI->BeginEState(esEditImages);

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
    SaveTextureParams();
    if (bCheckMode&&!check_tex_list.empty()){
        AnsiString info;
        UI->ProgressStart(check_tex_list.size(),"");
        for (AStringIt s_it=check_tex_list.begin(); s_it!=check_tex_list.end(); s_it++){
            info.sprintf("Making '%s'...",s_it->c_str());
            ImageManager.Synchronize(s_it->c_str());
            UI->ProgressInc();
        }
        UI->ProgressEnd();
    }
    UI->Command(COMMAND_REFRESH_TEXTURES,false);

	_DELETE(m_Thm);

	Action = caFree;
	form = 0;

	Scene->unlock();
    UI->EndEState(esEditImages);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Folder routines
//---------------------------------------------------------------------------
TElTreeItem* TfrmImageLib::FindFolder(const char* s)
{
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext())
        if (!node->Data && (AnsiString(node->Text) == s)) return node;
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmImageLib::FindItem(const char* s)
{
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext())
        if (node->Data && (AnsiString(node->Text) == s)) return node;
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmImageLib::AddFolder(const char* s)
{
    TElTreeItem* node = 0;
    if (s[0]!=0){
        node = tvItems->Items->AddObject(0,s,0);
        node->ParentStyle = false;
        node->Bold = true;
    }
    return node;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmImageLib::AddItem(TElTreeItem* node, const char* name)
{
    TElTreeItem* obj_node = tvItems->Items->AddChildObject(node, name, (void*)1);
    obj_node->ParentStyle = false;
    obj_node->Bold = false;
    return obj_node;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmImageLib::AddItemToFolder(const char* folder, const char* name){
	TElTreeItem* node = FindFolder(folder);
    if (!node) node = AddFolder(folder);
	return AddItem(node,name);
}
//---------------------------------------------------------------------------
void TfrmImageLib::InitItemsList(const char* nm)
{
	tvItems->IsUpdating = true;

    tvItems->Selected = 0;
    tvItems->Items->Clear();
    // fill
    if (bCheckMode||check_tex_list.size()){
    	for (AStringIt s=check_tex_list.begin(); s!=check_tex_list.end(); s++)
            form->AddItem(0,s->c_str());
    }else{
        int count = strlen(FS.m_Textures.m_Path);
        AStringVec& lst = FS.GetFiles(FS.m_Textures.m_Path);
        AnsiString thm=FS.m_TexturesThumbnail.m_Path;
        for (AStringIt it=lst.begin(); it!=lst.end(); it++){
            if (it->Pos(thm)==1) continue;
            form->AddItem(0,(it->Delete(1,count)).c_str());
        }
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
	if (m_Thm&&ImageProps->IsModified())
        m_Thm->Save();
}

void __fastcall TfrmImageLib::tvItemsItemFocused(TObject *Sender)
{
	TElTreeItem* Item = tvItems->Selected;
	if (Item&&Item->Data){
        AnsiString nm = Item->Text;
		// save previous data
        SaveTextureParams();
		_DELETE(m_Thm);
        // get new texture
        m_Thm = new EImageThumbnail(nm.c_str());
        if (!m_Thm->Valid())	pbImage->Repaint();
        else	 				pbImagePaint(Sender);
        lbFileName->Caption 	= "\""+ChangeFileExt(nm,"")+"\"";
		AnsiString temp; 		temp.sprintf("%d x %d x %s",m_Thm->_Width(),m_Thm->_Height(),m_Thm->_Format().HasAlphaChannel()?"32b":"24b");
        lbInfo->Caption			= temp;
        // set UI
        STextureParams& fmt 	= m_Thm->_Format();

        char key[255];
        TElTreeItem* marker_node=0;

        ImageProps->BeginFillMode("Image properties");
        ImageProps->AddItem(0,PROP_TOKEN,"Format",(LPDWORD)&fmt.fmt,(LPDWORD)tfmt_token);
        marker_node = ImageProps->AddItem(0,PROP_MARKER,"Flags");
        ImageProps->AddItem(marker_node,PROP_FLAG,"Generate MipMaps",	(LPDWORD)&fmt.flag,(LPDWORD)STextureParams::flGenerateMipMaps);
        ImageProps->AddItem(marker_node,PROP_FLAG,"Binary Alpha",		(LPDWORD)&fmt.flag,(LPDWORD)STextureParams::flBinaryAlpha);
        ImageProps->AddItem(marker_node,PROP_FLAG,"Normal Map",			(LPDWORD)&fmt.flag,(LPDWORD)STextureParams::flNormalMap);
        ImageProps->AddItem(marker_node,PROP_FLAG,"Du Dv Map",			(LPDWORD)&fmt.flag,(LPDWORD)STextureParams::flDuDvMap);
        ImageProps->AddItem(marker_node,PROP_FLAG,"Border",				(LPDWORD)&fmt.flag,(LPDWORD)STextureParams::flColorBorder);
        ImageProps->AddItem(marker_node,PROP_FLAG,"Alpha Border",		(LPDWORD)&fmt.flag,(LPDWORD)STextureParams::flAlphaBorder);
        ImageProps->AddItem(marker_node,PROP_FLAG,"Fade To Color",		(LPDWORD)&fmt.flag,(LPDWORD)STextureParams::flFadeToColor);
        ImageProps->AddItem(marker_node,PROP_FLAG,"Fade To Alpha",		(LPDWORD)&fmt.flag,(LPDWORD)STextureParams::flFadeToAlpha);
        ImageProps->AddItem(marker_node,PROP_FLAG,"Dither",				(LPDWORD)&fmt.flag,(LPDWORD)STextureParams::flDitherColor);
        ImageProps->AddItem(marker_node,PROP_FLAG,"Implicit Lighted",	(LPDWORD)&fmt.flag,(LPDWORD)STextureParams::flImplicitLighted);
        ImageProps->AddItem(0,PROP_TOKEN,"Mip Filter",		(LPDWORD)&fmt.mip_filter,(LPDWORD)tparam_token);
        ImageProps->AddItem(0,PROP_COLOR,"Border Color",	(LPDWORD)&fmt.border_color);
        ImageProps->AddItem(0,PROP_INTEGER,"Fade Color",	(LPDWORD)&fmt.fade_color);
        ImageProps->AddItem(0,PROP_COLOR,"Fade Amount",		(LPDWORD)&fmt.fade_amount);
        ImageProps->EndFillMode();
    }else{
		ImageProps->Clear();
		_DELETE(m_Thm);
		lbFileName->Caption	= "...";
		lbInfo->Caption		= "...";
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::pbImagePaint(TObject *Sender)
{
    if (m_Thm){
        RECT r;
        r.left = 2; r.top = 2;
        float w, h;
        w = m_Thm->_Width();
        h = m_Thm->_Height();
        if (w!=h)	pbImage->Canvas->FillRect(pbImage->BoundsRect);
        if (w>h){   r.right = pbImage->Width-1; r.bottom = h/w*pbImage->Height-1;
        }else{      r.right = w/h*pbImage->Width-1; r.bottom = pbImage->Height-1;}
        m_Thm->DrawStretch(paImage->Handle, &r);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::pbImageDblClick(TObject *Sender)
{
//S	if (sel_tex) TfrmPreviewImage::Run(sel_tex);
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ebConvertClick(TObject *Sender)
{
/*	if (!sel_tex) return;
	SaveExportParams();
	AnsiString fn = sel_tex->name();
	if(FS.GetSaveName(&FS.m_GameTextures,fn)){
    	UI->ProgressStart(2,"Convert texture...");
        UI->ProgressInc();
    	if(!sel_tex->SaveAsDDS(fn.c_str())){
        	ELog.DlgMsg(mtError,"Can't save picture.");
        }else{
	        UI->ProgressInc();
        	ELog.DlgMsg(mtInformation,"Picture %s succesfully converted.",sel_tex->name());
        }
        UI->ProgressEnd();
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


