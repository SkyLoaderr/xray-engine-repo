#ifndef GamePersistentH
#define GamePersistentH
#pragma once

#include "../IGame_Persistent.h"
class CMainUI;
class CUICursor;
class CParticlesObject;

class CGamePersistent: 
	public IGame_Persistent, 
	public IEventReceiver
{
	CParticlesObject*	ambient_particles; 
	u32					ambient_sound_next_time;
	u32					ambient_effect_next_time;
	u32					ambient_effect_stop_time;

	void				WeathersUpdate			();

	u32					m_eGameType;
public:
	IReader*			pDemoFile;
	u32					uTime2Change;
	EVENT				eDemoStart;

						CGamePersistent			();
	virtual				~CGamePersistent		();

	virtual void		Start					(LPCSTR op);
	virtual void		Disconnect				();

	virtual	void		OnAppActivate			();
	virtual void		OnAppDeactivate			();

	virtual void		OnAppStart				();
	virtual void		OnAppEnd				();
	virtual	void		OnGameStart				();
	virtual void		OnGameEnd				();
	virtual void		OnFrame					();
	virtual void		OnEvent					(EVENT E, u64 P1, u64 P2);

	virtual void		RegisterModel			(IRender_Visual* V);
	virtual	float		MtlTransparent			(u32 mtl_idx);
	virtual	void		Statistics				(CGameFont* F);
	virtual	u32			GameType				() {return m_eGameType;}
};

IC CGamePersistent&		GamePersistent()		{ return *((CGamePersistent*) g_pGamePersistent);			}

#endif //GamePersistentH

