// Level.h: interface for the CLevel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEVEL_H__38F63863_DB0C_494B_AFAB_C495876EC671__INCLUDED_)
#define AFX_LEVEL_H__38F63863_DB0C_494B_AFAB_C495876EC671__INCLUDED_
#pragma once

#include "infoportiondefs.h"
#include "script_export_space.h"
#include "StatGraph.h"

class	CHUDManager;
class	CParticlesObject;
class	xrServer;
struct	SMapLocation;
class	game_cl_GameState;
class	NET_Queue_Event;
class	CSE_Abstract;
class	CPatrolPathStorage;
class	CSpaceRestrictionManager;
class	CSeniorityHierarchyHolder;
class	CGameObject;

DEFINE_VECTOR (SMapLocation*, LOCATIONS_PTR_VECTOR, LOCATIONS_PTR_VECTOR_IT);
#define DEFAULT_FOV				90.f

const int maxRP					= 64;
const int maxTeams				= 32;

class CFogOfWar;
class CBulletManager;

class CLevel					: public IGame_Level, public IPureClient
{
private:
#ifdef DEBUG
	bool						m_bSynchronization;
#endif
protected:
	typedef IGame_Level			inherited;
	
	// patrol path storage
	CPatrolPathStorage			*m_patrol_path_storage;
	// movement restriction manager
	CSpaceRestrictionManager	*m_space_restriction_manager;
	// seniority hierarchy holder
	CSeniorityHierarchyHolder	*m_seniority_hierarchy_holder;
	// level name
	ref_str						m_name;
	// Local events
	EVENT						eChangeRP;
	EVENT						eDemoPlay;
	EVENT						eChangeTrack;
	EVENT						eEnvironment;
	EVENT						eEntitySpawn;

	CStatGraph					*pStatGraph;
public:
	////////////// network ////////////////////////
	u32							GetInterpolationSteps	();
	void						SetInterpolationSteps	(u32 InterpSteps);
	bool						InterpolationDisabled	();
	void						ReculcInterpolationSteps();
	u32							GetNumCrSteps			() const	{return m_dwNumSteps; };
	void						SetNumCrSteps			( u32 NumSteps );
	static void __stdcall		PhisStepsCallback		( u32 Time0, u32 Time1 );
	bool						In_NetCorrectionPrediction	() {return m_bIn_CrPr;};
private:
	BOOL						m_bNeed_CrPr;
	u32							m_dwNumSteps;
	bool						m_bIn_CrPr;

	DEF_VECTOR					(OBJECTS_LIST, CGameObject*);

	OBJECTS_LIST				pObjects4CrPr;
	OBJECTS_LIST				pActors4CrPr;

	CObject*					pCurrentControlEntity;
public:
	void						AddObject_To_Objects4CrPr	(CGameObject* pObj);
	void						AddActor_To_Actors4CrPr		(CGameObject* pActor);

	CObject*					CurrentControlEntity	( void ) const		{ return pCurrentControlEntity; }
	void						SetControlEntity		( CObject* O  )		{ pCurrentControlEntity=O; }
private:
	
	void						make_NetCorrectionPrediction	();

	u32							m_dwDeltaUpdate ;
	u32							m_dwLastNetUpdateTime;
	void						UpdateDeltaUpd					( u32 LastTime );
	void						BlockCheatLoad					()				;

public:
	//////////////////////////////////////////////	
	// static particles
	DEFINE_VECTOR				(CParticlesObject*,POVec,POIt);
	POVec						m_StaticParticles;

	game_cl_GameState			*game;
	BOOL						game_configured;
	NET_Queue_Event				*game_events;
	xr_deque<CSE_Abstract*>		game_spawn_queue;
	xrServer*					Server;

public:
	// sounds
	xr_vector<ref_sound*>		static_Sounds;

	// startup options
	ref_str						m_caServerOptions;
	ref_str						m_caClientOptions;

	// Starting/Loading
	virtual BOOL				net_Start				( LPCSTR op_server, LPCSTR op_client);
	virtual void				net_Load				( LPCSTR name );
	virtual void				net_Save				( LPCSTR name );
	virtual void				net_Stop				( );
	virtual BOOL				net_Start_client		( LPCSTR name );
	virtual void				net_Update				( );

	virtual BOOL				Load_GameSpecific_Before( );
	virtual BOOL				Load_GameSpecific_After ( );
	virtual void				Load_GameSpecific_CFORM	( CDB::TRI* T, u32 count );

	// Events
	virtual void				OnEvent					( EVENT E, u64 P1, u64 P2 );
	virtual void				OnFrame					( void );
	virtual void				OnRender				( );

