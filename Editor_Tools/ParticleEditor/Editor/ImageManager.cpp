#include "stdafx.h"
#pragma hdrstop

#include "ImageManager.h"
#include "ETextureParams.h"
#include "ImageThumbnail.h"
#include "xrImage_Resampler.h"
#include "freeimage.h"
#include "UI_Main.h"
CImageManager ImageManager;
//---------------------------------------------------------------------------
#pragma package(smart_init)
extern bool IsFormatRegister(LPCSTR ext);
extern FIBITMAP* Surface_Load(char* full_name);
extern "C" __declspec(dllexport)
bool DXTCompress(LPCSTR out_name, BYTE* raw_data, DWORD w, DWORD h, DWORD pitch,
				 STextureParams* options, DWORD depth);

//------------------------------------------------------------------------------
// проверяет было ли изменение
//------------------------------------------------------------------------------
/*
BOOL CImageManager::IfChanged(const AnsiString& name)
{
	AnsiString base_name 	= name;
	AnsiString thm_name		= name; thm_name 	= ChangeFileExt(thm_name,".thm");
	AnsiString game_name	= name; game_name 	= ChangeFileExt(game_name,".dds");
	FS.m_Textures.Update	(base_name);
	FS.m_Textures.Update	(thm_name);
	FS.m_GameTextures.Update(game_name);

	// get file ages
    int base_age 	= FS.GetFileAge(base_name);
    int thm_age 	= FS.GetFileAge(thm_name);
    int game_age 	= FS.GetFileAge(game_name);

    R_ASSERT2(base_age,"Can' find base texture.");

    return ((base_age!=thm_age)||(base_age!=game_age));
}
*/

//------------------------------------------------------------------------------
// создает тхм
//------------------------------------------------------------------------------
void CImageManager::MakeThumbnail(EImageThumbnail* THM, FIBITMAP* bm)
{
	R_ASSERT(THM);
	// create thumbnail
    if (THM->m_Pixels.empty()) THM->m_Pixels.resize(THUMB_SIZE);
	THM->m_TexParams.width = FreeImage_GetWidth (bm);
	THM->m_TexParams.height= FreeImage_GetHeight(bm);
	imf_Process(THM->m_Pixels.begin(),THUMB_WIDTH,THUMB_HEIGHT,(LPDWORD)FreeImage_GetScanLine(bm,0),THM->_Width(),THM->_Height(),imf_box);
}

//------------------------------------------------------------------------------
// создает новый тхм
// использовать в случае если не было такого файла
//------------------------------------------------------------------------------
void CImageManager::CreateThumbnail(EImageThumbnail* THM, const AnsiString& src_name){
	R_ASSERT(src_name.Length());
	AnsiString base_name 	= src_name;
	FS.m_Textures.Update	(base_name);
    FIBITMAP* bm=Surface_Load(base_name.c_str()); R_ASSERT(bm);
    MakeThumbnail(THM,bm);
	THM->m_Age = FS.GetFileAge(base_name);
	THM->m_TexParams.fmt = (FIC_RGBALPHA==FreeImage_GetColorType(bm))?STextureParams::tfDXT3:STextureParams::tfDXT1;
    if (bm) FreeImage_Free(bm);
}

//------------------------------------------------------------------------------
// создает новую текстуру
//------------------------------------------------------------------------------
void CImageManager::CreateGameTexture(const AnsiString& src_name, EImageThumbnail* thumb){
	R_ASSERT(src_name.Length());
    EImageThumbnail* THM 	= thumb?thumb:new EImageThumbnail(src_name.c_str(),EImageThumbnail::EITTexture);
	AnsiString base_name 	= src_name;
	AnsiString game_name 	= ChangeFileExt(src_name,".dds");
	FS.m_Textures.Update(base_name);
	FS.m_GameTextures.Update(game_name);
    int base_age 	= FS.GetFileAge(base_name);
    FIBITMAP* bm=Surface_Load(base_name.c_str()); R_ASSERT(bm);
    MakeGameTexture(THM,game_name.c_str(),bm);
    if (bm) FreeImage_Free(bm);
    FS.SetFileAge(game_name, base_age);
    if (!thumb) delete THM;
}

