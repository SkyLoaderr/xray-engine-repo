//----------------------------------------------------
// file: Texture.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Log.h"
#include "Texture.h"
#include "FileSystem.h"
#include "ui_main.h"
#include "ffileops.h"
#include "d3dx.h"
//----------------------------------------------------
ETM TM;

//----------------------------------------------------

void ETextureCore::Bind(){
    _ASSERTE( UI.ddvalid() );
    if (!m_Surface){
        if( !DDInit() ){
            Log.Msg( "Texture: DD failed to initialize %s", m_ShortName );
            return;
        };
    }

	CDX(UI.d3d()->SetTexture( 0, m_Surface ));
}

ETextureCore::ETextureCore( char *_ShortName ){

	_ASSERTE( _ShortName );
	strcpy( m_ShortName, _ShortName );
	
	m_RefCount = 0;
	
	m_Surface = 0;
	memset( &m_Desc, 0, sizeof(m_Desc) );
	
	m_Width = 0;
	m_Height = 0;
	m_AlphaPresent = 0;

	m_GDI_Thumbnail = 0;

	if( !Load() ){
		Log.Msg( "Texture: failed to load %s", _ShortName );
		return; };

/*
	if( !DDInit() ){
		Log.Msg( "Texture: DD failed to initialize %s", optionalname );
		return; };
*/
	Log.Msg( "Texture: %s loaded", _ShortName );
}

ETextureCore::ETextureCore( int w, int h, char *optionalname ){
	_ASSERTE( optionalname );
	strcpy( m_ShortName, optionalname );
	m_RefCount = 0;
	m_Surface = 0;
	memset( &m_Desc, 0, sizeof(m_Desc) );
	m_GDI_Thumbnail = 0;
	m_Width = w;
	m_Height = h;
	m_AlphaPresent = 0;
	m_Pixels.resize( m_Width*m_Height );
	memset( m_Pixels.begin(), 0, sizeof(FPcolor)*m_Pixels.size() );
	Log.Msg( "Texture: %s generated", optionalname );
}

ETextureCore::~ETextureCore(){
	_ASSERTE( m_RefCount == 0 );
	if( TM.m_Current == this ){
		TM.m_Current = 0; }
	DDClear();
	Unload();
}

//----------------------------------------------------


#pragma pack(push,1)
struct _fileT_header {
	DWORD fmt;
	DWORD dwFlags;
	DWORD dwWidth;
	DWORD dwHeight;
	DWORD dwMipCount;
	DWORD dwPitch;
	DWORD reserved[32-6];
};
#pragma pack(pop)


bool ETextureCore::MakeT( char *filename ){
	_ASSERTE( filename );

	_fileT_header header;
	memset( &header, 0, sizeof(header) );

	header.fmt = m_AlphaPresent ? 3 : 2;
	header.dwWidth = m_Width;
	header.dwHeight = m_Height;
	header.dwMipCount = 1;
	header.dwPitch = m_Width * 4;

	int handle = FS.create( filename );
	FS.write( handle, &header, sizeof(header) );
	FS.write( handle, m_Pixels.begin(), m_Pixels.size() * 4 );
	FS.close( handle );
	return true;
}

//----------------------------------------------------

bool ETextureCore::Load(){

	char fileext[MAX_PATH];
	_splitpath( m_ShortName, 0, 0, 0, fileext );

	if( 0==stricmp( fileext, ".bmp" )) return LoadBMP();
	if( 0==stricmp( fileext, ".tga" )) return LoadTGA();

	Log.Msg( "Error: '%s' - unsupported format !", m_ShortName );
	return false;
}

bool ETextureCore::LoadBMP(){
    char fn [MAX_PATH];

    strcpy(fn,m_ShortName);
	FS.m_Pictures.Update( fn );
	int hfile = open( fn, _O_RDONLY|_O_BINARY );
	if( hfile <=0 )	return false;

	DWORD datasize = filelength( hfile );
	if( datasize <= (sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)) ){
		Log.Msg( "Error: '%s' seems to be invalid !", fn );
		_close( hfile );
		return false; }

	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;

	_read( hfile, &bf, sizeof(bf) );
	_read( hfile, &bi, sizeof(bi) );

	if( bf.bfType != 0x4D42 ){
		Log.Msg( "Error: the '%s' may not be of BMP format ?!", fn );
		_close( hfile );
		return false; }

	if( bi.biCompression != BI_RGB ){
		Log.Msg( "Error: the '%s' is compressed (unsupported now) !", fn );
		_close( hfile );
		return false; }

	m_Width = bi.biWidth;
	m_Height = bi.biHeight;
	m_AlphaPresent = 0;
	m_Pixels.resize( m_Width*m_Height );

	DWORD scansize = m_Width * 3;
	if( (scansize%4) ) scansize += 4 - (scansize%4);

	LPBYTE p = new BYTE[scansize*m_Height];
	_lseek( hfile, bf.bfOffBits, SEEK_SET );
	_read( hfile, p, scansize*m_Height );
	_close( hfile );

	FPcolor *dest = m_Pixels.begin();

	for( int i=0; i<m_Height; i++){
		LPBYTE pscan = p + scansize * (m_Height-i-1);
		for( int i=0; i<m_Width; i++){
			dest->b = *pscan++;
			dest->g = *pscan++;
			dest->r = *pscan++;
			dest++;
		}
	}

	if( !CreateThumbnail(&bi,p) ){
		Log.Msg( "Error: can't create thumbnail for '%s'", fn );
		delete [] p;
		return false; }

	delete [] p;
	return true;
}

