#include "stdafx.h"
#pragma hdrstop

#include "SoundManager.h"
CSoundManager SoundManager;
//---------------------------------------------------------------------------
#pragma package(smart_init)

//------------------------------------------------------------------------------
// возвращает список всех звуков
//------------------------------------------------------------------------------
int CSoundManager::GetSounds(FS_QueryMap& files)
{
    return FS.file_list(files,"$game_sounds$",FS_ListFiles|FS_ClampExt,".wav");
}

