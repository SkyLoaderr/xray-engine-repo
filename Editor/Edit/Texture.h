//----------------------------------------------------
// file: Texture.h
//----------------------------------------------------

#ifndef _INCDEF_Texture_H_
#define _INCDEF_Texture_H_

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
	vector<FPcolor> m_Pixels;

	HBITMAP m_GDI_Thumbnail;

protected:

	bool CreateDDSurface();
	bool FillDDSurface();

	bool CreateThumbnail(
		LPBITMAPINFOHEADER infoheader,
		LPBYTE pixels );

	bool MakeT( char *filename );
	bool MakeTGA( char *filename );

	bool Load();
	void Unload();
	bool DDInit();
	void DDClear();

	bool LoadBMP();
	bool LoadTGA();

    void Vflip();
    void Hflip();
public:
    void Bind();

	void DrawThumbnail( HDC hdc, RECT *rect );
	void StretchThumbnail( HDC hdc, RECT *rect );

	__forceinline DWORD	GetPixel(int x, int y)			{ return m_Pixels[y*m_Width+x].c;}
	__forceinline void	PutPixel(int x, int y, DWORD p)	{ m_Pixels[y*m_Width+x].c = p;	}

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
    void BindNone();

	ETextureCore *SearchCore( char *_ShortName );

    void ReleaseD3DSurfaces(){
        BindNone();
        list<ETextureCore*>::iterator _It = m_Cores.begin();
        for(;_It!=m_Cores.end();_It++) (*_It)->DDClear();
    }

	ETM();
	~ETM();
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
    void Bind();
    bool valid(){return m_Ref;}

	__forceinline char *name(){ return m_ShortName; }
	__forceinline int width(){ return m_Ref->m_Width; }
	__forceinline int height(){ return m_Ref->m_Height; }
    __forceinline int alpha(){ return m_Ref->m_AlphaPresent;}

	__forceinline void thumb( HDC hdc, RECT *rect ){
		_ASSERTE( m_Ref && hdc && rect );
		m_Ref->DrawThumbnail(hdc,rect); }
	__forceinline void stretchthumb( HDC hdc, RECT *rect ){
		_ASSERTE( m_Ref && hdc && rect );
		m_Ref->StretchThumbnail(hdc,rect); }

	void DDReinit();
	bool MakeT( char *filename );
	bool MakeTGA( char *filename );

	ETexture( char *_ShortName );
	ETexture( ETexture *source );
	ETexture( int w, int h, char *optionalname );
	~ETexture();
};


typedef list<ETexture*> TextureList;
typedef list<ETexture*>::iterator TextureIt;

//----------------------------------------------------
#endif /*_INCDEF_Texture_H_*/

