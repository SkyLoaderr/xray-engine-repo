#include "stdafx.h"
#pragma hdrstop

#include "ImageManager.h"
#include "xrImage_Resampler.h"
#include "freeimage.h"
#include "Image.h"
#include "ui_main.h"
#include "EditObject.h"
#include "ResourceManager.h"
CImageManager ImageLib;
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
    if (!FS.exist(full_name)){ 
    	ELog.Msg(mtError,"Can't find file: '%s'",full_name);
    	return false;
    }
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

AnsiString CImageManager::UpdateFileName(AnsiString& fn)
{
	return EFS.AppendFolderToName(fn,1,TRUE);
}

//------------------------------------------------------------------------------
// ������� ���
//------------------------------------------------------------------------------
void CImageManager::MakeThumbnailImage(ETextureThumbnail* THM, u32* data, u32 w, u32 h, u32 a)
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
void CImageManager::CreateTextureThumbnail(ETextureThumbnail* THM, const AnsiString& src_name, LPCSTR initial, bool bSetDefParam){
	R_ASSERT(src_name.Length());
	AnsiString base_name;
    if (initial)	FS.update_path(base_name,initial,src_name.c_str());
    else			FS.update_path(base_name,_textures_,src_name.c_str());
    U32Vec data;
    u32 w, h, a;
    if (!Surface_Load(base_name.c_str(),data,w,h,a)){
    	ELog.Msg(mtError,"Can't load texture '%s'.\nCheck file existence",src_name.c_str());
     	return;
    }
    MakeThumbnailImage(THM,data.begin(),w,h,a);

    // ��������� ��������� ���������
	if (bSetDefParam){
		THM->m_Age 			= FS.get_file_age(base_name.c_str());
		THM->m_TexParams.fmt= (a)?STextureParams::tfDXT3:STextureParams::tfDXT1;
	    if ((h*6)==w){ 
        	THM->m_TexParams.type	= STextureParams::ttCubeMap;
        	THM->m_TexParams.flags.set(STextureParams::flGenerateMipMaps,FALSE);
        }
    }
}
//------------------------------------------------------------------------------
// ������� ����� ��������
//------------------------------------------------------------------------------
void CImageManager::CreateGameTexture(const AnsiString& src_name, ETextureThumbnail* thumb)
{
	R_ASSERT(src_name.Length());
    ETextureThumbnail* THM 	= thumb?thumb:xr_new<ETextureThumbnail>(src_name.c_str());
	AnsiString base_name 	= src_name;
	AnsiString game_name 	= ChangeFileExt(src_name,".dds");
	FS.update_path			(_textures_,base_name);
	FS.update_path			(_game_textures_,game_name);
    int base_age 			= FS.get_file_age(base_name.c_str());

    U32Vec data;
    u32 w, h, a;
    if (!Surface_Load(base_name.c_str(),data,w,h,a)) return;
    MakeGameTexture(THM,game_name.c_str(),data.begin());

    FS.set_file_age(game_name.c_str(), base_age);
    if (!thumb) xr_delete(THM);
}

//------------------------------------------------------------------------------
// ������� ������� ��������
//------------------------------------------------------------------------------
void CImageManager::MakeGameTexture(LPCSTR game_name, u32* data, u32 w, u32 h, STextureParams::ETFormat fmt, bool bGenMipMap)
{
	VerifyPath(game_name);
    // fill texture params
    STextureParams TP;
    TP.fmt 			= fmt;
    TP.mip_filter   = STextureParams::dMIPFilterBox;
    TP.type			= STextureParams::ttImage;
    TP.width		= w;
    TP.height		= h;
    TP.flags.set	(STextureParams::flGenerateMipMaps,bGenMipMap);
    TP.flags.set	(STextureParams::flDitherColor,TRUE);
	// compress
    u32 w4= w*4;
    bool bRes 		= DXTCompress(game_name, (u8*)data, w, h, w4, &TP, 4);
    if (!bRes){
    	FS.file_delete(game_name);
    	ELog.DlgMsg(mtError,"Can't make game texture '%s'.\nCheck texture size (%dx%d).",game_name,w,h);
		return;
    }
    R_ASSERT(bRes&&FS.file_length(game_name));
}
void CImageManager::MakeGameTexture(ETextureThumbnail* THM, LPCSTR game_name, u32* load_data)
{
	VerifyPath(game_name);
    // flip
    u32 w = THM->_Width();
    u32 h = THM->_Height();
    u32 w4= w*4;
    // compress
    bool bRes 	= DXTCompress(game_name, (u8*)load_data, w, h, w4, &THM->m_TexParams, 4);
    if (!bRes){
    	FS.file_delete(game_name);
    	ELog.DlgMsg(mtError,"Can't make game texture '%s'.\nCheck texture size (%dx%d).",game_name,w,h);
		return;
    }
    R_ASSERT(bRes&&FS.file_length(game_name));
}

