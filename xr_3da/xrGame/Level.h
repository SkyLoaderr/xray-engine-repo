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
#include "rain.h"
#include "ai_sounds.h"
#include "net_queue.h"

class CHUDManager;
class ENGINE_API CPGObject;

#define DEFAULT_FOV 90.f

const int maxGroups				= 32;
class CSquad
{
public:
	CEntity*					Leader;
	svector<CGroup,maxGroups>	Groups;

	objVisible					KnownEnemys;

	CSquad() : Leader(0)		{};

	bool						Empty			(){return Groups.empty();}
	int							Size			(){return Groups.size();}
};

const int maxTeams				= 32;
const int maxRP					= 64;
class CTeam
{
public:
	svector<CSquad,maxTeams>	Squads;
};

class CLevel					: public IGame_Level, public IPureClient
{
protected:
	typedef IGame_Level			inherited;

	// Local events
	EVENT						eChangeRP;
	EVENT						eDemoPlay;
	EVENT						eChangeTrack;
	EVENT						eEnvironment;
	EVENT						eEntitySpawn;
public:
	// static particles
	DEFINE_VECTOR(CPGObject*,PGOVec,PGOIt);
	PGOVec						m_StaticParticles;

	game_cl_GameState			game;
	BOOL						game_configured;
	NET_Queue_Event				game_events;
	xr_deque<CSE_Abstract*>		game_spawn_queue;

	xrServer*					Server;

	svector<CTeam,maxTeams>		Teams;

	CTracer						Tracers;
	CEffect_Rain				eff_Rain;

	// sounds
	xr_vector<sound*>			static_Sounds;

	// waypoints
	typedef struct tagSWayPoint{
		Fvector	tWayPoint;
		u32	dwFlags;
		u32	dwNodeID;
	} SWayPoint;
	
	typedef struct tagSWayLink{
		WORD	wFrom;
		WORD	wTo;
	} SWayLink;

	typedef xr_vector<Fvector>			SPointVector;

	typedef struct tagSPath {
		u32							dwType;
		xr_vector<SWayPoint>			tpaWayPoints;
		xr_vector<SWayLink>			tpaWayLinks;
		xr_vector<u32>					tpaWayPointIndexes;
		SPointVector				tpaVectors[3];
	} SPath;

	enum EPathTypes {
		PATH_LOOPED			= u32(1),
		PATH_BIDIRECTIONAL	= u32(2),
	};
	
	DEFINE_MAP					(LPSTR,SPath,SPathMap,SPathPairIt);
	SPathMap					m_PatrolPaths;

	// Starting/Loading
	virtual BOOL				net_Start				( LPCSTR op_server, LPCSTR op_client);
	virtual void				net_Load				( LPCSTR name );
	virtual void				net_Save				( LPCSTR name );
	virtual void				net_Stop				( );
	virtual BOOL				net_Start_client		( LPCSTR name );

	void						vfCreateAllPossiblePaths(string64 sName, SPath &tpPatrolPath);
	virtual BOOL				Load_GameSpecific_Before( );
	virtual BOOL				Load_GameSpecific_After ( );
	virtual void				Load_GameSpecific_CFORM	( CDB::TRI* T, u32 count );

	// Events
	virtual void				OnEvent					( EVENT E, u64 P1, u64 P2 );
	virtual void				OnFrame					( void );
	virtual void				OnRender				( );

	// Input
	virtual	void				OnKeyboardPress			( int btn );
	virtual void				OnKeyboardRelease		( int btn );
	virtual void				OnKeyboardHold			( int btn );
	virtual void				OnMousePress			( int btn );
	virtual void				OnMouseRelease			( int btn );
	virtual void				OnMouseHold				( int btn );
	virtual void				OnMouseMove				( int, int);
	virtual void				OnMouseStop				( int, int);

	// Misc
	CTeam&						get_team				(int ID)
	{
		if (ID >= (int)(Teams.size()))	Teams.resize(ID+1);
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
		if (S >= (int)(T.Squads.size()))	T.Squads.resize(S+1);
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
		if (G >= (int)(SQ.Groups.size()))	SQ.Groups.resize(G+1);
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

	void						vfMergeKnownEnemies		();

	// C/D
	CLevel();
	virtual ~CLevel();
};

IC CLevel&				Level()		{ return *((CLevel*) g_pGameLevel);			}
IC game_cl_GameState&	Game()		{ return Level().game;					}
IC u32					GameID()	{ return Game().type;					}
IC CHUDManager&			HUD()		{ return *((CHUDManager*)Level().pHUD);	}

class  CPHWorld;
extern CPHWorld*				ph_world;

#endif // !defined(AFX_LEVEL_H__38F63863_DB0C_494B_AFAB_C495876EC671__INCLUDED_)
