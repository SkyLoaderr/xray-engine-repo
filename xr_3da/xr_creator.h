#ifndef __XR_CREATOR_H__
#define __XR_CREATOR_H__

#include "fcontroller.h"
#include "xr_object_list.h"
#include "environment.h"
#include "cameramanager.h"
#include "xr_area.h"
#include "xr_snd_defs.h"

// refs
class ENGINE_API CCursor;
class ENGINE_API CCustomHUD;

//-----------------------------------------------------------------------------------------------------------
//World Container
//-----------------------------------------------------------------------------------------------------------
class	ENGINE_API				CCreator	:
	public DLL_Pure,
	public CController,
	public pureRender,
	public pureFrame,
	public CEventBase
{
public:
	// Loader
	typedef vector<char *>		vecC;
	typedef vecC::iterator		vecCIT;
	CStream*					LL_Stream;
	vecC						LL_strings;
	LPCSTR						getString	(int id)	{
		if (id<0)	return 0;
		R_ASSERT	(id<int(LL_strings.size()));
		return LL_strings[id];
	}

protected:
	// Network interface
	CObject*					pCurrentEntity;
	CObject*					pCurrentViewEntity;

	// Static sounds
	sound						Sounds_Ambience;
	vector<sound>				Sounds;
	vector<sound>				Sounds_Random;
	u32						Sounds_dwNextTime;
public:
	CObjectList					Objects;
	CEnvironment				Environment;
	CCameraManager				Cameras;
	CObjectSpace				ObjectSpace;

	BOOL						bReady;

	CInifile*					pLevel;
	CCustomHUD*					pHUD;
public:
	// Main, global functions
								CCreator				( );
	virtual						~CCreator				( );

	virtual BOOL				net_Start				( LPCSTR name, BOOL server	)	= 0;
	virtual void				net_Load				( LPCSTR name )					= 0;
	virtual void				net_Save				( LPCSTR name )					= 0;
	virtual void				net_Stop				( );

	virtual BOOL				Load					( u32 dwNum );
	virtual BOOL				Load_GameSpecific_Before( ) { return TRUE; };	// before object loading
	virtual BOOL				Load_GameSpecific_After	( ) { return TRUE; };	// after object loading

	virtual void				OnFrame					( void );
	virtual void				OnRender				( void );

	// Main interface
	CObject*					CurrentEntity			( void )		{ return pCurrentEntity; }
	CObject*					CurrentViewEntity		( void )		{ return pCurrentViewEntity; }
	void						ChangeEntity			( CLASS_ID id )	{ pCurrentEntity=pCurrentViewEntity=(CObject*)Objects.FindObjectByCLS_ID(id); }
	void						ChangeViewEntity		( CLASS_ID id )	{ pCurrentViewEntity=(CObject*)Objects.FindObjectByCLS_ID(id); }
	void						SetEntity				( CObject* O  ) { VERIFY(O); pCurrentEntity=pCurrentViewEntity=O; }

	// Loader interface
	Shader*						LL_CreateShader			(int S, int T, int M, int C);
};

extern ENGINE_API	CCreator*	pCreator;

#endif //__XR_CREATOR_H__
