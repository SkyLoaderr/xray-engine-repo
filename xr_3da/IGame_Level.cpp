#include "stdafx.h"
#include "igame_level.h"
#include "igame_persistent.h"

#include "x_ray.h"
#include "std_classes.h"
#include "customHUD.h"
#include "render.h"
#include "gamefont.h"
#include "xrLevel.h"
#include "ps_instance.h"

ENGINE_API	IGame_Level*	g_pGameLevel	= NULL;

IGame_Level::IGame_Level	()
{
	g_pGameLevel				= this;
	pLevel						= NULL;
	bReady						= false;
	pCurrentEntity				= NULL;
	pCurrentViewEntity			= NULL;
	LL_Stream					= NULL;
}

IGame_Level::~IGame_Level	()
{
	// Cleanup particles, some of them can be still active
	while (!ps_active.empty())
		xr_delete				( *ps_active.begin() );

	// 
	DEL_INSTANCE				( pHUD			);
	xr_delete					( pLevel		);
	FS.r_close					( LL_Stream		);

	Sound->destroy				(Sounds_Ambience);

	// Render-level unload
	Render->level_Unload		();

	// Unload sounds
	for (u32 i=0; i<Sounds.size(); i++)
		Sound->destroy	(Sounds[i]);
	Sounds.clear();

	// Unregister
	Device.seqRender.Remove		(this);
	Device.seqFrame.Remove		(this);
}

void IGame_Level::net_Stop			()
{
	// Destroy all objects
	Objects.Unload				( );
	IR_Release					( );

	bReady						= false;
}

//-------------------------------------------------------------------------------------------
extern CStatTimer				tscreate;

BOOL IGame_Level::Load			(u32 dwNum) 
{
	// Initialize level data
	pApp->Level_Set				( dwNum );
	string256					temp;
	if (!FS.exist(temp, "$level$", "level.ltx"))
		Debug.fatal	("Can't find level configuration file '%s'.",temp);
	pLevel						= xr_new<CInifile>	( temp );

	// Open
	pApp->LoadTitle				("Opening stream...");
	LL_Stream					= FS.r_open	("$level$","level");
	IReader	&fs					= *LL_Stream;

	// HUD + Environment
	pHUD						= (CCustomHUD*)NEW_INSTANCE	(CLSID_HUDMANAGER);

	// Header
	hdrLEVEL					H;
	fs.r_chunk_safe				(fsL_HEADER2,&H,sizeof(H));
	R_ASSERT2					(XRCL_PRODUCTION_VERSION==H.XRLC_version,"Incompatible level version.");

	// CForms
	pApp->LoadTitle				("Loading CFORM...");
	ObjectSpace.Load			();
	pApp->LoadSwitch			();

	// Render-level Load
	Render->level_Load			();
	tscreate.FrameEnd			();
	// Msg						("* S-CREATE: %f ms, %d times",tscreate.result,tscreate.count);

	// Objects
	pApp->LoadTitle				("Loading game...");
	g_pGamePersistent->Environment.mods_load	();
	R_ASSERT					(Load_GameSpecific_Before());
	Objects.Load				();
	R_ASSERT					(Load_GameSpecific_After ());

	// Done
	pApp->LoadTitle				("Syncronizing...");
	bReady						= true;
	if (!g_pGamePersistent->bDedicatedServer)	IR_Capture();
	Device.seqRender.Add		(this);
	Device.seqFrame.Add			(this);

	return TRUE;
}

int		psNET_DedicatedSleep	= 5;
void	IGame_Level::OnRender		( ) 
{
	if (_abs(Device.fTimeDelta)<EPS_S) return;

	// Level render, only when no client output required
	if (!g_pGamePersistent->bDedicatedServer)	{
		Render->Calculate			();
		Render->Render				();
	} else {
		Sleep						(psNET_DedicatedSleep);
	}

	// Font
	pApp->pFontSystem->OnRender	();
}

void	IGame_Level::OnFrame		( ) 
{
	// Log				("- level:on-frame: ",u32(Device.dwFrame));
	if (_abs(Device.fTimeDelta)<EPS_S) return;

	// Play req particle systems
	while (ps_needtoplay.size())
	{
		CPS_Instance*	psi		= ps_needtoplay.back	();
		ps_needtoplay.pop_back	();
		psi->Play				();
	}

	// Update all objects
	::Sound->update_events		( );
	VERIFY						(bReady);
	Engine.Sheduler.Update		( );
	Objects.Update				( );
	pHUD->OnFrame				( );

	// Destroy inactive particle systems
	while (ps_destroy.size())
	{
		CPS_Instance*	psi		= ps_destroy.back	();
		ps_destroy.pop_back		();
		xr_delete				(psi);
	}

	// Ambience
	if (Sounds_Random.size() && (Device.dwTimeGlobal > Sounds_dwNextTime))
	{
		Sounds_dwNextTime		= Device.dwTimeGlobal + ::Random.randI	(1000,10000);
		Fvector	pos;
		pos.random_dir().normalize().mul(::Random.randF(10,50)).add		(Device.vCameraPosition);
		int		id				= ::Random.randI(Sounds_Random.size());
		Sounds_Random[id].play_at_pos	(0,pos,0);
		Sounds_Random[id].set_volume	(1.f);
		Sounds_Random[id].set_range		(10,200);
	}
}