//------------------------------------------------------------------------------
// ��������� 32-bit ������
//------------------------------------------------------------------------------
bool CImageManager::LoadTextureData(const AnsiString& src_name, U32Vec& data, u32& w, u32& h)
{
	AnsiString fn;
	FS.update_path			(fn,_textures_,src_name.c_str());
    u32 a;
    if (!Surface_Load(fn.c_str(),data,w,h,a)) return false;
    return true;
}

//------------------------------------------------------------------------------
// �������� ����������� �������� � Import'a � Textures
// files - ������ ������ ��� �����������
//------------------------------------------------------------------------------
void CImageManager::SafeCopyLocalToServer(FS_QueryMap& files)
{
    AnsiString p_import;
    AnsiString p_textures;
    AnsiString src_name,dest_name;
    FS.update_path			(p_import,_import_,"");
    FS.update_path			(p_textures,_textures_,"");

    FS_QueryPairIt it	= files.begin();
	FS_QueryPairIt _E 	= files.end();
	for (; it!=_E; it++){
        // copy thm
		AnsiString fn = ChangeFileExt(it->first,".thm");
		src_name 	= p_import	+ AnsiString(fn);
		UpdateFileName(fn);
		dest_name 	= p_textures+ AnsiString(fn);
		FS.file_rename(src_name.c_str(),dest_name.c_str(),true);
    	// copy sources
		fn 			= it->first;
		src_name 	= p_import	+ fn;
		UpdateFileName(fn);
		dest_name 	= p_textures+ ChangeFileExt(fn,".tga");
        if (FS.exist(dest_name.c_str()))
	        EFS.BackupFile	(_textures_,ChangeFileExt(fn,".tga"));
        if (ExtractFileExt(src_name)==".tga"){
			FS.file_copy(src_name.c_str(),dest_name.c_str());
        }else{
        	// convert to TGA
            U32Vec data;
            u32 w,h,a;
		    R_ASSERT	(Surface_Load(src_name.c_str(),data,w,h,a));
            CImage* I 	= xr_new<CImage>();
            I->Create	(w,h,data.begin());
            I->Vflip	();
            I->SaveTGA	(dest_name.c_str());
            xr_delete	(I);
        }
        FS.set_file_age		(dest_name.c_str(), FS.get_file_age(src_name.c_str()));
        EFS.WriteAccessLog	(dest_name.c_str(),"Replace");
        EFS.MarkFile		(src_name,true);
    }
}    
//------------------------------------------------------------------------------
// ���������� ������ �� ������������������ (����������������) �������
// source_list - �������� ������ ������� � ������������
// sync_list - ������� ����������� ����� (����� ������������� ����������)
//------------------------------------------------------------------------------
void CImageManager::SynchronizeTextures(bool sync_thm, bool sync_game, bool bForceGame, FS_QueryMap* source_list, AStringVec* sync_list, FS_QueryMap* modif_map)
{   
	FS_QueryMap M_BASE;
	FS_QueryMap M_THUM;
    FS_QueryMap M_GAME;

    if (source_list) M_BASE = *source_list;
    else FS.file_list(M_BASE,_textures_,FS_ListFiles,".tga,.bmp");
    if (M_BASE.empty()) return;
    if (sync_thm) 	FS.file_list(M_THUM,_textures_,FS_ListFiles|FS_ClampExt,".thm");
    if (sync_game) 	FS.file_list(M_GAME,_game_textures_,FS_ListFiles|FS_ClampExt,".dds");

    bool bProgress = M_BASE.size()>1;
    
    // lock rescanning
    FS.lock_rescan	();
    
    // sync assoc
	AnsiString ltx_nm;
    FS.update_path(ltx_nm,_game_textures_,"textures.ltx");
	CInifile* ltx_ini = xr_new<CInifile>(ltx_nm.c_str(), FALSE, TRUE, TRUE);
    
    if (bProgress) UI.ProgressStart(M_BASE.size(),"Synchronize textures...");
    FS_QueryPairIt it=M_BASE.begin();
	FS_QueryPairIt _E = M_BASE.end();
	for (; it!=_E; it++){
	    U32Vec data;
    	u32 w, h, a;

        AnsiString base_name	=it->first.LowerCase();
        if (bProgress) 			UI.ProgressInc(base_name.c_str());
        AnsiString fn;
        FS.update_path			(fn,_textures_,base_name.c_str());
    	if (!FS.exist(fn.c_str())) continue;
        base_name				= ChangeFileExt(base_name,"");

		FS_QueryPairIt th 	= M_THUM.find(base_name);
    	bool bThm = ((th==M_THUM.end()) || ((th!=M_THUM.end())&&(th->second.modif!=it->second.modif)));
  		FS_QueryPairIt gm = M_GAME.find(base_name);
    	bool bGame= bThm || ((gm==M_GAME.end()) || ((gm!=M_GAME.end())&&(gm->second.modif!=it->second.modif)));

		ETextureThumbnail* THM=0;

    	// check thumbnail
    	if (sync_thm&&bThm){
        	THM = xr_new<ETextureThumbnail>(it->first.c_str());
		    bool bRes = Surface_Load(fn.c_str(),data,w,h,a); R_ASSERT(bRes);
            MakeThumbnailImage(THM,data.begin(),w,h,a);
            THM->Save	(it->second.modif);
        }
        // check game textures
    	if (bForceGame||(sync_game&&bGame)){
        	if (!THM) THM = xr_new<ETextureThumbnail>(it->first.c_str()); 
            R_ASSERT(THM);
            if (data.empty()){ bool bRes = Surface_Load(fn.c_str(),data,w,h,a); R_ASSERT(bRes);}
			if (IsValidSize(w,h)){
                AnsiString game_name=AnsiString(base_name)+".dds";
                FS.update_path			(_game_textures_,game_name);
                MakeGameTexture(THM,game_name.c_str(),data.begin());
                FS.set_file_age(game_name.c_str(), it->second.modif);
                if (sync_list) sync_list->push_back(base_name);
                if (modif_map) modif_map->insert(*it);
                // save to assoc ltx
                STextureParams& FMT = THM->_Format();
                if (FMT.flags.is(STextureParams::flHasDetailTexture)){
                    AnsiString det;                          
                    det.sprintf("%s, %f",FMT.detail_name,FMT.detail_scale);
                    ltx_ini->w_string("association", base_name.c_str(), det.c_str());
                }else{
                    ltx_ini->remove_line("association", base_name.c_str());
                }
            }else{
		    	ELog.DlgMsg(mtError,"Can't make game texture '%s'.\nInvalid size (%dx%d).",fn.c_str(),w,h);
            }
		}
		if (THM) xr_delete(THM);
		if (UI.NeedAbort()) break;
    }

    xr_delete(ltx_ini);
    
    if (bProgress) UI.ProgressEnd();
    // lock rescanning
    FS.unlock_rescan	();
}

