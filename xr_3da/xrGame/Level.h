// Level.h: interface for the CLevel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEVEL_H__38F63863_DB0C_494B_AFAB_C495876EC671__INCLUDED_)
#define AFX_LEVEL_H__38F63863_DB0C_494B_AFAB_C495876EC671__INCLUDED_
#pragma once

#include "team.h"
#include "infoportiondefs.h"

class	CHUDManager;
class	CParticlesObject;
class	xrServer;
class	CSquadManager;
struct	SMapLocation;
class	CEntity;
class	game_cl_GameState;
class	NET_Queue_Event;
class	CSE_Abstract;
class	CPatrolPathStorage;

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
	// Local events
	EVENT						eChangeRP;
	EVENT						eDemoPlay;
	EVENT						eChangeTrack;
	EVENT						eEnvironment;
	EVENT						eEntitySpawn;
public:
	////////////// network ////////////////////////
	u32							GetInterpolationSteps	();
	void						SetInterpolationSteps	(u32 InterpSteps);
	bool						InterpolationDisabled	();
	void						ReculcInterpolationSteps();
	void						SetNumCrSteps			( u32 NumSteps );
	static void __stdcall		PhisStepsCallback		( u32 Time0, u32 Time1 );
	bool						In_NetCorrectionPrediction	() {return m_bIn_CrPr;};
private:
	BOOL						m_bNeed_CrPr;
	u32							m_dwNumSteps;
	bool						m_bIn_CrPr;
	
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
	svector<CTeam,maxTeams>		Teams;
	CSquadManager				*SquadMan;

	// sounds
	xr_vector<ref_sound*>		static_Sounds;

	// startup options
	LPSTR						m_caServerOptions;
	LPSTR						m_caClientOptions;

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
	virtual	void				IR_OnKeyboardPress			( int btn );
	virtual void				IR_OnKeyboardRelease		( int btn );
	virtual void				IR_OnKeyboardHold			( int btn );
	virtual void				IR_OnMousePress			( int btn );
	virtual void				IR_OnMouseRelease			( int btn );
	virtual void				IR_OnMouseHold				( int btn );
	virtual void				IR_OnMouseMove				( int, int);
	virtual void				IR_OnMouseStop				( int, int);

	// Misc
	CTeam&						get_team				(int ID)
	{
		if (ID >= (int)(Teams.size()))	{
			R_ASSERT2	(ID < maxTeams,"Team number is out of range!");
			Teams.resize(ID+1);
		}
		return Teams[ID];
	}
	CTeam&						acc_team				(int ID)
	{
		VERIFY(ID < (int)(Teams.size()));
		return Teams[ID];
	}
	CSquad&						get_squad				(int ID, int S)
	{
		CTeam&	T = get_team(ID);
		if (S >= (int)(T.Squads.size())) {
			R_ASSERT2	(S < maxTeams,"Squad number is out of range!");
			T.Squads.resize(S+1);
		}
		return T.Squads[S];
	}
	CSquad&						acc_squad				(int ID, int S)
	{
		CTeam&	T = acc_team(ID);
		VERIFY(S < (int)(T.Squads.size()));
		return T.Squads[S];
	}
	CGroup&						get_group				(int ID, int S, int G)
	{
		CSquad&	SQ = get_squad(ID,S);
		if (G >= (int)(SQ.Groups.size())) {
			R_ASSERT2	(G < maxTeams,"Group number is out of range!");
			SQ.Groups.resize(G+1);
		}
		return SQ.Groups[G];
	}

	int							get_RPID				(LPCSTR name);


	// Game
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

	// C/D
	CLevel();
	virtual ~CLevel();


	//by Dandy
	//gets the time from the game simulation
	//receive game time
			ALife::_TIME_ID		GetGameTime				();
			float				GetGameTimeFactor		();
			void				SetGameTimeFactor		(const float fTimeFactor);
			void				SetGameTime				(ALife::_TIME_ID GameTime);

	IC		float	GetGameTimeSec()
	{
		return			(float(s64(GetGameTime()))/1000.f);
	}

	IC	float	GetGameDayTimeSec()
	{
		return			(float(s64(GetGameTime() % (24*60*60*1000)))/1000.f);
	}

protected:
	CFogOfWar*			m_pFogOfWar;
public:
	IC CFogOfWar&		FogOfWar() {return	*m_pFogOfWar;}

	//������ ������� �� �����, ������� ������������ � ������ ������
protected:	
	LOCATIONS_PTR_VECTOR	m_MapLocationVector;
public:
	LOCATIONS_PTR_VECTOR&   MapLocations				() {return m_MapLocationVector;}
	void					AddObjectMapLocation		(const CGameObject* object);
	void					AddMapLocation				(const SMapLocation& map_location);
	bool					RemoveMapLocationByID		(u16 object_id);
	bool					RemoveMapLocationByInfo		(INFO_ID info_portion_id);

	//������ � ������
protected:	
	CBulletManager*		m_pBulletManager;
public:
	IC CBulletManager&	BulletManager() {return	*m_pBulletManager;}

	// by Jim
	// gets current daytime [0..23]
	IC u8 GetDayTime() { 
		return ((u8) ((GetGameTime() / 3600000) % 24 )); 
	}

	IC		const CPatrolPathStorage &patrol_paths		() const
	{
		VERIFY				(m_patrol_path_storage);
		return				(*m_patrol_path_storage);
	}

	//by Mad Max 
			bool			IsServer					();
			bool			IsClient					();
			CSE_Abstract	*spawn_item					(LPCSTR section, const Fvector &position, u32 level_vertex_id, u16 parent_id, bool return_item = false);
};

IC CLevel&				Level()		{ return *((CLevel*) g_pGameLevel);			}
IC game_cl_GameState&	Game()		{ return *Level().game;					}
	u32					GameID();
IC CHUDManager&			HUD()		{ return *((CHUDManager*)Level().pHUD);	}
	//by Mad Max 
IC bool					OnServer()	{ return Level().IsServer();				}
IC bool					OnClient()	{ return Level().IsClient();				}

class  CPHWorld;
extern CPHWorld*				ph_world;

#endif // !defined(AFX_LEVEL_H__38F63863_DB0C_494B_AFAB_C495876EC671__INCLUDED_)