//-----------------------------------------------------------------------------
//    Load an TGA file
//-----------------------------------------------------------------------------
BOOL	isPowerOf2(DWORD d)
{
	switch (d) {
	case 1:
	case 2:
	case 4:
	case 8:
	case 16:
	case 32:
	case 64:
	case 128:
	case 256:
	case 512:
	case 1024:
	case 2048:
	case 4096:
	case 8192:
		return true;
	default:
		return false;
	}
}
void ETextureCore::Vflip()
{
	_ASSERTE(m_Pixels.size());
	for (int y=0; y<m_Height/2; y++){
		DWORD y2 = m_Height-y-1;
		for (int x=0; x<m_Height; x++) {
			DWORD t = GetPixel(x,y);
			PutPixel(x,y, GetPixel(x,y2));
			PutPixel(x,y2,t);
		}
	}
}
void ETextureCore::Hflip()
{
	_ASSERTE(m_Pixels.size());
	for (int y=0; y<m_Height; y++)
	{
		for (int x=0; x<m_Width/2; x++) {
			DWORD x2 = m_Width-x-1;
			DWORD t = GetPixel(x,y);
			PutPixel(x,y, GetPixel(x2,y));
			PutPixel(x2,y,t);
		}
	}
}
#define uchar           unsigned char
#pragma pack(push,1)     // Gotta pack these structures!
struct TGAHeader
{
	uchar	idlen;
	uchar	cmtype;
	uchar	imgtype;

	WORD	cmorg;
	WORD	cmlen;
	uchar	cmes;

	short	xorg;
	short	yorg;
	short	width;
	short	height;
	uchar	pixsize;
	uchar	desc;
};
#pragma pack(pop)

bool ETextureCore::LoadTGA( )
{
    char name   [MAX_PATH];
    strcpy      (name,m_ShortName);
	FS.m_Pictures.Update( name );

	CStream		TGA(name);
    TGAHeader   hdr;

	TGA.Read(&hdr,sizeof(TGAHeader));
	_ASSERTE((hdr.imgtype==2)||(hdr.imgtype==10));
	_ASSERTE((hdr.pixsize==24)||(hdr.pixsize==32));	// 24bpp/32bpp
	_ASSERTE(isPowerOf2(hdr.width));
	_ASSERTE(isPowerOf2(hdr.height));

    m_AlphaPresent = FALSE;
	m_Width		= hdr.width;
	m_Height	= hdr.height;
  	m_Pixels.resize( m_Width*m_Height );

	// Skip funky stuff
	if (hdr.idlen)	TGA.Advance(hdr.idlen);
	if (hdr.cmlen)	TGA.Advance(hdr.cmlen*((hdr.cmes+7)/8));

    DWORD pixel;
    for( int y=0; y<m_Height; y++ ){
        DWORD dwOffset = y*m_Width;

        if( 0 == ( hdr.desc & 0x0010 ) ) dwOffset = (m_Height-y-1)*m_Width;
        for( int x=0; x<m_Width; x ){
            if( hdr.imgtype == 10 ){
                BYTE PacketInfo; TGA.Read(&PacketInfo,1);
                WORD PacketType = 0x80 & PacketInfo;
                WORD PixelCount = ( 0x007f & PacketInfo ) + 1;
                if( PacketType ){
                    pixel = 0xffffffff;
                    if(hdr.pixsize==32) TGA.Read(&pixel,4);
                    else                TGA.Read(&pixel,3);
                    while( PixelCount-- ){
                        m_Pixels[dwOffset+x].set(pixel);
                        x++;
                    }
                }else{
                    while( PixelCount-- ){
                        pixel = 0xffffffff;
                        if(hdr.pixsize==32) TGA.Read(&pixel,4);
                        else                TGA.Read(&pixel,3);
                        m_Pixels[dwOffset+x].set(pixel);
                        x++;
                    }
                }
            }else{
                pixel = 0xffffffff;
                if(hdr.pixsize==32) TGA.Read(&pixel,4);
                else                TGA.Read(&pixel,3);
                m_Pixels[dwOffset+x].set(pixel);
                x++;
            }
        }
    }

    // Check for alpha content
    if(hdr.pixsize==32)
        for( int i=0; i<(m_Width*m_Height); i++ ){
            if(m_Pixels[i].a!=255){
                m_AlphaPresent = TRUE;
                break;
            }
        }

    BITMAPINFOHEADER bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.biSize = sizeof(bi);
    bi.biWidth = m_Width;
    bi.biHeight = m_Height;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    BYTE*	ptr	   = (BYTE*)&(m_Pixels.begin()->c);
    Vflip();
	if( !CreateThumbnail(&bi,ptr) ){
        Vflip();
		Log.Msg( "Error: can't create thumbnail for '%s'", name );
		return false; }
    Vflip();
	return true;
}

