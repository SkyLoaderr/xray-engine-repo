//----------------------------------------------------
// file: rpoint.h
//----------------------------------------------------

#ifndef _INCDEF_RPoint_H_
#define _INCDEF_RPoint_H_

#include "CustomObject.h"

#define RPOINT_SIZE 0.5f

class CFrustum;

class CRPoint : public CCustomObject {
    friend class    SceneBuilder;

    Fvector         m_Position;
public:
	enum EType{
    	etPlayer,
    	etEntity,
        force_dword=(-1) 
    };
    struct Flags{
    	DWORD		bActive:1;
    };
public:
    DWORD			m_dwTeamID;
    DWORD			m_dwSquadID;
    DWORD			m_dwGroupID;
    float			m_fHeading;
    Flags			m_Flags;
    EType			m_Type;
    string64		m_EntityRefs;
public:
	                CRPoint     ();
	                CRPoint     ( char *name );
    void            Construct   ();
	virtual         ~CRPoint    ();

	virtual void    Render      ( int priority, bool strictB2F );
	virtual bool    RayPick     ( float& distance,	Fvector& start,	Fvector& direction,
		                          SRayPickInfo* pinf = NULL );
    virtual bool 	FrustumPick	( const CFrustum& frustum );
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

