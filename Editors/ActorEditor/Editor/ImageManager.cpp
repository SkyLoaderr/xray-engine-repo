#include "stdafx.h"
#pragma hdrstop

#include "ImageManager.h"
#include "ETextureParams.h"          
#include "ImageThumbnail.h"
#include "xrImage_Resampler.h"
#include "freeimage.h"
#include "Image.h"
#include "ui_main.h"
CImageManager ImageManager;
//---------------------------------------------------------------------------
#pragma package(smart_init)
extern bool IsFormatRegister(LPCSTR ext);
extern FIBITMAP* Surface_Load(char* full_name);
extern "C" __declspec(dllimport)
bool DXTCompress(LPCSTR out_name, BYTE* raw_data, DWORD w, DWORD h, DWORD pitch,
				 STextureParams* options, DWORD depth);

bool IsValidSize(int w, int h){
	if (!btwIsPow2(h)) return false;
    if (h*6==w) return true;
	if (!btwIsPow2(w)) return false;
    return true;
}
                 
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
		if (!IsValidSize(w,h))	ELog.Msg(mtError,"Texture (%s) - invalid size: [%d, %d]",full_name,w,h);
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
			if (!IsValidSize(w,h))	ELog.Msg(mtError,"Texture (%s) - invalid size: [%d, %d]",full_name,w,h);
            return true;
        }
    }
	return false;
}

