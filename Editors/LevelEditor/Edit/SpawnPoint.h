//----------------------------------------------------
// file: rpoint.h
//----------------------------------------------------

#ifndef SpawnPointH
#define SpawnPointH

#include "xrServer_Entities.h"
#include "LevelGameDef.h"

#define RPOINT_SIZE 0.5f

DEFINE_MAP(AnsiString,Shader*,ShaderMap,ShaderPairIt);

class CSpawnPoint : public CCustomObject {
	typedef CCustomObject inherited;

    friend class    SceneBuilder;
public:
	struct SSpawnData{
		CLASS_ID		m_ClassID;
		xrServerEntity*	m_Data;
        CCustomObject*	m_Object;
        SSpawnData	()
        {
			m_ClassID	= 0;
			m_Data		= 0;
    	    m_Object	= 0;
        }
        void		Create	(LPCSTR entity_ref);
        void		Destroy	();
        bool		Valid	(){return m_Data;}
        void		Render  (const Fvector& pos);

        void		Save	(CFS_Base&);
        bool		Load	(CStream&);
		bool 		ExportGame(SExportStreams& F, LPCSTR name);
        
		void		FillProp(LPCSTR pref, PropItemVec& values);
	};

	SSpawnData    	m_SpawnData;
    
    EPointType		m_Type;
    DWORD			m_dwTeamID;

    static ShaderMap m_Icons;
    static Shader* 	CreateIcon(LPCSTR name);
    static Shader* 	GetIcon(LPCSTR name);
protected:
    virtual Fvector& GetPosition	()				{ return m_SpawnData.m_Data?m_SpawnData.m_Data->o_Position:FPosition;}
    virtual void 	SetPosition		(Fvector& pos)	{ if (m_SpawnData.m_Data) m_SpawnData.m_Data->o_Position.set(pos);else FPosition.set(pos);	UpdateTransform();}
    virtual Fvector& GetRotation	()				{ return m_SpawnData.m_Data?m_SpawnData.m_Data->o_Angle:FRotation; }
    virtual void 	SetRotation		(Fvector& rot)	{ if (m_SpawnData.m_Data) m_SpawnData.m_Data->o_Angle.set(rot);	else FRotation.set(rot); UpdateTransform();}
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

	virtual void	FillProp		(LPCSTR pref, PropItemVec& values);

    virtual void	OnDeviceCreate	();
    virtual void	OnDeviceDestroy	();
};

#endif /*_INCDEF_Glow_H_*/

