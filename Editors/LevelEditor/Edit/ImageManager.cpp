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
    if (!Engine.FS.Exist(full_name,true)) return false;
	AnsiString ext = ExtractFileExt(full_name).LowerCase();
	if (ext==".tga"){                     
    	CImage img;
        if (!img.LoadTGA	(full_name)) return false;
		w 					= img.dwWidth;
        h 					= img.dwHeight;
        a					= img.bAlpha;
        data.resize			(w*h);
		CopyMemory			(data.begin(),img.pData,sizeof(DWORD)*data.size());
		if (!btwIsPow2(w))	ELog.Msg(mtError,"Texture (%s) - invalid width: %d",full_name,w);
		if (!btwIsPow2(h))	ELog.Msg(mtError,"Texture (%s) - invalid height: %d",full_name,h);
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
            if (!btwIsPow2(w))	ELog.Msg(mtError,"Texture (%s) - invalid width: %d",full_name,w);
            if (!btwIsPow2(h))	ELog.Msg(mtError,"Texture (%s) - invalid height: %d",full_name,h);
            return true;
        }
    }
	return false;
}

//------------------------------------------------------------------------------
// создает тхм
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
// создает новый тхм
// использовать в случае если не было такого файла
//------------------------------------------------------------------------------
void CImageManager::CreateThumbnail(EImageThumbnail* THM, const AnsiString& src_name){
	R_ASSERT(src_name.Length());
	AnsiString base_name 	= src_name;
	Engine.FS.m_Textures.Update	(base_name);
    DWORDVec data;
    int w, h, a;
    if (!Surface_Load(base_name.c_str(),data,w,h,a)) return;
    MakeThumbnail(THM,data.begin(),w,h,a);
	THM->m_Age = Engine.FS.GetFileAge(base_name);
	THM->m_TexParams.fmt 	= (a)?STextureParams::tfDXT3:STextureParams::tfDXT1;
}

//------------------------------------------------------------------------------
// создает новую текстуру
//------------------------------------------------------------------------------
void CImageManager::CreateGameTexture(const AnsiString& src_name, EImageThumbnail* thumb){
	R_ASSERT(src_name.Length());
    EImageThumbnail* THM 	= thumb?thumb:new EImageThumbnail(src_name.c_str(),EImageThumbnail::EITTexture);
	AnsiString base_name 	= src_name;
	AnsiString game_name 	= ChangeFileExt(src_name,".dds");
	Engine.FS.m_Textures.Update(base_name);
	Engine.FS.m_GameTextures.Update(game_name);
    int base_age 			= Engine.FS.GetFileAge(base_name);

    DWORDVec data;
    int w, h, a;
    if (!Surface_Load(base_name.c_str(),data,w,h,a)) return;
    MakeGameTexture(THM,game_name.c_str(),data.begin());

    Engine.FS.SetFileAge(game_name, base_age);
    if (!thumb) delete THM;
}

//------------------------------------------------------------------------------
// создает игровую текстуру
//------------------------------------------------------------------------------
void CImageManager::MakeGameTexture(EImageThumbnail* THM, LPCSTR game_name, DWORD* load_data)
{
	Engine.FS.VerifyPath(game_name);
    // flip
    int w = THM->_Width();
    int h = THM->_Height();
    int w4= w*4;
    // compress
    bool bRes 	= DXTCompress(game_name, (LPBYTE)load_data, w, h, w4, &THM->m_TexParams, 4);
    if (!bRes){
    	Engine.FS.DeleteFileByName(game_name);
    	ELog.DlgMsg(mtError,"Can't make game texture '%s'.\nCheck texture size (%dx%d).",game_name,w,h);
		return;
    }
    R_ASSERT(bRes&&Engine.FS.FileLength(game_name));
}

//------------------------------------------------------------------------------
// загружает 32-bit данные
//------------------------------------------------------------------------------
bool CImageManager::LoadTextureData(const AnsiString& src_name, DWORDVec& data, int& w, int& h)
{
	AnsiString fn = src_name;
	Engine.FS.m_Textures.Update(fn);
    int a;
    if (!Surface_Load(fn.c_str(),data,w,h,a)) return false;
    return true;
}

