#include "stdafx.h"
#pragma hdrstop

#include "SoundManager.h"
#include "soundrender_environment.h"
#include "EThumbnail.h"
#include "ui_main.h"
CSoundManager SndLib;
//---------------------------------------------------------------------------
#pragma package(smart_init)

extern "C" int ogg_enc(const char* in_fn, const char* out_fn, float quality, void* comment, int size);
//------------------------------------------------------------------------------
AnsiString CSoundManager::UpdateFileName(AnsiString& fn)
{
	return EFS.AppendFolderToName(fn,-1,FALSE);
}

//------------------------------------------------------------------------------
// возвращает список всех звуков
//------------------------------------------------------------------------------
int CSoundManager::GetSounds(FS_QueryMap& files)
{
    return FS.file_list(files,_sounds_,FS_ListFiles|FS_ClampExt,".wav");
}

int	CSoundManager::GetSoundEnvs(AStringVec& items)
{
	SoundEnvironment_LIB* Lib = Sound->get_env_library(); 
    if (Lib){
	    for (SoundEnvironment_LIB::SE_IT it=Lib->Library().begin(); it!=Lib->Library().end(); it++)
    	    items.push_back((*it)->name);
    }
	return items.size();
}

bool CSoundManager::OnCreate()
{
	psSoundFreq			= sf_44K;
    Sound->_initialize	((u64)Device.m_hWnd);
    return true;
}

void CSoundManager::OnDestroy()
{
    Sound->_destroy		();
}

void CSoundManager::OnFrame()
{
#ifdef _LEVEL_EDITOR
	if (bNeedRefreshEnvGeom){
    	bNeedRefreshEnvGeom = false;
        RealRefreshEnvGeometry	();
    }
#endif
	::psSoundVEffects		= psDeviceFlags.is(rsMuteSounds)?0.f:1.f;
	Sound->update			(Device.m_Camera.GetPosition(), Device.m_Camera.GetDirection(), Device.m_Camera.GetNormal(), Device.fTimeDelta);
}

void CSoundManager::RefreshEnvLibrary()
{
	Sound->refresh_env_library();
    RefreshEnvGeometry		();
}

void CSoundManager::MuteSounds(BOOL bVal)
{
	if (bVal) 	::psSoundVEffects = 0.f;
    else		::psSoundVEffects = psDeviceFlags.is(rsMuteSounds)?0.f:1.f;
}

BOOL CSoundManager::RemoveSound(LPCSTR fname, EItemType type)
{
	if (TYPE_FOLDER==type){
    	FS.dir_delete			(fname,FALSE);
		return TRUE;
    }else if (TYPE_OBJECT==type){
        AnsiString src_name;
        FS.update_path			(src_name,_sounds_,fname);
        if (FS.exist(src_name.c_str())){
            AnsiString thm_name = ChangeFileExt(fname,".thm");
            AnsiString game_name= ChangeFileExt(fname,".ogg");
            // source
            EFS.BackupFile		(_sounds_,fname);
            FS.file_delete		(src_name.c_str());
            EFS.WriteAccessLog	(src_name.c_str(),"Remove");
            // thumbnail
            EFS.BackupFile		(_sounds_,thm_name.c_str());
            FS.file_delete		(_sounds_,thm_name.c_str());
            // game
            FS.file_delete		(_game_sounds_,game_name.c_str());
            return TRUE;
        }
    }
    return FALSE;
}

//------------------------------------------------------------------------------
// возвращает список новых звуков
//------------------------------------------------------------------------------
int CSoundManager::GetLocalNewSounds(FS_QueryMap& files)
{
    return FS.file_list(files,_import_,FS_ListFiles|FS_RootOnly|FS_ClampExt,".wav");
}

//------------------------------------------------------------------------------
// копирует обновленные звуки с Import'a в Sounds
// files - список файлов для копирование
//------------------------------------------------------------------------------
void CSoundManager::SafeCopyLocalToServer(FS_QueryMap& files)
{
    AnsiString p_import;
    AnsiString p_sounds;
    AnsiString src_name,dest_name;
    FS.update_path			(p_import,_import_,"");
    FS.update_path			(p_sounds,_sounds_,"");

    FS_QueryPairIt it	= files.begin();
	FS_QueryPairIt _E 	= files.end();
	for (; it!=_E; it++){
        // copy thm
		AnsiString fn = ChangeFileExt(it->first,".thm");
		src_name 	= p_import	+ fn;
		UpdateFileName(fn);
		dest_name 	= p_sounds	+ fn;
		FS.file_rename(src_name.c_str(),dest_name.c_str(),true);
    	// copy sources
		fn 			= ChangeFileExt(it->first,".wav");
		src_name 	= p_import	+ fn;
		UpdateFileName(fn);
		dest_name 	= p_sounds	+ fn;
        if (FS.exist(dest_name.c_str()))
	        EFS.BackupFile	(_sounds_,ChangeFileExt(fn,".wav"));
        FS.file_copy		(src_name.c_str(),dest_name.c_str());
        FS.set_file_age		(dest_name.c_str(),FS.get_file_age(src_name.c_str()));
        EFS.WriteAccessLog	(dest_name.c_str(),"Replace");
        EFS.MarkFile		(src_name,true);
    }
}    

