//----------------------------------------------------
// file: Texture.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Texture.h"
#include "Log.h"
#include "FileSystem.h"
#include "nvdxt_options.h"
#include "dxtlib.h"

bool MakeTGA( DWORDVec& pixels, DWORD w, DWORD h, bool bAlpha, LPCSTR filename );
//----------------------------------------------------
// BMP - part
//----------------------------------------------------
bool ETextureCore::GetBMPParams(){
	AnsiString	name;
    name 		= m_LoadName;
	FS.m_Textures.Update( name );

	int hfile = open( name.c_str(), _O_RDONLY|_O_BINARY );
	if( hfile <=0 )	return false;

	DWORD datasize = filelength( hfile );
	if( datasize <= (sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)) ){
		Log->DlgMsg( mtError, "Error: '%s' seems to be invalid !", name.c_str() );
		_close( hfile );
		return false;
    }

	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;

	_read( hfile, &bf, sizeof(bf) );
	_read( hfile, &bi, sizeof(bi) );

	if( bf.bfType != 0x4D42 ){
		Log->DlgMsg( mtError, "Error: the '%s' may not be of BMP format ?!", name.c_str() );
		_close( hfile );
		return false;
    }

	if( bi.biCompression != BI_RGB ){
		Log->DlgMsg( mtError, "Error: the '%s' is compressed (unsupported now) !", name.c_str() );
		_close( hfile );
		return false;
    }

	m_Width = bi.biWidth;
	m_Height = bi.biHeight;
    if (!btwIsPow2(m_Width)||!btwIsPow2(m_Height)){
		Log->DlgMsg(mtError,"'%s' Unsupported texture format!\nPlease convert width and height to size power 2.",name.c_str());
		_close( hfile );
        return false;
    }
	m_AlphaPresent = 0;
	_close( hfile );
	return true;
}

//----------------------------------------------------
bool ETextureCore::LoadBMP(){
	if (!Valid()) return false;

	AnsiString	name;
    name 		= m_LoadName;
	FS.m_Textures.Update( name );

	int hfile = open( name.c_str(), _O_RDONLY|_O_BINARY );
	if( hfile <=0 )	return false;

	DWORD datasize = filelength( hfile );
	if( datasize <= (sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)) ){
		Log->DlgMsg( mtError, "Error: '%s' seems to be invalid !", name.c_str() );
		_close( hfile );
		return false; }

	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;

	_read( hfile, &bf, sizeof(bf) );
	_read( hfile, &bi, sizeof(bi) );

	if( bf.bfType != 0x4D42 ){
		Log->DlgMsg( mtError, "Error: the '%s' may not be of BMP format ?!", name.c_str() );
		_close( hfile );
		return false; }

	if( bi.biCompression != BI_RGB ){
		Log->DlgMsg( mtError, "Error: the '%s' is compressed (unsupported now) !", name.c_str() );
		_close( hfile );
		return false; }

	m_Width = bi.biWidth;
	m_Height = bi.biHeight;
    if (!btwIsPow2(m_Width)||!btwIsPow2(m_Height)){
		Log->DlgMsg(mtError,"'%s' Unsupported texture format!\nPlease convert width and height to size power 2.",name.c_str());
		_close( hfile );
        return false;
    }
	m_AlphaPresent = 0;
	m_Pixels.resize( m_Width*m_Height );

	DWORD scansize = m_Width * 3;
	if( (scansize%4) ) scansize += 4 - (scansize%4);

	LPBYTE p = new BYTE[scansize*m_Height];
	_lseek( hfile, bf.bfOffBits, SEEK_SET );
	_read( hfile, p, scansize*m_Height );
	_close( hfile );

	DWORD *dest = m_Pixels.begin();

	for( int i=0; i<m_Height; i++){
		LPBYTE pscan = p + scansize * (m_Height-i-1);
		for( int i=0; i<m_Width; i++){
        	*dest++ = *((DWORD*)pscan);
			pscan	+= 3;
		}
	}
	delete [] p;
	return true;
}

