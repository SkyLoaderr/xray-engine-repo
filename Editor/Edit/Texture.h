//----------------------------------------------------
// file: Texture.h
//----------------------------------------------------

#ifndef _INCDEF_Texture_H_
#define _INCDEF_Texture_H_

#include "TextureParams.h"
//----------------------------------------------------

class ETextureCore;
class ETexture;
//----------------------------------------------------
#define THUMB_WIDTH 	128
#define THUMB_HEIGHT 	128
#define THUMB_SIZE 		THUMB_HEIGHT*THUMB_WIDTH
//----------------------------------------------------
class ETextureThumbnail{
	friend class ETextureCore;
    bool			m_bLoadFailed;
	AnsiString 		m_LoadName;
	DWORDVec 		m_Pixels;
    STextureParams*	m_TexParams;
// thumbnail routines    
    void 			Smooth			();
public:
					ETextureThumbnail(const char *_ShortName);
					~ETextureThumbnail();

    // thumbnail public routines
	bool 			Save			(int src_age);
	bool 			Load			();
	bool 			LoadTexParams	();
	void 			DrawNormal		(HANDLE handle, RECT *rect);
	void 			DrawStretch		(HANDLE handle, RECT *rect);
	bool 			CreateFromTexture(ETextureCore* tex);
	bool 			CreateFromTexture(const char* name=0);
	bool 			CreateFromData	(DWORDVec& p, int w, int h, int src_age, bool check=true, bool blur=false);

    STextureParams*	GetTextureParams();
    
    // validation
    IC bool			Valid			(){return (!m_Pixels.empty());}
};
//----------------------------------------------------
// all textures are DWORD arrays
class ETextureCore {
protected:
	friend class CShaderManager;
	friend class SceneBuilder;
	friend class ETextureThumbnail;

	AnsiString 		m_LoadName;
	char		 	m_ShortName[MAX_PATH];
	int 			m_RefCount;
    bool			m_bNullTex;
    bool			m_bLoadFailed;

	ETextureThumbnail* 		m_Thm;
	LPDIRECTDRAWSURFACE7 	m_Surface;

	int		   		m_Width;
	int		 		m_Height;
	BOOL    		m_AlphaPresent;
	DWORDVec 		m_Pixels;

protected:
	bool 			CreateDDSurface	();

    IC bool 		IsLoading		(){return m_Pixels.size();}

    // IO routines
    bool 			GetParams		();
    bool 			GetBMPParams	();
    bool 			GetTGAParams	();

    void			Refresh			(bool bOnlyNew);
	bool 			Load			();
	void 			Unload			();
	bool 			LoadBMP			();
	bool 			LoadTGA			();

	bool 			CheckVersionAndUpdateFiles(bool bDDS=true);

    // DX routines
	bool 			DDInit			();
	void 			DDClear			();

    int				IfNeedUpdate	(); // -1 has error
public:
					ETextureCore   	(const char *_ShortName);
	virtual			~ETextureCore  	();

	// get texture params
	IC char*		name			(){ return m_ShortName; }
	IC char*		load_name		(){ return m_LoadName.c_str(); }
	IC int 			width			(){ return m_Width; }
	IC int 			height			(){ return m_Height; }
    IC int 			alpha			(){ return m_AlphaPresent;}
    IC DWORD		GetPixel		(int x, int y){
    	R_ASSERT(Load()&&(x<m_Width)&&(y<m_Height)&&(x>=0)&&(y>=0));
        return m_Pixels[y*m_Width+x];
    }
    IC bool			GetPixel		(DWORD& clr, int x, int y){
    	if (!Load()||(x>=m_Width)||(y>=m_Height)||(x<0)||(y<0)) return false;
        clr=m_Pixels[y*m_Width+x];
        return true;
    }
    IC int			ConvertU		(float u){
	    int U 		= iFloor(u*float(m_Width) + 0.5f);
	    U 			%= m_Width;
        if (U<0) 	U+=m_Width;
        return U;
    }
    IC int			ConvertV		(float v){
    	int V 		= iFloor(v*float(m_Height)+ 0.5f);
    	V 			%= m_Height;
        if (V<0) 	V+=m_Height;
        return V;
    }
    IC bool			GetPixel		(DWORD& clr, float u, float v){
        if (!Load()) return false;
        int x = ConvertV(v);
        int y = ConvertU(u);
        if ((x>m_Width)||(y>m_Height)||(x<0)||(y<0)) return false;
    	clr=m_Pixels[y*m_Width+x];
        return true;
    }

    // set DX surface
    void 			Bind			(int stage=0);

    // thumbnail public routines
	void 			DrawThumbnail	(HANDLE handle, RECT *rect){ if (m_Thm) m_Thm->DrawNormal(handle,rect);}
	void 			StretchThumbnail(HANDLE handle, RECT *rect){ if (m_Thm) m_Thm->DrawStretch(handle,rect);}

	void 			DrawNormal		(HANDLE handle, RECT *rect);
	void 			DrawStretch		(HANDLE handle, RECT *rect);

    STextureParams*	GetTextureParams	();
    void			SaveTextureParams	();

	bool 			SaveAsTGA		(LPCSTR filename);
	bool 			SaveAsDDS		(LPCSTR filename);
    bool			MakeGameTexture	(LPCSTR filename);

    bool			UpdateTexture	(); // synchronize image,thm and dds
    bool			UpdateThumbnail	(); // synchronize thm only

    // validation
    IC bool			Valid			(){return (!m_LoadName.IsEmpty()&&(m_Width!=0)&&(m_Height!=0)&&!m_bLoadFailed);}
};
//----------------------------------------------------
#endif /*_INCDEF_Texture_H_*/