//------------------------------------------------------------------------------
// создает тхм
//------------------------------------------------------------------------------
void CSoundManager::CreateSoundThumbnail(ESoundThumbnail* THM, const AnsiString& src_name, LPCSTR initial, bool bSetDefParam)
{
	R_ASSERT(src_name.Length());
	AnsiString base_name;
    if (initial)	FS.update_path(base_name,initial,src_name.c_str());
    else			FS.update_path(base_name,_sounds_,src_name.c_str());
	base_name		= ChangeFileExt(base_name,".wav");

    // выставить начальные параметры
	if (bSetDefParam){
		THM->m_Age 		= FS.get_file_age(base_name.c_str());
		THM->m_fQuality = 0.f;
		THM->m_fMinDist = 1.f;
    }
}                             

void CSoundManager::MakeGameSound(ESoundThumbnail* THM, LPCSTR src_name, LPCSTR game_name)
{
	VerifyPath(game_name);
	if (!ogg_enc(src_name,game_name, THM->m_fQuality, &THM->m_fMinDist, sizeof(THM->m_fMinDist))){
    	FS.file_delete(game_name);
    	ELog.DlgMsg(mtError,"Can't make game sound '%s'.",game_name);
    }
}

//------------------------------------------------------------------------------
// возвращает список не синхронизированных (модифицированных) текстур
// source_list - содержит список текстур с расширениями
// sync_list - реально сохраненные файлы (после использования освободить)
//------------------------------------------------------------------------------
void CSoundManager::SynchronizeSounds(bool sync_thm, bool sync_game, bool bForceGame, FS_QueryMap* source_list, AStringVec* sync_list, FS_QueryMap* modif_map)
{        
	FS_QueryMap M_BASE;
	FS_QueryMap M_THUM;
    FS_QueryMap M_GAME;

    if (source_list) M_BASE = *source_list;
    else FS.file_list(M_BASE,_sounds_,FS_ListFiles,".wav");
    if (M_BASE.empty()) return;
    if (sync_thm) 	FS.file_list(M_THUM,_sounds_,FS_ListFiles|FS_ClampExt,".thm");
    if (sync_game) 	FS.file_list(M_GAME,_game_sounds_,FS_ListFiles|FS_ClampExt,".ogg");

    // lock rescanning
    FS.lock_rescan	();
    
    UI.ProgressStart(M_BASE.size(),"Synchronize sounds...");
    FS_QueryPairIt it=M_BASE.begin();
	FS_QueryPairIt _E = M_BASE.end();
	for (; it!=_E; it++){
        string256 base_name; strcpy(base_name,it->first.c_str()); strlwr(base_name);
        UI.ProgressInc(base_name);
        AnsiString fn;
        FS.update_path			(fn,_sounds_,it->first.c_str());
        if (strext(base_name)) *strext(base_name)=0;

		FS_QueryPairIt th = M_THUM.find(base_name);
    	bool bThm = ((th==M_THUM.end()) || ((th!=M_THUM.end())&&(th->second.modif!=it->second.modif)));
  		FS_QueryPairIt gm = M_GAME.find(base_name);
    	bool bGame= bThm || ((gm==M_GAME.end()) || ((gm!=M_GAME.end())&&(gm->second.modif!=it->second.modif)));

		ESoundThumbnail* THM=0;

    	// check thumbnail
    	if (sync_thm&&bThm){
        	THM = xr_new<ESoundThumbnail>(it->first.c_str());
//.		    bool bRes = Surface_Load(fn.c_str(),data,w,h,a); R_ASSERT(bRes);
//.			MakeThumbnailImage(THM,data.begin(),w,h,a);
            THM->Save	(it->second.modif);
        }
        // check game sounds
    	if (bForceGame||(sync_game&&bGame)){
        	if (!THM) THM = xr_new<ESoundThumbnail>(it->first.c_str()); 
            R_ASSERT(THM);
            AnsiString src_name=AnsiString(base_name)+".wav";
            FS.update_path	(_sounds_,src_name);
            AnsiString game_name=AnsiString(base_name)+".ogg";
            FS.update_path	(_game_sounds_,game_name);
            MakeGameSound	(THM,src_name.c_str(),game_name.c_str());
            FS.set_file_age	(game_name.c_str(), it->second.modif);
            if (sync_list) 	sync_list->push_back(base_name);
            if (modif_map) 	modif_map->insert(*it);
		}
		if (THM) xr_delete(THM);
		if (UI.NeedAbort()) break;
    }

    UI.ProgressEnd();
    // lock rescanning
    FS.unlock_rescan	();
}

//------------------------------------------------------------------------------
// если передан параметр modif - обновляем DX-Surface only и только из списка
// иначе полная синхронизация
//------------------------------------------------------------------------------
void CSoundManager::RefreshSounds(bool bSync)
{
	UI.SetStatus("Refresh sounds...");
    if (bSync) SynchronizeSounds(true,true,false,0,0);
    Sound->refresh_sources();
	UI.SetStatus("");
}

