#ifndef EThumbnailH
#define EThumbnailH

#include "ETextureParams.h"
#include "PropertiesListHelper.h"
#include "MXCtrls.hpp"
//------------------------------------------------------------------------------
// Custom class
//------------------------------------------------------------------------------
class ECustomThumbnail{   
public:
	enum THMType{
    	ETUndef		= -1,
    	ETObject	= 0,
        ETTexture	= 1,
        ETSound		= 2,
        force_dword = (-1)
    };
protected:
	AnsiString		m_SrcName;
	AnsiString		m_Name;
    int				m_Age;
    THMType			m_Type;
public:
					ECustomThumbnail(LPCSTR src_name, THMType type);
	virtual			~ECustomThumbnail();

    LPCSTR			Name			(){return m_Name.c_str();}
    LPCSTR			SrcName			(){return m_SrcName.c_str();}
    
    IC bool			IsClass			(THMType type){return m_Type==type;}

    // thumbnail public routines
	virtual bool 	Load			(LPCSTR src_name=0, LPCSTR path=0)=0;
	virtual void 	Save			(int age=0,LPCSTR path=0)=0;
    virtual bool	Valid			()=0;
	virtual void	FillProp		(PropItemVec& values)=0;
	virtual void	FillInfo		(PropItemVec& values)=0;
};
//------------------------------------------------------------------------------

class EImageThumbnail: public ECustomThumbnail{
	friend class CImageManager;
protected:
    U32Vec 			m_Pixels;
protected:
	void 			CreatePixels	(u32* p, u32 w, u32 h);
	void 			Draw			(TCanvas* pCanvas, const TRect& R, u32 w, u32 h, bool bUseAlpha=false);
	void 			Draw			(TMxPanel* panel, u32 w, u32 h, bool bUseAlpha=false);
    void			VFlip			();
public:
					EImageThumbnail	(LPCSTR src_name, THMType type):ECustomThumbnail(src_name, type){};
	virtual			~EImageThumbnail();
	virtual void 	Draw			(TCanvas* pCanvas, const TRect& R, bool bUseAlpha=false)=0;
	virtual void 	Draw			(TMxPanel* panel, bool bUseAlpha=false)=0;
    virtual	int		MemoryUsage		()=0;
};

class ETextureThumbnail: public EImageThumbnail{
	friend class CImageManager;
	typedef EImageThumbnail inherited;
private:
    STextureParams	m_TexParams;
public:
					ETextureThumbnail	(LPCSTR src_name, bool bLoad=true);
	virtual			~ETextureThumbnail	();

    // Texture routines
	void 			CreateFromData	(u32* p, u32 w, u32 h);
    IC u32			_Width			(){return m_TexParams.width;}
    IC u32			_Height			(){return m_TexParams.height;}
    IC u32			_Alpha			(){return m_TexParams.HasAlphaChannel();}
    // thumbnail public routines
    IC STextureParams& _Format		(){R_ASSERT(Valid());return m_TexParams;}
	virtual bool 	Load			(LPCSTR src_name=0, LPCSTR path=0);
	virtual void 	Save			(int age=0,LPCSTR path=0);
    virtual bool	Valid			(){return !m_Pixels.empty();}
	virtual void	FillProp		(PropItemVec& values);
	virtual void	FillInfo		(PropItemVec& values);
	virtual void 	Draw			(TCanvas* pCanvas, const TRect& R, bool bUseAlpha=false){inherited::Draw(pCanvas,R,_Width(),_Height(),bUseAlpha);}
	virtual void 	Draw			(TMxPanel* panel, bool bUseAlpha=false){inherited::Draw(panel,_Width(),_Height(),bUseAlpha);}

    virtual int		MemoryUsage		();
    LPCSTR			FormatString	();
};
//------------------------------------------------------------------------------

class EObjectThumbnail: public EImageThumbnail{   
	friend class CImageManager;
	typedef EImageThumbnail inherited;
private:
    u32				face_count;
    u32				vertex_count;
public:
					EObjectThumbnail	(LPCSTR src_name, bool bLoad=true);
	virtual			~EObjectThumbnail	();

    // Object routines
	void 			CreateFromData	(u32* p, u32 w, u32 h, int fc, int vc);
    IC int			_VertexCount	(){return vertex_count;}
    IC int			_FaceCount		(){return face_count;}

    // thumbnail public routines
	virtual bool 	Load			(LPCSTR src_name=0, LPCSTR path=0);
	virtual void 	Save			(int age=0,LPCSTR path=0);
    virtual bool	Valid			(){return !m_Pixels.empty();}
	virtual void	FillProp		(PropItemVec& values);
	virtual void	FillInfo		(PropItemVec& values);
	virtual void 	Draw			(TCanvas* pCanvas, const TRect& R, bool bUseAlpha=false){inherited::Draw(pCanvas,R,0,0,false);}
	virtual void 	Draw			(TMxPanel* panel, bool bUseAlpha=false){inherited::Draw(panel,0,0,false);}

    virtual int		MemoryUsage		(){return 0;}
};
//------------------------------------------------------------------------------

class ESoundThumbnail: public ECustomThumbnail{   
	friend class CSoundManager;
	typedef ECustomThumbnail inherited;
private:
	float			m_fQuality;
    float			m_fMinDist;
    float			m_fMaxDist;
    float			m_fVolume;
    u32				m_uGameType;
public:
					ESoundThumbnail	(LPCSTR src_name, bool bLoad=true);
	virtual			~ESoundThumbnail();

    // thumbnail public routines
	virtual bool 	Load			(LPCSTR src_name=0, LPCSTR path=0);
	virtual void 	Save			(int age=0,LPCSTR path=0);
    virtual bool	Valid			(){return true;}
	virtual void	FillProp		(PropItemVec& values);
	virtual void	FillInfo		(PropItemVec& values);
};
//------------------------------------------------------------------------------

EImageThumbnail* CreateThumbnail	(LPCSTR src_name, ECustomThumbnail::THMType type, bool bLoad=true);

#endif