/*
bool ETextureCore::LoadTGA()
{
    char name   [MAX_PATH];
    strcpy      (name,m_ShortName);
	FS.m_Pictures.Update( name );

	CStream		TGA(name);
	TGAHeader	hdr;
	BOOL		hflip, vflip;

	TGA.Read(&hdr,sizeof(TGAHeader));
	_ASSERTE(hdr.imgtype==2);							// Uncompressed
	_ASSERTE((hdr.pixsize==24) || (hdr.pixsize==32));	// 24bpp/32bpp
	_ASSERTE(isPowerOf2(hdr.width));
	_ASSERTE(isPowerOf2(hdr.height));

	// Skip funky stuff
	if (hdr.idlen)	TGA.Advance(hdr.idlen);
	if (hdr.cmlen)	TGA.Advance(hdr.cmlen*((hdr.cmes+7)/8));

	hflip		= (hdr.desc & 0x10) ? TRUE : FALSE;		// Need hflip
	vflip		= (hdr.desc & 0x20) ? TRUE : FALSE;		// Need vflip

	m_Width		= hdr.width;
	m_Height	= hdr.height;
  	m_Pixels.resize( m_Width*m_Height );

	if (hdr.pixsize==24){
    // 24bpp
		m_AlphaPresent  = FALSE;
		DWORD	pixel	= 0;
		DWORD*	ptr		= (DWORD*)m_Pixels.begin();
		for(int iy = 0; iy<hdr.height; ++iy) {
			for(int ix=0; ix<hdr.width; ++ix) {
				TGA.Read(&pixel,3);
				*ptr++ = pixel;
			}
		}
	}else{
    // 32bpp
		m_AlphaPresent = TRUE;
		DWORD*	ptr	   = &(m_Pixels.begin()->c);
		TGA.Read(ptr,hdr.width*hdr.height*4);
	}
    BITMAPINFOHEADER bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.biSize = sizeof(bi);
    bi.biWidth = m_Width;
    bi.biHeight = m_Height;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;

	if (vflip){
        BYTE*	ptr	   = (BYTE*)&(m_Pixels.begin()->c);
        if( !CreateThumbnail(&bi,ptr) ){
            Log.Msg( "Error: can't create thumbnail for '%s'", name );
            return false; }
        Vflip();
    }else{
        Vflip();
        BYTE*	ptr	   = (BYTE*)&(m_Pixels.begin()->c);
        if( !CreateThumbnail(&bi,ptr) ){
            Log.Msg( "Error: can't create thumbnail for '%s'", name );
            return false; }
        Vflip();
    }
	if (hflip) Hflip();

	return true;
}
*/
bool ETextureCore::MakeTGA( char *filename ){
	_ASSERTE( filename );

	TGAHeader header;
	memset( &header, 0, sizeof(header) );

    header.imgtype = 2;
	header.width = m_Width;
	header.height = m_Height;

	if( m_AlphaPresent ){
		header.pixsize = 32;
		header.desc = 0x0f | 32; // flip
	}else{
		header.pixsize = 24;
		header.desc = 32; // flip
	}

	int handle = FS.create( filename );
	FS.write( handle, &header, sizeof(header) );

    void* data = m_Pixels.begin();
	if( !m_AlphaPresent ){
		BYTE ab_buffer[4]={0,0,0,0};
		int real_sl = ((m_Width*3)) & 3;
		int ab_size = real_sl ? 4-real_sl : 0 ;
		for( int j=0; j<m_Height; j++){
			BYTE *p = (LPBYTE)data + m_Width*4*j;
			for( int i=0; i<m_Width; i++){
				BYTE buffer[3] = {p[0],p[1],p[2]};
				FS.write( handle, buffer, 3 );
				p+=4;
			}
			if(ab_size)
				FS.write( handle, ab_buffer, ab_size );
		}
	}else{
		for( int j=0; j<m_Height; j++){
			BYTE *p = (LPBYTE)data + m_Width*4*j;
			for( int i=0; i<m_Width; i++){
				BYTE buffer[4] = {p[0],p[1],p[2],p[3]};
				FS.write( handle, buffer, 4 );
				p+=4;
			}
		}
	}

	FS.close( handle );
    return true;
}

