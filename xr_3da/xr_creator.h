#ifndef __XR_CREATOR_H__
#define __XR_CREATOR_H__

#include "fcontroller.h"
#include "xr_object_list.h"
#include "environment.h"
#include "cameramanager.h"
#include "xr_area.h"
#include "xr_snd_defs.h"
#include "ai_space.h"

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
	friend class CRender;
protected:
	// Loader
	typedef vector<char *>		vecC;
	typedef vecC::iterator		vecCIT;
	CStream*					LL_Stream;
	vecC						LL_textures;
	vecC						LL_shaders;

	// Network interface
	CObject*					pCurrentEntity;
	CObject*					pCurrentViewEntity;

	// Static sounds
	vector<sound3D>				Sounds;
public:
	CObjectList					Objects;
	CEnvironment				Environment;
	CCameraManager				Cameras;
	CObjectSpace				ObjectSpace;
	CAI_Space					AI;

	BOOL						bReady;

	CInifile *					pLevel;
	CCustomHUD*					pHUD;
public:
	// Main, global functions
								CCreator				( );
	virtual						~CCreator				( );

	virtual BOOL				net_Server				( LPCSTR name_of_level	);
	virtual BOOL				net_Client				( LPCSTR name_of_server );
	virtual void				net_Disconnect			( );

	virtual BOOL				Load					( DWORD dwNum );
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
	Shader*						LL_CreateShader			(DWORD S, DWORD T);
};

extern ENGINE_API	CCreator*	pCreator;

#endif //__XR_CREATOR_H__