//------------------------------------------------------------------------------
// создает тхм
//------------------------------------------------------------------------------
void CImageManager::MakeThumbnailImage(EImageThumbnail* THM, DWORD* data, int w, int h, int a)
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
// создает тхм
//------------------------------------------------------------------------------
void CImageManager::CreateTextureThumbnail(EImageThumbnail* THM, const AnsiString& src_name, FSPath* path){
	R_ASSERT(src_name.Length());
	AnsiString base_name 		= src_name;
    if (path)	path->Update	(base_name);
    else		Engine.FS.m_Textures.Update	(base_name);
    DWORDVec data;
    int w, h, a;
    if (!Surface_Load(base_name.c_str(),data,w,h,a)){
    	ELog.DlgMsg(mtError,"Can't load texture '%s'.\nCheck file existance",src_name.c_str());
     	return;
    }
    MakeThumbnailImage(THM,data.begin(),w,h,a);

    // проверить если тхумбнайла не было выставить начальные параметры
    AnsiString 	fn 	= ChangeFileExt	(base_name,".thm");
	if (!Engine.FS.Exist(fn.c_str())){
		THM->m_Age 			= Engine.FS.GetFileAge(base_name);
		THM->m_TexParams.fmt= (a)?STextureParams::tfDXT3:STextureParams::tfDXT1;
	    if ((h*6)==w) THM->m_TexParams.type	= STextureParams::ttCubeMap;
    }
}
/*
	if (!Engine.FS.Exist(fn.c_str())){
    	ELog.DlgMsg(mtError,"Can't load texture '%s'.\nCheck file existance",fn.c_str());
        return;
    }
	bool bRes = Surface_Load(fn.c_str(),data,w,h,a);
    if (!bRes){
    	ELog.DlgMsg(mtError,"Can't load texture '%s'.\nCheck file existance or texture size.",fn.c_str(),w,h);
		return;
    }
*/
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
        // copy thm
		AnsiString fn = ChangeFileExt(it->first,".thm");
		src_name 	= p_import	+ AnsiString(fn);
		Engine.FS.UpdateTextureNameWithFolder(fn);
		dest_name 	= p_textures+ AnsiString(fn);
		Engine.FS.MoveFileTo	(src_name.c_str(),dest_name.c_str(),true);
    	// copy sources
		fn 			= it->first;
		src_name 	= p_import	+ AnsiString(fn);
		Engine.FS.UpdateTextureNameWithFolder(fn);
		dest_name 	= p_textures+ AnsiString(fn);
        Engine.FS.BackupFile	(&Engine.FS.m_Textures,AnsiString(fn));
		Engine.FS.CopyFileTo	(src_name.c_str(),dest_name.c_str(),true);
        Engine.FS.WriteAccessLog(dest_name.c_str(),"Replace");
        Engine.FS.MarkFile		(src_name,true);
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
	    DWORDVec data;
    	int w, h, a;

        string256 base_name; strcpy(base_name,it->first.c_str());
        UI.ProgressInc(base_name);
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
            MakeThumbnailImage(THM,data.begin(),w,h,a);
            THM->Save	(it->second);
        }
        // check game textures
    	if (bForceGame||(sync_game&&bGame)){
        	if (!THM) THM = new EImageThumbnail(it->first.c_str(),EImageThumbnail::EITTexture);
            if (data.empty()){ bool bRes = Surface_Load(fn.c_str(),data,w,h,a); R_ASSERT(bRes);}
			if (IsValidSize(w,h)){
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

void CImageManager::SynchronizeTexture(LPCSTR tex_name, int age)
{
    LPSTRVec modif;
    FileMap t_map;
    t_map[tex_name]=age;
    SynchronizeTextures(true,true,true,&t_map,&modif);
   	Device.RefreshTextures(&modif);
    ImageManager.FreeModifVec(modif);
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
        string256 base_name; strcpy(base_name,it->first.c_str());
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

IC void SetCamera(float angle, const Fvector& C, float height, float radius, float dist)
{
    Fvector 	D;
    Fvector 	hpb;
    Fmatrix 	P;

	hpb.set		(angle,0,0);
	D.setHP		(hpb.x,hpb.y);
    D.mul		(-dist);
    D.add		(C);

    float ta	= height/dist;
    float asp 	= height/radius;
	float fp	= dist+4.f*radius;
    float np	= dist-4.f*radius; clamp(np,0.1f,fp);
    Device.m_Camera.Set		(hpb,D);
    P.build_projection_HAT	(ta,asp,np,fp);
    Device.SetTransform		(D3DTS_PROJECTION,P);
}

IC void CopyLODImage(DWORDVec& src, DWORDVec& dest, int src_w, int src_h, int id, int pitch)
{
	for (int y=0; y<src_h; y++)
    	CopyMemory(dest.begin()+y*pitch+id*src_w,src.begin()+y*src_w,src_w*sizeof(DWORD));
}

IC void GET(DWORDVec& pixels, int w, int h, int x, int y, DWORD ref, DWORD &count, DWORD &r, DWORD &g, DWORD &b)
{
    // wrap pixels
    if (x<0) return; else if (x>=w)	return;
	if (y<0) return; else if (y>=h)	return;

    // summarize
    DWORD pixel = pixels[y*w + x];
    if (RGBA_GETALPHA(pixel)<=ref) return;

    r+=RGBA_GETRED  (pixel);
    g+=RGBA_GETGREEN(pixel);
    b+=RGBA_GETBLUE (pixel);
    count++;
}

BOOL ApplyBorders(DWORDVec& pixels, int w, int h, DWORD ref)
{
    BOOL    bNeedContinue = FALSE;

    try {
        DWORDVec result;
        result.resize(w*h);

        CopyMemory(result.begin(),pixels.begin(),w*h*4);
        for (int y=0; y<h; y++){
            for (int x=0; x<w; x++){
                if (RGBA_GETALPHA(pixels[y*w+x])==0) {
                    DWORD C=0,r=0,g=0,b=0;
                    GET(pixels,w,h,x-1,y-1,ref,C,r,g,b);
                    GET(pixels,w,h,x  ,y-1,ref,C,r,g,b);
                    GET(pixels,w,h,x+1,y-1,ref,C,r,g,b);

                    GET(pixels,w,h,x-1,y  ,ref,C,r,g,b);
                    GET(pixels,w,h,x+1,y  ,ref,C,r,g,b);

                    GET(pixels,w,h,x-1,y+1,ref,C,r,g,b);
                    GET(pixels,w,h,x  ,y+1,ref,C,r,g,b);
                    GET(pixels,w,h,x+1,y+1,ref,C,r,g,b);

                    if (C) {
                        result[y*w+x]	= RGBA_MAKE(r/C,g/C,b/C,ref);
                        bNeedContinue 	= TRUE;
                    }
                }
            }
        }
        CopyMemory(pixels.begin(),result.begin(),h*w*4);
    } catch (...)
    {
        Msg("* ERROR: ApplyBorders");
    }
    return bNeedContinue;
}

void CImageManager::CreateLODTexture(Fbox bbox, LPCSTR tex_name, int tgt_w, int tgt_h, int samples, int age)
{
    DWORD src_w=tgt_w,src_h=tgt_h;
	DWORDVec pixels;

    Fvector C;
    Fvector S;
    bbox.getradius(S);
    float R = max(S.x,S.z);// sqrtf(S.x*S.x+S.z*S.z);
    bbox.getcenter(C);

    Fmatrix save_projection	= Device.mProjection;
    Fvector save_pos 		= Device.m_Camera.GetPosition();
    Fvector save_hpb 		= Device.m_Camera.GetHPB();
	ECameraStyle save_style = Device.m_Camera.GetStyle();

    float D		= 500.f;
    DWORDVec new_pixels;
    new_pixels.resize(src_w*src_h*samples);
	Device.m_Camera.SetStyle(csPlaneMove);

    int pitch 		= src_w*samples;

    // save render params
    DWORD old_flag 	= 	psDeviceFlags;
    // set render params
    dwClearColor 	= 	0x0000000;
	psDeviceFlags 	&=~	rsStatistic;
    psDeviceFlags 	&=~	rsDrawGrid;
    psDeviceFlags 	|= 	rsFilterLinear;
    psDeviceFlags 	&=~	rsLighting;
    psDeviceFlags 	&=~	rsFog;

    for (int frame=0; frame<samples; frame++){
    	float angle = frame*(PI_MUL_2/samples);
	    SetCamera(angle,C,S.y,R,D);
	    Device.MakeScreenshot(pixels,src_w,src_h);
        // copy LOD to final
		for (DWORD y=0; y<src_h; y++)
    		CopyMemory(new_pixels.begin()+y*pitch+frame*src_w,pixels.begin()+y*src_w,src_w*sizeof(DWORD));
    }
    // restore render params
    psDeviceFlags 	= old_flag;
    dwClearColor	= DEFAULT_CLEARCOLOR;

    DWORDVec border_pixels = new_pixels;
    for (DWORDIt it=new_pixels.begin(); it!=new_pixels.end(); it++)
        *it=(RGBA_GETALPHA(*it)>200)?subst_alpha(*it,0xFF):subst_alpha(*it,0x00);
    for (DWORD ref=254; ref>0; ref--)
        ApplyBorders(new_pixels,pitch,src_h,ref);
    for (int t=0; t<int(new_pixels.size()); t++)
        new_pixels[t]=subst_alpha(new_pixels[t],RGBA_GETALPHA(border_pixels[t]));

    AnsiString out_name=tex_name;
    Engine.FS.m_Textures.Update(out_name);
    Engine.FS.VerifyPath(out_name.c_str());
    
    CImage* I = new CImage();
    I->Create	(tgt_w*samples,tgt_h,new_pixels.begin());
    I->Vflip	();
    I->SaveTGA	(out_name.c_str());
    _DELETE		(I);
    Engine.FS.SetFileAge(out_name.c_str(), age);
    ELog.Msg(mtInformation,"LOD texture created.");

    SynchronizeTexture(tex_name,age);

	Device.m_Camera.SetStyle(save_style);
    Device.SetTransform	(D3DTS_PROJECTION,save_projection);
    Device.m_Camera.Set(save_hpb,save_pos);
}

BOOL CImageManager::CreateOBJThumbnail(LPCSTR tex_name, int age)
{
	BOOL bResult = TRUE;
    // save render params
    DWORD old_flag 	= 	psDeviceFlags;
    // set render params
	psDeviceFlags 	&=~	rsStatistic;
    psDeviceFlags 	&=~	rsDrawGrid;

	DWORDVec pixels;
    DWORD w=256,h=256;
    if (Device.MakeScreenshot(pixels,w,h)){
        EImageThumbnail tex(tex_name,EImageThumbnail::EITObject,false);
        tex.CreateFromData(pixels.begin(),w,h);
        tex.Save(age);
        ELog.Msg(mtInformation,"Thumbnail created.");
    }else{
    	bResult = FALSE;
        ELog.DlgMsg(mtError,"Can't make screenshot.");
    }
    
    // restore render params
    psDeviceFlags 	= old_flag;
    return bResult;
}

BOOL CImageManager::RemoveTexture(LPCSTR fname)
{
	AnsiString src_name=fname;
    Engine.FS.m_Textures.Update(src_name);
	if (Engine.FS.Exist(src_name.c_str())){
    	// source
        Engine.FS.BackupFile		(&Engine.FS.m_Textures,fname);
        Engine.FS.DeleteFileByName	(src_name.c_str());
        Engine.FS.WriteAccessLog	(src_name.c_str(),"Remove");
        // thumbnail
        AnsiString thm_name 		= ChangeFileExt(fname,".thm");
        Engine.FS.BackupFile		(&Engine.FS.m_Textures,thm_name.c_str());
        Engine.FS.DeleteFileByName	(&Engine.FS.m_Textures,thm_name.c_str());
        // game
        AnsiString game_name 		= ChangeFileExt(fname,".thm");
        Engine.FS.DeleteFileByName	(&Engine.FS.m_GameTextures,game_name.c_str());
        return TRUE;
    }
    return FALSE;
}