//----------------------------------------------------
// TGA - part
//----------------------------------------------------
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
//----------------------------------------------------

bool ETextureCore::GetTGAParams(){
	AnsiString	name;
    name 		= m_LoadName;
	FS.m_Textures.Update( name );

	CFileStream	TGA(name.c_str());
    TGAHeader   hdr;

    if (!TGA.Length()) return false;
	TGA.Read(&hdr,sizeof(TGAHeader));
    if (!((hdr.pixsize==24)||(hdr.pixsize==32))){
        Log->DlgMsg(mtError, "'%s' Unsupported texture format!\nPlease convert to 24 bit or 32(if texture contain alpha channel).",name.c_str());
        return false;
    }

//	VERIFY((hdr.imgtype==2)||(hdr.imgtype==10));
	VERIFY((hdr.pixsize==24)||(hdr.pixsize==32));	// 24bpp/32bpp
    if (!btwIsPow2(hdr.width)||!btwIsPow2(hdr.height)){
		Log->DlgMsg(mtError,"'%s' Unsupported texture format!\nPlease convert width and height to size power 2.",name.c_str());
        return false;
    }

    m_AlphaPresent = (hdr.pixsize==32)?true:false;
	m_Width		= hdr.width;
	m_Height	= hdr.height;

	return true;
}
//----------------------------------------------------

bool ETextureCore::LoadTGA( )
{
	if (!Valid()) return false;

	AnsiString	name;
    name 		= m_LoadName;
	FS.m_Textures.Update( name );

	CFileStream	TGA(name.c_str());
    TGAHeader   hdr;

    if (!TGA.Length()) return false;
	TGA.Read(&hdr,sizeof(TGAHeader));
    if (!((hdr.pixsize==24)||(hdr.pixsize==32))){
        AnsiString temp;
        temp.sprintf("'%s' Unsupported texture format!\nPlease convert to 24 bit or 32(if texture contain alpha channel).",m_ShortName);
        MessageDlg(temp, mtError, TMsgDlgButtons() << mbOK, 0);
        return false;
    }

//	VERIFY((hdr.imgtype==2)||(hdr.imgtype==10));
	VERIFY((hdr.pixsize==24)||(hdr.pixsize==32));	// 24bpp/32bpp
    if (!btwIsPow2(hdr.width)||!btwIsPow2(hdr.height)){
		Log->DlgMsg(mtError,"'%s' Unsupported texture format!\nPlease convert width and height to size power 2.",m_ShortName);
        return false;
    }

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
                        m_Pixels[dwOffset+x]=pixel;
                        x++;
                    }
                }else{
                    while( PixelCount-- ){
                        pixel = 0xffffffff;
                        if(hdr.pixsize==32) TGA.Read(&pixel,4);
                        else                TGA.Read(&pixel,3);
                        m_Pixels[dwOffset+x]=pixel;
                        x++;
                    }
                }
            }else{
                pixel = 0xffffffff;
                if(hdr.pixsize==32) TGA.Read(&pixel,4);
                else                TGA.Read(&pixel,3);
                m_Pixels[dwOffset+x] = pixel;
                x++;
            }
        }
    }

    // Check for alpha content
    if(hdr.pixsize==32)
        for( int i=0; i<(m_Width*m_Height); i++ ){
            if(RGBA_GETALPHA(m_Pixels[i])!=255){
                m_AlphaPresent = TRUE;
                break;
            }
        }
	return true;
}