void CImageManager::SynchronizeTexture(LPCSTR tex_name, int age)
{
    AStringVec modif;
    FS_QueryMap t_map;
    t_map.insert		(mk_pair(tex_name,FS_QueryItem(0,age,0)));
    SynchronizeTextures	(true,true,true,&t_map,&modif);
    RefreshTextures		(&modif);
}
/*
int	CImageManager::GetServerModifiedTextures(FS_QueryMap& files)
{
	FileMap M_BASE;
	FileMap M_THUM;
    FileMap M_GAME;

    AnsiString p_base;
    AnsiString p_game;
    FS.update_path			(p_base,_textures_,"");
    FS.update_path			(p_game,_game_textures_,"");

    if (0==EFS.GetFileList(p_base.c_str(),M_BASE,true,false,false,"*.tga,*.bmp")) return 0;
    EFS.GetFileList(p_base.c_str(),M_THUM,true,true,false,"*.thm");
    EFS.GetFileList(p_game.c_str(),M_GAME,true,true,false,"*.dds");

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
*/
//------------------------------------------------------------------------------
// ���������� ������ ���� �������
//------------------------------------------------------------------------------
int CImageManager::GetTextures(FS_QueryMap& files, BOOL bFolders)
{                	
    return FS.file_list(files,_textures_,(bFolders?FS_ListFolders:0)|FS_ListFiles,".tga,.bmp"); 
}
//------------------------------------------------------------------------------
// ���������� ������ �������, ������� ����� ��������
//------------------------------------------------------------------------------
int CImageManager::GetLocalNewTextures(FS_QueryMap& files)
{
    return FS.file_list(files,_import_,FS_ListFiles|FS_RootOnly,".tga,.bmp");
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
    if ((1==w) || (1==h))				 return TRUE;

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
void CImageManager::CheckCompliance(FS_QueryMap& files, FS_QueryMap& compl)
{
	UI.ProgressStart(files.size(),"Check texture compliance: ");
    FS_QueryPairIt it	= files.begin();
	FS_QueryPairIt _E 	= files.end();
	for (; it!=_E; it++){
    	int val	= 0;
        AnsiString 	fname;
        FS.update_path			(fname,_textures_,it->first.c_str());
    	if (!CheckCompliance(fname.c_str(),val))
        	ELog.Msg(mtError,"Bad texture: '%s'",it->first.c_str());
        compl.insert			(mk_pair(it->first,FS_QueryItem(it->second.size,iFloor(val))));
    	UI.ProgressInc	(it->first.c_str());
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
    RCache.set_xform_project(P);
}

IC void CopyLODImage(U32Vec& src, U32Vec& dest, u32 src_w, u32 src_h, int id, int pitch)
{
	for (u32 y=0; y<src_h; y++)
    	CopyMemory(dest.begin()+y*pitch+id*src_w,src.begin()+y*src_w,src_w*sizeof(u32));
}

IC void GET(U32Vec& pixels, u32 w, u32 h, u32 x, u32 y, u32 ref, u32 &count, u32 &r, u32 &g, u32 &b)
{
    // wrap pixels
    if (x>=w)	return;
	if (y>=h)	return;

    // summarize
    u32 pixel = pixels[y*w + x];
    if (color_get_A(pixel)<=ref) return;

    r+=color_get_R  (pixel);
    g+=color_get_G	(pixel);
    b+=color_get_B 	(pixel);
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
                if (color_get_A(pixels[y*w+x])==0) {
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
                        result[y*w+x]	= color_rgba(r/C,g/C,b/C,ref);
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

#include "d3dutils.h"
void CImageManager::CreateLODTexture(Fbox bbox, LPCSTR tex_name, u32 tgt_w, u32 tgt_h, int samples, int age)
{
    u32 src_w=tgt_w,src_h=tgt_h;
	U32Vec pixels;

    Fvector C;
    Fvector S;
    bbox.getradius(S);
    float R = _max(S.x,S.z);
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
        *it=(color_get_A(*it)>200)?subst_alpha(*it,0xFF):subst_alpha(*it,0x00);
    for (u32 ref=254; ref>0; ref--)
        ApplyBorders(new_pixels,pitch,src_h,ref);
    for (int t=0; t<int(new_pixels.size()); t++)
        new_pixels[t]=subst_alpha(new_pixels[t],color_get_A(border_pixels[t]));

    AnsiString out_name;
    FS.update_path			(out_name,_textures_,tex_name);

    CImage* I 	= xr_new<CImage>();
    I->Create	(tgt_w*samples,tgt_h,new_pixels.begin());
    I->Vflip	();
    I->SaveTGA	(out_name.c_str());
    xr_delete	(I);
    FS.set_file_age(out_name.c_str(), age);

    SynchronizeTexture(tex_name,age);

    // restore render params
    psDeviceFlags 	= old_flag;
    dwClearColor	= DEFAULT_CLEARCOLOR;

	Device.m_Camera.SetStyle(save_style);
    RCache.set_xform_project(save_projection);
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
    dwClearColor 	= 	0x00333333;

	U32Vec pixels;
    u32 w=256,h=256;
    if (Device.MakeScreenshot(pixels,w,h)){
        EObjectThumbnail tex(tex_name,false);
        tex.CreateFromData(pixels.begin(),w,h,obj->GetFaceCount(),obj->GetVertexCount());
        tex.Save(age);
    }else{
    	bResult = FALSE;
        ELog.DlgMsg(mtError,"Can't make screenshot.");
    }

    // restore render params
    psDeviceFlags 	= old_flag;
    dwClearColor	= DEFAULT_CLEARCOLOR;
    return bResult;
}

BOOL CImageManager::RemoveTexture(LPCSTR fname, EItemType type)
{
	if (TYPE_FOLDER==type){
    	FS.dir_delete			(_textures_,fname,FALSE);
    	FS.dir_delete			(_game_textures_,fname,FALSE);
		return TRUE;
    }else if (TYPE_OBJECT==type){
        AnsiString src_name;
        FS.update_path			(src_name,_textures_,fname);
        if (FS.exist(src_name.c_str())){
            AnsiString base_name= ChangeFileExt(fname,"");
            AnsiString thm_name = ChangeFileExt(fname,".thm");
            AnsiString game_name= ChangeFileExt(fname,".dds");
            // source
            EFS.BackupFile		(_textures_,fname);
            FS.file_delete		(src_name.c_str());
            EFS.WriteAccessLog	(src_name.c_str(),"Remove");
            // thumbnail
            EFS.BackupFile		(_textures_,thm_name.c_str());
            FS.file_delete		(_textures_,thm_name.c_str());
            // game
            FS.file_delete		(_game_textures_,game_name.c_str());
            // assoc
            AnsiString ltx_nm;
            FS.update_path		(ltx_nm,_game_textures_,"textures.ltx");
            CInifile* ltx_ini 	= xr_new<CInifile>(ltx_nm.c_str(), FALSE, TRUE, TRUE);
            ltx_ini->remove_line("association", base_name.c_str());
            xr_delete			(ltx_ini);
            return TRUE;
        }
    }
    return FALSE;
}

EImageThumbnail* CImageManager::CreateThumbnail(LPCSTR src_name, ECustomThumbnail::THMType type, bool bLoad)
{
    switch (type){
    case ECustomThumbnail::ETObject: 	return xr_new<EObjectThumbnail>	(src_name,bLoad);
    case ECustomThumbnail::ETTexture:	return xr_new<ETextureThumbnail>(src_name,bLoad);
    default: NODEFAULT;
    }
    return 0;
}

//------------------------------------------------------------------------------
// ���� ������� �������� modif - ��������� DX-Surface only � ������ �� ������
// ����� ������ �������������
//------------------------------------------------------------------------------
void CImageManager::RefreshTextures(AStringVec* modif)
{
    if (modif) Device.Resources->ED_UpdateTextures(modif);
	else{
		UI.SetStatus("Refresh textures...");
    	AStringVec modif_files;
    	ImageLib.SynchronizeTextures(true,true,false,0,&modif_files);
        Device.Resources->ED_UpdateTextures(&modif_files);
		UI.SetStatus("");
    }
}