void ETextureCore::Unload(){
	if( m_GDI_Thumbnail ){
		DeleteObject( m_GDI_Thumbnail );
		m_GDI_Thumbnail = 0; }
	m_Width = 0;
	m_Height = 0;
	m_Pixels.clear();
}

bool ETextureCore::DDInit(){

	if( m_Width==0 || m_Height==0 ){
		Log.Msg( "Texture: trying to initialize invalid texture" );
		return false; }

	if( !CreateDDSurface() ){
		Log.Msg( "Texture: can't create d3d texture" );
		return false; }

	if( !FillDDSurface() ){
		Log.Msg( "Texture: can't fill d3d surface" );
		return false; }

	return true;
}

void ETextureCore::DDClear(){
	SAFE_RELEASE( m_Surface );
}

//----------------------------------------------------

bool ETextureCore::CreateThumbnail(
	LPBITMAPINFOHEADER infoheader,
	LPBYTE pixels )
{
	HDC hdc = GetDC(0);
	m_GDI_Thumbnail = CreateDIBitmap(
		hdc, infoheader, CBM_INIT, pixels,
		(LPBITMAPINFO) infoheader, DIB_RGB_COLORS );
	ReleaseDC( 0, hdc );
	return (m_GDI_Thumbnail!=0);
}

void ETextureCore::DrawThumbnail( HDC hdc, RECT *rect ){
	
	if( m_GDI_Thumbnail == 0 ){
		BitBlt(hdc,rect->left,rect->top,
			rect->right-rect->left,
			rect->bottom-rect->top,0,0,0,BLACKNESS);
		return;
	}

	HDC hdcmem = CreateCompatibleDC( hdc );
	HBITMAP oldbitmap = (HBITMAP) SelectObject( hdcmem, m_GDI_Thumbnail );

	BitBlt(hdc,rect->left,rect->top,
		rect->right-rect->left,
		rect->bottom-rect->top,hdcmem,0,0,SRCCOPY);

	SelectObject( hdcmem, oldbitmap );
	DeleteDC( hdcmem );
}

void ETextureCore::StretchThumbnail( HDC hdc, RECT *rect ){
	
	if( m_GDI_Thumbnail == 0 ){
		BitBlt(hdc,rect->left,rect->top,
			rect->right-rect->left,
			rect->bottom-rect->top,0,0,0,BLACKNESS);
		return;
	}

	HDC hdcmem = CreateCompatibleDC( hdc );
	HBITMAP oldbitmap = (HBITMAP) SelectObject( hdcmem, m_GDI_Thumbnail );

    SetStretchBltMode(hdc, STRETCH_HALFTONE);

	StretchBlt(hdc,rect->left,rect->top,
		rect->right-rect->left,
		rect->bottom-rect->top,
		hdcmem,0,0,
		m_Width,m_Height,
		SRCCOPY);

	SelectObject( hdcmem, oldbitmap );
	DeleteDC( hdcmem );
}

