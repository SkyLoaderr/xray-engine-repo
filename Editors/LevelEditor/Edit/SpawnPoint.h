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
	xrServerEntity*	m_SpawnData;
protected:
    virtual Fvector& GetPosition	()				{ R_ASSERT(m_SpawnData); return m_SpawnData->o_Position; }
    virtual void 	SetPosition		(Fvector& pos)	{ R_ASSERT(m_SpawnData); m_SpawnData->o_Position.set(pos);	UpdateTransform();}
    virtual Fvector& GetRotation	()				{ R_ASSERT(m_SpawnData); return m_SpawnData->o_Angle; }
    virtual void 	SetRotation		(Fvector& rot)	{ R_ASSERT(m_SpawnData); m_SpawnData->o_Angle.set(rot); 	UpdateTransform();}
public:
	                CSpawnPoint    	(LPVOID data);
    void            Construct   	(LPVOID data);
    virtual         ~CSpawnPoint   	();

    bool			CreateSpawnData	(LPCSTR entity_ref);
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

