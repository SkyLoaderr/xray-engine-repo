#include "stdafx.h"
#pragma hdrstop

#include "ImageManager.h"
#include "ETextureParams.h"          
#include "ImageThumbnail.h"
#include "xrImage_Resampler.h"
#include "freeimage.h"
#include "Image.h"
#include "ui_main.h"
#include "EditObject.h"
CImageManager ImageManager;
//---------------------------------------------------------------------------
#pragma package(smart_init)
extern bool IsFormatRegister(LPCSTR ext);
extern FIBITMAP* Surface_Load(char* full_name);
extern "C" __declspec(dllimport)
bool DXTCompress(LPCSTR out_name, u8* raw_data, u32 w, u32 h, u32 pitch,
				 STextureParams* options, u32 depth);

bool IsValidSize(u32 w, u32 h){
	if (!btwIsPow2(h)) return false;
    if (h*6==w) return true;
	if (!btwIsPow2(w)) return false;
    return true;
}
                 
bool Surface_Load(LPCSTR full_name, U32Vec& data, u32& w, u32& h, u32& a)
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
		CopyMemory			(data.begin(),img.pData,sizeof(u32)*data.size());
		if (!IsValidSize(w,h))	ELog.Msg(mtError,"Texture (%s) - invalid size: [%d, %d]",full_name,w,h);
        return true;
    }else{
        FIBITMAP* bm 		= Surface_Load((LPSTR)full_name);
        if (bm){
            w 				= FreeImage_GetWidth (bm);
            h 				= FreeImage_GetHeight(bm);
		    u32 w4			= w*4;
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
// ������� ���
//------------------------------------------------------------------------------
void CImageManager::MakeThumbnailImage(EImageThumbnail* THM, u32* data, u32 w, u32 h, u32 a)
{
	R_ASSERT(THM);
	// create thumbnail
    if (THM->m_Pixels.empty()) THM->m_Pixels.resize(THUMB_SIZE);
	THM->m_TexParams.width = w;
	THM->m_TexParams.height= h;
    THM->m_TexParams.flags.set(STextureParams::flHasAlpha,a);
	imf_Process(THM->m_Pixels.begin(),THUMB_WIDTH,THUMB_HEIGHT,data,THM->_Width(),THM->_Height(),imf_box);
    THM->VFlip();
}

//------------------------------------------------------------------------------
// ������� ���
//------------------------------------------------------------------------------
void CImageManager::CreateTextureThumbnail(EImageThumbnail* THM, const AnsiString& src_name, FSPath* path, bool bSetDefParam){
	R_ASSERT(src_name.Length());
	AnsiString base_name 		= src_name;
    if (path)	path->Update	(base_name);
    else		Engine.FS.m_Textures.Update	(base_name);
    U32Vec data;
    u32 w, h, a;
    if (!Surface_Load(base_name.c_str(),data,w,h,a)){
    	ELog.DlgMsg(mtError,"Can't load texture '%s'.\nCheck file existance",src_name.c_str());
     	return;
    }
    MakeThumbnailImage(THM,data.begin(),w,h,a);

    // ��������� ��������� ���������
	if (bSetDefParam){
		THM->m_Age 			= Engine.FS.GetFileAge(base_name);
		THM->m_TexParams.fmt= (a)?STextureParams::tfDXT3:STextureParams::tfDXT1;
	    if ((h*6)==w) THM->m_TexParams.type	= STextureParams::ttCubeMap;
    }
}
//------------------------------------------------------------------------------
// ������� ����� ��������
//------------------------------------------------------------------------------
void CImageManager::CreateGameTexture(const AnsiString& src_name, EImageThumbnail* thumb){
	R_ASSERT(src_name.Length());
    EImageThumbnail* THM 	= thumb?thumb:xr_new<EImageThumbnail>(src_name.c_str(),EImageThumbnail::EITTexture);
	AnsiString base_name 	= src_name;       
	AnsiString game_name 	= ChangeFileExt(src_name,".dds");
	Engine.FS.m_Textures.Update(base_name);
	Engine.FS.m_GameTextures.Update(game_name);
    int base_age 			= Engine.FS.GetFileAge(base_name);

    U32Vec data;
    u32 w, h, a;
    if (!Surface_Load(base_name.c_str(),data,w,h,a)) return;
    MakeGameTexture(THM,game_name.c_str(),data.begin());

    Engine.FS.SetFileAge(game_name, base_age);
    if (!thumb) xr_delete(THM);
}

//------------------------------------------------------------------------------
// ������� ������� ��������
//------------------------------------------------------------------------------
void CImageManager::MakeGameTexture(EImageThumbnail* THM, LPCSTR game_name, u32* load_data)
{
	Engine.FS.VerifyPath(game_name);
    // flip
    u32 w = THM->_Width();
    u32 h = THM->_Height();
    u32 w4= w*4;
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
// ��������� 32-bit ������
//------------------------------------------------------------------------------
bool CImageManager::LoadTextureData(const AnsiString& src_name, U32Vec& data, u32& w, u32& h)
{
	AnsiString fn = src_name;
	Engine.FS.m_Textures.Update(fn);
    u32 a;
    if (!Surface_Load(fn.c_str(),data,w,h,a)) return false;
    return true;
}

//------------------------------------------------------------------------------
// �������� ����������� �������� � Import'a � Textures
// files - ������ ������ ��� �����������
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
// ���������� ������ �� ������������������ (����������������) �������
// source_list - �������� ������ ������� � ������������
// sync_list - ������� ����������� ����� (����� ������������� ����������)
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
	    U32Vec data;
    	u32 w, h, a;

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
        	THM = xr_new<EImageThumbnail>(it->first.c_str(),EImageThumbnail::EITTexture);
		    bool bRes = Surface_Load(fn.c_str(),data,w,h,a); R_ASSERT(bRes);
            MakeThumbnailImage(THM,data.begin(),w,h,a);
            THM->Save	(it->second);
        }
        // check game textures
    	if (bForceGame||(sync_game&&bGame)){
        	if (!THM) THM = xr_new<EImageThumbnail>(it->first.c_str(),EImageThumbnail::EITTexture);
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
		if (THM) xr_delete(THM);
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
// ���������� ������ ���� �������
//------------------------------------------------------------------------------
int CImageManager::GetTextures(FileMap& files)
{
    AnsiString p_base;
    Engine.FS.m_Textures.Update(p_base);
    if (0==Engine.FS.GetFileList(p_base.c_str(),files,true,false,false,"*.tga,*.bmp")) return 0;
    return files.size();
}
//------------------------------------------------------------------------------
// ���������� ������ �������, ������� ����� ��������
//------------------------------------------------------------------------------
int CImageManager::GetLocalNewTextures(FileMap& files)
{
    AnsiString p_base;
    Engine.FS.m_Import.Update(p_base);
    if (0==Engine.FS.GetFileList(p_base.c_str(),files,true,false,true,"*.tga,*.bmp")) return 0;
    return files.size();
}
//------------------------------------------------------------------------------
// ��������� ������������ ������� �������
// input: 	������ ������ ��� ������������
// output: 	������������
//------------------------------------------------------------------------------
#define SQR(a) ((a)*(a))
BOOL CImageManager::CheckCompliance(LPCSTR fname, int& compl)
{
	compl 			= 0;
    U32Vec data;
    u32 w, h, a;
    if (!Surface_Load(fname,data,w,h,a)) return FALSE;
    if (1==w & 1==h)					 return TRUE;

    u32 w_2 	= (1==w)?w:w/2;
    u32 h_2 	= (1==h)?h:h/2;
    // scale down(lanczos3) and up (bilinear, as video board)
    u32* pScaled     = (u32*)(xr_malloc((w_2)*(h_2)*4));
    u32* pRestored   = (u32*)(xr_malloc(w*h*4));
    try {
    	imf_Process     (pScaled,	w_2,h_2,data.begin(),w,h,imf_lanczos3	);
        imf_Process		(pRestored,	w,h,pScaled,w_2,h_2,imf_filter 		    );
    } catch (...)
    {
        Msg             ("* ERROR: imf_Process");
        xr_free   (pScaled);
        xr_free   (pRestored);
        return  FALSE;
    }
    // Analyze
    float 		difference	= 0;
    float 		maximal 	= 0;
    for (u32 p=0; p<data.size(); p++)
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
    xr_free			(pScaled);
    xr_free   		(pRestored);
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

IC void CopyLODImage(U32Vec& src, U32Vec& dest, u32 src_w, u32 src_h, int id, int pitch)
{
	for (u32 y=0; y<src_h; y++)
    	CopyMemory(dest.begin()+y*pitch+id*src_w,src.begin()+y*src_w,src_w*sizeof(u32));
}

IC void GET(U32Vec& pixels, u32 w, u32 h, u32 x, u32 y, u32 ref, u32 &count, u32 &r, u32 &g, u32 &b)
{
    // wrap pixels
    if (x<0) return; else if (x>=w)	return;
	if (y<0) return; else if (y>=h)	return;

    // summarize
    u32 pixel = pixels[y*w + x];
    if (RGBA_GETALPHA(pixel)<=ref) return;

    r+=RGBA_GETRED  (pixel);
    g+=RGBA_GETGREEN(pixel);
    b+=RGBA_GETBLUE (pixel);
    count++;
}

BOOL ApplyBorders(U32Vec& pixels, u32 w, u32 h, u32 ref)
{
    BOOL    bNeedContinue = FALSE;

    try {
        U32Vec result;
        result.resize(w*h);

        CopyMemory(result.begin(),pixels.begin(),w*h*4);
        for (u32 y=0; y<h; y++){
            for (u32 x=0; x<w; x++){
                if (RGBA_GETALPHA(pixels[y*w+x])==0) {
                    u32 C=0,r=0,g=0,b=0;
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

void CImageManager::CreateLODTexture(Fbox bbox, LPCSTR tex_name, u32 tgt_w, u32 tgt_h, int samples, int age)
{
    u32 src_w=tgt_w,src_h=tgt_h;
	U32Vec pixels;

    Fvector C;
    Fvector S;
    bbox.getradius(S);
    float R = max(S.x,S.z);
    bbox.getcenter(C);

    Fmatrix save_projection	= Device.mProjection;
    Fvector save_pos 		= Device.m_Camera.GetPosition();
    Fvector save_hpb 		= Device.m_Camera.GetHPB();
    float save_far		 	= Device.m_Camera._Zfar();
	ECameraStyle save_style = Device.m_Camera.GetStyle();

    float D		= 500.f;
    U32Vec new_pixels;
    new_pixels.resize(src_w*src_h*samples);
	Device.m_Camera.SetStyle(csPlaneMove);
    Device.m_Camera.SetDepth(D*2,true);

    u32 pitch 		= src_w*samples;

    // save render params
    Flags32 old_flag= 	psDeviceFlags;
    // set render params

    dwClearColor 	= 	0x0000000;
	psDeviceFlags.set(rsStatistic|rsDrawGrid|rsLighting|rsFog,FALSE);
	psDeviceFlags.set(rsFilterLinear,TRUE);

    SetCamera(0,C,S.y,R,D);

    for (int frame=0; frame<samples; frame++){
    	float angle = frame*(PI_MUL_2/samples);
	    SetCamera(angle,C,S.y,R,D);
	    Device.MakeScreenshot(pixels,src_w,src_h);
        // copy LOD to final
		for (u32 y=0; y<src_h; y++)
    		CopyMemory(new_pixels.begin()+y*pitch+frame*src_w,pixels.begin()+y*src_w,src_w*sizeof(u32));
    }
    U32Vec border_pixels = new_pixels;
    for (U32It it=new_pixels.begin(); it!=new_pixels.end(); it++)
        *it=(RGBA_GETALPHA(*it)>200)?subst_alpha(*it,0xFF):subst_alpha(*it,0x00);
    for (u32 ref=254; ref>0; ref--)
        ApplyBorders(new_pixels,pitch,src_h,ref);
    for (int t=0; t<int(new_pixels.size()); t++)
        new_pixels[t]=subst_alpha(new_pixels[t],RGBA_GETALPHA(border_pixels[t]));

    AnsiString out_name=tex_name;
    Engine.FS.m_Textures.Update(out_name);
    Engine.FS.VerifyPath(out_name.c_str());
    
    CImage* I = xr_new<CImage>();
    I->Create	(tgt_w*samples,tgt_h,new_pixels.begin());
    I->Vflip	();
    I->SaveTGA	(out_name.c_str());
    xr_delete		(I);
    Engine.FS.SetFileAge(out_name.c_str(), age);

    SynchronizeTexture(tex_name,age);

    // restore render params
    psDeviceFlags 	= old_flag;
    dwClearColor	= DEFAULT_CLEARCOLOR;

	Device.m_Camera.SetStyle(save_style);
    Device.SetTransform	(D3DTS_PROJECTION,save_projection);
    Device.m_Camera.Set(save_hpb,save_pos);
    Device.m_Camera.Set(save_hpb,save_pos);
    Device.m_Camera.SetDepth(save_far,false);
}

BOOL CImageManager::CreateOBJThumbnail(LPCSTR tex_name, CEditableObject* obj, int age)
{
	BOOL bResult = TRUE;
    // save render params
    Flags32 old_flag= 	psDeviceFlags;
    // set render params
    psDeviceFlags.set(rsStatistic|rsDrawGrid,FALSE);

	U32Vec pixels;
    u32 w=256,h=256;
    if (Device.MakeScreenshot(pixels,w,h)){
        EImageThumbnail tex(tex_name,EImageThumbnail::EITObject,false);
        tex.CreateFromData(pixels.begin(),w,h,obj->GetFaceCount(),obj->GetVertexCount());
        tex.Save(age);
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
        AnsiString game_name 		= ChangeFileExt(fname,".dds");
        Engine.FS.DeleteFileByName	(&Engine.FS.m_GameTextures,game_name.c_str());
        return TRUE;
    }
    return FALSE;
}

