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
FS_QueryMap	TfrmSoundLib::modif_map;
//---------------------------------------------------------------------------
__fastcall TfrmSoundLib::TfrmSoundLib(TComponent* Owner)
    : TForm(Owner)
{
    DEFINE_INI(fsStorage);
	bFormLocked = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSoundLib::FormCreate(TObject *Sender)
{
	m_ItemProps = TProperties::CreateForm("",paProperties,alClient);
    m_ItemList	= TItemList::CreateForm("Items",paItems,alClient,TItemList::ilEditMenu|TItemList::ilMultiSelect);
    m_ItemList->OnItemRemove	= RemoveSound;
    m_ItemList->OnItemRename	= RenameSound;
    m_ItemList->OnItemsFocused 	= OnItemsFocused;
    m_ItemList->SetImages		(ImageList);
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
        form->ebRemoveCurrent->Enabled = !bImport;
        form->ebRenameCurrent->Enabled = !bImport;
		form->modif_map.clear	();
    }

    form->ShowModal			();
    UI.RedrawScene			();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSoundLib::UpdateLib()
{
    SaveSoundParams();
    // save game sounds
    if (modif_map.size()){
        AStringVec modif;
        LockForm();
        SndLib.SynchronizeSounds(true,true,true,&modif_map,0);
        UnlockForm();
        SndLib.RefreshSounds(false);
    }
}

bool __fastcall TfrmSoundLib::HideLib()
{
	if (form){
    	form->Close();
		modif_map.clear();
    }
    return true;
}
//---------------------------------------------------------------------------

void TfrmSoundLib::AppendModif(LPCSTR nm)
{
    FS_QueryItem 	dest;
    bool bFind		= FS.file_find(dest,_sounds_,ChangeFileExt(nm,".wav").c_str(),true); R_ASSERT(bFind);
    modif_map.insert(mk_pair(nm,dest));
}
//---------------------------------------------------------------------------

BOOL __fastcall TfrmSoundLib::RemoveSound(LPCSTR fname, EItemType type)
{
	// delete it from modif map
    FS_QueryPairIt it=modif_map.find(fname); 
    if (it!=modif_map.end()) modif_map.erase(it);
	// remove sound source
	return SndLib.RemoveSound(fname,type);
}
//---------------------------------------------------------------------------

void __fastcall TfrmSoundLib::RenameSound(LPCSTR p0, LPCSTR p1, EItemType type)
{
    // rename sound source
	SndLib.RenameSound(p0,p1,type);
	// delete old from map
    FS_QueryPairIt old_it=modif_map.find(p0); 
    if (old_it!=modif_map.end()){
    	modif_map.erase	(old_it);
        AppendModif		(p1);
	}	
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
    FS_QueryMap		sound_map;
    SndLib.GetSounds(sound_map,TRUE);

	ListItemsVec items;

    ListItem* V;
    // fill items
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
	if (m_ItemProps->IsModified()){
	    for (THMIt t_it=m_THMs.begin(); t_it!=m_THMs.end(); t_it++){
            (*t_it)->Save	(0,0);
            AppendModif		((*t_it)->SrcName());
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

void __fastcall TfrmSoundLib::ebRemoveCurrentClick(TObject *Sender)
{
	m_ItemList->RemoveSelItems();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSoundLib::ebRenameCurrentClick(TObject *Sender)
{
	m_ItemList->RenameSelItem();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSoundLib::ebImportSoundClick(TObject *Sender)
{
    AnsiString open_nm;
    if (EFS.GetOpenName(_import_,open_nm,true,0,4)){
		// load
    	AStringVec lst;
        _SequenceToList(lst,open_nm.c_str());
		bool bNeedUpdate=false;
        // folder name
        AnsiString folder;
        TElTreeItem* item = m_ItemList->GetSelected(); 
        if (item) FHelper.MakeName(item,0,folder,true);
        //
//		AnsiString path; // нужен при multi-open для сохранения последнего пути
		AnsiString m_LastSelection;
        for (AStringIt it=lst.begin(); it!=lst.end(); it++){
            AnsiString 			dest_name = AnsiString(folder+ExtractFileName(*it));
            AnsiString			dest_full_name;
            FS.update_path		(dest_full_name,_sounds_,dest_name.c_str());
            if (FS.exist(dest_full_name.c_str())){
            	int res = ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"File '%s' already exist. Owerwrite it?",ExtractFileName(*it).c_str());
                if (res==mrCancel) 	break;
                if (res==mrNo)		continue;
            }
            FS.file_copy		(it->c_str(),dest_full_name.c_str());
            FS_QueryMap 		QM; 
            FS.file_list		(QM,_sounds_,FS_ListFiles|FS_ClampExt,dest_name.c_str());
            SndLib.SynchronizeSounds(true, true, true, &QM, 0);
//.            EFS.MarkFile		(*it,true);
            EFS.BackupFile		(_sounds_,dest_name);
            EFS.WriteAccessLog	(dest_full_name.c_str(),"Import");
            bNeedUpdate			= true;
            m_LastSelection 	= ChangeFileExt(dest_name,"");
        }
        if (bNeedUpdate){
        	SndLib.RefreshSounds(false);
			InitItemsList		();
			m_ItemList->SelectItem	(m_LastSelection,true,false,true);
        }
        // refresh selected
//		RefreshSelected();
    }
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
    case 2:{ 
    	ButtonValue* B = dynamic_cast<ButtonValue*>(sender); VERIFY(B);
    	bAutoPlay=!bAutoPlay; 
        B->value[V->btn_num] = AnsiString().sprintf("Auto (%s)",bAutoPlay?"on":"off");
    }break;
	}
    bModif = false;
}
//------------------------------------------------------------------------------

void __fastcall TfrmSoundLib::OnControl2Click(PropValue* sender, bool& bModif)
{
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    switch (V->btn_num){
    case 0:{
    	ButtonValue* B = dynamic_cast<ButtonValue*>(sender); VERIFY(B);
    	bAutoPlay=!bAutoPlay; 
        B->value[V->btn_num] = bAutoPlay?"on":"off";
    }break;
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
            	ESoundThumbnail* thm=xr_new<ESoundThumbnail>(prop->Key());
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
        PHelper.CreateCaption(props,"File Length",	AnsiString().sprintf("%.2f Kb",float(size)/1024.f));
        PHelper.CreateCaption(props,"Total Time",	AnsiString().sprintf("%.2f sec",float(time)/1000.f));
        ButtonValue* B=PHelper.CreateButton	(props, "Control",	"Play,Stop",ButtonValue::flFirstOnly);
        B->OnBtnClickEvent		= OnControlClick;
        B=PHelper.CreateButton	(props, "Auto Play",bAutoPlay?"on":"off",ButtonValue::flFirstOnly);
        B->OnBtnClickEvent		= OnControl2Click;
    }
    
	m_ItemProps->AssignItems		(props,true);
}
//---------------------------------------------------------------------------

void TfrmSoundLib::PlaySound(LPCSTR name, u32& size, u32& time)
{
    const CLocatorAPI::file* file	= FS.exist(_game_sounds_,ChangeFileExt(name,".ogg").c_str());
    if (file){
        m_Snd.create		(TRUE,name);
        m_Snd.play			(0,FALSE);
        m_Snd.set_position	(Device.m_Camera.GetPosition());
        CSoundRender_Source* src= (CSoundRender_Source*)m_Snd.handle;
        size				= file->size_real;
        time				= src->dwTimeTotal;
    	if (!bAutoPlay)		m_Snd.stop();
    }
}



