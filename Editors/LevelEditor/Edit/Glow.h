//----------------------------------------------------
// file: Glow.h
//----------------------------------------------------

#ifndef _INCDEF_Glow_H_
#define _INCDEF_Glow_H_

#include "tlsprite.h"
#include "customobject.h"
class CFrustum;                                 

class CGlow : public CCustomObject {
	typedef CCustomObject inherited ;
    CTLSprite       m_RenderSprite;
public:
	enum EFlags{
    	gfFixedSize	= (1<<0),
    	gfForceDWORD= u8(-1)
    };
    Shader*       	m_GShader;
    AnsiString		m_ShaderName;
    AnsiString		m_TexName;
    float           m_fRadius;
    Flags8			m_Flags;

    bool			m_bDefLoad;
    void __fastcall	ShaderChange	(PropValue* value);
protected:
    virtual Fvector& GetScale		()	{ FScale.set(m_fRadius,m_fRadius,m_fRadius); return FScale; 	}
	virtual void 	SetScale		(Fvector& sc){float v=m_fRadius; if (!fsimilar(FScale.x,sc.x)) v=sc.x; if (!fsimilar(FScale.y,sc.y)) v=sc.y; if (!fsimilar(FScale.z,sc.z)) v=sc.z; FScale.set(v,v,v); m_fRadius=v; UpdateTransform();}
public:
	                CGlow       (LPVOID data, LPCSTR name);
    void            Construct   (LPVOID data);
	virtual         ~CGlow      ();

    void			Compile		();

	virtual void    Render      (int priority, bool strictB2F);
	virtual bool    RayPick     ( float& distance,	Fvector& start,	Fvector& direction,
		                          SRayPickInfo* pinf = NULL );
    virtual bool 	FrustumPick	( const CFrustum& frustum );
  	virtual bool 	Load		(CStream&);
	virtual void 	Save		(CFS_Base&);
	virtual bool    GetBox      ( Fbox& box );
    virtual void	FillProp	( LPCSTR pref, PropItemVec& values );

    // device dependent routine
	virtual void 	OnDeviceCreate 	();
	virtual void 	OnDeviceDestroy	();
};

#endif /*_INCDEF_Glow_H_*/