bool ETextureCore::CreateDDSurface(){

	_ASSERTE( m_Width && m_Height );
	_ASSERTE( UI.ddvalid() );

	memset( &m_Desc, 0, sizeof(m_Desc) );
	m_Desc.dwSize = sizeof(m_Desc);

	m_Desc.dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_CAPS;
	m_Desc.dwHeight = m_Height;
	m_Desc.dwWidth = m_Width;

    ZeroMemory(&m_Desc.ddpfPixelFormat,sizeof(m_Desc.ddpfPixelFormat));
    m_Desc.ddpfPixelFormat.dwSize = sizeof(m_Desc.ddpfPixelFormat);
    m_Desc.ddpfPixelFormat.dwFlags = m_AlphaPresent?(DDPF_RGB|DDPF_ALPHAPIXELS):(DDPF_RGB);
    m_Desc.ddpfPixelFormat.dwRGBBitCount = 32;
    m_Desc.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
    m_Desc.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
    m_Desc.ddpfPixelFormat.dwBBitMask = 0x000000FF;
    m_Desc.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;

	m_Desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
	m_Desc.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;

	if( DD_OK!=UI.dd()->CreateSurface(&m_Desc,&m_Surface, 0 ) )
		return false;

	return true;
}
bool ETextureCore::FillDDSurface(){
    DDSURFACEDESC2       ddsd2;
    LPDIRECTDRAWSURFACE7 lpDDS;

    // Initialize the surface description.
    ZeroMemory(&ddsd2, sizeof(DDSURFACEDESC2));
    ddsd2.dwSize = sizeof(ddsd2);
    ddsd2.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_LPSURFACE |
                    DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_CAPS;
    ddsd2.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    ddsd2.dwHeight = m_Height;
    ddsd2.dwWidth = m_Width;
    ddsd2.lPitch = m_Width*4;
    ddsd2.lpSurface = &(m_Pixels.front().c);
    CopyMemory(&ddsd2.ddpfPixelFormat, &m_Desc.ddpfPixelFormat, sizeof(DDPIXELFORMAT));
    // Create the surface
    CDX(UI.dd()->CreateSurface(&ddsd2, &lpDDS, NULL));
    VERIFY(lpDDS);
	CDX(m_Surface->Blt(NULL,lpDDS,NULL,DDBLT_WAIT,NULL));
    return true;
}

//----------------------------------------------------

ETexture::ETexture( char *_ShortName ){
	_ASSERTE( _ShortName );
	strcpy( m_ShortName, _ShortName );
	if( 0==(m_Ref=TM.SearchCore( m_ShortName )) ){
		m_Ref=new ETextureCore( m_ShortName );
		TM.m_Cores.push_back( m_Ref );
    }
	m_Ref->m_RefCount++;
	TM.m_Tex.push_back( this );
}

ETexture::ETexture( int w, int h, char *optionalname ){
	_ASSERTE( optionalname );
	strcpy( m_ShortName, optionalname );
	if( 0==(m_Ref=TM.SearchCore( m_ShortName )) ){
		m_Ref=new ETextureCore( w, h, m_ShortName );
		TM.m_Cores.push_back( m_Ref ); }
	m_Ref->m_RefCount++;
	TM.m_Tex.push_back( this );
}

ETexture::ETexture( ETexture *source ){
	_ASSERTE( source );
	_ASSERTE( source->m_Ref );
	strcpy( m_ShortName, source->m_ShortName );
	m_Ref=source->m_Ref;
	m_Ref->m_RefCount++;
	TM.m_Tex.push_back( this );
}

ETexture::~ETexture(){
	_ASSERTE( m_Ref );
	TM.m_Tex.remove( this );
	m_Ref->m_RefCount--;
	if( m_Ref->m_RefCount == 0 ){
		TM.m_Cores.remove( m_Ref );
		delete m_Ref; }
}

bool ETexture::MakeT( char *filename ){
	_ASSERTE( filename );
	if( !m_Ref )
		return false;
	return m_Ref->MakeT( filename );
}

bool ETexture::MakeTGA( char *filename ){
	_ASSERTE( filename );
	if( !m_Ref )
		return false;
	return m_Ref->MakeTGA( filename );
}

void ETexture::DDReinit(){
	_ASSERTE( m_Ref );
	_ASSERTE( m_Ref->m_Surface );
	m_Ref->FillDDSurface();
}

void ETexture::Bind(){
	_ASSERTE( m_Ref );
	if( m_Ref != TM.m_Current ){
		TM.m_Current = m_Ref;
		m_Ref->Bind();
    }
}
//----------------------------------------------------
//----------------------------------------------------


ETM::ETM(){
	_ASSERTE( m_Tex.size() == 0 );
	_ASSERTE( m_Cores.size() == 0 );
	m_Current = 0;
}

ETM::~ETM(){
	_ASSERTE( m_Tex.size() == 0 );
	_ASSERTE( m_Cores.size() == 0 );
}

void ETM::BindNone(){
    _ASSERTE( UI.ddvalid() );
	UI.d3d()->SetTexture( 0, 0 );
	m_Current = 0;
}

ETextureCore *ETM::SearchCore( char *_ShortName ){
	_ASSERTE( _ShortName );
	list<ETextureCore*>::iterator _It = m_Cores.begin();
	for(;_It!=m_Cores.end();_It++)
		if( 0==stricmp((*_It)->m_ShortName,_ShortName) )
			return (*_It);
	return 0;
}

//----------------------------------------------------
