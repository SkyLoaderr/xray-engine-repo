// Level.h: interface for the CLevel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEVEL_H__38F63863_DB0C_494B_AFAB_C495876EC671__INCLUDED_)
#define AFX_LEVEL_H__38F63863_DB0C_494B_AFAB_C495876EC671__INCLUDED_
#pragma once

#include "xrServer.h"
#include "game_cl_base.h"
#include "group.h"
#include "tracer.h"
#include "ai_sounds.h"
#include "net_queue.h"
#include "ai/ai_monster_group.h"
#include "memory_space.h"

class CHUDManager;
class CParticlesObject;

#define DEFAULT_FOV				90.f
const int maxGroups				= 32;

class CSquad
{
public:
	CEntity*					Leader;
	xr_vector<CGroup>			Groups;
	xr_vector<CVisibleObject>	m_visible_objects;
	xr_vector<CSoundObject>		m_sound_objects;
	xr_vector<CHitObject>		m_hit_objects;
	u32							m_member_count;

								CSquad() : 
									Leader(0),
									m_member_count(0)
	{
	}

								~CSquad()
	{
	}

	bool						Empty			(){return Groups.empty();}
	int							Size			(){return Groups.size();}
};

const int maxTeams				= 32;
const int maxSquads				= 32;
const int maxRP					= 64;
class CTeam
{
public:
	svector<CSquad,maxSquads>	Squads;
};

//by Dandy
//for the fog over the map
class CFogOfWar;
class CScriptProcessor;

class CLevel					: public IGame_Level, public IPureClient
{
private:
#ifdef DEBUG
	bool						m_bSynchronization;
#endif
protected:
	typedef IGame_Level			inherited;

	// Local events
	EVENT						eChangeRP;
	EVENT						eDemoPlay;
	EVENT						eChangeTrack;
	EVENT						eEnvironment;
	EVENT						eEntitySpawn;
public:
	////////////// network ////////////////////////
	BOOL						m_bNeed_CrPr;
	u32							m_dwNumSteps;
	u32							GetInterpolationSteps ();
	void						SetInterpolationSteps (u32 InterpSteps);
	bool						InterpolationDisabled ();


	void						ReculcInterpolationSteps ();
private:
	void						make_NetCorrectionPrediction	();

	u32							m_dwDeltaUpdate ;
	u32							m_dwLastNetUpdateTime;
	void						UpdateDeltaUpd	( u32 LastTime );
public:
	//////////////////////////////////////////////	
	
	// scripts
	CScriptProcessor			*m_tpScriptProcessor;
	CScriptProcessor*			GetScriptProcessor() {return m_tpScriptProcessor;}
	// static particles
	DEFINE_VECTOR(CParticlesObject*,POVec,POIt);
	POVec						m_StaticParticles;

	game_cl_GameState			game;
	BOOL						game_configured;
	NET_Queue_Event				game_events;
	xr_deque<CSE_Abstract*>		game_spawn_queue;

	xrServer*					Server;

	svector<CTeam,maxTeams>		Teams;
	CSquadManager				SquadMan;
	

	CTracer						Tracers;

	// sounds
	xr_vector<ref_sound*>		static_Sounds;

	// startup options
	LPSTR						m_caServerOptions;
	LPSTR						m_caClientOptions;

	// waypoints
	typedef struct tagSWayPoint{
		Fvector	tWayPoint;
		u32		dwFlags;
		u32		dwNodeID;
		ref_str	name;
	} SWayPoint;
	
	typedef struct tagSWayLink{
		u16		wFrom;
		u16		wTo;
		float	fProbability;
	} SWayLink;

	typedef struct tagSPath {
		xr_vector<SWayPoint>			tpaWayPoints;
		xr_vector<SWayLink>				tpaWayLinks;
		xr_vector<u32>					tpaWayPointIndexes;
	} SPath;

	enum EPathTypes {
		PATH_LOOPED			= u32(1),
		PATH_BIDIRECTIONAL	= u32(2),
	};
	
	DEFINE_MAP_PRED				(LPCSTR,SPath,SPathMap,SPathPairIt,pred_str);
	SPathMap					m_PatrolPaths;

	// Starting/Loading
	virtual BOOL				net_Start				( LPCSTR op_server, LPCSTR op_client);
	virtual void				net_Load				( LPCSTR name );
	virtual void				net_Save				( LPCSTR name );
	virtual void				net_Stop				( );
	virtual BOOL				net_Start_client		( LPCSTR name );

	void						vfCreateAllPossiblePaths( LPCSTR sName, SPath &tpPatrolPath);
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
	IC	ALife::_TIME_ID GetGameTime()
	{
		return			(Server->game->GetGameTime());
	}

	IC	float	GetGameTimeFactor()
	{
		return			(Server->game->GetGameTimeFactor());
	}

	IC	void	SetGameTimeFactor(const float fTimeFactor)
	{
		Server->game->SetGameTimeFactor(fTimeFactor);
	}

	IC	float GetGameTimeSec()
	{
		return			(float(s64(GetGameTime()))/1000.f);
	}

	IC	float GetGameDayTimeSec()
	{
		return			(float(s64(GetGameTime() % (24*60*60*1000)))/1000.f);
	}

	CFogOfWar* m_pFogOfWar;

	// by Jim
	// gets current daytime [0..23]
	IC u8 GetDayTime() { 
		return ((u8) ((GetGameTime() / 3600000) % 24 )); 
	}
	//by Mad Max 
	IC bool	IsServer ()
	{
		return (Server->client_Count() != 0);
	};
	IC bool IsClient ()
	{
		return (Server->client_Count() == 0);
	};
};

IC CLevel&				Level()		{ return *((CLevel*) g_pGameLevel);			}
IC game_cl_GameState&	Game()		{ return Level().game;					}
IC u32					GameID()	{ return Game().type;					}
IC CHUDManager&			HUD()		{ return *((CHUDManager*)Level().pHUD);	}
//by Mad Max 
IC bool					OnServer()	{ return Level().IsServer();				}
IC bool					OnClient()	{ return Level().IsClient();				}


class  CPHWorld;
extern CPHWorld*				ph_world;

#endif // !defined(AFX_LEVEL_H__38F63863_DB0C_494B_AFAB_C495876EC671__INCLUDED_)
