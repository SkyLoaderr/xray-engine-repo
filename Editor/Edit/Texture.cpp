//----------------------------------------------------
// file: Texture.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Texture.h"
#include "ui_main.h"
#include "d3dx.h"
//----------------------------------------------------
bool CreateBitmap(HBITMAP& th, DWORDVec& data, int w, int h){
    bool err=false;
	char lpMsgBuf[1024];
    BITMAPINFOHEADER bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = w;
    bi.biHeight = h;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = w*h*4;

	HDC hdc = GetDC(UI->GetHWND());
//	th = CreateDIBitmap( hdc, &bi, CBM_INIT, (BYTE*)data.begin(), (LPBITMAPINFO)&bi, DIB_RGB_COLORS );
//	th = CreateBitmap( w, h, 1, 32, (BYTE*)data.begin() );
	th = CreateCompatibleBitmap( hdc,w,h );
	if (!th&&(GetLastError()!=0)){
    	err = true;
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,GetLastError(),0,lpMsgBuf,1024,0);
        Log->Msg(mtError, lpMsgBuf);
    }
    int ln = SetDIBits( hdc, th, 0, h, (BYTE*)data.begin(), (LPBITMAPINFO)&bi, DIB_RGB_COLORS);
	if ((ln==0)&&(GetLastError()!=0)){
    	err = true;
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,GetLastError(),0,lpMsgBuf,1024,0);
        Log->Msg(mtError, lpMsgBuf);
    }
	ReleaseDC( UI->GetHWND(), hdc );
	return (!err);
}
//----------------------------------------------------

ETextureCore::ETextureCore( const char *_ShortName ){
	VERIFY( _ShortName );
	strcpy( m_ShortName, _ShortName );
    if (!strlen(m_ShortName)){
        Log->DlgMsg( mtError, "Texture contains unknown name.");
        throw -1;
    }
	m_RefCount = 0;
	m_Surface = 0;
	m_Width = 0;
	m_Height = 0;
	m_AlphaPresent = 0;
    m_bLoadFailed = false;

    m_Thm = new ETextureThumbnail(_ShortName);

	if (0==strcmp(m_ShortName,"$null")){ m_bNullTex=true; return; }
    m_bNullTex = false; 
    
	if( !GetParams() ){
		Log->Msg( mtError, "Texture: failed to load %s", _ShortName );
		return;
    };

	Log->Msg( mtInformation, "Texture: %s loaded", _ShortName );
}

ETextureCore::~ETextureCore(){
    _DELETE(m_Thm);
	VERIFY( m_RefCount == 0 );
	DDClear();
	Unload();
}
//----------------------------------------------------

bool ETextureCore::GetParams(){
    bool bRes=false;
    m_bLoadFailed = false;

    AnsiString tga_name, bmp_name;
    bool bTGA, bBMP; 
    tga_name = m_ShortName; tga_name += ".tga";
    bmp_name = m_ShortName; bmp_name += ".bmp";

    // check file exists
    bTGA = FS.Exist(&FS.m_Textures,tga_name.c_str(),false);
    bBMP = FS.Exist(&FS.m_Textures,bmp_name.c_str(),false);
    
    if (!bTGA&&!bBMP){ 
    	Log->DlgMsg( mtError, "Error: Texture '%s' not found. Check file existance and try again.", m_ShortName);
        m_bLoadFailed=true;
        return false;
    }
    
    if (bTGA)		{m_LoadName = tga_name; bRes = GetTGAParams();}
    else if (bBMP) 	{m_LoadName = bmp_name; bRes = GetBMPParams();}

    // load texture params
    if (bRes){
	    VERIFY(m_Thm);
    	if (!m_Thm->GetTextureParams()){
        	m_Thm->CreateFromTexture(this);
            Unload();
        }

	//    if (bRes){		CheckVersionAndUpdateFiles();
	//    }else			Log->DlgMsg( mtError, "Error: File '%s' have unsupported texture format.", m_LoadName.c_str() );

    }
	m_bLoadFailed=!bRes;

	return bRes;
}
//----------------------------------------------------

bool ETextureCore::Load(){
    if (!Valid()) return false;
	if (IsLoading()) return true;

    VERIFY(!m_LoadName.IsEmpty());

    AnsiString ext = ExtractFileExt(m_LoadName).LowerCase();
    if (ext==".tga") return LoadTGA();
    if (ext==".bmp") return LoadBMP();

	Log->DlgMsg( mtError, "Error: Can't find file '%s.bmp' or '%s.tga' \nor unsupported texture format.", m_ShortName, m_ShortName );
	return false;
}

void ETextureCore::Unload(){
	m_Pixels.clear();
}

void ETextureCore::Refresh(bool bOnlyNew){
    if (m_bNullTex) return;
    // check file version if non equal remake
    if (!bOnlyNew||(bOnlyNew&&(IfNeedUpdate()==1))){//&&CheckVersionAndUpdateFiles()){
		DDClear();
    	Unload();
	    GetParams();
    }
}

bool ETextureCore::DDInit(){
	if( !Valid() ){
    	if (!GetParams()) return false;
		return false; 
    }

	if( !CreateDDSurface() ){
		Log->DlgMsg( mtError, "Texture: Can't create D3D texture!\n\nFile '%s' corrupted or\nnot found in 'textures->render' directory.", m_ShortName );
		return false; 
    }

	return true;
}

void ETextureCore::DDClear(){
    ReleaseTexture( m_Surface );
}

