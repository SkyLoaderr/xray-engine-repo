//----------------------------------------------------
// file: rpoint.h
//----------------------------------------------------

#ifndef SpawnPointH
#define SpawnPointH

#include "xrServer_Objects_ALife_All.h"
#include "LevelGameDef.h"

#define RPOINT_SIZE 0.5f

DEFINE_MAP(AnsiString,ref_shader,ShaderMap,ShaderPairIt);

class CSpawnPoint : public CCustomObject {
	typedef CCustomObject inherited;

    friend class    SceneBuilder;
public:                           
	struct SSpawnData{
		CLASS_ID		m_ClassID;
		CSE_Abstract*	m_Data;
        SSpawnData	()
        {
			m_ClassID	= 0;
			m_Data		= 0;
        }
        ~SSpawnData	()
        {
        	Destroy	();
        }
        void		Create	(LPCSTR entity_ref);
        void		Destroy	();
        bool		Valid	(){return m_Data;}

        void		Save	(IWriter&);
        bool		Load	(IReader&);
		bool 		ExportGame(SExportStreams& F, CSpawnPoint* owner);

		void		FillProp(LPCSTR pref, PropItemVec& values);

		void    	Render	(const Fmatrix& parent,int priority, bool strictB2F);
		void    	OnFrame	();

    	void		OnDeviceCreate	();
		void		OnDeviceDestroy	();
	};

	SSpawnData    	m_SpawnData;
	CCustomObject*	m_AttachedObject;

    EPointType		m_Type;
    u32			m_dwTeamID;

    static ShaderMap m_Icons;
    static ref_shader 	CreateIcon(LPCSTR name);
    static ref_shader 	GetIcon(LPCSTR name);

    bool 			OnAppendObject	(CCustomObject* object);
protected:
    virtual void 	SetPosition		(const Fvector& pos);
    virtual void 	SetRotation		(const Fvector& rot);
    virtual void 	SetScale		(const Fvector& scale);
public:
	                CSpawnPoint    	(LPVOID data, LPCSTR name);
    void            Construct   	(LPVOID data);
    virtual         ~CSpawnPoint   	();

	IC bool 		RefCompare		(LPCSTR ref){return ref&&ref[0]&&m_SpawnData.Valid()?(strcmp(ref,m_SpawnData.m_Data->s_name)==0):false; }
    IC LPCSTR		GetRefName		() 			{return m_SpawnData.Valid()?m_SpawnData.m_Data->s_name:0;}

    bool			CreateSpawnData	(LPCSTR entity_ref);
	virtual void    Render      	( int priority, bool strictB2F );
	virtual bool    RayPick     	( float& distance,	const Fvector& start,	const Fvector& direction, SRayPickInfo* pinf = NULL );
    virtual bool 	FrustumPick		( const CFrustum& frustum );
	virtual bool    GetBox      	(Fbox& box);

	virtual void 	OnFrame			();

	virtual void 	Select			(int  flag);

  	virtual bool 	Load			(IReader&);
	virtual void 	Save			(IWriter&);
    virtual bool	ExportGame		(SExportStreams& data);

	virtual void	FillProp		(LPCSTR pref, PropItemVec& values);

    virtual void	OnDeviceCreate	();
    virtual void	OnDeviceDestroy	();

    bool			AttachObject	(CCustomObject* obj);
    void			DetachObject	();
    
    virtual bool	OnChooseQuery	(LPCSTR specific);
};

#endif /*_INCDEF_Glow_H_*/

