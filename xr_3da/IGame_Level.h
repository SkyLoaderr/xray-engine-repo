#pragma once

#include "iinputreceiver.h"
#include "CameraManager.h"
#include "xr_object_list.h"
#include "xr_area.h"

// refs
class ENGINE_API CCursor;
class ENGINE_API CCustomHUD;
class ENGINE_API CPS_Instance;
class ENGINE_API ISpatial;

//-----------------------------------------------------------------------------------------------------------
class ENGINE_API	IGame_Level		: 
	public DLL_Pure,
	public IInputReceiver,
	public pureRender,
	public pureFrame,
	public IEventReceiver
{
public:
	// Loader
	IReader*					LL_Stream;
protected:
	// Network interface
	CObject*					pCurrentEntity;
	CObject*					pCurrentViewEntity;

	// Static sounds
	ref_sound					Sounds_Ambience;
	xr_vector<ref_sound>		Sounds;
	xr_vector<ref_sound>		Sounds_Random;
	u32							Sounds_dwNextTime;
public:
	CObjectList					Objects;
	CCameraManager				Cameras;
	CObjectSpace				ObjectSpace;

	BOOL						bReady;

	CInifile*					pLevel;
	CCustomHUD*					pHUD;

	xr_set<CPS_Instance*>		ps_active;
	xr_vector<CPS_Instance*>	ps_destroy;
	xr_vector<CPS_Instance*>	ps_needtoplay;
	xr_vector<ISpatial*>		snd_ER;					// event receivers
public:
	// Main, global functions
	IGame_Level					();
	virtual ~IGame_Level		();

	virtual BOOL				net_Start				( LPCSTR op_server, LPCSTR op_client)	= 0;
	virtual void				net_Load				( LPCSTR name )							= 0;
	virtual void				net_Save				( LPCSTR name )							= 0;
	virtual void				net_Stop				( );
	virtual void				net_Update				( )										= 0;

	virtual BOOL				Load					( u32 dwNum );
	virtual BOOL				Load_GameSpecific_Before( )										{ return TRUE; };		// before object loading
	virtual BOOL				Load_GameSpecific_After	( )										{ return TRUE; };		// after object loading
	virtual void				Load_GameSpecific_CFORM	( CDB::TRI* T, u32 count )				= 0;

	virtual void				OnFrame					( void );
	virtual void				OnRender				( void );

	// Main interface
	CObject*					CurrentEntity			( void ) const							{ return pCurrentEntity;				}
	CObject*					CurrentViewEntity		( void ) const							{ return pCurrentViewEntity;			}
	void						SetEntity				( CObject* O  )							{ pCurrentEntity=pCurrentViewEntity=O;	}
	void						SetViewEntity			( CObject* O  )							{ pCurrentViewEntity=O;					}
	void						SoundEvent				( ref_sound* S, float range );

	// Loader interface
	ref_shader					LL_CreateShader			(int S, int T, int M, int C);
	void						LL_CheckTextures		();
};

//-----------------------------------------------------------------------------------------------------------
extern ENGINE_API	IGame_Level*	g_pGameLevel;
