//----------------------------------------------------
// file: Texture.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "Texture.h"
#include "FileSystem.h"


//----------------------------------------------------

ETM TM;

//----------------------------------------------------


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
		NConsole.print( "Texture: failed to load %s", _ShortName );
		return; };

	if( !DDInit() ){
		NConsole.print( "Texture: DD failed to initialize %s", _ShortName );
		return; };

	NConsole.print( "Texture: %s loaded", _ShortName );
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
	memset( m_Pixels.begin(), 0, sizeof(IColor)*m_Pixels.size() );

	if( !DDInit() ){
		NConsole.print( "Texture: DD failed to initialize %s", optionalname );
		return; };

	NConsole.print( "Texture: %s generated", optionalname );
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

	NConsole.print( "Error: '%s' - unsupported format !", m_ShortName );
	return false;
}

bool ETextureCore::LoadBMP(){

	char _FileName[MAX_PATH];

	strcpy( _FileName, m_ShortName );
	FS.m_Pictures.Update( _FileName );
	int hfile = _open( _FileName, _O_RDONLY|_O_BINARY );
	if( hfile <=0 ){
		strcpy( _FileName, m_ShortName );
		FS.m_LandPics.Update( _FileName );
		hfile = _open( _FileName, _O_RDONLY|_O_BINARY );
		if( hfile <=0 )
			return false;
	}
		
	long datasize = filelength( hfile );
	if( datasize <= (sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)) ){
		NConsole.print( "Error: '%s' seems to be invalid !", _FileName );
		_close( hfile );
		return false; }

	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;

	_read( hfile, &bf, sizeof(bf) );
	_read( hfile, &bi, sizeof(bi) );

	if( bf.bfType != 0x4D42 ){
		NConsole.print( "Error: the '%s' may not be of BMP format ?!", _FileName );
		_close( hfile );
		return false; }

	if( bi.biCompression != BI_RGB ){
		NConsole.print( "Error: the '%s' is compressed (unsupported now) !", _FileName );
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

	IColor *dest = m_Pixels.begin();

	for( int i=0; i<m_Height; i++){
		LPBYTE pscan = p + scansize * (m_Height-i-1);
		for( int i=0; i<m_Width; i++){
			dest->b = *pscan++;
			dest->g = *pscan++;
			dest->r = *pscan++;
			dest++;
		}
	}

	if( !CreateThumbnail(&bf,&bi,p) ){
		NConsole.print( "Error: can't create thumbnail for '%s'", _FileName );
		delete [] p;
		return false; }

	delete [] p;
	return true;
}

bool ETextureCore::LoadTGA(){
	return false;
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
		NConsole.print( "Texture: trying to initialize invalid texture" );
		return false; }

	if( !CreateDDSurface() ){
		NConsole.print( "Texture: can't create d3d texture" );
		return false; }

	if( !FillDDSurface() ){
		NConsole.print( "Texture: can't fill d3d surface" );
		return false; }

	return true;
}

void ETextureCore::DDClear(){
	SAFE_RELEASE( m_Surface );
}

//----------------------------------------------------

bool ETextureCore::CreateThumbnail(
	LPBITMAPFILEHEADER fileheader,
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

	memcpy(
		&m_Desc.ddpfPixelFormat,
		UI.backdesc(),
		sizeof(m_Desc.ddpfPixelFormat));

	m_Desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
	m_Desc.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;

	if( DD_OK!=UI.dd()->CreateSurface(&m_Desc,&m_Surface, 0 ) )
		return false;

	return true;
}

bool ETextureCore::FillDDSurface(){

	if( m_Surface == 0 )
		return false;

	if( DD_OK != m_Surface->Lock(0,&m_Desc,
		DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT|DDLOCK_WRITEONLY,0 ) )
			return false;

	WORD off_r,off_g,off_b;
	DWORD mask_r,mask_g,mask_b;
	DWORD loopW,loopH;

	_ASSERTE( (m_Desc.ddpfPixelFormat.dwRGBBitCount % 8)==0 );
	_ASSERTE( m_Desc.ddpfPixelFormat.dwRGBBitCount >= 8 );
	_ASSERTE( m_Desc.lpSurface );
	_ASSERTE( m_Desc.ddpfPixelFormat.dwRBitMask );
	_ASSERTE( m_Desc.ddpfPixelFormat.dwGBitMask );
	_ASSERTE( m_Desc.ddpfPixelFormat.dwBBitMask );

	mask_r = m_Desc.ddpfPixelFormat.dwRBitMask;
	mask_g = m_Desc.ddpfPixelFormat.dwGBitMask;
	mask_b = m_Desc.ddpfPixelFormat.dwBBitMask;

	DWORD bytepixsize = m_Desc.ddpfPixelFormat.dwRGBBitCount / 8;
	LPBYTE px = (LPBYTE) m_Desc.lpSurface;
	LPBYTE orgdata = (LPBYTE) m_Pixels.begin();

	loopW = m_Width;
	loopH = m_Height;

	DWORD addptr = 
		m_Desc.lPitch - loopW*bytepixsize;

	__asm{

			mov eax,mask_r
			bsr ebx,eax
			mov dx,31
			sub dx,bx
			mov off_r,dx

			mov eax,mask_g
			bsr ebx,eax
			mov dx,31
			sub dx,bx
			mov off_g,dx

			mov eax,mask_b
			bsr ebx,eax
			mov dx,31
			sub dx,bx
			mov off_b,dx
	}

	__asm{

			// WH loops prepare
			
			mov esi,orgdata
			mov edi,px
			mov ecx,loopH

scanloop_rgba_H:

			push ecx
			mov ecx,loopW

scanloop_rgba_W:

			mov edx,ecx

			mov ebx,0

			// B
			mov al, byte ptr [esi]
			shl eax, 24
			mov cx,  off_b
			shr eax, cl
			and eax, mask_b
			or  ebx, eax

			// G
			mov al, byte ptr [esi+1]
			shl eax,24
			mov cx,off_g
			shr eax,cl
			and eax,mask_g
			or  ebx,eax

			// R
			mov al, byte ptr [esi+2]
			shl eax,24
			mov cx,off_r
			shr eax,cl
			and eax,mask_r
			or  ebx,eax

			// store pixel

			mov ecx,bytepixsize             
mvloop_rgba:
			mov byte ptr [edi], bl
			shr ebx,8
			inc edi
			loop mvloop_rgba


			add esi,4
			mov ecx,edx
			loop scanloop_rgba_W

			// adjust scanline
			add edi,addptr

			pop ecx
			loop scanloop_rgba_H
	}



	m_Surface->Unlock(0);
	return true;
}

//----------------------------------------------------


ETexture::ETexture( char *_ShortName ){
	_ASSERTE( _ShortName );
	strcpy( m_ShortName, _ShortName );
	if( 0==(m_Ref=TM.SearchCore( m_ShortName )) ){
		m_Ref=new ETextureCore( m_ShortName );
		TM.m_Cores.push_back( m_Ref ); }
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

void ETexture::DDReinit(){
	_ASSERTE( m_Ref );
	_ASSERTE( m_Ref->m_Surface );
	m_Ref->FillDDSurface();
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

ETextureCore *ETM::SearchCore( char *_ShortName ){
	_ASSERTE( _ShortName );
	list<ETextureCore*>::iterator _It = m_Cores.begin();
	for(;_It!=m_Cores.end();_It++)
		if( 0==stricmp((*_It)->m_ShortName,_ShortName) )
			return (*_It);
	return 0;
}

//----------------------------------------------------
