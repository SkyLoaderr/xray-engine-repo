//----------------------------------------------------
// file: Texture.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Texture.h"
#include "Log.h"
#include "ui_main.h"

#define THM_SIGN "THM"

//----------------------------------------------------
extern "C" float DLL_API gauss [7][7];
extern "C" void DLL_API ip_BuildKernel	(float* dest, float* src, int DIM=3, float norm=1.f);
extern "C" void DLL_API ip_ProcessKernel(DWORD* dest, DWORD* src, int w, int h, float* kern, int DIM=3);
extern bool CreateBitmap(HBITMAP& th, DWORDVec& data, int w, int h);

ETextureThumbnail::ETextureThumbnail(const char *_ShortName){
    m_LoadName = ChangeFileExt(_ShortName,".thm");
    m_bLoadFailed = false;
    m_TexParams = 0;
}

ETextureThumbnail::~ETextureThumbnail(){
	m_Pixels.clear();
    _DELETE(m_TexParams);
}

STextureParams* ETextureThumbnail::GetTextureParams(){
	if (!m_TexParams){
    	if (!LoadTexParams())
        	m_TexParams=new STextureParams();
    }
    return m_TexParams;
}

bool ETextureThumbnail::CreateFromData(DWORDVec& p, int width, int height, int src_age, bool check, bool blur){
	R_ASSERT(!p.empty());
	if ((width<=0)&&(height<=0)) return false;
    
    AnsiString name=m_LoadName;
    FS.m_TexturesThumbnail.Update(name);

    if ((!check)||(check&&(!FS.Exist(name.c_str())||(FS.GetFileAge(name)!=src_age)))){
		UI->ProgressStart(4,"Creating thumbnail...");
        DWORDVec im_s;
        if ((width>THUMB_WIDTH)&&(height>THUMB_HEIGHT)&&blur){
            im_s.resize(width*height);
            // smooth
            float my_gauss[7*7];
			UI->ProgressInc();
            ip_BuildKernel(my_gauss,(float*)gauss,3,1);
			UI->ProgressInc();
            ip_ProcessKernel(im_s.begin(), p.begin(), width, height, my_gauss, 3);
			UI->ProgressInc();
        }else{
            im_s = p;
        }

        int W[THUMB_WIDTH];
        int H[THUMB_HEIGHT];
        m_Pixels.resize(THUMB_SIZE);
        for (int w=0; w<THUMB_WIDTH; w++){ 
            float f=float(w)/float(THUMB_WIDTH);
            W[w]=f*width;
        }
        for (int h=0; h<THUMB_HEIGHT; h++) {
            float f=float(h)/float(THUMB_HEIGHT); 
            H[h]=f*height;
        }
        for (h=0; h<THUMB_HEIGHT; h++)
            for (w=0; w<THUMB_WIDTH; w++)
                m_Pixels[h*THUMB_WIDTH+w] = im_s[(height-H[h]-1)*width+W[w]];

        Save(src_age);
		UI->ProgressEnd();
        return true;
    }
    return false;
}

bool ETextureThumbnail::CreateFromTexture(ETextureCore* tex){
	R_ASSERT(tex);
	if (!tex->Valid()) return false;
    bool bRes=false;
	AnsiString name=tex->m_LoadName;
    FS.m_Textures.Update(name);
	STextureParams* TP=GetTextureParams();
    if (FS.Exist(name.c_str())){
        int src_age = FS.GetFileAge(name);
        if (!tex->Load()) return false;
        if (tex->alpha()) TP->fmt = STextureParams::tfDXT3;
        bRes = CreateFromData(tex->m_Pixels,tex->m_Width,tex->m_Height,src_age,false);
        if (bRes) m_bLoadFailed = false;
		tex->Unload();
    }
    return bRes;
}

bool ETextureThumbnail::CreateFromTexture(const char* name){
    AnsiString fn;
	if (name) fn=name;
    else{ 
        fn = ChangeFileExt(m_LoadName,".");
        FS.m_Textures.Update(fn);
    }
	ETextureCore* tex = new ETextureCore(fn.c_str());
    bool bRes = CreateFromTexture(tex);
    _DELETE(tex);
    return bRes;
}

bool ETextureThumbnail::Save(int src_age){
	if (!Valid()) return false;

	AnsiString name = m_LoadName;
	FS.m_TexturesThumbnail.Update(name);

    CFS_Memory F;
	F.open_chunk	(THM_CHUNK_VERSION);
	F.Wword			(THM_CURRENT_VERSION);
	F.close_chunk	();

	F.open_chunk	(THM_CHUNK_DATA);
    F.Wdword		(THUMB_SIZE);
    F.write			(m_Pixels.begin(),m_Pixels.size()*sizeof(DWORD));
	F.close_chunk	();

    STextureParams* TP = GetTextureParams();
	F.open_chunk	(THM_CHUNK_TEXTUREPARAM);
    F.write			(TP,sizeof(STextureParams));
	F.close_chunk	();

    F.SaveTo		(name.c_str(),THM_SIGN);
    
	FS.SetFileAge	(name,src_age);
    
	return true;
}

