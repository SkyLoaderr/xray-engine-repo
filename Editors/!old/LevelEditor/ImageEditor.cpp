//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ImageEditor.h"
#include "EThumbnail.h"
#include "ImageManager.h"
#include "PropertiesList.h"
#include "FolderLib.h"
#include "ui_main.h"
#include "ItemList.h"
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
bool TfrmImageLib::bFormLocked=false;
Flags32 TfrmImageLib::m_Flags={0};
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
	m_ItemProps 				= TProperties::CreateForm	("",paProperties,alClient);
    m_ItemList					= TItemList::CreateForm		("Items",paItems,alClient);
    m_ItemList->SetOnItemsFocusedEvent	(TOnILItemsFocused().bind(this,&TfrmImageLib::OnItemsFocused));
    m_ItemList->SetOnItemRemoveEvent	(TOnItemRemove().bind(&ImageLib,&CImageManager::RemoveTexture));
    m_ItemList->SetImages		(ImageList);
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
		form->modif_map.clear	();
        m_Flags.zero		();
    }

    form->ShowModal();
    UI->RedrawScene();
}
//---------------------------------------------------------------------------

ETextureThumbnail* TfrmImageLib::FindUsedTHM(LPCSTR name)
{
	for (THMIt it=m_THM_Used.begin(); it!=m_THM_Used.end(); it++)
    	if (0==strcmp((*it)->SrcName(),name)) return *it;
    return 0;
}
//---------------------------------------------------------------------------

