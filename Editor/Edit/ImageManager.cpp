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
extern bool IsFormatRegister(LPCSTR ext);
extern FIBITMAP* Surface_Load(char* full_name);
extern "C" __declspec(dllexport)
bool DXTCompress(LPCSTR out_name, BYTE* raw_data, DWORD w, DWORD h, DWORD pitch,
				 STextureParams* options, DWORD depth);

//------------------------------------------------------------------------------
// проверяет было ли изменение
//------------------------------------------------------------------------------
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
// создает игровую текстуру
//------------------------------------------------------------------------------
void CImageManager::MakeGameTexture(EImageThumbnail* THM, LPCSTR game_name, FIBITMAP* bm)
{
	FS.VerifyPath(game_name);
    // time check
    bool bRes 	= DXTCompress(game_name, FreeImage_GetScanLine(bm,0), THM->_Width(), THM->_Height(), THM->_Width()*4, &THM->m_TexParams, 4);
    R_ASSERT(bRes&&FS.FileLength(game_name));
}

//------------------------------------------------------------------------------
// синхронизирует все три файла на диске
//------------------------------------------------------------------------------
void CImageManager::Synchronize(const AnsiString& name)
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
int CImageManager::GetModifiedFiles(AStringVec& files)
{
    int count = strlen(FS.m_Textures.m_Path);
    AStringVec& lst = FS.GetFiles(FS.m_Textures.m_Path);
    for (AStringIt it=lst.begin(); it!=lst.end(); it++){
	    AnsiString ext = ExtractFileExt(*it).LowerCase();
        ext = ext.SubString(2,ext.Length());
        if (ext=="thm") continue;
        if (!IsFormatRegister(ext.c_str())) continue;
        AnsiString t = it->Delete(1,count);
		if (IfChanged(t.c_str())) files.push_back(t);
    }
    return files.size();
}

//------------------------------------------------------------------------------
// возвращает список всех текстур
//------------------------------------------------------------------------------
int CImageManager::GetFiles(AStringVec& files)
{
    int count = strlen(FS.m_Textures.m_Path);
    AStringVec& lst = FS.GetFiles(FS.m_Textures.m_Path);
    for (AStringIt it=lst.begin(); it!=lst.end(); it++){
	    AnsiString ext = ExtractFileExt(*it).LowerCase();
//        ext = ext.SubString(2,ext.Length());
        if (ext=="thm") continue;
        if (!IsFormatRegister(ext.c_str())) continue;
     	files.push_back(it->Delete(1,count));
    }
    return files.size();
}

