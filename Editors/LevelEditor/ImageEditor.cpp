//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ImageEditor.h"
#include "EThumbnail.h"
#include "ImageManager.h"
#include "PropertiesList.h"
#include "FolderLib.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElTree"
#pragma link "ElXPThemedControl"
#pragma link "ExtBtn"
#pragma link "mxPlacemnt"
#pragma link "MXCtrls"
#pragma resource "*.dfm"
TfrmImageLib* TfrmImageLib::form = 0;
FS_QueryMap	TfrmImageLib::texture_map;
FS_QueryMap	TfrmImageLib::modif_map;
FS_QueryMap	TfrmImageLib::compl_map;
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
	m_ItemProps = TProperties::CreateForm("",paProperties,alClient);
    m_ItemList	= TItemList::CreateForm("Items",paItems,alClient);
    m_ItemList->OnItemsFocused 	= OnItemsFocused;
    m_ItemList->SetImages(ImageList);
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::FormDestroy(TObject *Sender)
{
    TProperties::DestroyForm(m_ItemProps);
    TItemList::DestroyForm	(m_ItemList);
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::EditLib(AnsiString& title, bool bImport)
{
	if (!form){
        form 				= xr_new<TfrmImageLib>((TComponent*)0);
        form->Caption 		= title;
	    form->bImportMode 	= bImport;
        form->ebRemoveTexture->Enabled = !bImport;
        compl_map.clear		();

		form->modif_map.clear	();
    }

    form->ShowModal();
    UI.RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ImportTextures()
{
	texture_map.clear();
    int new_cnt = ImageLib.GetLocalNewTextures(texture_map);
    if (new_cnt){
    	if (ELog.DlgMsg(mtInformation,"Found %d new texture(s)",new_cnt))
    		EditLib(AnsiString("Update images"),true);
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find new textures.");
    }
}

void __fastcall TfrmImageLib::UpdateImageLib()
{
    SaveTextureParams();
    if (bImportMode&&!texture_map.empty()){
    	AStringVec modif;
        LockForm();
        ImageLib.SafeCopyLocalToServer(texture_map);
		// rename with folder
		FS_QueryMap	files=texture_map;
        texture_map.clear();
        AnsiString fn;
        FS_QueryPairIt it=files.begin();
        FS_QueryPairIt _E=files.end();
        for (;it!=_E; it++){
        	fn = ChangeFileExt(it->first,".tga");
        	ImageLib.UpdateFileName(fn);
            texture_map.insert(mk_pair(fn,FS_QueryItem(it->second.size,it->second.modif,it->second.flags.get())));
        }
        // sync
		ImageLib.SynchronizeTextures(true,true,true,&texture_map,&modif);
        UnlockForm();
    	ImageLib.RefreshTextures(&modif);
    }else{
	    // save game textures
        if (modif_map.size()){
            AStringVec modif;
	        LockForm();
            ImageLib.SynchronizeTextures(true,true,true,&modif_map,&modif);
            UnlockForm();
	        ImageLib.RefreshTextures(&modif);
        }
	}
}

bool __fastcall TfrmImageLib::HideLib()
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
    InitItemsList		();
	// check window position
	UI.CheckWindowPos	(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::FormClose(TObject *Sender, TCloseAction &Action)
{
	if (!form) 		return;
    form->Enabled 	= false;
    DestroyTHMs		();
	form 			= 0;
	Action 			= caFree;
}
//---------------------------------------------------------------------------
void TfrmImageLib::InitItemsList()
{
    if (!form->bImportMode)  ImageLib.GetTextures(texture_map);
    
	ListItemsVec items;

    ListItem* V;
    // fill
	FS_QueryPairIt it = texture_map.begin();
	FS_QueryPairIt _E = texture_map.end();
    if (compl_map.size()){
        for (; it!=_E; it++){
        	V = LHelper.CreateItem	(items,it->first.c_str(),0);
			FS_QueryPairIt c_it	= compl_map.find(it->first);
            if (c_it!=compl_map.end()){
                int A,M;
                ExtractCompValue(c_it->second.modif,A,M);
                V->icon_index = ((A<=2)&&(M<=50))?0:1;
            }
        }
    }else{
        for (; it!=_E; it++)
        	LHelper.CreateItem(items,it->first.c_str(),0);
    }
    m_ItemList->AssignItems(items,false,true);
}

//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Shift.Contains(ssCtrl)){
    	if (Key==VK_CANCEL)		UI.Command(COMMAND_BREAK_LAST_OPERATION);
    }else{
        if (Key==VK_ESCAPE){
            if (bFormLocked)	UI.Command(COMMAND_BREAK_LAST_OPERATION);
            Key = 0; // :-) нужно для того чтобы AccessVoilation не вылазил по ESCAPE
        }
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmImageLib::ebOkClick(TObject *Sender)
{
	if (bFormLocked) return;

	UpdateImageLib();
    HideLib();
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ebCancelClick(TObject *Sender)
{
	if (bFormLocked){
		UI.Command(COMMAND_BREAK_LAST_OPERATION);
    	return;
    }

    HideLib();
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::SaveTextureParams()
{
	if (m_ItemProps->IsModified()||bImportMode){
	    for (THMIt t_it=m_THMs.begin(); t_it!=m_THMs.end(); t_it++){
            (*t_it)->Save(0,bImportMode?_import_:0);
            AnsiString fn = ChangeFileExt((*t_it)->Name(),".tga");
            FS_QueryPairIt it=texture_map.find(fn); R_ASSERT(it!=texture_map.end());
            modif_map.insert(*it);
        }
    }
}

void __fastcall TfrmImageLib::fsStorageRestorePlacement(TObject *Sender)
{
	m_ItemProps->RestoreParams(fsStorage);
    m_ItemList->LoadParams(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::fsStorageSavePlacement(TObject *Sender)
{
	m_ItemProps->SaveParams(fsStorage);
    m_ItemList->SaveParams(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ebCheckAllComplianceClick(TObject *Sender)
{
	if (bFormLocked) return;

	LockForm();
    ImageLib.CheckCompliance(texture_map,compl_map);
	UnlockForm();
    InitItemsList();
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ebCheckSelComplianceClick(TObject *Sender)
{
	if (bFormLocked) return;

    ListItemsVec items;
    if (m_ItemList->GetSelected(0,items,false)){
        for (ListItemsIt it=items.begin(); it!=items.end(); it++){
	    	int compl			= 0;
            AnsiString 	fname,src_name=ChangeFileExt((*it)->Key(),".tga");
            FS.update_path		(fname,_textures_,src_name.c_str());
            if (ImageLib.CheckCompliance(fname.c_str(),compl)){
                compl_map.insert(mk_pair(src_name,FS_QueryItem(0,compl)));
            }else{
                ELog.DlgMsg(mtError,"Some error found in check.");
            }
        }
        InitItemsList();
    }else{
    	ELog.DlgMsg(mtError,"At first select item!");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ebRebuildAssociationClick(TObject *Sender)
{
	if (ELog.DlgMsg(mtConfirmation,TMsgDlgButtons()<<mbYes<<mbNo,"Are you sure to export assosiation?")==mrNo) return;
    // save previous data
    SaveTextureParams();

	AnsiString nm;
    FS.update_path			(nm,_game_textures_,"textures.ltx");
	CInifile* ini 			= xr_new<CInifile>(nm.c_str(), FALSE, FALSE, TRUE);

	LockForm();

    string256 fn;
    FS_QueryPairIt it		= texture_map.begin();
    FS_QueryPairIt _E		= texture_map.end();
    UI.ProgressStart		(texture_map.size(),"Export association");
    bool bRes=true;
    for (;it!=_E; it++){
        ETextureThumbnail* m_Thm = xr_new<ETextureThumbnail>(it->first.c_str());
	    UI.ProgressInc(it->first.c_str());
        if (m_Thm->Valid()&&(m_Thm->_Format().flags.is(STextureParams::flHasDetailTexture))){
        	AnsiString det;
            string128 src;
            det.sprintf("%s, %f",m_Thm->_Format().detail_name,m_Thm->_Format().detail_scale);
            strcpy(src,it->first.c_str());
	    	if (strext(src)) *strext(src)=0;
	    	ini->w_string("association", src, det.c_str());
        }
        xr_delete(m_Thm);
		if (UI.NeedAbort()){ bRes=false; break; }
    }
    UI.ProgressEnd();

	UnlockForm();

    if (!bRes) ini->bSaveAtEnd = false;
	xr_delete(ini);
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ebRemoveTextureClick(TObject *Sender)
{
	m_ItemList->RemoveSelItems(ImageLib.RemoveTexture);
}
//---------------------------------------------------------------------------

void TfrmImageLib::DestroyTHMs()
{
    for (THMIt it=m_THMs.begin(); it!=m_THMs.end(); it++)
    	xr_delete(*it);
    m_THMs.clear();
}

void __fastcall TfrmImageLib::OnItemsFocused(ListItemsVec& items)
{
	PropItemVec props;

    SaveTextureParams	();
    DestroyTHMs			();
    
	if (!items.empty()){
	    for (ListItemsIt it=items.begin(); it!=items.end(); it++){
            ListItem* prop = *it;
            if (prop){
            	ETextureThumbnail* thm=0;
                if (bImportMode){
                    thm = xr_new<ETextureThumbnail>(prop->Key(),false);
                    AnsiString fn = prop->Key();
                    ImageLib.UpdateFileName(fn);
        //            if (!(m_Thm->Load(m_SelectedName.c_str(),&EFS.m_Import)||m_Thm->Load(fn.c_str(),&EFS.m_Textures)))
                    if (!thm->Load(prop->Key(),_import_)){
                        bool bLoad = thm->Load(fn.c_str(),_textures_);
                        ImageLib.CreateTextureThumbnail(thm,prop->Key(),_import_,!bLoad);
                    }
                }else thm = xr_new<ETextureThumbnail>(prop->Key());
                m_THMs.push_back	(thm);
                thm->FillProp		(props);
            }
        }
    }
	m_ItemProps->AssignItems		(props,true);

    if (m_THMs.size()==1){
        ETextureThumbnail* thm		= m_THMs.back();
        if (!thm->Valid()){
        	paImage->Repaint		();
            lbFileName->Caption 	= "/Error/";
            lbInfo->Caption			= "/Error/";
        }else{
        	paImage->Repaint		();
            lbFileName->Caption 	= "\""+ChangeFileExt(thm->Name(),"")+"\"";
            AnsiString temp; 		temp.sprintf("%d x %d x %s",thm->_Width(),thm->_Height(),thm->_Format().HasAlpha()?"32b":"24b");
            if (!compl_map.empty()){
				FS_QueryPairIt it 	= compl_map.find(ChangeFileExt(thm->Name(),".tga"));
                if (it!=compl_map.end()){
                	int A,M;
	                ExtractCompValue(it->second.modif,A,M);
	    	        AnsiString t2; 	t2.sprintf(" [A:%d%% M:%d%%]",A,M);
            		temp			+= t2;
                }
            }
            lbInfo->Caption			= temp;
        }
	}else{
		lbFileName->Caption	= "...";
		lbInfo->Caption		= "...";
       	paImage->Repaint	();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::paImageResize(TObject *Sender)
{
	paImage->Height = paImage->Width;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::paImagePaint(TObject *Sender)
{
	if (m_THMs.size()==1) m_THMs.back()->Draw(paImage);
}
//---------------------------------------------------------------------------

