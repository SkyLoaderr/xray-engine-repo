// Level.h: interface for the CLevel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEVEL_H__38F63863_DB0C_494B_AFAB_C495876EC671__INCLUDED_)
#define AFX_LEVEL_H__38F63863_DB0C_494B_AFAB_C495876EC671__INCLUDED_
#pragma once

#include "xrServer.h"
#include "group.h"
#include "tracer.h"
#include "rain.h"
#include "ai_space.h"
#include "ai_sounds.h"
#include "..\fixedset.h"

class CHUDManager;

#define DEFAULT_FOV 90.f

const int maxGroups				= 32;
class CSquad
{
public:
	CEntity*					Leader;
	svector<CGroup,maxGroups>	Groups;

	CSquad() : Leader(0)		{};

	bool						Empty			(){return Groups.empty();}
	int							Size			(){return Groups.size();}
};

const int maxTeams				= 32;
const int maxRP					= 64;
typedef FixedSET<CObject*>		objVisible;
class CTeam
{
public:
	svector<CSquad,maxTeams>	Squads;
	svector<Fvector4,maxRP>		RespawnPoints;

	objVisible					KnownEnemys;
};

class CLevel : public CCreator, public IPureClient
{
protected:
	typedef CCreator inherited;

	// Local events
	EVENT						eChangeRP;
	EVENT						eDemoPlay;
	EVENT						eChangeTrack;
	EVENT						eDoorOpen;
	EVENT						eDoorClose;
	EVENT						eDA_PlayMotion;
	EVENT						eDA_StopMotion;
	EVENT						eEnvironment;
	EVENT						eEntitySpawn;

public:
	xrServer*					Server;

	CAI_Space					AI;
	vector<CTeam>				Teams;
	CTracer						Tracers;
	CEffect_Rain				eff_Rain;

	// waypoints
	typedef struct tagSWayPoint{
		Fvector	tWayPoint;
		DWORD	dwFlags;
		DWORD	dwNodeID;
	} SWayPoint;
	
	typedef struct tagSWayLink{
		WORD	wFrom;
		WORD	wTo;
	} SWayLink;

	typedef vector<Fvector>			SPointVector;

	typedef struct tagSPath {
		DWORD						dwType;
		vector<SWayPoint>			tpaWayPoints;
		vector<SWayLink>			tpaWayLinks;
		vector<DWORD>				tpaWayPointIndexes;
		SPointVector				tpaVectors[3];
	} SPath;

	enum EPathTypes {
		PATH_LOOPED			= DWORD(1),
		PATH_BIDIRECTIONAL	= DWORD(2),
	};
	
	DEFINE_MAP					(LPSTR,SPath,SPathMap,SPathPairIt);
	SPathMap					m_PatrolPaths;

	// Starting/Loading
	virtual BOOL				net_Start				( LPCSTR name, BOOL server	);
	virtual void				net_Load				( LPCSTR name );
	virtual void				net_Save				( LPCSTR name );
	virtual void				net_Stop				( );
	virtual void				net_Start_client		( LPCSTR name );

	void						vfCreateAllPossiblePaths(string64 sName, SPath &tpPatrolPath);
	virtual BOOL				Load_GameSpecific_Before( );
	virtual BOOL				Load_GameSpecific_After ( );

	// Events
	virtual void				OnEvent					( EVENT E, DWORD P1, DWORD P2 );
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
		if (ID >= int(Teams.size()))	Teams.resize(ID+1);
		return Teams[ID];
	}
	CTeam&						acc_team				(int ID)
	{
		VERIFY(ID < int(Teams.size()));
		return Teams[ID];
	}
	CSquad&						get_squad				(int ID, int S)
	{
		CTeam&	T = get_team(ID);
		if (S >= int(T.Squads.size()))	T.Squads.resize(S+1);
		return T.Squads[S];
	}
	CSquad&						acc_squad				(int ID, int S)
	{
		CTeam&	T = acc_team(ID);
		VERIFY(S < int(T.Squads.size()));
		return T.Squads[S];
	}
	CGroup&						get_group				(int ID, int S, int G)
	{
		CSquad&	SQ = get_squad(ID,S);
		if (G >= int(SQ.Groups.size()))	SQ.Groups.resize(G+1);
		return SQ.Groups[G];
	}
	int							get_RPID				(LPCSTR name);


	// Game
	void						ClientReceive			();
	void						ClientSend				();
	
	void						g_cl_Spawn				(LPCSTR name, int rp, int team, int squad, int group);	// only signal 2 server
	void						g_sv_Spawn				(NET_Packet* P);								// server reply parsing and spawning
	
	CHUDManager*				HUD						()	{ return (CHUDManager*)pHUD; }

	// Save/Load/State
	void						SLS_Load				(LPCSTR name);		// Game Load
	void						SLS_Default				();					// Default/Editor Load

	// C/D
	CLevel();
	virtual ~CLevel();
};

IC CLevel& Level()				{ return *((CLevel*) pCreator); }
class  CPHWorld;
extern CPHWorld*				ph_world;

#endif // !defined(AFX_LEVEL_H__38F63863_DB0C_494B_AFAB_C495876EC671__INCLUDED_)
