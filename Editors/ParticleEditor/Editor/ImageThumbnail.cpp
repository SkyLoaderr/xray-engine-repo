#include "stdafx.h"
#pragma hdrstop

#include "ImageThumbnail.h"
#include "ImageManager.h"
#include "xrImage_Resampler.h"
#include "xr_tokens.h"

//----------------------------------------------------
bool DrawThumbnail(HDC hdc, DWORDVec& data, int offs_x, int offs_y, int dest_w, int dest_h, int src_w, int src_h)
{
    BITMAPINFO bi;
    ZeroMemory					(&bi, sizeof(bi));
    bi.bmiHeader.biSize 		= sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth 		= src_w;
    bi.bmiHeader.biHeight 		= src_h;
    bi.bmiHeader.biPlanes 		= 1;
    bi.bmiHeader.biBitCount 	= 32;
    bi.bmiHeader.biCompression	= BI_RGB;
    bi.bmiHeader.biSizeImage 	= src_w*src_h*4;

    SetStretchBltMode			(hdc, STRETCH_HALFTONE);
    int ln = StretchDIBits		(hdc, offs_x,offs_y, dest_w,dest_h, 0,0, src_w,src_h, (BYTE*)data.begin(), &bi, DIB_RGB_COLORS, SRCCOPY);
	if (ln==GDI_ERROR){ 
    	ELog.Msg(mtError,"%s",Engine.LastWindowsError());
    	return false;
    }
	return true;
}
//----------------------------------------------------

void EImageThumbnail::VFlip()
{
//	return;
	R_ASSERT(!m_Pixels.empty());
	DWORD line[THUMB_WIDTH];
    DWORD sz_ln=sizeof(DWORD)*THUMB_WIDTH;
    DWORD y2 = THUMB_WIDTH-1;
    for (int y=0; y<THUMB_HEIGHT/2; y++,y2--){
    	CopyMemory(line,m_Pixels.begin()+y2*THUMB_WIDTH,sz_ln);
    	CopyMemory(m_Pixels.begin()+y2*THUMB_WIDTH,m_Pixels.begin()+y*THUMB_WIDTH,sz_ln);
    	CopyMemory(m_Pixels.begin()+y*THUMB_WIDTH,line,sz_ln);
    }
}
//----------------------------------------------------

EImageThumbnail::EImageThumbnail(LPCSTR src_name, THMType type, bool bLoad)
{
	m_Type	= type;
	m_Name 	= ChangeFileExt(src_name,".thm");
    m_Age	= 0;
    if (bLoad) 	if (!Load()&&IsTexture()) ImageManager.CreateTextureThumbnail(this,src_name);
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
    m_TexParams.flag&=~STextureParams::flHasAlpha;
}

bool EImageThumbnail::Load(LPCSTR src_name, FSPath* path)
{
	AnsiString fn = ChangeFileExt(src_name?AnsiString(src_name):m_Name,".thm");
    if (path) path->Update(fn);
    else{
	    switch (m_Type){
    	case EITObject: Engine.FS.m_Objects.Update(fn); 	break;
	    case EITTexture:Engine.FS.m_Textures.Update(fn); 	break;
    	}
    }
    if (!Engine.FS.Exist(fn.c_str())) return false;
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
    F.Read			(&m_TexParams.fmt,sizeof(STextureParams::ETFormat));
    m_TexParams.flag		= F.Rdword();
    m_TexParams.border_color= F.Rdword();
    m_TexParams.fade_color	= F.Rdword();
    m_TexParams.fade_amount	= F.Rdword();
    m_TexParams.mip_filter	= F.Rdword();
    m_TexParams.width		= F.Rdword();
    m_TexParams.height		= F.Rdword();

    if (F.FindChunk(THM_CHUNK_TEXTURE_TYPE)){
    	m_TexParams.type	= F.Rdword();
    }
    
    if (F.FindChunk(THM_CHUNK_DETAIL_EXT)){
        F.RstringZ			(m_TexParams.detail_name);
        m_TexParams.detail_scale = F.Rfloat();
    }

    R_ASSERT(F.FindChunk(THM_CHUNK_TYPE));
    m_Type	= THMType(F.Rdword());

    m_Age = Engine.FS.GetFileAge(fn);

    return true;
}

