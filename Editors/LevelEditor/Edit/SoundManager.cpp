#include "stdafx.h"
#pragma hdrstop

#include "SoundManager.h"
#include "ETextureParams.h"          
#include "ImageThumbnail.h"
#include "xrImage_Resampler.h"
#include "freeimage.h"
#include "UI_Main.h"
#include "Image.h"
CSoundManager SoundManager;
//---------------------------------------------------------------------------
#pragma package(smart_init)

//------------------------------------------------------------------------------
// возвращает список всех звуков
//------------------------------------------------------------------------------
int CSoundManager::GetSounds(FileMap& files)
{
    AnsiString p_base;
    Engine.FS.m_Sounds.Update(p_base);
    if (0==Engine.FS.GetFileList(p_base.c_str(),files,true,false,false,"*.wav")) return 0;
    return files.size();
}

