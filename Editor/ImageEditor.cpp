//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ImageEditor.h"
#include "UI_Main.h"
#include "scene.h"
#include "main.h"
#include "texture.h"
#include "previewimage.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Placemnt"
#pragma link "multi_color"
#pragma resource "*.dfm"
TfrmImageLib* TfrmImageLib::form = 0;
AStringVec TfrmImageLib::check_tex_list;
//---------------------------------------------------------------------------
__fastcall TfrmImageLib::TfrmImageLib(TComponent* Owner)
    : TForm(Owner)
{
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
	sel_tex = 0;
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

bool CheckVersion(const AnsiString& f){
	// thm
	AnsiString thm_name, dds_name, tex_name=f;
    FS.m_Textures.Update(tex_name);
    thm_name = ChangeFileExt(f,".thm");
    FS.m_TexturesThumbnail.Update(thm_name);
    int f1 = FS.CompareFileAge(tex_name,thm_name);
    // dds
    dds_name = ChangeFileExt(f,".dds");
    FS.m_GameTextures.Update(dds_name);
    int f2 = FS.CompareFileAge(tex_name,dds_name);
    return (f1==1)&&(f2==1);
}

//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::CheckImageLib(){
	check_tex_list.clear();
    AnsiString mask = "*.tga";
    FS.m_Textures.Update(mask);
    LPCSTR T=0;
    if (T=FS.FindFirst(mask.c_str())){
        do{
        	if (CheckVersion(T)) continue;
            check_tex_list.push_back(ChangeFileExt(T,""));
        }while(T=FS.FindNext());
    }
    mask = "*.bmp";
    FS.m_Textures.Update(mask);
    if (T=FS.FindFirst(mask.c_str())){
        do{
        	if (CheckVersion(T)) continue;
            check_tex_list.push_back(ChangeFileExt(T,""));
        }while(T=FS.FindNext());
    }
    if (check_tex_list.size())
    	EditImageLib(AnsiString("Check image params"),true);
}

