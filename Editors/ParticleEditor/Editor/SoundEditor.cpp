//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "SoundEditor.h"
#include "EThumbnail.h"
#include "SoundManager.h"
#include "PropertiesList.h"
#include "FolderLib.h"
#include "ui_main.h"
#include "soundrender_source.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElTree"
#pragma link "ElXPThemedControl"
#pragma link "ExtBtn"
#pragma link "mxPlacemnt"
#pragma link "MXCtrls"
#pragma resource "*.dfm"

TfrmSoundLib* TfrmSoundLib::form = 0;
FS_QueryMap	TfrmSoundLib::sound_map;
FS_QueryMap	TfrmSoundLib::modif_map;
//---------------------------------------------------------------------------
__fastcall TfrmSoundLib::TfrmSoundLib(TComponent* Owner)
    : TForm(Owner)
{
    DEFINE_INI(fsStorage);
    bImportMode = false;
	bFormLocked = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSoundLib::FormCreate(TObject *Sender)
{
	m_ItemProps = TProperties::CreateForm("",paProperties,alClient);
    m_ItemList	= TItemList::CreateForm("Items",paItems,alClient);
    m_ItemList->OnItemsFocused 	= OnItemsFocused;
    m_ItemList->SetImages(ImageList);
    bAutoPlay 	= FALSE;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSoundLib::FormDestroy(TObject *Sender)
{
    TProperties::DestroyForm(m_ItemProps);
    TItemList::DestroyForm	(m_ItemList);
	m_Snd.destroy			();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSoundLib::EditLib(AnsiString& title, bool bImport)
{
	if (!form){
        form 				= xr_new<TfrmSoundLib>((TComponent*)0);
        form->Caption 		= title;
	    form->bImportMode 	= bImport;
        form->ebRemoveSound->Enabled = !bImport;

        if (!form->bImportMode)  SndLib.GetSounds(sound_map);
		form->modif_map.clear	();

        form->paSoundCount->Caption = AnsiString(" Sounds in list: ")+AnsiString(sound_map.size());
    }

    form->ShowModal();
    UI.RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSoundLib::ImportSounds()
{
	sound_map.clear();
    int new_cnt = SndLib.GetLocalNewSounds(sound_map);
    if (new_cnt){
    	if (ELog.DlgMsg(mtInformation,"Found %d new sound(s)",new_cnt))
    		EditLib(AnsiString("Update sounds"),true);
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find new sounds.");
    }
}

void __fastcall TfrmSoundLib::UpdateLib()
{
    SaveSoundParams();
    if (bImportMode&&!sound_map.empty()){
        LockForm();
        SndLib.SafeCopyLocalToServer(sound_map);
		// rename with folder
		FS_QueryMap	files=sound_map;
        sound_map.clear();
        string256 fn;
        FS_QueryPairIt it=files.begin();
        FS_QueryPairIt _E=files.end();
        for (;it!=_E; it++){
        	EFS.AppendFolderToName(it->first.c_str(),fn);
            sound_map.insert(mk_pair(fn,FS_QueryItem(it->second.size,it->second.modif,it->second.flags.get())));
        }
        // sync
		SndLib.SynchronizeSounds(true,true,true,&sound_map,0);
        UnlockForm();
    	SndLib.RefreshSounds(false);
    }else{
	    // save game sounds
        if (modif_map.size()){
            AStringVec modif;
	        LockForm();
            SndLib.SynchronizeSounds(true,true,true,&modif_map,0);
            UnlockForm();
	    	SndLib.RefreshSounds(false);
        }
	}
}

bool __fastcall TfrmSoundLib::HideLib()
{
	if (form){
    	form->Close();
    	sound_map.clear();
		modif_map.clear();
    }
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmSoundLib::FormShow(TObject *Sender)
{
    InitItemsList		();
	// check window position
	UI.CheckWindowPos	(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmSoundLib::FormClose(TObject *Sender, TCloseAction &Action)
{
	if (!form) 		return;
    form->Enabled 	= false;
    DestroyTHMs		();
	form 			= 0;
	Action 			= caFree;
}
//---------------------------------------------------------------------------
void TfrmSoundLib::InitItemsList()
{
	ListItemsVec items;

    ListItem* V;
    // fill
	FS_QueryPairIt it = sound_map.begin();
	FS_QueryPairIt _E = sound_map.end();
    for (; it!=_E; it++)
        LHelper.CreateItem(items,it->first.c_str(),0);
    m_ItemList->AssignItems(items,false,true);
}

//---------------------------------------------------------------------------
void __fastcall TfrmSoundLib::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (Key==VK_ESCAPE){
		if (bFormLocked)UI.Command(COMMAND_BREAK_LAST_OPERATION);
        Key = 0; // :-) нужно для того чтобы AccessVoilation не вылазил по ESCAPE
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmSoundLib::ebOkClick(TObject *Sender)
{
	if (bFormLocked) return;
    m_Snd.destroy	();
    
	UpdateLib		();
    HideLib			();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSoundLib::ebCancelClick(TObject *Sender)
{
	if (bFormLocked){
		UI.Command(COMMAND_BREAK_LAST_OPERATION);
    	return;
    }

    HideLib			();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSoundLib::SaveSoundParams()
{
	if (m_ItemProps->IsModified()||bImportMode){
	    for (THMIt t_it=m_THMs.begin(); t_it!=m_THMs.end(); t_it++){
            (*t_it)->Save(0,bImportMode?_import_:0);
            AnsiString fn = (*t_it)->SrcName();
            FS_QueryPairIt it=sound_map.find(fn); R_ASSERT(it!=sound_map.end());
            modif_map.insert(*it);
        }
    }
}

void __fastcall TfrmSoundLib::fsStorageRestorePlacement(TObject *Sender)
{
	m_ItemProps->RestoreParams(fsStorage);
    m_ItemList->LoadParams(fsStorage);
    bAutoPlay = fsStorage->ReadInteger("auto_play",TRUE);
}
//---------------------------------------------------------------------------

void __fastcall TfrmSoundLib::fsStorageSavePlacement(TObject *Sender)
{
	m_ItemProps->SaveParams(fsStorage);
    m_ItemList->SaveParams(fsStorage);
    fsStorage->WriteInteger("auto_play",bAutoPlay);
}
//---------------------------------------------------------------------------

void __fastcall TfrmSoundLib::ebRemoveSoundClick(TObject *Sender)
{
	m_ItemList->RemoveSelItems(SndLib.RemoveSound);
}
//---------------------------------------------------------------------------

void TfrmSoundLib::DestroyTHMs()
{
    for (THMIt it=m_THMs.begin(); it!=m_THMs.end(); it++)
    	xr_delete(*it);
    m_THMs.clear();
}
//------------------------------------------------------------------------------

void __fastcall TfrmSoundLib::OnControlClick(PropValue* sender, bool& bModif)
{
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    switch (V->btn_num){
    case 0: m_Snd.play_at_pos(0,Device.m_Camera.GetPosition(),FALSE); 	break;
    case 1: m_Snd.stop(); 												break;
	}
    bModif = false;
}
//------------------------------------------------------------------------------

void __fastcall TfrmSoundLib::OnItemsFocused(ListItemsVec& items)
{
	PropItemVec props;

    SaveSoundParams	();
    DestroyTHMs		();
    m_Snd.destroy	();
                                          
	if (!items.empty()){
	    for (ListItemsIt it=items.begin(); it!=items.end(); it++){
            ListItem* prop = *it;
            if (prop){
            	ESoundThumbnail* thm=0;
                if (bImportMode){
                    thm = xr_new<ESoundThumbnail>(prop->Key(),false);
                    AnsiString fn = prop->Key();
                    EFS.AppendFolderToName(fn);
        //            if (!(m_Thm->Load(m_SelectedName.c_str(),&EFS.m_Import)||m_Thm->Load(fn.c_str(),&EFS.m_Textures)))
                    if (!thm->Load(prop->Key(),_import_)){
                        bool bLoad = thm->Load(fn.c_str(),_sounds_);
                        SndLib.CreateSoundThumbnail(thm,prop->Key(),_import_,!bLoad);
                    }
                }else thm = xr_new<ESoundThumbnail>(prop->Key());
                m_THMs.push_back	(thm);
                thm->FillProp		(props);
            }
        }
    }

    if (m_THMs.size()==1)
    {
        ESoundThumbnail* thm=m_THMs.back();
        u32 size=0;
        u32 time=0;
		PlaySound(thm->SrcName(), size, time);
        PHelper.CreateCaption(props,"File length",	AnsiString().sprintf("%.2f Kb",float(size)/1024.f));
        PHelper.CreateCaption(props,"Total time",	AnsiString().sprintf("%.2f sec",float(time)/1000.f));
        if (!bImportMode){
            ButtonValue* B=PHelper.CreateButton	(props, "Control",	"Play,Stop",ButtonValue::flFirstOnly);
            B->OnBtnClickEvent		= OnControlClick;
            PHelper.CreateBOOL		(props, "Auto Play", &bAutoPlay);
        }
    }
    
	m_ItemProps->AssignItems		(props,true);
}
//---------------------------------------------------------------------------

void TfrmSoundLib::PlaySound(LPCSTR name, u32& size, u32& time)
{
	if (bImportMode&&bAutoPlay) return;
    const CLocatorAPI::file* file	= FS.exist(_game_sounds_,ChangeFileExt(name,".ogg").c_str());
    if (file){
        m_Snd.create		(TRUE,name);
        m_Snd.play			(0,FALSE);
        m_Snd.set_position	(Device.m_Camera.GetPosition());
        CSoundRender_Source* src= (CSoundRender_Source*)m_Snd.handle;
        size				= file->size_real;
        time				= src->dwTimeTotal;
    }
}

