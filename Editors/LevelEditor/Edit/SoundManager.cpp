#include "stdafx.h"
#pragma hdrstop

#include "SoundManager.h"
CSoundManager SoundManager;
//---------------------------------------------------------------------------
#pragma package(smart_init)

//------------------------------------------------------------------------------
// возвращает список всех звуков
//------------------------------------------------------------------------------
int CSoundManager::GetSounds(FileMap& files)
{
    AnsiString p_base;
    Engine.FS.m_GameSounds.Update(p_base);
    if (0==Engine.FS.GetFileList(p_base.c_str(),files,true,true,false,"*.wav")) return 0;
    return files.size();
}