void __fastcall TfrmImageLib::HideImageLib(){
	if (form){ 
    	form->Close();
    	check_tex_list.clear();
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::OnRender(){
	if (!Visible()) return;
}

//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::OnIdle(){
	if (!Visible()) return;
}
//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::FormShow(TObject *Sender)
{
    InitItemsList();
    UI->BeginEState(esEditImages);
	bSetMode = false;

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
	UI->Command(COMMAND_REFRESH_TEXTURES);
    SaveExportParams();
    if (bCheckMode&&!check_tex_list.empty()){
        AnsiString info;
        UI->ProgressStart(check_tex_list.size()*2,"");
        for (AStringIt s_it=check_tex_list.begin(); s_it!=check_tex_list.end(); s_it++){
            info.sprintf("Making '%s'...",s_it->c_str());
            UI->SetStatus(info.c_str());
            UI->ProgressInc();
            ETextureCore* tex = new ETextureCore(s_it->c_str());
            tex->SaveTextureParams();
            tex->UpdateTexture();
            UI->ProgressInc();
            _DELETE(tex);
        }
        UI->ProgressEnd();
    }

    UI->Command(COMMAND_REFRESH_TEXTURES,false);
    
	_DELETE(sel_tex);
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
        if (!node->Data && (node->Text == s)) return node;
    return 0;
}
//---------------------------------------------------------------------------
TElTreeItem* TfrmImageLib::FindItem(const char* s)
{
    for ( TElTreeItem* node = tvItems->Items->GetFirstNode(); node; node = node->GetNext())
        if (node->Data && (node->Text == s)) return node;
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
	SendMessage(tvItems->Handle,WM_SETREDRAW,0,0);
    tvItems->Selected = 0;
    tvItems->Items->Clear();
    // fill
	TElTreeItem* fld = form->AddFolder("Pictures");
    if (bCheckMode||check_tex_list.size()){
    	for (AStringIt s=check_tex_list.begin(); s!=check_tex_list.end(); s++)
            form->AddItem(fld,s->c_str());
    }else{
        AnsiString mask = "*.tga";
        FS.m_Textures.Update(mask);
        LPCSTR T=0;
        if (T=FS.FindFirst(mask.c_str())){
            do form->AddItem(fld,T);
            while(T=FS.FindNext());
        }
        mask = "*.bmp";
        FS.m_Textures.Update(mask);
        if (T=FS.FindFirst(mask.c_str())){
            do form->AddItem(fld,T);
            while(T=FS.FindNext());
        }
    }
    fld->Expand(true);
    
    // redraw
	SendMessage(tvItems->Handle,WM_SETREDRAW,1,0);
	tvItems->Repaint();
    if (nm)
		tvItems->Selected = tvItems->Items->LookForItem(0,nm,0,0,false,true,false,false,true);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
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

IC DWORD GetPowerOf2Plus1(DWORD v){
	DWORD cnt=0;
	while (v) {v>>=1; cnt++; };
	return cnt;
}

void __fastcall TfrmImageLib::SaveExportParams(){
    if (bModified){
        STextureParams* fmt				= sel_tex->GetTextureParams();
        fmt->fmt 						= STextureParams::ETFormat(rgSaveFormat->ItemIndex);
        fmt->flag 						= 0;
        switch (rgMIPmaps->ItemIndex){
        case 0: fmt->flag |= EF_GENMIPMAP; 			break;
        case 1: fmt->flag |= EF_USE_EXIST_MIPMAP; 	break;
        case 2: fmt->flag |= EF_NO_MIPMAP; 			break;
        }
        if (cbDither->Checked) 			fmt->flag |= EF_DITHER;
        if (cbBinaryAlpha->Checked) 	fmt->flag |= EF_BINARYALPHA;
        if (cbAlphaZeroBorder->Checked) fmt->flag |= EF_ALPHAZEROBORDER;
        if (cbNormalMap->Checked) 		fmt->flag |= EF_NORMALMAP;
        if (cbAllowFade->Checked) 		fmt->flag |= EF_FADE;
        if (cbImplicitLighted->Checked)	fmt->flag |= EF_IMPLICIT_LIGHTED;
        Fcolor C; C.set_windows			(mcFadeColor->Brush->Color);
        fmt->fade_color					= C.get();
        fmt->fade_mips					= cbNumFadeMips->ItemIndex;
        sel_tex->SaveTextureParams		();
        bModified						= false;
        if (!bCheckMode) sel_tex->UpdateTexture();
    }
}

void __fastcall TfrmImageLib::tvItemsItemSelectedChange(TObject *Sender,
      TElTreeItem *Item)
{
	if (Item==tvItems->Selected) return;
	if (Item&&Item->Data){
        AnsiString nm = ChangeFileExt(Item->Text,"");
		// save previous data
        if (sel_tex){
        	SaveExportParams();
			_DELETE(sel_tex);
        }
        // get new texture
        sel_tex = new ETextureCore(nm.c_str());
        sel_tex->UpdateThumbnail();
        if (!sel_tex->Valid())	pbImage->Repaint();
        else	 				pbImagePaint(Sender);
        lbFileName->Caption 	= "\""+Item->Text+"\"";
		AnsiString temp; 		temp.sprintf("%d x %d x %s",sel_tex->width(),sel_tex->height(),sel_tex->alpha()?"32b":"24b");
        lbInfo->Caption			= temp;
        // set UI
        STextureParams* fmt		= sel_tex->GetTextureParams();
        if (fmt){
        	bSetMode = true;
	        rgSaveFormat->ItemIndex		= fmt->fmt;

            if (fmt->flag&EF_GENMIPMAP) 			rgMIPmaps->ItemIndex = 0;
            else if(fmt->flag&EF_USE_EXIST_MIPMAP) 	rgMIPmaps->ItemIndex = 1;
            else if(fmt->flag&EF_NO_MIPMAP) 		rgMIPmaps->ItemIndex = 2;
	        cbDither->Checked			= fmt->flag&EF_DITHER;
	        cbBinaryAlpha->Checked		= fmt->flag&EF_BINARYALPHA;
	        cbAlphaZeroBorder->Checked	= fmt->flag&EF_ALPHAZEROBORDER;
	        cbNormalMap->Checked		= fmt->flag&EF_NORMALMAP;
	        cbAllowFade->Checked		= fmt->flag&EF_FADE;
            cbImplicitLighted->Checked	= fmt->flag&EF_IMPLICIT_LIGHTED;
            Fcolor C; C.set(fmt->fade_color);
            mcFadeColor->_Set			(C.get_windows());
            cbNumFadeMips->Items->Clear	();
            cbNumFadeMips->Items->Add	("Complete");
            cbNumFadeMips->Items->Add	("None (1)");
            int mip;
            if (sel_tex->width()<sel_tex->height()) mip = GetPowerOf2Plus1(sel_tex->width())-1;
            else									mip = GetPowerOf2Plus1(sel_tex->height())-1;
            for (int i=2; i<mip; i++) cbNumFadeMips->Items->Add(i);
            cbNumFadeMips->ItemIndex 	= fmt->fade_mips;
        	bSetMode = false;
        }
    }else{
		_DELETE(sel_tex);
		lbFileName->Caption	= "...";
		lbInfo->Caption		= "...";
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::pbImagePaint(TObject *Sender)
{
    if (sel_tex){
        RECT r; 
        r.left = 2; r.top = 2;
        float w, h;
        w = sel_tex->width();
        h = sel_tex->height();
        if (w!=h)	pbImage->Canvas->FillRect(pbImage->BoundsRect);
        if (w>h){   r.right = pbImage->Width-1; r.bottom = h/w*pbImage->Height-1;
        }else{      r.right = w/h*pbImage->Width-1; r.bottom = pbImage->Height-1;}
        sel_tex->StretchThumbnail(paImage->Handle, &r);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::rgChangeClick(TObject *Sender)
{
	if (bSetMode) return;
	bModified = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::mcFadeColorMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	DWORD color = ((TMultiObjColor*)Sender)->Brush->Color;
	if (SelectColorWin(&color,&color)){ 
    	((TMultiObjColor*)Sender)->_Set(color);
		if (!bSetMode) bModified = true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::pbImageDblClick(TObject *Sender)
{
	if (sel_tex) TfrmPreviewImage::Run(sel_tex);
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ebConvertClick(TObject *Sender)
{
	if (!sel_tex) return;
	SaveExportParams();
	AnsiString fn = sel_tex->name();
	if(FS.GetSaveName(&FS.m_GameTextures,fn)){
    	UI->ProgressStart(2,"Convert texture...");
        UI->ProgressInc();
    	if(!sel_tex->SaveAsDDS(fn.c_str())){
        	Log->DlgMsg(mtError,"Can't save picture.");
        }else{
	        UI->ProgressInc();
        	Log->DlgMsg(mtInformation,"Picture %s succesfully converted.",sel_tex->name());
        }
        UI->ProgressEnd();
    }
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

