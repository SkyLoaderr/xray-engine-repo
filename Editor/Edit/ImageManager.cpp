#include "stdafx.h"
#pragma hdrstop

#include "ImageManager.h"
#include "ETextureParams.h"
#include "ImageThumbnail.h"
#include "xrImage_Resampler.h"
#include "freeimage.h"
#include "dxtlib.h"

CImageManager ImageManager;
//---------------------------------------------------------------------------
#pragma package(smart_init)
extern FIBITMAP* Surface_Load(char* name);
extern "C" __declspec(dllexport)
bool DXTCompress(LPCSTR out_name, BYTE* raw_data, DWORD w, DWORD h, DWORD pitch,
				 STextureParams* options, DWORD depth);

BOOL CImageManager::IfChanged(LPCSTR name)
{
	AnsiString base_name 	= name;
	AnsiString thm_name		= name; thm_name 	= ChangeFileExt(thm_name,".thm");
	AnsiString game_name	= name; game_name 	= ChangeFileExt(game_name,".dds");
	FS.m_Textures.Update			(base_name);
	FS.m_TexturesThumbnail.Update	(thm_name);
	FS.m_GameTextures.Update		(game_name);

	// get file ages
    int base_age 	= FS.GetFileAge(base_name);
    int thm_age 	= FS.GetFileAge(thm_name);
    int game_age 	= FS.GetFileAge(game_name);

    R_ASSERT(base_age);

    return ((base_age==thm_age)&&(base_age==game_age));
}

void CImageManager::MakeThumbnail(EImageThumbnail* THM, FIBITMAP* bm, int age)
{
	R_ASSERT(THM);
	// create thumbnail
    if (THM->m_Pixels.empty()) THM->m_Pixels.resize(THUMB_SIZE);
	int w		= FreeImage_GetWidth	(bm);
	int h		= FreeImage_GetHeight	(bm);
	imf_Process(THM->m_Pixels.begin(),THUMB_WIDTH,THUMB_HEIGHT,(LPDWORD)FreeImage_GetScanLine(bm,0),w,h,imf_box);
    // save
    THM->Save	(age);
}

void CImageManager::MakeGameTexture(STextureParams* P, LPCSTR game_name, FIBITMAP* bm, int age){
	int w		= FreeImage_GetWidth	(bm);
	int h		= FreeImage_GetHeight	(bm);

    // time check
    bool bRes 	= DXTCompress(game_name, FreeImage_GetScanLine(bm,0), w, h, w*4, P, 4);
    R_ASSERT(bRes&&FS.FileLength(game_name));
    // change date-time
    FS.SetFileAge(game_name, age);
}

void CImageManager::Synchronize(LPCSTR name)
{
	AnsiString base_name 	= name;
	AnsiString thm_name		= name; thm_name 	= ChangeFileExt(thm_name,".thm");
	AnsiString game_name	= name; game_name 	= ChangeFileExt(game_name,".dds");
	FS.m_Textures.Update			(base_name);
	FS.m_TexturesThumbnail.Update	(thm_name);
	FS.m_GameTextures.Update		(game_name);

	// get file ages
    int base_age 	= FS.GetFileAge(base_name);
    int thm_age 	= FS.GetFileAge(thm_name);
    int game_age 	= FS.GetFileAge(game_name);

    R_ASSERT(base_age);

    FIBITMAP* bm=0;
	EImageThumbnail THM(thm_name.c_str());
    // check THM version
    if (!thm_age||(base_age!=thm_age)){
    	bm = Surface_Load(base_name.c_str()); R_ASSERT(bm);
        MakeThumbnail(&THM,bm,base_age);
    }

	// check DDS version
    if (!game_age||(base_age!=game_age)||(0==FS.FileLength(game_name.c_str()))){
    	if (!bm) bm = Surface_Load(base_name.c_str()); R_ASSERT(bm);
	    MakeGameTexture(&THM.m_TexParams,game_name.c_str(),bm,base_age);
    }
    if (bm) FreeImage_Free(bm);
}

