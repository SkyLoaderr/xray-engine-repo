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
	virtual bool    RayPick     ( float& distance,	Fvector& start,	Fvector& direction,
		                          Fmatrix& parent, SRayPickInfo* pinf = NULL );
    virtual bool 	FrustumPick	( const CFrustum& frustum, const Fmatrix& parent );
	virtual void    Move        ( Fvector& amount );
	virtual void 	Rotate		(Fvector& center, Fvector& axis, float angle);
	virtual void 	LocalRotate	(Fvector& axis, float angle);
  	virtual bool 	Load		(CStream&);
	virtual void 	Save		(CFS_Base&);
	virtual bool    GetBox      (Fbox& box);

    virtual bool 	GetPosition	(Fvector& pos){pos.set(m_Position); return true; }
    virtual bool 	GetRotate	(Fvector& rot){rot.set(m_fHeading,0,0); return true;}

    virtual void 	SetPosition	(Fvector& pos){m_Position.set(pos);}
    virtual void 	SetRotate	(Fvector& rot){m_fHeading = rot.x;}
};

#endif /*_INCDEF_Glow_H_*/

