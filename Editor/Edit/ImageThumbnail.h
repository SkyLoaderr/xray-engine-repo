#ifndef _INCDEF_ImageThumbnail_H_
#define _INCDEF_ImageThumbnail_H_

#include "ETextureParams.h"
//----------------------------------------------------

//----------------------------------------------------
class EImageThumbnail{
public:
	enum THMType{
    	EITObject	= 0,
        EITTexture	= 1,
        force_dword = (-1)
    };
private:
friend class CImageManager;
	AnsiString		m_Name;
    int				m_Age;
	DWORDVec 		m_Pixels;
    STextureParams	m_TexParams;
    THMType			m_Type;
public:
					EImageThumbnail	(LPCSTR src_name, THMType type, bool bLoad=true);
					~EImageThumbnail();

    // thumbnail public routines
    IC int			_Width			(){return m_TexParams.width;}
    IC int			_Height			(){return m_TexParams.height;}
    IC int			_Alpha			(){return m_TexParams.HasAlphaChannel();}
    IC STextureParams& _Format		(){R_ASSERT(Valid());return m_TexParams;}
	bool 			Load			();
	void 			Save			(int age=0);
	void 			DrawNormal		(HANDLE handle, RECT *rect);
	void 			DrawStretch		(HANDLE handle, RECT *rect);
	void 			CreateFromData	(LPDWORD p, int w, int h);
    IC bool			IsObject		(){return m_Type==EITObject;}
    IC bool			IsTexture		(){return m_Type==EITTexture;}
    IC bool			Valid			(){return !m_Pixels.empty()&&(_Width()>0)&&(_Height()>0);}
};
//----------------------------------------------------
#endif /*_INCDEF_Texture_H_*/

