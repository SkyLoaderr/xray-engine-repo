#ifndef _INCDEF_ImageThumbnail_H_
#define _INCDEF_ImageThumbnail_H_

#include "ETextureParams.h"
//----------------------------------------------------

//----------------------------------------------------
class EImageThumbnail{
friend class CImageManager;
	AnsiString		m_Name;
    int				m_Age;
	DWORDVec 		m_Pixels;
    STextureParams	m_TexParams;
    int				m_Width;
    int				m_Height;
public:
					EImageThumbnail	(LPCSTR src_name);
					~EImageThumbnail();

    // thumbnail public routines
    IC int			_Width			(){return m_Width;}
    IC int			_Height			(){return m_Height;}
    IC STextureParams& _Format		(){R_ASSERT(Valid());return m_TexParams;}
	bool 			Load			();
	void 			Save			(int age=0);
	void 			DrawNormal		(HANDLE handle, RECT *rect);
	void 			DrawStretch		(HANDLE handle, RECT *rect);
	void 			CreateFromData	(LPDWORD p, int w, int h);
    IC bool			Valid			(){return !m_Pixels.empty()&&(m_Width>0)&&(m_Height>0);}
};
//----------------------------------------------------
#endif /*_INCDEF_Texture_H_*/

