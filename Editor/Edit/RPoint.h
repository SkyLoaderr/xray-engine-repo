//----------------------------------------------------
// file: rpoint.h
//----------------------------------------------------

#ifndef _INCDEF_RPoint_H_
#define _INCDEF_RPoint_H_

#include "SceneObject.h"

#define RPOINT_SIZE 0.5f

class CFrustum;

class CRPoint : public SceneObject {
    friend class    SceneBuilder;

    Fvector         m_Position;
public:
    DWORD			m_dwTeamID;
    float			m_fHeading;
public:
	                CRPoint     ();
	                CRPoint     ( char *name );
    void            Construct   ();
	virtual         ~CRPoint    ();

	virtual void    Render      ( Fmatrix& parent, ERenderPriority flag );
	virtual bool    RTL_Pick    ( float& distance,	Fvector& start,	Fvector& direction,
		                          Fmatrix& parent, SPickInfo* pinf = NULL );
    virtual bool 	FrustumPick	( const CFrustum& frustum, const Fmatrix& parent );
	virtual void    Move        ( Fvector& amount );
	virtual void 	Rotate		(Fvector& center, Fvector& axis, float angle);
	virtual void 	LocalRotate	(Fvector& axis, float angle);
  	virtual bool 	Load		(CStream&);
	virtual void 	Save		(CFS_Base&);
	virtual bool    GetBox      (Fbox& box);
};

#endif /*_INCDEF_Glow_H_*/

