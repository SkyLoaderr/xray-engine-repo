//----------------------------------------------------
// file: Glow.h
//----------------------------------------------------

#ifndef _INCDEF_Glow_H_
#define _INCDEF_Glow_H_

#include "CustomObject.h"
#include "tlsprite.h"
class CFrustum;                                 

class CGlow : public CCustomObject {
    CTLSprite       m_RenderSprite;
public:
    Shader*       	m_GShader;
    AnsiString		m_ShaderName;
    AnsiString		m_TexName;
    float           m_Range;
protected:
    virtual Fvector& GetScale		()	{ FScale.set(m_Range,m_Range,m_Range); return FScale; 	}
	virtual void 	SetScale		(Fvector& sc){float v=m_Range; if (!fsimilar(FScale.x,sc.x)) v=sc.x; if (!fsimilar(FScale.y,sc.y)) v=sc.y; if (!fsimilar(FScale.z,sc.z)) v=sc.z; FScale.set(v,v,v); m_Range=v; UpdateTransform();}
public:
	                CGlow       (LPVOID data);
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

    // device dependent routine
	virtual void 	OnDeviceCreate 			();
	virtual void 	OnDeviceDestroy			();
};

#endif /*_INCDEF_Glow_H_*/

