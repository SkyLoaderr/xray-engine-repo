//----------------------------------------------------
// file: rpoint.h
//----------------------------------------------------

#ifndef _INCDEF_RPoint_H_
#define _INCDEF_RPoint_H_

#include "xrServer_Entities.h"
#include "LevelGameDef.h"

#define RPOINT_SIZE 0.5f

class CSpawnPoint : public CCustomObject {
	typedef CCustomObject inherited;

    friend class    SceneBuilder;
public:
	CLASS_ID		m_SpawnClassID;
	xrServerEntity*	m_SpawnData;

    EPointType		m_Type;
    DWORD			m_dwTeamID;
protected:
    virtual Fvector& GetPosition	()				{ return m_SpawnData?m_SpawnData->o_Position:FPosition;}
    virtual void 	SetPosition		(Fvector& pos)	{ if (m_SpawnData) m_SpawnData->o_Position.set(pos);else FPosition.set(pos);	UpdateTransform();}
    virtual Fvector& GetRotation	()				{ return m_SpawnData?m_SpawnData->o_Angle:FRotation; }
    virtual void 	SetRotation		(Fvector& rot)	{ if (m_SpawnData) m_SpawnData->o_Angle.set(rot);	else FRotation.set(rot); UpdateTransform();}
public:
	                CSpawnPoint    	(LPVOID data, LPCSTR name);
    void            Construct   	(LPVOID data);
    virtual         ~CSpawnPoint   	();

    bool			CreateSpawnData	(LPCSTR entity_ref);
	virtual void    Render      	( int priority, bool strictB2F );
	virtual bool    RayPick     	( float& distance,	Fvector& start,	Fvector& direction,
		                          	  SRayPickInfo* pinf = NULL );
    virtual bool 	FrustumPick		( const CFrustum& frustum );
	virtual bool    GetBox      	(Fbox& box);

  	virtual bool 	Load			(CStream&);
	virtual void 	Save			(CFS_Base&);
    virtual bool	ExportGame		(SExportStreams& data);

	virtual void	FillProp		(LPCSTR pref, PropValueVec& values);
};

#endif /*_INCDEF_Glow_H_*/

