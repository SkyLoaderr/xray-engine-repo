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

bool CSoundManager::OnCreate()
{
	psSoundFreq			= sf_22K;
    Sound->_initialize	((u32)Device.m_hWnd);
}

void CSoundManager::OnDestroy()
{
    Sound->_destroy		();
}

void CSoundManager::OnFrame()
{
	Sound->update(Device.m_Camera.GetPosition(), Device.m_Camera.GetDirection(), Device.m_Camera.GetNormal(), Device.fTimeDelta);
}