void CImageManager::SynchronizeThumbnail(EImageThumbnail* THM, LPCSTR src_name)
{
    DWORDVec data;
    int w, h, a;
	AnsiString fn = src_name;
	Engine.FS.m_Textures.Update(fn);
	if (!Engine.FS.Exist(fn.c_str())){
    	ELog.DlgMsg(mtError,"Can't load texture '%s'.\nCheck file existance",fn.c_str());
        return;
    }
	bool bRes = Surface_Load(fn.c_str(),data,w,h,a);
    if (!bRes){
    	ELog.DlgMsg(mtError,"Can't load texture '%s'.\nCheck file existance or texture size.",fn.c_str(),w,h);
		return;
    }
    R_ASSERT2(bRes,"");
	MakeThumbnail(THM,data.begin(),w,h,a);
}
//------------------------------------------------------------------------------
// копирует обновленные текстуры с Import'a в Textures
// files - список файлов для копирование
//------------------------------------------------------------------------------
void CImageManager::SafeCopyLocalToServer(FileMap& files)
{
    AnsiString p_import;
    AnsiString p_textures;
    AnsiString src_name,dest_name;
    Engine.FS.m_Import.Update(p_import);
    Engine.FS.m_Textures.Update(p_textures);       

    FilePairIt it	= files.begin();
	FilePairIt _E 	= files.end();
	for (; it!=_E; it++){
		string256 fn; strcpy(fn,it->first.c_str());
		src_name 	= p_import	+ AnsiString(fn);
		Engine.FS.UpdateTextureNameWithFolder(fn);
		dest_name 	= p_textures+ AnsiString(fn);
        Engine.FS.BackupFile	(&Engine.FS.m_Textures,AnsiString(fn));
		Engine.FS.CopyFileTo	(src_name.c_str(),dest_name.c_str(),true);
        Engine.FS.WriteAccessLog(dest_name.c_str(),"Replace");
    }
}
//------------------------------------------------------------------------------
// возвращает список не синхронизированных (модифицированных) текстур
// source_list - содержит список текстур с расширениями
// sync_list - реально сохраненные файлы (после использования освободить)
//------------------------------------------------------------------------------
void CImageManager::SynchronizeTextures(bool sync_thm, bool sync_game, bool bForceGame, FileMap* source_list, LPSTRVec* sync_list, FileMap* modif_map)
{
	FileMap M_BASE;
	FileMap M_THUM;
    FileMap M_GAME;

    AnsiString p_base;
    AnsiString p_game;
    Engine.FS.m_Textures.Update(p_base);
    Engine.FS.m_GameTextures.Update(p_game);

    if (source_list) M_BASE = *source_list;
    else Engine.FS.GetFileList(p_base.c_str(),M_BASE,true,false,false,"*.tga,*.bmp");
    if (M_BASE.empty()) return;
    if (sync_thm) Engine.FS.GetFileList(p_base.c_str(),M_THUM,true,true,false,"*.thm");
    if (sync_game) Engine.FS.GetFileList(p_game.c_str(),M_GAME,true,true,false,"*.dds");

    UI.ProgressStart(M_BASE.size(),"Synchronize textures...");
    FilePairIt it=M_BASE.begin();
	FilePairIt _E = M_BASE.end();
	for (; it!=_E; it++){
        UI.ProgressInc();
	    DWORDVec data;
    	int w, h, a;

        sh_name base_name; strcpy(base_name,it->first.c_str());
        AnsiString fn = it->first;
        Engine.FS.m_Textures.Update(fn);
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
			if (btwIsPow2(w)&&btwIsPow2(h)){
                AnsiString game_name=AnsiString(base_name)+".dds";
                Engine.FS.m_GameTextures.Update(game_name);
                MakeGameTexture(THM,game_name.c_str(),data.begin());
                Engine.FS.SetFileAge(game_name, it->second);
                if (sync_list) sync_list->push_back(strdup(base_name));
                if (modif_map) (*modif_map)[it->first]=it->second;
            }else{
		    	ELog.DlgMsg(mtError,"Can't make game texture '%s'.\nInvalid size (%dx%d).",fn.c_str(),w,h);
            }
		}
		if (THM) _DELETE(THM);
		if (UI.NeedAbort()) break;
    }
    UI.ProgressEnd();
}

