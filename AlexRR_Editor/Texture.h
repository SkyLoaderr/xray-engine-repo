//----------------------------------------------------
// file: Texture.h
//----------------------------------------------------

#ifndef _INCDEF_Texture_H_
#define _INCDEF_Texture_H_

// to access D3D & DDraw
#include "UI_Main.h"
//----------------------------------------------------

class ETextureCore;
class ETexture;
class ETM;

//----------------------------------------------------

// all textures are IColor arrays
class ETextureCore {
protected:

	friend class SceneBuilder;
	friend class ETexture;
	friend class ETM;

	char m_ShortName[MAX_PATH];
	int m_RefCount;

	LPDIRECTDRAWSURFACE7 m_Surface;
	DDSURFACEDESC2 m_Desc;

	int m_Width;
	int m_Height;
	int m_AlphaPresent;
	vector<IColor> m_Pixels;

	HBITMAP m_GDI_Thumbnail;

protected:

	bool CreateDDSurface();
	bool FillDDSurface();

	bool CreateThumbnail(
		LPBITMAPFILEHEADER fileheader,
		LPBITMAPINFOHEADER infoheader,
		LPBYTE pixels );
	
	bool MakeT( char *filename );

	bool Load();
	void Unload();
	bool DDInit();
	void DDClear();

	bool LoadBMP();
	bool LoadTGA();

public:

	__forceinline void Bind(){
		_ASSERTE( UI.ddvalid() );
		UI.d3d()->SetTexture( 0, m_Surface ); }

	void DrawThumbnail( HDC hdc, RECT *rect );
	void StretchThumbnail( HDC hdc, RECT *rect );

	ETextureCore( char *_ShortName );
	ETextureCore( int w, int h, char *optionalname );
	~ETextureCore();
};


//----------------------------------------------------

class ETM {
protected:

	friend class SceneBuilder;
	friend class ETextureCore;
	friend class ETexture;
	
	list<ETextureCore*> m_Cores;
	list<ETexture*> m_Tex;
	
	ETextureCore *m_Current;

public:

	ETextureCore *SearchCore( char *_ShortName );
	ETM();
	~ETM();

	__forceinline void BindNone(){
		_ASSERTE( UI.ddvalid() );
		UI.d3d()->SetTexture( 0, 0 );
		m_Current = 0; }
};

//----------------------------------------------------

extern ETM TM;

//----------------------------------------------------

class ETexture {
protected:

	friend class SceneBuilder;
	friend class ETextureCore;
	friend class ETM;

	ETextureCore *m_Ref;
	char m_ShortName[MAX_PATH];

public:

	__forceinline void Bind(){
		_ASSERTE( m_Ref );
		if( m_Ref != TM.m_Current ){
			TM.m_Current = m_Ref;
			m_Ref->Bind(); }}

	__forceinline char *name(){ return m_ShortName; }
	__forceinline int width(){ return m_Ref->m_Width; }
	__forceinline int height(){ return m_Ref->m_Height; }
	
	__forceinline void thumb( HDC hdc, RECT *rect ){
		_ASSERTE( m_Ref && hdc && rect );
		m_Ref->DrawThumbnail(hdc,rect); }
	__forceinline void stretchthumb( HDC hdc, RECT *rect ){
		_ASSERTE( m_Ref && hdc && rect );
		m_Ref->StretchThumbnail(hdc,rect); }
	
	void DDReinit();
	bool MakeT( char *filename );

	ETexture( char *_ShortName );
	ETexture( ETexture *source );
	ETexture( int w, int h, char *optionalname );
	~ETexture();
};


typedef list<ETexture*> TextureList;
typedef list<ETexture*>::iterator TextureIt;

//----------------------------------------------------
#endif /*_INCDEF_Texture_H_*/