//------------------------------------------------------------------------------
// создает игровую текстуру
//------------------------------------------------------------------------------
void CImageManager::MakeGameTexture(EImageThumbnail* THM, LPCSTR game_name, FIBITMAP* bm)
{
	FS.VerifyPath(game_name);
    // flip
    DWORDVec data;
    int w = THM->_Width();
    int h = THM->_Height();
    int w4= w*4;
    data.resize(w*h);
    for (int y=h-1; y>=0; y--) CopyMemory(data.begin()+(h-y-1)*w,FreeImage_GetScanLine(bm,y),w4);
    // compress
    bool bRes 	= DXTCompress(game_name, (LPBYTE)data.begin(), w, h, w4, &THM->m_TexParams, 4);
    R_ASSERT(bRes&&FS.FileLength(game_name));
}

//------------------------------------------------------------------------------
// загружает 32-bit данные
//------------------------------------------------------------------------------
bool CImageManager::LoadTextureData(const AnsiString& src_name, DWORDVec& data, int& w, int& h)
{
	AnsiString fn = src_name;
	FS.m_Textures.Update(fn);
    FIBITMAP* bm = Surface_Load(fn.c_str());
    if (!bm) return false;
    // flip
	w = FreeImage_GetWidth (bm);
	h = FreeImage_GetHeight(bm);
    int w4= w*4;
    data.resize(w*h);
    for (int y=h-1; y>=0; y--) CopyMemory(data.begin()+(h-y-1)*w,FreeImage_GetScanLine(bm,y),w4);
    FreeImage_Free(bm);
    return true;
}

//------------------------------------------------------------------------------
// синхронизирует все три файла на диске
//------------------------------------------------------------------------------
/*
void CImageManager::SynchronizeTextures()
{
	AnsiString base_name 	= name;
	AnsiString thm_name		= name; thm_name 	= ChangeFileExt(thm_name,".thm");
	AnsiString game_name	= name; game_name 	= ChangeFileExt(game_name,".dds");
	FS.m_Textures.Update	(base_name);
	FS.m_Textures.Update	(thm_name);
	FS.m_GameTextures.Update(game_name);

	// get file ages
    int base_age 	= FS.GetFileAge(base_name);
    int thm_age 	= FS.GetFileAge(thm_name);
    int game_age 	= FS.GetFileAge(game_name);

    R_ASSERT(base_age);

    FIBITMAP* bm=0;
	EImageThumbnail THM(name.c_str(),EImageThumbnail::EITTexture);
    // check THM version
    if (!thm_age||(base_age!=thm_age)){
    	bm = Surface_Load(base_name.c_str()); R_ASSERT(bm);
        MakeThumbnail(&THM,bm);
	    // save
    	THM.Save	(base_age);
    }

	// check DDS version
    if (!game_age||(base_age!=game_age)||(0==FS.FileLength(game_name.c_str()))){
    	if (!bm) bm = Surface_Load(base_name.c_str()); R_ASSERT(bm);
	    MakeGameTexture(&THM,game_name.c_str(),bm);
	    // change date-time
    	FS.SetFileAge(game_name, base_age);
    }
    if (bm) FreeImage_Free(bm);
}

//------------------------------------------------------------------------------
// синхронизиция каталогов
//------------------------------------------------------------------------------
/*
void CImageManager::SynchronizePath()
{
	FSPath P;
    AStringVec& lst	= FS.GetDirectories(FS.m_Textures.m_Path);
    AnsiString thm	= FS.m_TexturesThumbnail.m_Path;
    for (AStringIt it=lst.begin(); it!=lst.end(); it++){
        if (it->Pos(thm)==1) continue;
    	*it = it->Delete(1,strlen(FS.m_Textures.m_Path));
	   	P.Init  ( FS.m_GameTextures.m_Path, it->c_str(), "", "" ); 		P.VerifyPath();
	   	P.Init  ( FS.m_TexturesThumbnail.m_Path, it->c_str(), "", "" ); P.VerifyPath();
    }
}
*/

