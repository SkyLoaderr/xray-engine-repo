#include "stdafx.h"
#pragma hdrstop

#include "ImageThumbnail.h"
#include "ImageManager.h"
#include "xrImage_Resampler.h"
#include "xr_tokens.h"

//----------------------------------------------------
bool DrawThumbnail(HDC hdc, U32Vec& data, int offs_x, int offs_y, int dest_w, int dest_h, int src_w, int src_h)
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
	u32 line[THUMB_WIDTH];
    u32 sz_ln=sizeof(u32)*THUMB_WIDTH;
    u32 y2 = THUMB_WIDTH-1;
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

void EImageThumbnail::CreateFromData(u32* p, u32 w, u32 h)
{
	R_ASSERT(IsTexture()); 
	R_ASSERT(p&&(w>0)&&(h>0));
//	imf_filter	imf_box  imf_triangle  imf_bell  imf_b_spline  imf_lanczos3  imf_mitchell
	m_Pixels.resize(THUMB_SIZE);
	imf_Process(m_Pixels.begin(),THUMB_WIDTH,THUMB_HEIGHT,p,w,h,imf_box);
    m_TexParams.width = w;
    m_TexParams.height= h;
    m_TexParams.flags.set(STextureParams::flHasAlpha,FALSE);
}

void EImageThumbnail::CreateFromData(u32* p, u32 w, u32 h, int fc, int vc)
{
	R_ASSERT(p&&(w>0)&&(h>0));
	m_Pixels.resize(THUMB_SIZE);
	imf_Process(m_Pixels.begin(),THUMB_WIDTH,THUMB_HEIGHT,p,w,h,imf_box);
    m_TexParams.vertex_count = vc;
    m_TexParams.face_count	 = fc;
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

    u32 version = 0;

    R_ASSERT(F.ReadChunk(THM_CHUNK_VERSION,&version));
    if( version!=THM_CURRENT_VERSION ){
        ELog.Msg( mtError, "Thumbnail: Unsupported version.");
        return false;
    }

    R_ASSERT(F.FindChunk(THM_CHUNK_DATA));
    m_Pixels.resize(THUMB_SIZE);
    F.Read(m_Pixels.begin(),THUMB_SIZE*sizeof(u32));

    R_ASSERT(F.FindChunk(THM_CHUNK_TYPE));
    m_Type	= THMType(F.Rdword());

    if (IsTexture()){
        R_ASSERT(F.FindChunk(THM_CHUNK_TEXTUREPARAM));
        F.Read			(&m_TexParams.fmt,sizeof(STextureParams::ETFormat));
        m_TexParams.flags.set	(F.Rdword());
        m_TexParams.border_color= F.Rdword();
        m_TexParams.fade_color	= F.Rdword();
        m_TexParams.fade_amount	= F.Rdword();
        m_TexParams.mip_filter	= F.Rdword();
        m_TexParams.width		= F.Rdword();
        m_TexParams.height		= F.Rdword();

        if (F.FindChunk(THM_CHUNK_TEXTURE_TYPE)){
            m_TexParams.type	= (STextureParams::ETType)F.Rdword();
        }
    
        if (F.FindChunk(THM_CHUNK_DETAIL_EXT)){
            F.RstringZ			(m_TexParams.detail_name);
            m_TexParams.detail_scale = F.Rfloat();
        }
    }else{
        if (F.FindChunk(THM_CHUNK_OBJECTPARAM)){
            m_TexParams.face_count 		= F.Rdword();
            m_TexParams.vertex_count 	= F.Rdword();
        }
    }

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
    F.write			(m_Pixels.begin(),m_Pixels.size()*sizeof(u32));
	F.close_chunk	();

    F.open_chunk	(THM_CHUNK_TYPE);
    F.Wdword		(m_Type);
	F.close_chunk	();
    
	if (IsTexture()){
        F.open_chunk	(THM_CHUNK_TEXTUREPARAM);
        F.write			(&m_TexParams.fmt,sizeof(STextureParams::ETFormat));
        F.Wdword		(m_TexParams.flags.get());
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
    }else{
        F.open_chunk	(THM_CHUNK_OBJECTPARAM);
        F.Wdword		(m_TexParams.face_count);
        F.Wdword		(m_TexParams.vertex_count);
        F.close_chunk	();
    }

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

void EImageThumbnail::FillProp(PropItemVec& items)
{
	STextureParams& F	= m_TexParams;
	if (IsTexture()){
        PHelper.CreateToken		(items, "Format",					(u32*)&F.fmt, 			tfmt_token,4);
        PHelper.CreateToken		(items, "Type",						(u32*)&F.type,			ttype_token,4);

        PHelper.CreateFlag		(items, "MipMaps\\Enabled",			&F.flags,				STextureParams::flGenerateMipMaps);
        PHelper.CreateToken		(items, "MipMaps\\Filter",			&F.mip_filter,			tparam_token,4);

        PHelper.CreateFlag		(items, "Details\\Enabled",			&F.flags,				STextureParams::flHasDetailTexture);
        PHelper.CreateTexture	(items, "Details\\Texture",			F.detail_name,			sizeof(F.detail_name));    
        PHelper.CreateFloat		(items, "Details\\Scale",			&F.detail_scale,		0.1f,10000.f,0.1f,2);

        PHelper.CreateFlag		(items, "Flags\\Grayscale",			&F.flags,				STextureParams::flGreyScale);
        PHelper.CreateFlag		(items, "Flags\\Binary Alpha",		&F.flags,				STextureParams::flBinaryAlpha);
        PHelper.CreateFlag		(items, "Flags\\Dither",			&F.flags,				STextureParams::flDitherColor);
        PHelper.CreateFlag		(items, "Flags\\Dither Each MIP",	&F.flags,				STextureParams::flDitherEachMIPLevel);
        PHelper.CreateFlag		(items, "Flags\\Implicit Lighted",	&F.flags,				STextureParams::flImplicitLighted);

        PHelper.CreateFlag		(items, "Fade\\Enabled Color",		&F.flags,				STextureParams::flFadeToColor);
        PHelper.CreateFlag		(items, "Fade\\Enabled Alpha",		&F.flags,				STextureParams::flFadeToAlpha);
        PHelper.CreateU32		(items, "Fade\\Amount",				&F.fade_amount,			0,1000,0);
        PHelper.CreateColor		(items, "Fade\\Color",				&F.fade_color			);

        PHelper.CreateFlag		(items, "Border\\Enabled Color",	&F.flags,				STextureParams::flColorBorder);
        PHelper.CreateFlag		(items, "Border\\Enabled Alpha",	&F.flags,				STextureParams::flAlphaBorder);
        PHelper.CreateColor		(items, "Border\\Color",			&F.border_color			);
    }else{
        PHelper.CreateCaption	(items, "Face Count",				AnsiString(F.face_count).c_str());
        PHelper.CreateCaption	(items, "Vertex Count",				AnsiString(F.vertex_count).c_str());
    }
}

void EImageThumbnail::Draw(TPanel* panel, TPaintBox* pbox, bool bStretch)
{
	if (IsTexture()){
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
    }else{
        RECT r;		r.left = 2; r.top = 2;
        r.right 	= pbox->Width-1; r.bottom = pbox->Height-1;
        HDC hdc 	= GetDC	(panel->Handle);
        DrawThumbnail(hdc,m_Pixels,r.left,r.top,r.right-r.left,r.bottom-r.top,THUMB_WIDTH,THUMB_HEIGHT);
        ReleaseDC	(panel->Handle,hdc);
    }
}

