#include "stdafx.h"
#pragma hdrstop

#include "ImageManager.h"
#include "ETextureParams.h"
#include "ImageThumbnail.h"
#include "xrImage_Resampler.h"
#include "freeimage.h"
#include "UI_Main.h"
#include "Image.h"
CImageManager ImageManager;
//---------------------------------------------------------------------------
#pragma package(smart_init)
extern bool IsFormatRegister(LPCSTR ext);
extern FIBITMAP* Surface_Load(char* full_name);
extern "C" __declspec(dllimport)
bool DXTCompress(LPCSTR out_name, BYTE* raw_data, DWORD w, DWORD h, DWORD pitch,
				 STextureParams* options, DWORD depth);

bool Surface_Load(LPCSTR full_name, DWORDVec& data, int& w, int& h, int& a)
{
    if (!FS.Exist(full_name,true)) return false;
	AnsiString ext = ExtractFileExt(full_name).LowerCase();
	if (ext==".tga"){
    	CImage img;
        img.LoadTGA			(full_name);
		w 					= img.dwWidth;
        h 					= img.dwHeight;
        a					= img.bAlpha;
        data.resize			(w*h);
		CopyMemory			(data.begin(),img.pData,sizeof(DWORD)*data.size());
        return true;
    }else{
        FIBITMAP* bm 		= Surface_Load((LPSTR)full_name);
        if (bm){
            w 				= FreeImage_GetWidth (bm);
            h 				= FreeImage_GetHeight(bm);
		    int w4			= w*4;
            data.resize		(w*h);
            for (int y=h-1; y>=0; y--) CopyMemory(data.begin()+(h-y-1)*w,FreeImage_GetScanLine(bm,y),w4);
            a				= FIC_RGBALPHA==FreeImage_GetColorType(bm);
            FreeImage_Free	(bm);
            return true;
        }
    }
	return false;
}

//------------------------------------------------------------------------------
// ������� ���
//------------------------------------------------------------------------------
void CImageManager::MakeThumbnail(EImageThumbnail* THM, DWORD* data, int w, int h, int a)
{
	R_ASSERT(THM);
	// create thumbnail
    if (THM->m_Pixels.empty()) THM->m_Pixels.resize(THUMB_SIZE);
	THM->m_TexParams.width = w;
	THM->m_TexParams.height= h;
    if (a) 	THM->m_TexParams.flag |= STextureParams::flHasAlpha;
    else	THM->m_TexParams.flag &=~STextureParams::flHasAlpha;
	imf_Process(THM->m_Pixels.begin(),THUMB_WIDTH,THUMB_HEIGHT,data,THM->_Width(),THM->_Height(),imf_box);
    THM->VFlip();
}

//------------------------------------------------------------------------------
// ������� ����� ���
// ������������ � ������ ���� �� ���� ������ �����
//------------------------------------------------------------------------------
void CImageManager::CreateThumbnail(EImageThumbnail* THM, const AnsiString& src_name){
	R_ASSERT(src_name.Length());
	AnsiString base_name 	= src_name;
	FS.m_Textures.Update	(base_name);
    DWORDVec data;
    int w, h, a;
    if (!Surface_Load(base_name.c_str(),data,w,h,a)) return;
    MakeThumbnail(THM,data.begin(),w,h,a);
	THM->m_Age = FS.GetFileAge(base_name);
	THM->m_TexParams.fmt 	= (a)?STextureParams::tfDXT3:STextureParams::tfDXT1;
}

//------------------------------------------------------------------------------
// ������� ����� ��������
//------------------------------------------------------------------------------
void CImageManager::CreateGameTexture(const AnsiString& src_name, EImageThumbnail* thumb){
	R_ASSERT(src_name.Length());
    EImageThumbnail* THM 	= thumb?thumb:new EImageThumbnail(src_name.c_str(),EImageThumbnail::EITTexture);
	AnsiString base_name 	= src_name;
	AnsiString game_name 	= ChangeFileExt(src_name,".dds");
	FS.m_Textures.Update(base_name);
	FS.m_GameTextures.Update(game_name);
    int base_age 			= FS.GetFileAge(base_name);

    DWORDVec data;
    int w, h, a;
    if (!Surface_Load(base_name.c_str(),data,w,h,a)) return;
    MakeGameTexture(THM,game_name.c_str(),data.begin());

    FS.SetFileAge(game_name, base_age);
    if (!thumb) delete THM;
}

//------------------------------------------------------------------------------
// ������� ������� ��������
//------------------------------------------------------------------------------
void CImageManager::MakeGameTexture(EImageThumbnail* THM, LPCSTR game_name, DWORD* load_data)
{
	FS.VerifyPath(game_name);
    // flip
    int w = THM->_Width();
    int h = THM->_Height();
    int w4= w*4;
    // compress
    bool bRes 	= DXTCompress(game_name, (LPBYTE)load_data, w, h, w4, &THM->m_TexParams, 4);
    R_ASSERT(bRes&&FS.FileLength(game_name));
}

//------------------------------------------------------------------------------
// ��������� 32-bit ������
//------------------------------------------------------------------------------
bool CImageManager::LoadTextureData(const AnsiString& src_name, DWORDVec& data, int& w, int& h)
{
	AnsiString fn = src_name;
	FS.m_Textures.Update(fn);
    int a;
    if (!Surface_Load(fn.c_str(),data,w,h,a)) return false;
    return true;
}