//------------------------------------------------------------------------------
// возвращает список не синхронизированных (модифицированных) текстур
//------------------------------------------------------------------------------
void CImageManager::SynchronizeTextures(LPSTRVec* files)
{
	FindDataMap M_BASE;
	FindDataMap M_THUM;
    FindDataMap M_GAME;

    AnsiString p_base;
    AnsiString p_game;
    FS.m_Textures.Update(p_base);
    FS.m_GameTextures.Update(p_game);

    if (0==FS.GetFiles(p_base.c_str(),M_BASE,true,false,"*.tga,*.bmp")) return;
    FS.GetFiles(p_base.c_str(),M_THUM,true,true,"*.thm");
    FS.GetFiles(p_game.c_str(),M_GAME,true,true,"*.dds");

    UI.ProgressStart(M_BASE.size(),"Synchronize textures...");
	for (FindDataPairIt it=M_BASE.begin(); it!=M_BASE.end(); it++){
        UI.ProgressInc();
		FIBITMAP* bm=0;

        sh_name base_name; strcpy(base_name,it->first.c_str());
        AnsiString fn = it->first;
        FS.m_Textures.Update(fn);
        if (strext(base_name)) *strext(base_name)=0;

		FindDataPairIt th = M_THUM.find(base_name);
    	bool bThm = ((th==M_THUM.end()) || ((th!=M_THUM.end())&&(th->second!=it->second)));
		FindDataPairIt gm = M_GAME.find(base_name);
    	bool bGame= ((gm==M_GAME.end()) || ((gm!=M_GAME.end())&&(gm->second!=it->second)));

        if (!bThm&&!bGame){
        	continue;
        }

		EImageThumbnail THM(it->first.c_str(),EImageThumbnail::EITTexture);

    	// check thumbnail
    	if (bThm){
			bm = Surface_Load(fn.c_str()); R_ASSERT(bm);
            MakeThumbnail(&THM,bm);
            THM.Save	(it->second);
        }
        // check game textures
    	if (bGame){
            if (!bm) bm = Surface_Load(fn.c_str()); R_ASSERT(bm);
            AnsiString game_name=AnsiString(base_name)+".dds";
            FS.m_GameTextures.Update(game_name);
            MakeGameTexture(&THM,game_name.c_str(),bm);
            FS.SetFileAge(game_name, it->second);
            if (files) files->push_back(strdup(base_name));
		}
		if (bm) FreeImage_Free(bm);
		if (UI.NeedAbort()) break;
    }
    UI.ProgressEnd();
}

int	CImageManager::GetModifiedFiles(AStringVec& files)
{
	FindDataMap M_BASE;
	FindDataMap M_THUM;
    FindDataMap M_GAME;

    AnsiString p_base;
    AnsiString p_game;
    FS.m_Textures.Update(p_base);
    FS.m_GameTextures.Update(p_game);

    if (0==FS.GetFiles(p_base.c_str(),M_BASE,true,false,"*.tga,*.bmp")) return 0;
    FS.GetFiles(p_base.c_str(),M_THUM,true,true,"*.thm");
    FS.GetFiles(p_game.c_str(),M_GAME,true,true,"*.dds");

	for (FindDataPairIt it=M_BASE.begin(); it!=M_BASE.end(); it++){
        sh_name base_name; strcpy(base_name,it->first.c_str());
        if (strext(base_name)) *strext(base_name)=0;
    	// check thumbnail
		FindDataPairIt th = M_THUM.find(base_name);
    	if ((th==M_THUM.end()) || ((th!=M_THUM.end())&&(th->second!=it->second))){
        	files.push_back(it->first);
            continue;
        }
        // check game textures
		FindDataPairIt gm = M_GAME.find(base_name);
    	if ((gm==M_GAME.end()) || ((gm!=M_GAME.end())&&(gm->second!=it->second))){
        	files.push_back(it->first);
            continue;
        }
    }
    return files.size();
}
//------------------------------------------------------------------------------
// возвращает список всех текстур
//------------------------------------------------------------------------------
int CImageManager::GetFiles(AStringVec& files)
{
	FindDataMap M_BASE;
    AnsiString p_base;
    FS.m_Textures.Update(p_base);

    if (0==FS.GetFiles(p_base.c_str(),M_BASE,true,false,"*.tga,*.bmp")) return 0;

    files.resize(M_BASE.size());
    AStringIt s_it=files.begin();
	for (FindDataPairIt it=M_BASE.begin(); it!=M_BASE.end(); it++,s_it++)
		*s_it = it->first;
    return files.size();
}