void EImageThumbnail::Save(int age, FSPath* path){
	if (!Valid()) return;

    CFS_Memory F;
	F.open_chunk	(THM_CHUNK_VERSION);
	F.Wword			(THM_CURRENT_VERSION);
	F.close_chunk	();

	F.open_chunk	(THM_CHUNK_DATA);
    F.write			(m_Pixels.begin(),m_Pixels.size()*sizeof(DWORD));
	F.close_chunk	();

	F.open_chunk	(THM_CHUNK_TEXTUREPARAM);
    F.write			(&m_TexParams.fmt,sizeof(STextureParams::ETFormat));
    F.Wdword		(m_TexParams.flag);
    F.Wdword		(m_TexParams.border_color);
    F.Wdword		(m_TexParams.fade_color);
    F.Wdword		(m_TexParams.fade_amount);
    F.Wdword		(m_TexParams.mip_filter);
    F.Wdword		(m_TexParams.width);
    F.Wdword		(m_TexParams.height);
	F.close_chunk	();

	F.open_chunk	(THM_CHUNK_TEXTURE_TYPE);
    F.Wdword		(m_TexParams.type);
	F.close_chunk	();

    
	F.open_chunk	(THM_CHUNK_DETAIL_EXT);
    F.WstringZ		(m_TexParams.detail_name);
    F.Wfloat		(m_TexParams.detail_scale);
	F.close_chunk	();

    F.open_chunk	(THM_CHUNK_TYPE);
    F.Wdword		(m_Type);
	F.close_chunk	();

	AnsiString fn 	= m_Name;
    if (path) path->Update(fn);
    else{
        switch (m_Type){
        case EITObject: Engine.FS.m_Objects.Update(fn); 	break;
        case EITTexture:Engine.FS.m_Textures.Update(fn); 	break;
        }
    }
	Engine.FS.VerifyPath(fn.c_str());

    F.SaveTo		(fn.c_str(),THM_SIGN);

    Engine.FS.SetFileAge	(fn,age?age:m_Age);
}

bool EImageThumbnail::FillProp(PropValueVec& values)
{
	STextureParams& F	= m_TexParams;
    FILL_PROP(values,	"Format",				(LPVOID)&F.fmt,			PROP::CreateToken	(tfmt_token));
    FILL_PROP(values,	"Type",					(LPVOID)&F.type,		PROP::CreateToken	(ttype_token));
            
    FILL_PROP(values,	"MipMaps\\Enabled",		&F.flag,				PROP::CreateFlag	(STextureParams::flGenerateMipMaps));
    FILL_PROP(values,	"MipMaps\\Filter",		(LPVOID)&F.mip_filter,	PROP::CreateToken	(tparam_token));

    FILL_PROP(values,	"Details\\Enabled",		&F.flag,				PROP::CreateFlag	(STextureParams::flHasDetailTexture));
    FILL_PROP(values,	"Details\\Texture",		F.detail_name,			PROP::CreateTexture	(sizeof(F.detail_name)));
    FILL_PROP(values,	"Details\\Scale",		&F.detail_scale,		PROP::CreateFloat	(0.1f,10000.f,0.1f,2));

    FILL_PROP(values,	"Flags\\Grayscale",		&F.flag,				PROP::CreateFlag	(STextureParams::flGreyScale));
    FILL_PROP(values,	"Flags\\Binary Alpha",	&F.flag,				PROP::CreateFlag	(STextureParams::flBinaryAlpha));
    FILL_PROP(values,	"Flags\\Dither",		&F.flag,				PROP::CreateFlag	(STextureParams::flDitherColor));
    FILL_PROP(values,	"Flags\\Dither Each MIP",&F.flag,				PROP::CreateFlag	(STextureParams::flDitherEachMIPLevel));
    FILL_PROP(values,	"Flags\\Implicit Lighted",&F.flag,				PROP::CreateFlag	(STextureParams::flImplicitLighted));

    FILL_PROP(values,	"Fade\\Enabled Color",	&F.flag,				PROP::CreateFlag	(STextureParams::flFadeToColor));
    FILL_PROP(values,	"Fade\\Enabled Alpha",	&F.flag,				PROP::CreateFlag	(STextureParams::flFadeToAlpha));
    FILL_PROP(values,	"Fade\\Amount",			&F.fade_amount,			PROP::CreateU32		(0,1000,0));
    FILL_PROP(values,	"Fade\\Color",			&F.fade_color,			PROP::CreateColor	());
            
    FILL_PROP(values,	"Border\\Enabled Color",&F.flag,				PROP::CreateFlag	(STextureParams::flColorBorder));
    FILL_PROP(values,	"Border\\Enabled Alpha",&F.flag,				PROP::CreateFlag	(STextureParams::flAlphaBorder));
    FILL_PROP(values,	"Border\\Color",		&F.border_color,		PROP::CreateColor	());
    return true;
}

void EImageThumbnail::Draw(TPanel* panel, TPaintBox* pbox, bool bStretch)
{
    RECT r;
    r.left = 2; r.top = 2;
    float w, h;
    w = _Width();
    h = _Height();
    if (w!=h)	pbox->Canvas->FillRect(pbox->BoundsRect);
    if (w>h){   r.right = pbox->Width-1; r.bottom = h/w*pbox->Height-1;
    }else{      r.right = w/h*pbox->Width-1; r.bottom = pbox->Height-1;}
    HDC hdc 	= GetDC	(panel->Handle);
	DrawThumbnail(hdc,m_Pixels,r.left,r.top,r.right-r.left,r.bottom-r.top,THUMB_WIDTH,THUMB_HEIGHT);
   	ReleaseDC	(panel->Handle,hdc);
}

