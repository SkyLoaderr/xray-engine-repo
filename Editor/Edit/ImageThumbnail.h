#ifndef _INCDEF_ImageThumbnail_H_
#define _INCDEF_ImageThumbnail_H_

#include "ETextureParams.h"
//----------------------------------------------------

//----------------------------------------------------
class EImageThumbnail{
	DWORDVec 		m_Pixels;
    STextureParams	m_TexParams;
public:
					EImageThumbnail	(const char *_ShortName);
					~EImageThumbnail();

    // thumbnail public routines
	void 			Save			(int src_age);
	bool 			Load			();
	void 			DrawNormal		(HANDLE handle, RECT *rect);
	void 			DrawStretch		(HANDLE handle, RECT *rect);
	bool 			CreateFromData	(LPDWORD p, int w, int h, int src_age);
};
//----------------------------------------------------
#endif /*_INCDEF_Texture_H_*/

