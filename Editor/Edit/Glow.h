//----------------------------------------------------
// file: Glow.h
//----------------------------------------------------

#ifndef _INCDEF_Glow_H_
#define _INCDEF_Glow_H_

#include "CustomObject.h"
#include "tlsprite.h"
class Shader;
class CFrustum;

class CGlow : public CCustomObject {
    CTLSprite       m_RenderSprite;
public:
    Shader*       	m_GShader;
    AnsiString		m_ShaderName;
    AnsiString		m_TexName;
    float           m_Range;
    Fvector         m_Position;
public:
	                CGlow       ();
	                CGlow       ( char *name );
    void            Construct   ();
	virtual         ~CGlow      ();

    void			Compile		();

	virtual void    Render      (int priority, bool strictB2F);
	virtual bool    RayPick     ( float& distance,	Fvector& start,	Fvector& direction,
		                          SRayPickInfo* pinf = NULL );
    virtual bool 	FrustumPick	( const CFrustum& frustum );
	virtual void    Move        ( Fvector& amount );
	virtual void 	Rotate		(Fvector& center, Fvector& axis, float angle);
  	virtual bool 	Load		(CStream&);
	virtual void 	Save		(CFS_Base&);
	virtual bool    GetBox      ( Fbox& box );
};

#endif /*_INCDEF_Glow_H_*/

