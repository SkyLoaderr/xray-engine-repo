// Spawn_Event.h: interface for the CSpawn_Event class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPAWN_EVENT_H__7515F9EA_5014_4080_BB28_71497FCFCC6A__INCLUDED_)
#define AFX_SPAWN_EVENT_H__7515F9EA_5014_4080_BB28_71497FCFCC6A__INCLUDED_
#pragma once

#include "Spawn.h"

class CSpawn_Event : public CSpawn  
{
public:
	enum	ev_types
	{
		typeNone		= 0,
		typeActivate,
		typeDeactivate,
		typeCustom,

		typeFORCEDWORD	= DWORD(-1)
	};
	struct	Action 
	{
		xrP_TOKEN	type;
		string64	target;	// object name
		string64	custom;	// custom event
	};
	struct	Pair
	{
		xrP_BOOL	bOnce;
		xrP_TOKEN	
		Action		OnEnter;
		Action		OnExit;
	};
public:
	

	vector<Pair>	Commands;


public:
	CSpawn_Event();
	virtual ~CSpawn_Event();
};

#endif // !defined(AFX_SPAWN_EVENT_H__7515F9EA_5014_4080_BB28_71497FCFCC6A__INCLUDED_)
