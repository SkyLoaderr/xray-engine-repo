#ifndef _INCDEF_ImageThumbnail_H_
#define _INCDEF_ImageThumbnail_H_

#include "ETextureParams.h"
#include "PropertiesListHelper.h"
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
	U32Vec 			m_Pixels;
    STextureParams	m_TexParams;
    THMType			m_Type;
public:
					EImageThumbnail	(LPCSTR src_name, THMType type, bool bLoad=true);
	virtual			~EImageThumbnail();

    IC bool			IsObject		(){return m_Type==EITObject;}
    IC bool			IsTexture		(){return m_Type==EITTexture;}
    // Object routines
    IC int			_VertexCount	(){R_ASSERT(IsObject()); return m_TexParams.vertex_count;}
    IC int			_FaceCount		(){R_ASSERT(IsObject()); return m_TexParams.face_count;}
	void 			CreateFromData	(u32* p, u32 w, u32 h, int fc, int vc);
    // Texture routines
    IC u32			_Width			(){R_ASSERT(IsTexture()); return m_TexParams.width;}
    IC u32			_Height			(){R_ASSERT(IsTexture()); return m_TexParams.height;}
    IC u32			_Alpha			(){R_ASSERT(IsTexture()); return m_TexParams.HasAlphaChannel();}
	void 			CreateFromData	(u32* p, u32 w, u32 h);
	void 			Draw			(TPanel* panel, TPaintBox* pbox, bool bStretch);
    void			VFlip			();
    // thumbnail public routines
    IC STextureParams& _Format		(){R_ASSERT(Valid());return m_TexParams;}
	bool 			Load			(LPCSTR src_name=0, FSPath* path=0);
	void 			Save			(int age=0,FSPath* path=0);
    IC bool			Valid			(){return !m_Pixels.empty();}
	void			FillProp		(PropItemVec& values);
};
//----------------------------------------------------
#endif /*_INCDEF_Texture_H_*/

