//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ImageEditor.h"
#include "ImageThumbnail.h"
#include "ImageManager.h"
#include "PropertiesList.h"
#include "FolderLib.h"
#include "xr_tokens.h"
#include "ui_main.h"
#include "xr_ini.h"
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
bool TfrmImageLib::bFormLocked=false;
//---------------------------------------------------------------------------
__fastcall TfrmImageLib::TfrmImageLib(TComponent* Owner)
    : TForm(Owner)
{
    DEFINE_INI(fsStorage);
    bImportMode = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::FormCreate(TObject *Sender)
{
	ImageProps = TProperties::CreateForm(paProperties,alClient);
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::EditImageLib(AnsiString& title, bool bImport){
	if (!form){
        form 				= new TfrmImageLib(0);
        form->Caption 		= title;
	    form->bImportMode 	= bImport;
        form->ebRemoveTexture->Enabled = !bImport;
        compl_map.clear		();

        if (!form->bImportMode) ImageManager.GetTextures(texture_map);
		form->modif_map.clear	();
        form->m_Thm 			= 0;
        form->m_SelectedName 	= "";

        form->paTextureCount->Caption = AnsiString(" Images in list: ")+AnsiString(texture_map.size());
    }

    form->ShowModal();
    UI.RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ImportTextures()
{
	texture_map.clear();
    int new_cnt = ImageManager.GetLocalNewTextures(texture_map);
    if (new_cnt){
    	if (ELog.DlgMsg(mtInformation,"Found %d new texture(s)",new_cnt))
    		EditImageLib(AnsiString("Update images"),true);
    }else{
    	ELog.DlgMsg(mtInformation,"No new textures found.");
    }
}

void __fastcall TfrmImageLib::UpdateImageLib()
{
    SaveTextureParams();
    if (bImportMode&&!texture_map.empty()){
    	LPSTRVec modif;
        LockForm();
        ImageManager.SafeCopyLocalToServer(texture_map);
		// rename with folder
		FileMap files=texture_map;
        texture_map.clear();
        string256 fn;
        FilePairIt it=files.begin();
        FilePairIt _E=files.end();
        for (;it!=_E; it++)
            texture_map[Engine.FS.UpdateTextureNameWithFolder(it->first.c_str(),fn)]=it->second;
        // sync
		ImageManager.SynchronizeTextures(true,true,true,&texture_map,&modif);
        UnlockForm();
    	Device.RefreshTextures(&modif);
		ImageManager.FreeModifVec(modif);
    }else{
	    // save game textures
        if (modif_map.size()){
            LPSTRVec modif;
	        LockForm();
            ImageManager.SynchronizeTextures(true,true,true,&modif_map,&modif);
            UnlockForm();
	        Device.RefreshTextures(&modif);
            ImageManager.FreeModifVec(modif);
        }
	}
}

bool __fastcall TfrmImageLib::HideImageLib()
{
	if (form){
    	form->Close();
    	texture_map.clear();
		modif_map.clear();
    }
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::FormShow(TObject *Sender)
{
    InitItemsList(m_LastSelection.IsEmpty()?0:m_LastSelection.c_str());
	// check window position
	UI.CheckWindowPos(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::FormClose(TObject *Sender, TCloseAction &Action)
{
	if (!form) return;
    form->Enabled = false;

	_DELETE(m_Thm);
    m_SelectedName = "";

    TProperties::DestroyForm(ImageProps);

	form = 0;
	Action = caFree;
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
    if (compl_map.size()){
        for (; it!=_E; it++){
            TElTreeItem* node = FOLDER::AppendObject(tvItems,it->first.c_str());
            FilePairIt c_it = compl_map.find(it->first);
            if (c_it!=compl_map.end()){
                int A,M;
                ExtractCompValue(c_it->second,A,M);
                if ((A<2)&&(M<50)){
                	node->ImageIndex = 0;
                    if (node->Parent) node->Parent->Expand(false);
                }
            }
        }
    }else{
        for (; it!=_E; it++)
            FOLDER::AppendObject(tvItems,it->first.c_str());
    }

    // redraw
    TElTreeItem *node=0;
    if (!nm)	node = tvItems->Items->GetFirstNode();
    else 		node = FOLDER::FindObject(tvItems,nm);
	tvItems->IsUpdating = false;
    FOLDER::RestoreSelection(tvItems,node);
}

//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key==VK_ESCAPE){
		if (bFormLocked)UI.Command(COMMAND_BREAK_LAST_OPERATION);
        ebCancel->Click	();
        Key = 0; // :-) нужно для того чтобы AccessVoilation не вылазил по ESCAPE
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmImageLib::ebOkClick(TObject *Sender)
{
	if (bFormLocked) return;

	UpdateImageLib();
    HideImageLib();
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ebCancelClick(TObject *Sender)
{
	if (bFormLocked){
		UI.Command(COMMAND_BREAK_LAST_OPERATION);
    	return;
    }

    HideImageLib();
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::SaveTextureParams(){
	if (m_Thm&&(ImageProps->IsModified()||bImportMode)){
        m_Thm->Save(0,bImportMode?&Engine.FS.m_Import:0);
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
        if (bImportMode){ 
        	m_Thm = new EImageThumbnail(m_SelectedName.c_str(),EImageThumbnail::EITTexture,false);
            AnsiString fn = m_SelectedName;
            Engine.FS.UpdateTextureNameWithFolder(fn);         
//            if (!(m_Thm->Load(m_SelectedName.c_str(),&Engine.FS.m_Import)||m_Thm->Load(fn.c_str(),&Engine.FS.m_Textures)))
            if (!m_Thm->Load(m_SelectedName.c_str(),&Engine.FS.m_Import)){
            	bool bLoad = m_Thm->Load(fn.c_str(),&Engine.FS.m_Textures);
            	ImageManager.CreateTextureThumbnail(m_Thm,m_SelectedName.c_str(),&Engine.FS.m_Import,!bLoad);
            }
        }else			 m_Thm = new EImageThumbnail(m_SelectedName.c_str(),EImageThumbnail::EITTexture);
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
                	int A,M;
	                ExtractCompValue(it->second,A,M);
	    	        AnsiString t2; 	t2.sprintf(" [A:%d%% M:%d%%]",A,M);
            		temp			+= t2;
                }
            }
            lbInfo->Caption			= temp;

		    PropValueVec values;
            m_Thm->FillProp(values);
            ImageProps->AssignValues(values,true);

            m_LastSelection = m_SelectedName;
        }
    }else{
		ImageProps->ClearProperties();
		_DELETE(m_Thm);
        m_SelectedName = "";
		lbFileName->Caption	= "...";
		lbInfo->Caption		= "...";
       	pbImage->Repaint();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::pbImagePaint(TObject *Sender)
{
    if (m_Thm) m_Thm->Draw(paImage,pbImage,true);
}
//---------------------------------------------------------------------------


extern bool __fastcall LookupFunc(TElTreeItem* Item, void* SearchDetails);

void __fastcall TfrmImageLib::tvItemsKeyPress(TObject *Sender, char &Key)
{
	TElTreeItem* node = tvItems->Items->LookForItemEx(tvItems->Selected,-1,false,false,false,&Key,LookupFunc);
    if (!node) node = tvItems->Items->LookForItemEx(0,-1,false,false,false,&Key,LookupFunc);
    FOLDER::RestoreSelection(tvItems,node);
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
	if (bFormLocked) return;

	LockForm();
    ImageManager.CheckCompliance(texture_map,compl_map);
	UnlockForm();
    AnsiString last_name = tvItems->Selected?AnsiString(tvItems->Selected->Text):AnsiString("");
    InitItemsList(last_name.IsEmpty()?0:last_name.c_str());
//	tvItemsItemFocused(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ebCheckSelComplianceClick(TObject *Sender)
{
	if (bFormLocked) return;

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

void __fastcall TfrmImageLib::ebExportAssociationClick(TObject *Sender)
{
	if (ELog.DlgMsg(mtConfirmation,TMsgDlgButtons()<<mbYes<<mbNo,"Export assosiation?")==mrNo) return;
    // save previous data
    SaveTextureParams();
    
	AnsiString nm = "textures.ltx";
    Engine.FS.m_GameTextures.Update(nm);
	CInifile* ini = new CInifile(nm.c_str(), FALSE, FALSE, TRUE);

	LockForm();
    
    string256 fn;
    FilePairIt it=texture_map.begin();
    FilePairIt _E=texture_map.end();
    UI.ProgressStart(texture_map.size(),"Export association");
    bool bRes=true;
    for (;it!=_E; it++){
        EImageThumbnail* m_Thm = new EImageThumbnail(it->first.c_str(),EImageThumbnail::EITTexture);
	    UI.ProgressInc(it->first.c_str());
        if (m_Thm->Valid()&&(m_Thm->_Format().flag&STextureParams::flHasDetailTexture)){
        	AnsiString det;
            string128 src;
            det.sprintf("%s, %f",m_Thm->_Format().detail_name,m_Thm->_Format().detail_scale);
            strcpy(src,it->first.c_str());
	    	if (strext(src)) *strext(src)=0;
	    	ini->WriteString("association", src, det.c_str());
        }
        _DELETE(m_Thm);
		if (UI.NeedAbort()){ bRes=false; break; }
    }
    UI.ProgressEnd();

	UnlockForm();

    if (!bRes) ini->bSaveAtEnd = false;
	_DELETE(ini);    
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ebRemoveTextureClick(TObject *Sender)
{
  	FOLDER::RemoveItem(tvItems,tvItems->Selected,ImageManager.RemoveTexture);
}
//---------------------------------------------------------------------------