void CImageManager::SynchronizeThumbnail(EImageThumbnail* THM, LPCSTR src_name)
{
    DWORDVec data;
    int w, h, a;
	AnsiString fn = src_name;
	FS.m_Textures.Update(fn);
	R_ASSERT(FS.Exist(fn.c_str()));
	bool bRes = Surface_Load(fn.c_str(),data,w,h,a); R_ASSERT(bRes);
	MakeThumbnail(THM,data.begin(),w,h,a);
}
//------------------------------------------------------------------------------
// ���������� ������ �� ������������������ (����������������) �������
// source_list - �������� ������ ������� � ������������
// sync_list - ������� ����������� ����� (����� ������������� ����������)
//------------------------------------------------------------------------------
void CImageManager::SynchronizeTextures(bool sync_thm, bool sync_game, bool bForceGame, FileMap* source_list, LPSTRVec* sync_list)
{
	FileMap M_BASE;
	FileMap M_THUM;
    FileMap M_GAME;

    AnsiString p_base;
    AnsiString p_game;
    FS.m_Textures.Update(p_base);
    FS.m_GameTextures.Update(p_game);

    if (source_list) M_BASE = *source_list;
    else FS.GetFiles(p_base.c_str(),M_BASE,true,false,"*.tga,*.bmp");
    if (M_BASE.empty()) return;
    if (sync_thm) FS.GetFiles(p_base.c_str(),M_THUM,true,true,"*.thm");
    if (sync_game) FS.GetFiles(p_game.c_str(),M_GAME,true,true,"*.dds");

    UI.ProgressStart(M_BASE.size(),"Synchronize textures...");
    FilePairIt it=M_BASE.begin();
	FilePairIt _E = M_BASE.end();
	for (; it!=_E; it++){
        UI.ProgressInc();
	    DWORDVec data;
    	int w, h, a;

        sh_name base_name; strcpy(base_name,it->first.c_str());
        AnsiString fn = it->first;
        FS.m_Textures.Update(fn);
        if (strext(base_name)) *strext(base_name)=0;

		FilePairIt th = M_THUM.find(base_name);
    	bool bThm = ((th==M_THUM.end()) || ((th!=M_THUM.end())&&(th->second!=it->second)));
		FilePairIt gm = M_GAME.find(base_name);
    	bool bGame= bThm || ((gm==M_GAME.end()) || ((gm!=M_GAME.end())&&(gm->second!=it->second)));

		EImageThumbnail* THM=0;

    	// check thumbnail
    	if (sync_thm&&bThm){
        	THM = new EImageThumbnail(it->first.c_str(),EImageThumbnail::EITTexture);
		    bool bRes = Surface_Load(fn.c_str(),data,w,h,a); R_ASSERT(bRes);
            MakeThumbnail(THM,data.begin(),w,h,a);
            THM->Save	(it->second);
        }
        // check game textures
    	if (bForceGame||(sync_game&&bGame)){
        	if (!THM) THM = new EImageThumbnail(it->first.c_str(),EImageThumbnail::EITTexture);
            if (data.empty()){ bool bRes = Surface_Load(fn.c_str(),data,w,h,a); R_ASSERT(bRes);}
            AnsiString game_name=AnsiString(base_name)+".dds";
            FS.m_GameTextures.Update(game_name);
            MakeGameTexture(THM,game_name.c_str(),data.begin());
            FS.SetFileAge(game_name, it->second);
            if (sync_list) sync_list->push_back(strdup(base_name));
		}
		if (THM) _DELETE(THM);
		if (UI.NeedAbort()) break;
    }
    UI.ProgressEnd();
}

int	CImageManager::GetModifiedTextures(FileMap& files)
{
	FileMap M_BASE;
	FileMap M_THUM;
    FileMap M_GAME;

    AnsiString p_base;
    AnsiString p_game;
    FS.m_Textures.Update(p_base);
    FS.m_GameTextures.Update(p_game);

    if (0==FS.GetFiles(p_base.c_str(),M_BASE,true,false,"*.tga,*.bmp")) return 0;
    FS.GetFiles(p_base.c_str(),M_THUM,true,true,"*.thm");
    FS.GetFiles(p_game.c_str(),M_GAME,true,true,"*.dds");

    FilePairIt it=M_BASE.begin();
	FilePairIt _E = M_BASE.end();
	for (; it!=_E; it++){
        sh_name base_name; strcpy(base_name,it->first.c_str());
        if (strext(base_name)) *strext(base_name)=0;
    	// check thumbnail
		FilePairIt th = M_THUM.find(base_name);
    	if ((th==M_THUM.end()) || ((th!=M_THUM.end())&&(th->second!=it->second))){
        	files.insert(*it);
            continue;
        }
        // check game textures
		FilePairIt gm = M_GAME.find(base_name);
    	if ((gm==M_GAME.end()) || ((gm!=M_GAME.end())&&(gm->second!=it->second))){
        	files.insert(*it);
            continue;
        }
    }
    return files.size();
}
//------------------------------------------------------------------------------
// ���������� ������ ���� �������
//------------------------------------------------------------------------------
int CImageManager::GetTextures(FileMap& files)
{
    AnsiString p_base;
    FS.m_Textures.Update(p_base);
    if (0==FS.GetFiles(p_base.c_str(),files,true,false,"*.tga,*.bmp")) return 0;
    return files.size();
}

