//----------------------------------------------------
// file: rpoint.h
//----------------------------------------------------

#ifndef _INCDEF_RPoint_H_
#define _INCDEF_RPoint_H_

#include "CustomObject.h"
#include "xrServer_Entities.h"

#define RPOINT_SIZE 0.5f

class CFrustum;

class CSpawnPoint : public CCustomObject {
    friend class    SceneBuilder;
public:
	AnsiString		m_EntityRef;
	xrServerEntity*	m_SpawnData;
protected:
    virtual Fvector& GetPosition	()				{ m_SpawnData?return m_SpawnData->o_Position:return FPosition; }
    virtual void 	SetPosition		(Fvector& pos)	{ if (m_SpawnData) m_SpawnData->o_Position.set(pos) else FPosition.set(pos); UpdateTransform();}
    virtual Fvector& GetRotation	()				{ m_SpawnData?return m_SpawnData->o_Position:return FPosition; }
    virtual void 	SetRotation		(Fvector& rot)	{ if (m_SpawnData) m_SpawnData->o_Angle.set(pos); else PRotation.set(rot); UpdateTransform();}
public:
	                CSpawnPoint    	();
	                CSpawnPoint    	( char *name );
    void            Construct   	();
	virtual         ~CSpawnPoint   	();

    void			CreateSpawnData	(LPCSTR entity_ref);
	virtual void    Render      	( int priority, bool strictB2F );
	virtual bool    RayPick     	( float& distance,	Fvector& start,	Fvector& direction,
		                          	  SRayPickInfo* pinf = NULL );
    virtual bool 	FrustumPick		( const CFrustum& frustum );
  	virtual bool 	Load			(CStream&);
	virtual void 	Save			(CFS_Base&);
    virtual bool	ExportSpawn		(CFS_Base&, int chunk_id);
	virtual bool    GetBox      	(Fbox& box);
};

#endif /*_INCDEF_Glow_H_*/

