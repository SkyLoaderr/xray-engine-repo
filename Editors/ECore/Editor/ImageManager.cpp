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
int DXTCompress	(LPCSTR out_name, u8* raw_data, u8* ext_data, u32 w, u32 h, u32 pitch,
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

xr_string CImageManager::UpdateFileName(xr_string& fn)
{
	return EFS.AppendFolderToName(fn,1,TRUE);
}

//------------------------------------------------------------------------------
// создает тхм
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
// создает тхм
//------------------------------------------------------------------------------
void CImageManager::CreateTextureThumbnail(ETextureThumbnail* THM, const AnsiString& src_name, LPCSTR initial, bool bSetDefParam)
{
	R_ASSERT(src_name.Length());
	xr_string base_name;
    if (initial)	FS.update_path(base_name,initial,src_name.c_str());
    else			FS.update_path(base_name,_textures_,src_name.c_str());
    U32Vec data;
    u32 w, h, a;
    if (!Surface_Load(base_name.c_str(),data,w,h,a)){
    	ELog.Msg(mtError,"Can't load texture '%s'.\nCheck file existence",src_name.c_str());
     	return;
    }
    MakeThumbnailImage(THM,data.begin(),w,h,a);

    // выставить начальные параметры
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
// создает новую текстуру
//------------------------------------------------------------------------------
void CImageManager::CreateGameTexture(LPCSTR src_name, ETextureThumbnail* thumb)
{
	R_ASSERT(src_name&&src_name[0]);
    ETextureThumbnail* THM 	= thumb?thumb:xr_new<ETextureThumbnail>(src_name);
	xr_string base_name 	= src_name;
	xr_string game_name 	= EFS.ChangeFileExt(src_name,".dds");
	FS.update_path			(base_name,_textures_,base_name.c_str());
	FS.update_path			(game_name,_game_textures_,game_name.c_str());
    int base_age 			= FS.get_file_age(base_name.c_str());

    U32Vec data;
    u32 w, h, a;
    if (!Surface_Load(base_name.c_str(),data,w,h,a)) return;
    MakeGameTexture(THM,game_name.c_str(),data.begin());

    FS.set_file_age(game_name.c_str(), base_age);
    if (!thumb) xr_delete(THM);
}

//------------------------------------------------------------------------------
// создает игровую текстуру
//------------------------------------------------------------------------------
bool CImageManager::MakeGameTexture(LPCSTR game_name, u32* data, u32 w, u32 h, STextureParams::ETFormat fmt, STextureParams::ETType type, u32 flags)
{
	VerifyPath(game_name);
    // fill texture params
    STextureParams TP;
    TP.fmt 			= fmt;
    TP.mip_filter   = STextureParams::kMIPFilterBox;
    TP.type			= type; //STextureParams::ttImage
    TP.width		= w;
    TP.height		= h;
    TP.flags.assign	(flags);
	// compress
    u32 w4= w*4;
    int res			= DXTCompress(game_name, (u8*)data, 0, w, h, w4, &TP, 4);
    if (1!=res){
    	FS.file_delete(game_name);
        switch(res){
        case 0:		ELog.DlgMsg	(mtError,"Can't make game texture '%s'.",game_name);break;
        case -1000:	ELog.DlgMsg	(mtError,"Can't make game texture '%s'. Invalid gloss mask.",game_name);break;
        }
		return false;
    }
    R_ASSERT((res==1)&&FS.file_length(game_name));
    return res==1;
}
bool CImageManager::MakeGameTexture(ETextureThumbnail* THM, LPCSTR game_name, u32* load_data)
{
	VerifyPath(game_name);
    // flip
    u32 w = THM->_Width();
    u32 h = THM->_Height();
    u32 w4= w*4;
	// remove old
    FS.file_delete			(game_name);
    AnsiString game_name2 	= ChangeFileExt(game_name,"#.dds");
    FS.file_delete			(game_name2.c_str());

    U32Vec 	ext_data;
    if ((THM->m_TexParams.type==STextureParams::ttBumpMap)&&(THM->m_TexParams.ext_normal_map_name.size())){
    	bool e_res			= true;
        LPCSTR e_name		= THM->m_TexParams.ext_normal_map_name.c_str();
        ETextureThumbnail* 	NM_THM = xr_new<ETextureThumbnail>(e_name);
        if (NM_THM->_Format().type==STextureParams::ttImage){
	        if (NM_THM->_Format().fmt==STextureParams::tfRGBA){
            	u32 _w,_h;
                if (!LoadTextureData(e_name,ext_data,_w,_h)){
                    ELog.DlgMsg	(mtError,"Can't load special normal map texture '%s'.",e_name);
                    e_res		= false;
                }else if ((_w!=w)||(_h!=h)){
                    ELog.DlgMsg	(mtError,"Invalid load special normal map size '%s'. It should be [%dx%d]",e_name,w,h);
                    e_res		= false;
                }
        	}else{
                ELog.DlgMsg		(mtError,"Invalid special normal map format '%s'. It should be '32 bit (8:8:8:8)'",e_name);
                e_res			= false;
            }
        }else{
            ELog.DlgMsg		(mtError,"Invalid special normal map type '%s'. It should be 'Image'",e_name);
            e_res			= false;
        }
        xr_delete			(NM_THM);
        if (false==e_res)	return false;
    }
    // compress
    
    int res 	= DXTCompress(game_name, (u8*)load_data, (u8*)(ext_data.empty()?0:ext_data.begin()), w, h, w4, &THM->m_TexParams, 4);
    if (1!=res){
    	if (-1000!=res){ //. Special for Oles (glos<10%) 
            FS.file_delete	(game_name);
            FS.file_delete	(game_name2.c_str());
        }
        switch(res){
        case 0:		ELog.DlgMsg	(mtError,"Can't make game texture '%s'.",THM->m_SrcName.c_str());break;
        case -1000:	ELog.DlgMsg	(mtError,"Can't make game texture '%s'. Invalid gloss mask.",THM->m_SrcName.c_str());break;
        }
		return false;
    }
    R_ASSERT((res==1)&&FS.file_length(game_name));
    return res==1;
}

//------------------------------------------------------------------------------
// загружает 32-bit данные
//------------------------------------------------------------------------------
bool CImageManager::LoadTextureData(LPCSTR src_name, U32Vec& data, u32& w, u32& h, int* age)
{
	xr_string fn;
	FS.update_path			(fn,_textures_,ChangeFileExt(src_name,".tga").c_str());
    u32 a;
    if (!Surface_Load(fn.c_str(),data,w,h,a)) return false;
    if (age) *age			= FS.get_file_age(fn.c_str());
    return true;
}

//------------------------------------------------------------------------------
// копирует обновленные текстуры с Import'a в Textures
// files - список файлов для копирование
//------------------------------------------------------------------------------
void CImageManager::SafeCopyLocalToServer(FS_QueryMap& files)
{
    xr_string p_import, p_textures;
    xr_string src_name,dest_name;
    FS.update_path	   	(p_import,_import_,"");
    FS.update_path	   	(p_textures,_textures_,"");

    FS_QueryPairIt it	= files.begin();
	FS_QueryPairIt _E 	= files.end();
	for (; it!=_E; it++){
        // copy thm
		xr_string fn 	= EFS.ChangeFileExt(it->first,".thm");
		src_name 		= p_import	+ fn;
		UpdateFileName	(fn);
		dest_name 		= p_textures+ fn;
		FS.file_rename	(src_name.c_str(),dest_name.c_str(),true);
    	// copy sources
		fn 				= it->first;
		src_name	 	= p_import	+ fn;
		UpdateFileName	(fn);
		dest_name 		= p_textures + EFS.ChangeFileExt(fn,".tga");
        if (FS.exist(dest_name.c_str()))
	        EFS.BackupFile	(_textures_,EFS.ChangeFileExt(fn,".tga").c_str());
        if (0==strcmp(strext(src_name.c_str()),".tga")){
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
        EFS.MarkFile		(src_name.c_str(),true);
    }
}    
//------------------------------------------------------------------------------
// возвращает список не синхронизированных (модифицированных) текстур
// source_list - содержит список текстур с расширениями
// sync_list - реально сохраненные файлы (после использования освободить)
//------------------------------------------------------------------------------
void CImageManager::SynchronizeTextures(bool sync_thm, bool sync_game, bool bForceGame, FS_QueryMap* source_list, AStringVec* sync_list, FS_QueryMap* modif_map, bool bForceBaseAge)
{   
	FS_QueryMap M_BASE;
	FS_QueryMap M_THUM;
    FS_QueryMap M_GAME;

    if (source_list) M_BASE = *source_list;
    else FS.file_list(M_BASE,_textures_,FS_ListFiles|FS_ClampExt,".tga");
    if (M_BASE.empty()) return;
    if (sync_thm) 	FS.file_list(M_THUM,_textures_,FS_ListFiles|FS_ClampExt,".thm");
    if (sync_game) 	FS.file_list(M_GAME,_game_textures_,FS_ListFiles|FS_ClampExt,".dds");

    bool bProgress = M_BASE.size()>1;
    
    // lock rescanning
    FS.lock_rescan	();
    
    int m_age		= time(NULL);

    // sync assoc
	xr_string 	ltx_nm;
    FS.update_path	(ltx_nm,_game_textures_,"textures.ltx");
	CInifile* ltx_ini = xr_new<CInifile>(ltx_nm.c_str(), FALSE, TRUE, TRUE);
    
	SPBItem* pb=0;
    if (bProgress) pb = UI->ProgressStart(M_BASE.size(),"Synchronize textures...");
    FS_QueryPairIt it=M_BASE.begin();
	FS_QueryPairIt _E = M_BASE.end();
	for (; it!=_E; it++){
	    U32Vec data;
    	u32 w, h, a;

        xr_string base_name	= EFS.ChangeFileExt(it->first,""); xr_strlwr(base_name);
        xr_string				fn;
        FS.update_path			(fn,_textures_,EFS.ChangeFileExt(base_name,".tga").c_str());
    	if (!FS.exist(fn.c_str())) continue;

		FS_QueryPairIt th 	= M_THUM.find(base_name);
    	bool bThm = ((th==M_THUM.end()) || ((th!=M_THUM.end())&&(th->second.modif!=it->second.modif)));
  		FS_QueryPairIt gm = M_GAME.find(base_name);
    	bool bGame= bThm || ((gm==M_GAME.end()) || ((gm!=M_GAME.end())&&(gm->second.modif!=it->second.modif)));

		ETextureThumbnail* THM=0;

        BOOL bUpdated 	= FALSE;
        BOOL bFailed 	= FALSE;
    	// check thumbnail
    	if (sync_thm&&bThm){
        	THM = xr_new<ETextureThumbnail>(it->first.c_str());
		    bool bRes = Surface_Load(fn.c_str(),data,w,h,a); R_ASSERT(bRes);
            MakeThumbnailImage(THM,data.begin(),w,h,a);
            THM->Save	(it->second.modif);
            bUpdated = TRUE;
        }
        // check game textures
    	if (bForceGame||(sync_game&&bGame)){
        	if (!THM) THM = xr_new<ETextureThumbnail>(it->first.c_str()); 
            R_ASSERT(THM);
            if (data.empty()){ bool bRes = Surface_Load(fn.c_str(),data,w,h,a); R_ASSERT(bRes);}
			if (IsValidSize(w,h)){
                STextureParams& FMT 	= THM->_Format();
                xr_string game_name	= base_name+".dds";
                FS.update_path			(game_name,_game_textures_,game_name.c_str());
                if (MakeGameTexture(THM,game_name.c_str(),data.begin())){
                    if (sync_list) 		sync_list->push_back(base_name.c_str());
                    if (modif_map) 		modif_map->insert(*it);
                    // save to assoc ltx
                    WriteAssociation	(ltx_ini,base_name.c_str(),FMT);
                }else{
					bFailed				= TRUE;
                }
                bUpdated 				= TRUE;
            }else{
		    	ELog.DlgMsg(mtError,"Can't make game texture '%s'.\nInvalid size (%dx%d).",base_name.c_str(),w,h);
            }
		}
		if (THM) xr_delete(THM);
		if (UI->NeedAbort()) break;
        
        if (bProgress) 
		    pb->Inc(bUpdated?xr_string(base_name+(bFailed?" - FAILED":" - UPDATED.")).c_str():base_name.c_str(),bUpdated);
            
        if (bUpdated){
            xr_string	tga_fn,thm_fn,dds_fn;
            FS.update_path			    (tga_fn,_textures_,		EFS.ChangeFileExt(base_name,".tga").c_str());
            FS.update_path			    (thm_fn,_textures_,		EFS.ChangeFileExt(base_name,".thm").c_str());
            FS.update_path			    (dds_fn,_game_textures_,EFS.ChangeFileExt(base_name,".dds").c_str());
            if (bForceBaseAge){
            	int age 				= it->second.modif;
                FS.set_file_age			(tga_fn.c_str(),age);
                FS.set_file_age			(thm_fn.c_str(),age);
                FS.set_file_age			(dds_fn.c_str(),age);
            }else{
                FS.set_file_age			(tga_fn.c_str(),m_age);
                FS.set_file_age			(thm_fn.c_str(),m_age);
                FS.set_file_age			(dds_fn.c_str(),m_age);
            }
        }
    }

    xr_delete(ltx_ini);
    
    if (bProgress) 	UI->ProgressEnd(pb);
    // lock rescanning
    FS.unlock_rescan	();
}
/*
void CImageManager::ChangeFileAgeTo(FS_QueryMap* tgt_map, int age)
{
	VERIFY(tgt_map);
	FS_QueryMap* 	M_BASE 		= tgt_map;
    
    // lock rescanning
    FS.lock_rescan	();
    // change
	SPBItem* pb=0;
    if (M_BASE->size()>1) pb	= UI->ProgressStart(M_BASE->size(),"Change textures age...");
    FS_QueryPairIt it			= M_BASE->begin();
	FS_QueryPairIt _E 			= M_BASE->end();
	for (; it!=_E; it++){
        xr_string base_name	= EFS.ChangeFileExt(it->first,""); xr_strlwr(base_name);
        xr_string	tga_fn,thm_fn,dds_fn;
        FS.update_path			(tga_fn,_textures_,		EFS.ChangeFileExt(base_name,".tga").c_str());
        FS.update_path			(thm_fn,_textures_,		EFS.ChangeFileExt(base_name,".thm").c_str());
        FS.update_path			(dds_fn,_game_textures_,EFS.ChangeFileExt(base_name,".dds").c_str());
        FS.set_file_age			(tga_fn.c_str(),age);
        FS.set_file_age			(thm_fn.c_str(),age);
        FS.set_file_age			(dds_fn.c_str(),age);
        if (pb) 			    pb->Inc();
    }
    if (pb) 					UI->ProgressEnd(pb);
    // lock rescanning
    FS.unlock_rescan			();
}
*/
void CImageManager::WriteAssociation(CInifile* ltx_ini, LPCSTR base_name, const STextureParams& fmt)
{
	if (STextureParams::ttImage==fmt.type){
        // save to assoc ltx
        // details
        if (*fmt.detail_name&&fmt.flags.is_any(STextureParams::flDiffuseDetail|STextureParams::flBumpDetail)){
            AnsiString usage;
            if (fmt.flags.is(STextureParams::flDiffuseDetail)) 	usage="diffuse";
            if (fmt.flags.is(STextureParams::flBumpDetail))		usage=usage.Length()?usage+"_or_bump":AnsiString("bump");
            ltx_ini->w_string	("association", base_name, 
                                AnsiString().sprintf("%s, %f, usage[%s]", 
                                                    *fmt.detail_name?*fmt.detail_name:"", 
                                                    fmt.detail_scale, usage.c_str()).c_str());
        }else{
            ltx_ini->remove_line("association", base_name);
        }    
        // specification                                              
        AnsiString 			bm;
        AnsiString 			det;
        switch(fmt.bump_mode){
        case STextureParams::tbmNone: 		bm="none"; 		det=""; 											break;
        case STextureParams::tbmUse: 		bm="use"; 		det.sprintf(":%s",*fmt.bump_name?*fmt.bump_name:"");break;
        default: NODEFAULT;
        }
        ltx_ini->w_string		("specification", base_name, 	
                                AnsiString().sprintf("bump_mode[%s%s], material[%3.2f]",
                                					bm.c_str(),det.c_str(),fmt.material+fmt.material_weight).c_str());
    }else{
        ltx_ini->remove_line("association", base_name);
        ltx_ini->remove_line("specification", base_name);
    }
}

void CImageManager::SynchronizeTexture(LPCSTR tex_name, int age)
{
    AStringVec modif;
    FS_QueryMap t_map;
    t_map.insert		(mk_pair(tex_name,FS_QueryItem(0,age,0)));
    SynchronizeTextures	(true,true,true,&t_map,&modif,0,age);
    RefreshTextures		(&modif);
}
//------------------------------------------------------------------------------
// возвращает список всех текстур
//------------------------------------------------------------------------------
int CImageManager::GetTextures(FS_QueryMap& files, BOOL bFolders)
{                	
    return FS.file_list(files,_textures_,(bFolders?FS_ListFolders:0)|FS_ListFiles|FS_ClampExt,".tga"); 
}
//------------------------------------------------------------------------------
// возвращает список текстур, которые нужно обновить
//------------------------------------------------------------------------------
int CImageManager::GetLocalNewTextures(FS_QueryMap& files)
{
    return FS.file_list(files,_import_,FS_ListFiles|FS_RootOnly,".tga,.bmp");
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
	SPBItem* pb = UI->ProgressStart(files.size(),"Check texture compliance: ");
    FS_QueryPairIt it	= files.begin();
	FS_QueryPairIt _E 	= files.end();
	for (; it!=_E; it++){
    	int val	= 0;
        xr_string 	fname;
        FS.update_path			(fname,_textures_,it->first.c_str());
    	if (!CheckCompliance(fname.c_str(),val))
        	ELog.Msg(mtError,"Bad texture: '%s'",it->first.c_str());
        compl.insert			(mk_pair(it->first,FS_QueryItem(it->second.size,iFloor(val))));
	    pb->Inc					();
		if (UI->NeedAbort()) break;
    }
	UI->ProgressEnd(pb);
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

BOOL _ApplyBorders(U32Vec& pixels, u32 w, u32 h, u32 ref)
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

void CImageManager::ApplyBorders(U32Vec& tgt_data, u32 w, u32 h)
{
    U32Vec border_pixels 		= tgt_data;
    for (U32It it=tgt_data.begin(); it!=tgt_data.end(); it++)
        *it=(color_get_A(*it)>200)?subst_alpha(*it,0xFF):subst_alpha(*it,0x00);
    for (u32 ref=254; ref>0; ref--)
        _ApplyBorders(tgt_data,w,h,ref);
    for (int t=0; t<int(tgt_data.size()); t++)
        tgt_data[t]=subst_alpha(tgt_data[t],color_get_A(border_pixels[t]));
}

BOOL CImageManager::CreateOBJThumbnail(LPCSTR tex_name, CEditableObject* obj, int age)
{
	BOOL bResult = TRUE;
    // save render params
    Flags32 old_flag= 	psDeviceFlags;
    // set render params
    psDeviceFlags.set(rsStatistic|rsDrawGrid,FALSE);
    u32 cc						= 	EPrefs.scene_clear_color;
    EPrefs.scene_clear_color 	= 	0x00333333;

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
    psDeviceFlags 				= old_flag;
    EPrefs.scene_clear_color 	= cc;
    return bResult;
}

void CImageManager::RemoveTexture(LPCSTR fname, EItemType type, bool& res)
{
	if (TYPE_FOLDER==type){
    	FS.dir_delete			(_textures_,fname,FALSE);
    	FS.dir_delete			(_game_textures_,fname,FALSE);
        res 					= true;
        return;
    }else if (TYPE_OBJECT==type){
        xr_string src_name;
        FS.update_path			(src_name,_textures_,fname);
        src_name				= EFS.ChangeFileExt(src_name,".tga");
        if (FS.exist(src_name.c_str())){
            xr_string base_name= EFS.ChangeFileExt(fname,"");
            xr_string thm_name = EFS.ChangeFileExt(fname,".thm");
            xr_string game_name= EFS.ChangeFileExt(fname,".dds");
            xr_string game_name2=EFS.ChangeFileExt(fname,"#.dds");
            // source
            EFS.BackupFile		(_textures_,fname);
            FS.file_delete		(src_name.c_str());
            EFS.WriteAccessLog	(src_name.c_str(),"Remove");
            // thumbnail
            EFS.BackupFile		(_textures_,thm_name.c_str());
            FS.file_delete		(_textures_,thm_name.c_str());
            // game
            FS.file_delete		(_game_textures_,game_name.c_str());
            // game 2
            FS.file_delete		(_game_textures_,game_name2.c_str());
            // assoc
            xr_string ltx_nm;
            FS.update_path		(ltx_nm,_game_textures_,"textures.ltx");
            CInifile* ltx_ini 	= xr_new<CInifile>(ltx_nm.c_str(), FALSE, TRUE, TRUE);
            ltx_ini->remove_line("association", base_name.c_str());
            ltx_ini->remove_line("specification", base_name.c_str());
            xr_delete			(ltx_ini);
            res 				= true;
            return;
        }
    }
    res 						= false;
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
// если передан параметр modif - обновляем DX-Surface only и только из списка
// иначе полная синхронизация
//------------------------------------------------------------------------------
void CImageManager::RefreshTextures(AStringVec* modif)
{
    if (modif) Device.Resources->ED_UpdateTextures(modif);
	else{
		UI->SetStatus("Refresh textures...");
    	AStringVec modif_files;
    	ImageLib.SynchronizeTextures(true,true,false,0,&modif_files);
        Device.Resources->ED_UpdateTextures(&modif_files);
		UI->SetStatus("");
    }
}

void __stdcall pb_callback(void* data, float& v)
{
	SPBItem* PB = (SPBItem*)(data);
	PB->Update(v);
}

#include "ETools.h"
BOOL CImageManager::CreateSmallerCubeMap(LPCSTR src_name, LPCSTR dst_name)
{
    U32Vec data;
    u32 w, wf, h, a;
    xr_string full_name;
    FS.update_path	(full_name,_textures_,src_name);
    full_name 			+= ".tga";
    if (Surface_Load(full_name.c_str(),data,wf,h,a)){
    	w				= wf/6;
	    u32 sm_w=32, sm_wf=6*sm_w, sm_h=32;
        if (!btwIsPow2(h)||(h*6!=wf)||(wf<sm_wf)||(h<sm_h)){	
        	ELog.Msg(mtError,"Texture '%s' - invalid size: [%d, %d]",src_name,wf,h);
            return 		FALSE;
        }
        // generate smaller
	    U32Vec sm_data	(sm_wf*sm_h,0);
		SPBItem* PB		= UI->ProgressStart(1.f,"Cube Map: scale image...");
        CTimer T; T.Start();
        ETOOLS::SimplifyCubeMap	(data.begin(),w,h,sm_data.begin(),sm_w,sm_h,16.f,pb_callback,PB);
        T.Stop			();
		UI->ProgressEnd	(PB);
        // write texture
        xr_string out_name;
        FS.update_path	(out_name,_game_textures_,dst_name);
        out_name		+= ".dds";
		if (!MakeGameTexture(out_name.c_str(),&*sm_data.begin(),sm_wf,sm_h,STextureParams::tfRGBA,STextureParams::ttCubeMap,false))
        	return FALSE;
        ELog.DlgMsg(mtInformation,"Smaller cubemap successfylly created [%3.2f sec].",T.Get());
        return TRUE;
    }else{
        ELog.Msg(mtError,"Can't load texture '%s'.",src_name);
    }
    return FALSE;
}