int ETextureCore::IfNeedUpdate(){
	if (m_bNullTex) return 0;
    if (m_LoadName.IsEmpty()) return 1;
	AnsiString tex_name=m_LoadName;
	FS.m_Textures.Update(tex_name);
    if (!FS.Exist(tex_name.c_str(),true)) return -1;
	AnsiString thm_name=AnsiString(m_ShortName)+AnsiString(".thm");
	FS.m_TexturesThumbnail.Update(thm_name);
	AnsiString dds_name=AnsiString(m_ShortName)+AnsiString(".dds");
	FS.m_GameTextures.Update(dds_name);
	return !((FS.CompareFileAge(tex_name,thm_name)==1)&&(FS.CompareFileAge(tex_name,dds_name)==1));
}

bool ETextureCore::CheckVersionAndUpdateFiles(bool bDDS){
	if (m_bNullTex) return true;
	VERIFY(m_Thm);
	AnsiString tex_name=m_LoadName;
	FS.m_Textures.Update(tex_name);
    if (!FS.Exist(tex_name.c_str(),true)) return false;
    // check THM version
	AnsiString thm_name=AnsiString(m_ShortName)+AnsiString(".thm");
	FS.m_TexturesThumbnail.Update(thm_name);
    int tex_thm = FS.CompareFileAge(tex_name,thm_name);
    if (tex_thm!=1) if (!m_Thm->CreateFromTexture(this)) return false;

    if (bDDS){
        // check DDS version
        AnsiString dds_name=AnsiString(m_ShortName)+AnsiString(".dds");
        FS.m_GameTextures.Update(dds_name);
        int tex_dds = FS.CompareFileAge(tex_name,dds_name);
        if ((tex_dds==1)&&FS.FileLength(dds_name.c_str())) return true;
        if (!SaveAsDDS(dds_name.c_str())) return false;
    }
	return true;
}

bool ETextureCore::UpdateTexture(){
	return CheckVersionAndUpdateFiles();
}

bool ETextureCore::UpdateThumbnail(){
	return CheckVersionAndUpdateFiles(false);
}

bool ETextureCore::CreateDDSurface(){
	AnsiString name,aname;
    name 		= AnsiString(m_ShortName)+AnsiString(".dds");
	FS.m_GameTextures.Update(name);
    aname 		= AnsiString(m_LoadName);
	FS.m_Textures.Update(aname);
    // check file version if non equal remake
	CheckVersionAndUpdateFiles();
 	HRESULT hr = CreateTexture(m_Surface, name.c_str(), aname.c_str(), &m_Width, &m_Height, &m_AlphaPresent);
    if (hr!=DD_OK){ 
    	Log->Msg(mtError,"D3D Error: %s",InterpretError(hr));
        return false;
    }
    return true;
}
//----------------------------------------------------

void ETextureCore::Bind(int stage){
    if (!m_Surface&&!m_bNullTex)
        if(!m_bLoadFailed) m_bLoadFailed=!DDInit();
	if (UI->bRenderTextures)UI->Device.SetTexture( stage, m_Surface );
    else                  	UI->Device.SetTexture( stage, 0 );
}

STextureParams* ETextureCore::GetTextureParams(){
	return (m_Thm)?m_Thm->GetTextureParams():0;
}

void ETextureCore::SaveTextureParams(){
	if (m_Thm){
    	// delete exist dds
        AnsiString gf;
        // render
        gf = m_ShortName;
    	FS.m_GameTextures.Update(gf);
        gf = ChangeFileExt(gf,".dds");
        FS.DeleteFileByName(gf.c_str());
        // save thm
		AnsiString name=m_LoadName;
    	FS.m_Textures.Update(name);
     	m_Thm->Save(FS.GetFileAge(name));
    }
}

void ETextureCore::DrawNormal(HANDLE handle, RECT *r){
    if (m_bLoadFailed) return;
    Load();	if (!IsLoading()) return;
	HBITMAP th;
	if( !CreateBitmap(th,m_Pixels,m_Width,m_Height) ){
		Log->Msg( mtError, "Error: can't create bitmap from thumbnail for '%s'", m_ShortName );
		return; 
    }
    HDC hdc = GetDC(handle);
	HDC hdcmem = CreateCompatibleDC( hdc );
	HBITMAP oldbitmap = (HBITMAP) SelectObject( hdcmem, th );
    SetStretchBltMode(hdc, STRETCH_HALFTONE);
	BitBlt(hdc,r->left,r->top, r->right-r->left, r->bottom-r->top,hdcmem,0,0,SRCCOPY);
	SelectObject( hdcmem, oldbitmap );
	DeleteDC( hdcmem );
	ReleaseDC(handle,hdc);
    DeleteObject(th);
}

void ETextureCore::DrawStretch(HANDLE handle, RECT *r){
    if (m_bLoadFailed) return;
    Load();	if (!IsLoading()) return;
	HBITMAP th;
	if( !CreateBitmap(th,m_Pixels,m_Width,m_Height) ){
		Log->Msg( mtError, "Error: can't create bitmap for '%s'", m_ShortName );
		return; 
    }

    HDC hdc = GetDC(handle);
	HDC hdcmem = CreateCompatibleDC( hdc );
	HBITMAP oldbitmap = (HBITMAP) SelectObject( hdcmem, th );
    SetStretchBltMode(hdc, STRETCH_HALFTONE);
	StretchBlt(hdc,r->left,r->top, r->right-r->left, r->bottom-r->top,hdcmem,0,m_Height-1,m_Width,-m_Height,SRCCOPY);
	SelectObject( hdcmem, oldbitmap );
	DeleteDC( hdcmem );
	ReleaseDC(handle,hdc);
    DeleteObject(th);
}


