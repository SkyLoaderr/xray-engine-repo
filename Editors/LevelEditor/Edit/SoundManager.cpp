#include "stdafx.h"
#pragma hdrstop

#include "SoundManager.h"
CSoundManager SndLib;
//---------------------------------------------------------------------------
#pragma package(smart_init)

//------------------------------------------------------------------------------
// возвращает список всех звуков
//------------------------------------------------------------------------------
int CSoundManager::GetSounds(FS_QueryMap& files)
{
    return FS.file_list(files,_game_sounds_,FS_ListFiles|FS_ClampExt,".wav");
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
	psSoundFreq			= sf_22K;
    Sound->_initialize	((u32)Device.m_hWnd);
    return true;
}

void CSoundManager::OnDestroy()
{
    Sound->_destroy		();
}

void CSoundManager::OnFrame()
{
	::psSoundVEffects	= psDeviceFlags.is(rsMuteSounds)?0.f:1.f;
	Sound->update		(Device.m_Camera.GetPosition(), Device.m_Camera.GetDirection(), Device.m_Camera.GetNormal(), Device.fTimeDelta);
}

void CSoundManager::RefreshEnvLibrary()
{
	Sound->refresh_env_library();
    SetEnvGeometry		();
}

