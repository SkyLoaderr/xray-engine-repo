#include "stdafx.h"
#pragma hdrstop

#include "ImageThumbnail.h"
#include "ImageManager.h"
#include "xrImage_Resampler.h"

//----------------------------------------------------
//----------------------------------------------------
bool CreateBitmap(HDC hdc, HBITMAP& th, DWORDVec& data, int w, int h){
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

//	th = CreateDIBitmap( hdc, &bi, CBM_INIT, (BYTE*)data.begin(), (LPBITMAPINFO)&bi, DIB_RGB_COLORS );
//	th = CreateBitmap( w, h, 1, 32, (BYTE*)data.begin() );
	th = CreateCompatibleBitmap( hdc,w,h );
	if (!th&&(GetLastError()!=0)){
    	err = true;
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,GetLastError(),0,lpMsgBuf,1024,0);
        ELog.Msg(mtError, lpMsgBuf);
    }
    int ln = SetDIBits( hdc, th, 0, h, (BYTE*)data.begin(), (LPBITMAPINFO)&bi, DIB_RGB_COLORS);
	if ((ln==0)&&(GetLastError()!=0)){
    	err = true;
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,GetLastError(),0,lpMsgBuf,1024,0);
        ELog.Msg(mtError, lpMsgBuf);
    }
	return (!err);
}
//----------------------------------------------------

EImageThumbnail::EImageThumbnail(LPCSTR src_name, THMType type, bool bLoad)
{
	m_Type	= type;
	m_Name 	= ChangeFileExt(src_name,".thm");
    m_Age	= 0;
    if (bLoad)
		if (!Load()&&IsTexture()) ImageManager.CreateThumbnail(this,src_name);
}

EImageThumbnail::~EImageThumbnail()
{
	m_Pixels.clear();
}

void EImageThumbnail::CreateFromData(LPDWORD p, int w, int h){
	R_ASSERT(p&&(w>0)&&(h>0));
//	imf_filter	imf_box  imf_triangle  imf_bell  imf_b_spline  imf_lanczos3  imf_mitchell
	m_Pixels.resize(THUMB_SIZE);
	imf_Process(m_Pixels.begin(),THUMB_WIDTH,THUMB_HEIGHT,p,w,h,imf_box);
    m_TexParams.width = w;
    m_TexParams.height= h;
}

bool EImageThumbnail::Load()
{
	AnsiString fn = m_Name;
    switch (m_Type){
    case EITObject: FS.m_Objects.Update(fn); 	break;
    case EITTexture:FS.m_Textures.Update(fn); 	break;
    }
    if (!FS.Exist(fn.c_str())) return false;
    CFileStream FN(fn.c_str());
    char MARK[8]; FN.Read(MARK,8);
    if (strncmp(MARK,THM_SIGN,8)!=0){
        ELog.Msg( mtError, "Thumbnail: Unsupported version.");
        return false;
    }

    CCompressedStream F(fn.c_str(),THM_SIGN);

    DWORD version = 0;

    R_ASSERT(F.ReadChunk(THM_CHUNK_VERSION,&version));
    if( version!=THM_CURRENT_VERSION ){
        ELog.Msg( mtError, "Thumbnail: Unsupported version.");
        return false;
    }

    R_ASSERT(F.FindChunk(THM_CHUNK_DATA));
    m_Pixels.resize(THUMB_SIZE);
    F.Read(m_Pixels.begin(),THUMB_SIZE*sizeof(DWORD));

    R_ASSERT(F.FindChunk(THM_CHUNK_TEXTUREPARAM));
    F.Read(&m_TexParams,sizeof(STextureParams));

    R_ASSERT(F.FindChunk(THM_CHUNK_TYPE));
    m_Type	= THMType(F.Rdword());

    m_Age = FS.GetFileAge(fn);

    return true;
}

void EImageThumbnail::Save(int age){
	R_ASSERT(Valid());

    CFS_Memory F;
	F.open_chunk	(THM_CHUNK_VERSION);
	F.Wword			(THM_CURRENT_VERSION);
	F.close_chunk	();

	F.open_chunk	(THM_CHUNK_DATA);
    F.write			(m_Pixels.begin(),m_Pixels.size()*sizeof(DWORD));
	F.close_chunk	();

	F.open_chunk	(THM_CHUNK_TEXTUREPARAM);
    F.write			(&m_TexParams,sizeof(STextureParams));
	F.close_chunk	();

    F.open_chunk	(THM_CHUNK_TYPE);
    F.Wdword		(m_Type);
	F.close_chunk	();

	AnsiString fn 	= m_Name;
    switch (m_Type){
    case EITObject: FS.m_Objects.Update(fn); 	break;
    case EITTexture:FS.m_Textures.Update(fn); 	break;
    }
	FS.VerifyPath	(fn.c_str());

    F.SaveTo		(fn.c_str(),THM_SIGN);

    FS.SetFileAge	(fn,age?age:m_Age);
}

void EImageThumbnail::DrawNormal( HANDLE handle, RECT *r ){
    if (!Valid()) return;

	HBITMAP th;
    HDC hdc = GetDC(handle);
	if( !CreateBitmap(hdc,th,m_Pixels,THUMB_WIDTH,THUMB_HEIGHT) ){
		ELog.Msg( mtError, "Error: can't create bitmap from thumbnail for '%s'", m_Name );
		return;
    }

	HDC hdcmem = CreateCompatibleDC( hdc );
	HBITMAP oldbitmap = (HBITMAP) SelectObject( hdcmem, th );
    SetStretchBltMode(hdc, STRETCH_HALFTONE);
	BitBlt(hdc,r->left,r->top,r->right-r->left,r->bottom-r->top,hdcmem,0,0,SRCCOPY);
	SelectObject( hdcmem, oldbitmap );
	DeleteDC( hdcmem );
	ReleaseDC(handle,hdc);
    DeleteObject(th);
}

void EImageThumbnail::DrawStretch( HANDLE handle, RECT *r ){
    if (!Valid()) return;

	HBITMAP th;
    HDC hdc = GetDC(handle);
	if( !CreateBitmap(hdc,th,m_Pixels,THUMB_WIDTH,THUMB_HEIGHT) ){
		ELog.Msg( mtError, "Error: can't create bitmap from thumbnail for '%s'", m_Name );
		return; }

	HDC hdcmem = CreateCompatibleDC( hdc );
	HBITMAP oldbitmap = (HBITMAP) SelectObject( hdcmem, th );
    SetStretchBltMode(hdc, STRETCH_HALFTONE);
	StretchBlt(hdc,r->left,r->top,r->right-r->left,r->bottom-r->top,hdcmem,0,0,THUMB_WIDTH,THUMB_HEIGHT,SRCCOPY);
	SelectObject( hdcmem, oldbitmap );
	DeleteDC( hdcmem );
	ReleaseDC(handle,hdc);
    DeleteObject(th);
}