//------------------------------------------------------------------------------
bool ETextureCore::SaveAsTGA(LPCSTR filename){
	VERIFY( filename );

    if (!Valid()) return false;
    
    // time check
    AnsiString 	src_name;
    src_name	= m_LoadName;
	FS.m_Textures.Update( src_name );
    if (!FS.Exist(src_name.c_str())) return false;

    if (FS.Exist(filename)&&(FS.CompareFileAge(src_name,filename)==1)&&FS.FileLength(filename)) return true;
//-----------------------------------
    if (!IsLoading())
        if (!Load()) return false;

	bool bRes = MakeTGA( m_Pixels, m_Width, m_Height, m_AlphaPresent, filename );

    // change date-time
    if (bRes){
    	FS.SetFileAgeFrom(src_name,filename);
        // copy to game folder
	    AnsiString 	game_name = ChangeFileExt(m_ShortName,".tga");
		FS.m_GameTextures.Update(game_name);
        FS.CopyFileTo(filename,game_name.c_str(),true);
    }

    Unload();
    return bRes;
}

bool MakeTGA( DWORDVec& pixels, DWORD w, DWORD h, bool bAlpha, LPCSTR filename ){
	VERIFY( filename );

    if (pixels.empty()||(w==0)||(h==0)) return false;

	TGAHeader header;
	memset( &header, 0, sizeof(header) );

    header.imgtype  = 2;
	header.width    = w;
	header.height   = h;

	if( bAlpha ){
		header.pixsize = 32;
		header.desc = 0x0f | 32; // flip
	}else{
		header.pixsize = 24;
		header.desc = 32; // flip
	}

    CFS_Memory F;
	F.write( &header, sizeof(header) );

    void* data = pixels.begin();
	if( !bAlpha ){
//		BYTE ab_buffer[4]={0,0,0,0};
//		int ab_size = 0; //4-(w*3)%4;
		for( DWORD j=0; j<h; j++){
			BYTE *p = (LPBYTE)data + w*4*j;
			for( DWORD i=0; i<w; i++){
				BYTE buffer[3] = {p[0],p[1],p[2]};
				F.write( buffer, 3 );
				p+=4;
			}
//			if(ab_size)
//				F.write( ab_buffer, ab_size );
		}
	}else{
		for( DWORD j=0; j<h; j++){
			BYTE *p = (LPBYTE)data + w*4*j;
			for( DWORD i=0; i<w; i++){
				BYTE buffer[4] = {p[0],p[1],p[2],p[3]};
				F.write( buffer, 4 );
				p+=4;
			}
		}
	}
    F.SaveTo(filename,0);
    if (FS.FileLength(filename)==0) return false;

    return true;
}

extern "C" __declspec(dllexport) 
bool DXTCompress(LPCSTR out_name, BYTE* raw_data, DWORD w, DWORD h, DWORD pitch, 
				 STextureParams* options, DWORD depth);

//--------------------------------------------------------------------------------------------------
bool ETextureCore::SaveAsDDS(LPCSTR filename){
	VERIFY( filename );

    if (!Valid()) return false;

    // time check
    AnsiString 	src_name;
    src_name	= m_LoadName;
	FS.m_Textures.Update( src_name );
    if (!FS.Exist(src_name.c_str())) return false;

    if (FS.Exist(filename)&&(FS.CompareFileAge(src_name,filename)==1)&&FS.FileLength(filename)) return true;
//-----------------------------------
    if (!IsLoading())
        if (!Load()||!GetTextureParams()) return false;

    bool bRes = DXTCompress(filename, (BYTE*)m_Pixels.begin(), m_Width, m_Height, m_Width*4, GetTextureParams(), 4);
    if (FS.FileLength(filename)==0) bRes=false;
    // change date-time
    if (bRes) FS.SetFileAgeFrom(src_name, AnsiString(filename));
    Unload();
    return bRes;
}

// only implicit lighted
bool ETextureCore::MakeGameTexture(LPCSTR filename){
	VERIFY(m_Thm);
    if (!Valid()) return false;

    CheckVersionAndUpdateFiles();

    STextureParams* TP=GetTextureParams();
    VERIFY(TP);
    if (TP->flag&EF_IMPLICIT_LIGHTED){
	    // save to game folder
	    AnsiString game_name=AnsiString(filename)+AnsiString(".tga");
		FS.m_GameTextures.Update(game_name);
	    return SaveAsTGA(game_name.c_str());
    }
    return true;
}