	// Input
	virtual	void				IR_OnKeyboardPress		( int btn );
	virtual void				IR_OnKeyboardRelease	( int btn );
	virtual void				IR_OnKeyboardHold		( int btn );
	virtual void				IR_OnMousePress			( int btn );
	virtual void				IR_OnMouseRelease		( int btn );
	virtual void				IR_OnMouseHold			( int btn );
	virtual void				IR_OnMouseMove			( int, int);
	virtual void				IR_OnMouseStop			( int, int);
	
			int					get_RPID				(LPCSTR name);


	// Game
	void						InitializeClientGame	(NET_Packet& P);
	void						ClientReceive			();
	void						ClientSend				();
	void						ClientSave				();
			u32					Objects_net_Save		(NET_Packet* _Packet, u32 start, u32 count);
	virtual	void				Send					(NET_Packet& P, u32 dwFlags=DPNSEND_GUARANTEED, u32 dwTimeout=0);
	
	void						g_cl_Spawn				(LPCSTR name, u8 rp, u16 flags);		// only ask server
	void						g_sv_Spawn				(CSE_Abstract* E);					// server reply/command spawning
	
	// Save/Load/State
	void						SLS_Load				(LPCSTR name);		// Game Load
	void						SLS_Default				();					// Default/Editor Load
	
	IC CSpaceRestrictionManager	&space_restriction_manager	();
	IC CSeniorityHierarchyHolder &seniority_holder			();

	// C/D
	CLevel();
	virtual ~CLevel();

	//названияе текущего уровня
	IC	ref_str			name					() const;

	//gets the time from the game simulation
	
	//возвращает время в милисекундах относительно начала игры
	ALife::_TIME_ID		GetGameTime				();
	//игровое время в отформатированном виде
	void				GetGameDateTime			(u32& year, u32& month, u32& day, u32& hours, u32& mins, u32& secs, u32& milisecs);

	float				GetGameTimeFactor		();
	void				SetGameTimeFactor		(const float fTimeFactor);
	void				SetGameTimeFactor		(ALife::_TIME_ID GameTime, const float fTimeFactor);
//	void				SetGameTime				(ALife::_TIME_ID GameTime);

	// gets current daytime [0..23]
	u8					GetDayTime();
	float				GetGameDayTimeSec();

protected:
	CFogOfWar*			m_pFogOfWar;
public:
	IC CFogOfWar&		FogOfWar() {return	*m_pFogOfWar;}

	//список локаций на карте, которые отображаются в данный момент
protected:	
	LOCATIONS_PTR_VECTOR	m_MapLocationVector;
public:
	LOCATIONS_PTR_VECTOR&   MapLocations				() {return m_MapLocationVector;}
	void					AddObjectMapLocation		(const CGameObject* object);
	void					AddMapLocation				(const SMapLocation& map_location);
	bool					RemoveMapLocationByID		(u16 object_id);
	bool					RemoveMapLocationByInfo		(INFO_INDEX info_index);
	void					RemoveMapLocations			();

	//работа с пулями
protected:	
	CBulletManager*		m_pBulletManager;
public:
	IC CBulletManager&	BulletManager() {return	*m_pBulletManager;}

	IC		const CPatrolPathStorage &patrol_paths		() const
	{
		VERIFY				(m_patrol_path_storage);
		return				(*m_patrol_path_storage);
	}

	//by Mad Max 
			bool			IsServer					();
			bool			IsClient					();
			CSE_Abstract	*spawn_item					(LPCSTR section, const Fvector &position, u32 level_vertex_id, u16 parent_id, bool return_item = false);
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CLevel)
#undef script_type_list
#define script_type_list save_type_list(CLevel)

IC CLevel&				Level()		{ return *((CLevel*) g_pGameLevel);			}
IC game_cl_GameState&	Game()		{ return *Level().game;					}
	u32					GameID();
IC CHUDManager&			HUD()		{ return *((CHUDManager*)Level().pHUD);	}

IC CSpaceRestrictionManager	&CLevel::space_restriction_manager()
{
	VERIFY				(m_space_restriction_manager);
	return				(*m_space_restriction_manager);
}

IC CSeniorityHierarchyHolder &CLevel::seniority_holder()
{
	VERIFY				(m_seniority_hierarchy_holder);
	return				(*m_seniority_hierarchy_holder);
}

IC	ref_str	CLevel::name	() const
{
	return				(m_name);
}

//by Mad Max 
IC bool					OnServer()	{ return Level().IsServer();				}
IC bool					OnClient()	{ return Level().IsClient();				}

class  CPHWorld;
extern CPHWorld*				ph_world;

#endif // !defined(AFX_LEVEL_H__38F63863_DB0C_494B_AFAB_C495876EC671__INCLUDED_)