void TfrmImageLib::SaveUsedTHM()
{
	for (THMIt t_it=m_THM_Used.begin(); t_it!=m_THM_Used.end(); t_it++)
    	if (modif_map.find((*t_it)->SrcName())!=modif_map.end()) 
        	(*t_it)->Save(0,bImportMode?_import_:0);
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

void __fastcall TfrmImageLib::UpdateLib()
{
    RegisterModifiedTHM	();
    SaveUsedTHM			();
    if (bImportMode&&!texture_map.empty()){
    	AStringVec modif;
        LockForm();
        ImageLib.SafeCopyLocalToServer(texture_map);
		// rename with folder
		FS_QueryMap	files=texture_map;
        texture_map.clear();
        std::string fn;
        FS_QueryPairIt it=files.begin();
        FS_QueryPairIt _E=files.end();
        for (;it!=_E; it++){
        	fn = EFS.ChangeFileExt(it->first.c_str(),"");
        	ImageLib.UpdateFileName(fn);
            texture_map.insert(mk_pair(fn.c_str(),FS_QueryItem(it->second.size,it->second.modif,it->second.flags.get())));
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
	UI->CheckWindowPos	(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::FormClose(TObject *Sender, TCloseAction &Action)
{
	if (!form) 		return;
    form->Enabled 	= false;
    DestroyUsedTHM	();
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
    for (; it!=_E; it++) LHelper().CreateItem(items,it->first.c_str(),0);
    m_ItemList->AssignItems(items,false,true);
}

//---------------------------------------------------------------------------
void __fastcall TfrmImageLib::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Shift.Contains(ssCtrl)){
    	if (Key==VK_CANCEL)		ExecCommand(COMMAND_BREAK_LAST_OPERATION);
    }else{
        if (Key==VK_ESCAPE){
            if (bFormLocked)	ExecCommand(COMMAND_BREAK_LAST_OPERATION);
            Key = 0; // :-) нужно для того чтобы AccessVoilation не вылазил по ESCAPE
        }
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmImageLib::ebOkClick(TObject *Sender)
{
	if (bFormLocked) return;

	UpdateLib	();
    HideLib		();
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ebCancelClick(TObject *Sender)
{
	if (bFormLocked){
		ExecCommand(COMMAND_BREAK_LAST_OPERATION);
    	return;
    }

    HideLib();
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::RegisterModifiedTHM()
{
	if (m_ItemProps->IsModified()||bImportMode){
	    for (THMIt t_it=m_THM_Current.begin(); t_it!=m_THM_Current.end(); t_it++){
            std::string fn = (*t_it)->SrcName();
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

void __fastcall TfrmImageLib::ebRebuildAssociationClick(TObject *Sender)
{
	if (ELog.DlgMsg(mtConfirmation,TMsgDlgButtons()<<mbYes<<mbNo,"Are you sure to export association?")==mrNo) return;

    if (!modif_map.empty()){
        int res = ELog.DlgMsg(mtConfirmation,TMsgDlgButtons()<<mbYes<<mbNo<<mbCancel,"Save modified properties?");
        switch (res){
        case mrYes: 	UpdateLib();	break;
        case mrNo: 			  			break;
        case mrCancel: 		  			return;
        }
    }

	std::string nm;
    FS.update_path			(nm,_game_textures_,"textures.ltx");
	CInifile* ini 			= xr_new<CInifile>(nm.c_str(), FALSE, FALSE, TRUE);

	LockForm				();

    string256 fn;
    FS_QueryPairIt it		= texture_map.begin();
    FS_QueryPairIt _E		= texture_map.end();
	SPBItem* pb = UI->ProgressStart(texture_map.size(),"Export association");
    bool bRes=true;
    for (;it!=_E; it++){
        ETextureThumbnail* m_Thm = xr_new<ETextureThumbnail>(it->first.c_str());
	    pb->Inc				(it->first.c_str());
        AnsiString base_name= ChangeFileExt(it->first.c_str(),"");
        ImageLib.WriteAssociation	(ini,base_name.c_str(),m_Thm->_Format());
        xr_delete			(m_Thm);
		if (UI->NeedAbort()){ bRes=false; break; }
    }
	UI->ProgressEnd(pb);

	UnlockForm();

    if (!bRes) ini->bSaveAtEnd = false;
	xr_delete(ini);
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::ebRemoveTextureClick(TObject *Sender)
{
	m_ItemList->RemoveSelItems(0);
}
//---------------------------------------------------------------------------

void TfrmImageLib::DestroyUsedTHM()
{
    for (THMIt it=m_THM_Used.begin(); it!=m_THM_Used.end(); it++)
    	xr_delete(*it);
    m_THM_Used.clear();
}

void TfrmImageLib::OnTypeChange(PropValue* prop)
{
	UpdateProperties();
}

void TfrmImageLib::OnCubeMapBtnClick(PropValue* value, bool& bModif, bool& bSafe)
{
	ButtonValue* B = dynamic_cast<ButtonValue*>(value); R_ASSERT(B);
    bModif = false;
	switch(B->btn_num){
    case 0:{
        RStringVec items;
        if (0!=m_ItemList->GetSelected(items)){
            for (RStringVecIt it=items.begin(); it!=items.end(); it++){
                AnsiString new_name = AnsiString(it->c_str())+"#small";
                ImageLib.CreateSmallerCubeMap(it->c_str(),new_name.c_str());	
            }
        }
    }break;
	}
}

void TfrmImageLib::OnItemsFocused(ListItemsVec& items)
{
	PropItemVec props;

    RegisterModifiedTHM	();
    m_THM_Current.clear	();
    
	if (!items.empty()){
	    for (ListItemsIt it=items.begin(); it!=items.end(); it++){
            ListItem* prop = *it;
            if (prop){
            	ETextureThumbnail* thm=0;
                if (bImportMode){
                    thm = FindUsedTHM(prop->Key());
                    if (!thm) m_THM_Used.push_back(thm=xr_new<ETextureThumbnail>(prop->Key(),false));
                    std::string fn = prop->Key();
                    ImageLib.UpdateFileName(fn);
                    if (!thm->Load(prop->Key(),_import_)){
                        bool bLoad = thm->Load(fn.c_str(),_textures_);
                        ImageLib.CreateTextureThumbnail(thm,prop->Key(),_import_,!bLoad);
                    }
                }else{ 
                    thm = FindUsedTHM(prop->Key());
                    if (!thm) m_THM_Used.push_back(thm=xr_new<ETextureThumbnail>(prop->Key()));
                }
                m_THM_Current.push_back	(thm);
                thm->FillProp			(props,PropValue::TOnChange().bind(this,&TfrmImageLib::OnTypeChange));
				if (thm->_Format().type==STextureParams::ttCubeMap){
				    ButtonValue* B		= PHelper().CreateButton (props, "CubeMap\\Edit", "Make Small", 0);
        			B->OnBtnClickEvent.bind(this,&TfrmImageLib::OnCubeMapBtnClick);
                }
            }
        }
    }
    paImage->Repaint				();
	m_ItemProps->AssignItems		(props);
}
//---------------------------------------------------------------------------

void __fastcall TfrmImageLib::paImagePaint(TObject *Sender)
{
	if (m_THM_Current.size()==1) m_THM_Current.back()->Draw(paImage);
}
//---------------------------------------------------------------------------

void TfrmImageLib::OnFrame()
{
	if (form){
    	if (m_Flags.is(flUpdateProperties)){
        	form->m_ItemList->FireOnItemFocused();
        	m_Flags.set(flUpdateProperties,FALSE);
        }
    }
}
//    void 				ExtractCompValue	(int val, int& A, int& M){	A = val/1000; M = val-A*1000; }
/*
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
*/