bool ETextureThumbnail::LoadTexParams(){
    // loading
	AnsiString fn=m_LoadName;
    FS.m_TexturesThumbnail.Update(fn);
    if (!FS.Exist(fn.c_str())){
		m_bLoadFailed = true;
    	return false;
    }

    CFileStream FN(fn.c_str());
    char MARK[8]; FN.Read(MARK,8);
    if (strcmp(MARK,THM_SIGN)!=0) return false;

    CCompressedStream F(fn.c_str(),THM_SIGN);

	DWORD version = 0;

    R_ASSERT(F.ReadChunk(THM_CHUNK_VERSION,&version));
    if( version!=THM_CURRENT_VERSION ){
        Log->DlgMsg( mtError, "Thumbnail: Unsupported version.");
		m_bLoadFailed = true;
        return false;
    }

	if (!m_TexParams) m_TexParams = new STextureParams;
    R_ASSERT(F.FindChunk(THM_CHUNK_TEXTUREPARAM));
    F.Read(m_TexParams,sizeof(STextureParams));
    return true;
}

bool ETextureThumbnail::Load(){
	AnsiString fn=m_LoadName;
    FS.m_TexturesThumbnail.Update(fn);
    if (!FS.Exist(fn.c_str())){ 
		m_bLoadFailed = true;
    	return false;
    }

    CFileStream FN(fn.c_str());
    char MARK[8]; FN.Read(MARK,8);
    if (strcmp(MARK,THM_SIGN)!=0) return false;

    CCompressedStream F(fn.c_str(),THM_SIGN);

	DWORD version = 0;

    R_ASSERT(F.ReadChunk(THM_CHUNK_VERSION,&version));
    if( version!=THM_CURRENT_VERSION ){
        Log->DlgMsg( mtError, "Thumbnail: Unsupported version.");
		m_bLoadFailed = true;
        return false;
    }
    
    R_ASSERT(F.FindChunk(THM_CHUNK_DATA));
    int sz = F.Rdword();
    m_Pixels.resize(sz);
    F.Read(m_Pixels.begin(),sz*sizeof(DWORD));

	STextureParams* TP=GetTextureParams();    
    R_ASSERT(F.FindChunk(THM_CHUNK_TEXTUREPARAM));
    F.Read(TP,sizeof(STextureParams));
    return true;
}

void ETextureThumbnail::DrawNormal( HANDLE handle, RECT *r ){
    if (m_bLoadFailed) return;
	if (!Valid()) 
    	if (!Load()){
			Log->Msg( mtError, "Error: can't load thumbnail for '%s'", m_LoadName );
	     	return;
    	}

	HBITMAP th;
	if( !CreateBitmap(th,m_Pixels,THUMB_WIDTH,THUMB_HEIGHT) ){
		Log->Msg( mtError, "Error: can't create bitmap from thumbnail for '%s'", m_LoadName );
		return; 
    }

    HDC hdc = GetDC(handle);
	HDC hdcmem = CreateCompatibleDC( hdc );
	HBITMAP oldbitmap = (HBITMAP) SelectObject( hdcmem, th );
    SetStretchBltMode(hdc, STRETCH_HALFTONE);
	BitBlt(hdc,r->left,r->top,r->right-r->left,r->bottom-r->top,hdcmem,0,0,SRCCOPY);
	SelectObject( hdcmem, oldbitmap );
	DeleteDC( hdcmem );
	ReleaseDC(handle,hdc);
    DeleteObject(th);
}

void ETextureThumbnail::DrawStretch( HANDLE handle, RECT *r ){
    if (m_bLoadFailed) return;

	if (!Valid()) 
    	if (!Load()){
			Log->Msg( mtError, "Error: can't load thumbnail for '%s'", m_LoadName );
	     	return;
    	}

	HBITMAP th;
	if( !CreateBitmap(th,m_Pixels,THUMB_WIDTH,THUMB_HEIGHT) ){
		Log->Msg( mtError, "Error: can't create bitmap from thumbnail for '%s'", m_LoadName );
		return; }

    HDC hdc = GetDC(handle);
	HDC hdcmem = CreateCompatibleDC( hdc );
	HBITMAP oldbitmap = (HBITMAP) SelectObject( hdcmem, th );
    SetStretchBltMode(hdc, STRETCH_HALFTONE);
	StretchBlt(hdc,r->left,r->top,r->right-r->left,r->bottom-r->top,hdcmem,0,0,THUMB_WIDTH,THUMB_HEIGHT,SRCCOPY);
	SelectObject( hdcmem, oldbitmap );
	DeleteDC( hdcmem );
	ReleaseDC(handle,hdc);
    DeleteObject(th);
}

 