int	CImageManager::GetServerModifiedTextures(FileMap& files)
{
	FileMap M_BASE;
	FileMap M_THUM;
    FileMap M_GAME;

    AnsiString p_base;
    AnsiString p_game;
    Engine.FS.m_Textures.Update(p_base);
    Engine.FS.m_GameTextures.Update(p_game);

    if (0==Engine.FS.GetFileList(p_base.c_str(),M_BASE,true,false,false,"*.tga,*.bmp")) return 0;
    Engine.FS.GetFileList(p_base.c_str(),M_THUM,true,true,false,"*.thm");
    Engine.FS.GetFileList(p_game.c_str(),M_GAME,true,true,false,"*.dds");

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
// возвращает список всех текстур
//------------------------------------------------------------------------------
int CImageManager::GetTextures(FileMap& files)
{
    AnsiString p_base;
    Engine.FS.m_Textures.Update(p_base);
    if (0==Engine.FS.GetFileList(p_base.c_str(),files,true,false,false,"*.tga,*.bmp")) return 0;
    return files.size();
}
//------------------------------------------------------------------------------
// возвращает список текстур, которые нужно обновить
//------------------------------------------------------------------------------
int CImageManager::GetLocalNewTextures(FileMap& files)
{
    AnsiString p_base;
    Engine.FS.m_Import.Update(p_base);
    if (0==Engine.FS.GetFileList(p_base.c_str(),files,true,false,true,"*.tga,*.bmp")) return 0;
    return files.size();
}
//------------------------------------------------------------------------------
// проверяет соответствие размера текстур
// input: 	список файлов для тестирования
// output: 	соответствие
//------------------------------------------------------------------------------
#define SQR(a) ((a)*(a))
BOOL CImageManager::CheckCompliance(LPCSTR fname, int& compl)
{
	compl 			= 0;
    DWORDVec data;
    int w, h, a;
    if (!Surface_Load(fname,data,w,h,a)) return FALSE;
    if (1==w & 1==h)					 return TRUE;

    DWORD w_2 	= (1==w)?w:w/2;
    DWORD h_2 	= (1==h)?h:h/2;
    // scale down(lanczos3) and up (bilinear, as video board)
    LPDWORD pScaled     = LPDWORD(malloc((w_2)*(h_2)*4));
    LPDWORD pRestored   = LPDWORD(malloc(w*h*4));
    try {
    	imf_Process     (pScaled,	w_2,h_2,data.begin(),w,h,imf_lanczos3	);
        imf_Process		(pRestored,	w,h,pScaled,w_2,h_2,imf_filter 		    );
    } catch (...)
    {
        Msg             ("* ERROR: imf_Process");
        _FREE   (pScaled);
        _FREE   (pRestored);
        return  FALSE;
    }
    // Analyze
    float 		difference	= 0;
    float 		maximal 	= 0;
    for (DWORD p=0; p<data.size(); p++)
    {
        Fcolor 		c1,c2;
    	c1.set		(data[p]);
        c2.set		(pRestored[p]);
        float 	E 	= 0;
        if (a)		E = sqrtf(SQR(c1.r-c2.r)+SQR(c1.g-c2.g)+SQR(c1.b-c2.b)+SQR(c1.a-c2.a))*c1.a;	// q(4)
		else 		E = sqrtf(SQR(c1.r-c2.r)+SQR(c1.g-c2.g)+SQR(c1.b-c2.b));						// q(3)
        difference 	+= 	E;
        maximal 	=  	_max(maximal,E);
    }
    difference		/= 	float(data.size());
    difference		= 	difference/(a ? 2.f : sqrtf(3.f));
    difference		=  	difference*100.f;
    clamp 			(difference,0.f,100.f);
    compl			= 	iFloor(difference)*1000;
    maximal 		=	maximal/(a ? 2.f : sqrtf(3.f));
    maximal			=  	maximal*100.f;
    clamp 			(maximal,0.f,100.f);
    compl			+= 	iFloor(maximal);

    // free
    _FREE           (pScaled);
    _FREE   		(pRestored);
    return 			TRUE;
}
void CImageManager::CheckCompliance(FileMap& files, FileMap& compl)
{
	UI.ProgressStart(files.size(),"Check texture compliance...");
    FilePairIt it	= files.begin();
	FilePairIt _E 	= files.end();
	for (; it!=_E; it++){
    	int val	= 0;
        AnsiString 	fname=it->first.c_str();
        Engine.FS.m_Textures.Update(fname);
    	if (!CheckCompliance(fname.c_str(),val))
        	ELog.Msg(mtError,"Bad texture: '%s'",it->first.c_str());
    	compl[it->first] = iFloor(val);
    	UI.ProgressInc();
		if (UI.NeedAbort()) break;
    }
	UI.ProgressEnd();
}

void CImageManager::CreateLODTexture(Fbox bbox, LPCSTR out_name, int tgt_w, int tgt_h, int age)
{
    int scr_w=320,scr_h=320;
	DWORDVec pixels[4];
    ECameraStyle old_cs = Device.m_Camera.GetStyle();
    Device.m_Camera.SetStyle(cs3DArcBall);
    Device.m_Camera.ViewLeft();

    Fvector sz;
    bbox.getsize(sz);

    Fmatrix projection=Device.mProjection;

    float D		= 5000.f;
    float t_a	= (sz.y/2)/D;
    float asp 	= sz.y/sz.x;
    float near_p= D-2.f*sz.z;
	float far_p	= D+2.f*sz.z;
    Device.mProjection.build_projection_HAT(t_a,asp,near_p,far_p);
    Device.m_Camera.Set(0,0,0,0,0,-D);
//    Device.mProjection.build_projection( 0.5f, 0.82, 0.1, 7000 );

//    Device.SetTransform	(D3DTS_PROJECTION,projection);
    Device.SetTransform	(D3DTS_PROJECTION,Device.mProjection);

    return;

    if (Device.MakeScreenshot(pixels[0],scr_w,scr_h)){
        Device.m_Camera.ViewRight();
        if (Device.MakeScreenshot(pixels[1],scr_w,scr_h)){
            Device.m_Camera.ViewFront();
            if (Device.MakeScreenshot(pixels[2],scr_w,scr_h)){
                Device.m_Camera.ViewBack();
                if (Device.MakeScreenshot(pixels[3],scr_w,scr_h)){
                    int min_w=scr_w, max_w=-scr_w;
                    int min_h=scr_h, max_h=-scr_h;
                    for (int k=0; k<4; k++){
                        int x,y;
                        // left
                        for (x=0; x<scr_w; x++)
                            for (y=0; y<scr_h; y++){
                                BYTE a = (BYTE)(pixels[k][x+y*scr_w]>>24);
                                if (a&&(x<min_w)){
                                    min_w=x;
                                    break;
                                }
                            }
                        // right
                        for (x=scr_w-1; x>=0; x--)
                            for (y=0; y<scr_h; y++){
                                BYTE a = (BYTE)(pixels[k][x+y*scr_w]>>24);
                                if (a&&(x>max_w)){
                                    max_w=x;
                                    break;
                                }
                            }
                        // top
                        for (y=0; y<scr_h; y++)
                            for (x=0; x<scr_w; x++){
                                BYTE a = (BYTE)(pixels[k][x+y*scr_w]>>24);
                                if (a&&(y<min_h)){
                                    min_h=y;
                                    break;
                                }
                            }
                        // bottom
                        for (y=scr_h-1; y>=0; y--)
                            for (x=0; x<scr_w; x++){
                                BYTE a = (BYTE)(pixels[k][x+y*scr_w]>>24);
                                if (a&&(y>max_h)){
                                    max_h=y;
                                    break;
                                }
                            }
                        min_w-=4; max_w+=4;
                        min_h-=4; max_h+=4;

                        clamp(min_w,0,scr_w); clamp(max_w,0,scr_w);
                        clamp(min_h,0,scr_h); clamp(max_h,0,scr_h);

                        int new_w=max_w-min_w, new_h=max_h-min_h;
                        DWORDVec new_pixels;
                        new_pixels.resize(new_w*new_h*4);
                        for (int i=0; i<2; i++){
                            int x_offs = i*new_w;
                            for (int j=0; j<2; j++){
                                int y_offs = j*new_h;
                                for (int y=0; y<new_h; y++)
                                    CopyMemory(new_pixels.begin()+(y+y_offs)*new_w*2+x_offs,pixels[i+j*2].begin()+(y+min_h)*scr_w+min_w,new_w*sizeof(DWORD));
                            }
                        }

                        DWORDVec final_pixels;
                        final_pixels.resize(tgt_h*tgt_w);
                        // scale down(lanczos3) and up (bilinear, as video board)
                        try {
                            imf_Process     (final_pixels.begin(),	tgt_w,tgt_h,new_pixels.begin(),new_w*2,new_h*2,imf_lanczos3);
                        } catch (...)
                        {
                            ELog.DlgMsg(mtError,"* ERROR: imf_Process");
                            return;
                        }

                        CImage* I = new CImage();
                        I->Create(tgt_w,tgt_h,final_pixels.begin());
                        I->Vflip();
                        I->SaveTGA(out_name);
                        _DELETE(I);
                        Engine.FS.SetFileAge(out_name, age);
                        ELog.Msg(mtInformation,"LOD texture created.");
                    }
                }
            }
        }
    }else{
        ELog.DlgMsg(mtError,"Can't make screenshot.");
    }

    Device.m_Camera.ViewFront();
    Device.m_Camera.SetStyle(old_cs);
}

