//----------------------------------------------------
// file: Glow.h
//----------------------------------------------------

#ifndef _INCDEF_Glow_H_
#define _INCDEF_Glow_H_

#include "SceneObject.h"
#include "tlsprite.h"
class Shader;
class CFrustum;

class CGlow : public SceneObject {
    CTLSprite       m_RenderSprite;
public:
    Shader*       	m_GShader;
    AnsiString		m_ShaderName;
    AStringVec		m_TexNames;
    float           m_Range;
    Fvector         m_Position;
public:
	                CGlow       ();
	                CGlow       ( char *name );
    void            Construct   ();
	virtual         ~CGlow      ();

    void			Compile		();
    
	virtual void    Render      ( Fmatrix& parent, ERenderPriority flag );
	virtual bool    RayPick     ( float& distance,	Fvector& start,	Fvector& direction,
		                          Fmatrix& parent, SRayPickInfo* pinf = NULL );
    virtual bool 	FrustumPick	( const CFrustum& frustum, const Fmatrix& parent );
	virtual void    Move        ( Fvector& amount );
	virtual void 	Rotate		(Fvector& center, Fvector& axis, float angle);
  	virtual bool 	Load		(CStream&);
	virtual void 	Save		(CFS_Base&);
	virtual bool    GetBox      ( Fbox& box );
};

#endif /*_INCDEF_Glow_H_*/

