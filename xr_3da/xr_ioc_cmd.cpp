#include "stdafx.h"
#include "igame_level.h"

#include "xr_effgamma.h"
#include "x_ray.h"
#include "xr_ioconsole.h"
#include "xr_ioc_cmd.h"
#include "fbasicvisual.h"
#include "cameramanager.h"
#include "environment.h"
#include "xr_input.h"
#include "CustomHUD.h"

#include "xr_object.h"

xr_token							snd_freq_token							[ ]={
	{ "22khz",						sf_22K										},
	{ "44khz",						sf_44K										},
	{ 0,							0											}
};
xr_token							snd_model_token							[ ]={
	{ "Default",					0											},
	{ "Normal",						1											},
	{ "Light",						2											},
	{ "High",						3											},
	{ 0,							0											}
};
xr_token							vid_mode_token							[ ]={
#ifdef DEBUG
	{ "320x240",					320											},
	{ "512x384",					512											},
#endif
	{ "640x480",					640											},
	{ "800x600",					800											},
	{ "1024x768",					1024										},
	{ "1280x1024",					1280										},
	{ "1600x1200",					1600										},
	{ 0,							0											}
};
xr_token							vid_bpp_token							[ ]={
	{ "16",							16											},
	{ "32",							32											},
	{ 0,							0											}
};

//-----------------------------------------------------------------------
class CCC_Quit : public IConsole_Command
{
public:
	CCC_Quit(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Console->Hide();
		Engine.Event.Defer("KERNEL:disconnect");
		Engine.Event.Defer("KERNEL:quit");
	}
};
//-----------------------------------------------------------------------
class CCC_MemStat : public IConsole_Command
{
public:
	CCC_MemStat(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Memory.mem_statistic();
		Msg	("* ----- shared memory -----");
		g_pSharedMemoryContainer->dump();
		Msg	("* ----- string storage -----");
		g_pStringContainer->dump();
	}
};
//-----------------------------------------------------------------------
class CCC_E_Dump : public IConsole_Command
{
public:
	CCC_E_Dump(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Engine.Event.Dump();
	}
};
class CCC_E_Signal : public IConsole_Command
{
public:
	CCC_E_Signal(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		char	Event[128],Param[128];
		Event[0]=0; Param[0]=0;
		sscanf	(args,"%[^,],%s",Event,Param);
		Engine.Event.Signal	(Event,(u64)Param);
	}
};
//-----------------------------------------------------------------------
class CCC_Help : public IConsole_Command
{
public:
	CCC_Help(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Log("- --- Command listing: start ---");
		CConsole::vecCMD_IT it;
		for (it=Console->Commands.begin(); it!=Console->Commands.end(); it++)
		{
			IConsole_Command &C = *(it->second);
			TStatus _S; C.Status(_S);
			TInfo	_I;	C.Info	(_I);
			
			Msg("%-20s (%-10s) --- %s",	C.Name(), _S, _I);
		}
		Log("- --- Command listing: end ----");
	}
};
//-----------------------------------------------------------------------
class CCC_SaveCFG : public IConsole_Command
{
public:
	CCC_SaveCFG(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) 
	{
		LPCSTR	c_name		= (xr_strlen(args)>0)?args:Console->ConfigFile;
		SetFileAttributes	(c_name,FILE_ATTRIBUTE_NORMAL);
		IWriter* F			= FS.w_open(c_name);
		R_ASSERT			(F);

		CConsole::vecCMD_IT it;
		for (it=Console->Commands.begin(); it!=Console->Commands.end(); it++)
			it->second->Save(F);

		FS.w_close			(F);
	}
};
class CCC_LoadCFG : public IConsole_Command
{
public:
	CCC_LoadCFG(LPCSTR N) : IConsole_Command(N) {};
	virtual void Execute(LPCSTR args) {
		Msg("Executing config-script \"%s\"...",args);
		char str[1024];

		//RecordCommands	= false;
		strcpy	(str,args);
		if (strext(str))	*strext(str) = 0;
		strcat	(str,".ltx");
		
		IReader* F = FS.r_open(str);
		if (F!=NULL) {
			while (!F->eof()) {
				F->r_string		(str);
				Console->Execute	(str);
			}
			FS.r_close(F);
		} else {
			Log("! Cannot open script file.");
		}
		//RecordCommands	= true;
	}
};
//-----------------------------------------------------------------------
class CCC_Start : public IConsole_Command
{
	void	parse		(LPSTR dest, LPCSTR args, LPCSTR name)
	{
		dest[0]	= 0;
		if (strstr(args,name))
			sscanf(strstr(args,name)+xr_strlen(name),"(%[^)])",dest);
	}
public:
	CCC_Start(LPCSTR N) : IConsole_Command(N) {};
	virtual void Execute(LPCSTR args) {
		if (g_pGameLevel)	{
			Log		("! Please disconnect/unload first");
			return;
		}
		string256	op_server,op_client;
		
		parse		(op_server,args,"server");	// 1. server
		parse		(op_client,args,"client");	// 2. client

		if (0==xr_strlen(op_client))	
		{
			Log("! Can't start game without client. Arguments: '%s'.",args);
			return;
		}
		Engine.Event.Defer	("KERNEL:start",u64(xr_strlen(op_server)?xr_strdup(op_server):0),u64(xr_strdup(op_client)));
	}
};

class CCC_ServerLoad : public IConsole_Command
{
public:
	CCC_ServerLoad(LPCSTR N) : IConsole_Command(N) {};
	virtual void Execute(LPCSTR args) {
		if (g_pGameLevel)	{
			Log		("! Please disconnect/unload first");
			return	;
		}
		string256	fn;
		if (FS.exist(fn,"",args,".save"))
		{
			Engine.Event.Defer("KERNEL:server_load",u64(xr_strdup(fn)));
		} else {
			Log("! Cannot find save-game '%s'.",fn);
		}
	}
};

class CCC_Disconnect : public IConsole_Command
{
public:
	CCC_Disconnect(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Engine.Event.Defer("KERNEL:disconnect");
	}
};
//-----------------------------------------------------------------------
class CCC_VID_Reset : public IConsole_Command
{
public:
	CCC_VID_Reset(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		if (Device.bReady) {
			Device.Reset	();
		}
	}
};
//-----------------------------------------------------------------------
class CCC_SND_Restart : public IConsole_Command
{
public:
	CCC_SND_Restart(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Sound->_restart();
	}
};

//-----------------------------------------------------------------------
class CCC_Gamma : public CCC_Float
{
	float	gamma;
public:
	CCC_Gamma(LPCSTR N) : CCC_Float(N, &gamma, 0, 2), gamma(1) {};
	virtual void Execute(LPCSTR args)
	{
		CCC_Float::Execute	(args);
		Device.Gamma.Gamma	(gamma);
		Device.Gamma.Update	();
	}
};
//-----------------------------------------------------------------------
class CCC_Brightness : public CCC_Float
{
	float	value;
public:
	CCC_Brightness(LPCSTR N) : CCC_Float(N, &value, 0, 2), value(1) {};
	virtual void Execute(LPCSTR args)
	{
		CCC_Float::Execute		(args);
		Device.Gamma.Brightness	(value);
		Device.Gamma.Update		();
	}
};
//-----------------------------------------------------------------------
class CCC_Contrast  : public CCC_Float
{
	float	value;
public:
	CCC_Contrast(LPCSTR N) : CCC_Float(N, &value, 0, 2), value(1) {};
	virtual void Execute(LPCSTR args)
	{
		CCC_Float::Execute		(args);
		Device.Gamma.Contrast	(value);
		Device.Gamma.Update		();
	}
};
//-----------------------------------------------------------------------
ENGINE_API float	psHUD_FOV=0.5f;

extern int			psSkeletonUpdate;
extern int			rsDVB_Size;
extern int			rsDIB_Size;
extern int			psNET_ClientUpdate;
extern int			psNET_ClientPending;
extern int			psNET_ServerUpdate;
extern int			psNET_ServerPending;
extern int			psNET_DedicatedSleep;
extern char			psNET_Name[32];
//extern int			psNET_Port;
extern int			psSheduler;
extern float		psShedulerLoadBalance;
extern Flags32		psEnvFlags;
extern float		rsDT_Range;
ENGINE_API int			ps_r__Supersample			= 1;
void CCC_Register()
{
	// General
	CMD1(CCC_Help,		"help"					);
	CMD1(CCC_Quit,		"quit"					);
	CMD1(CCC_Start,		"start"					);
	CMD1(CCC_ServerLoad,"server_load"			);
	CMD1(CCC_Disconnect,"disconnect"			);
	CMD1(CCC_SaveCFG,	"cfg_save"				);
	CMD1(CCC_LoadCFG,	"cfg_load"				);


#ifdef DEBUG
	CMD1(CCC_MemStat,	"stat_mem"				);

	CMD3(CCC_Mask,		"mt_sound",				&psDeviceFlags,			mtSound);
	CMD3(CCC_Mask,		"mt_physics",			&psDeviceFlags,			mtPhysics);
	CMD3(CCC_Mask,		"mt_network",			&psDeviceFlags,			mtNetwork);
	CMD4(CCC_Integer,	"mt_sheduler",			&psSheduler,			1000,	100000	);
	CMD4(CCC_Float,		"mt_sheduler_load",		&psShedulerLoadBalance,	.1f,	10.f	);
	
	// Events
	CMD1(CCC_E_Dump,	"e_list"				);
	CMD1(CCC_E_Signal,	"e_signal"				);

	CMD3(CCC_Mask,		"rs_wireframe",			&psDeviceFlags,		rsWireframe);
	CMD3(CCC_Mask,		"rs_clear_bb",			&psDeviceFlags,		rsClearBB);
	CMD3(CCC_Mask,		"rs_occlusion",			&psDeviceFlags,		rsOcclusion);

	CMD3(CCC_Mask,		"rs_detail",			&psDeviceFlags,		rsDetails	);
	CMD4(CCC_Float,		"rs_dt_range",			&rsDT_Range,		5,		50	);
#endif

	// Render device states

	CMD4(CCC_Integer,	"r__supersample",		&ps_r__Supersample,			1,		4		);

	CMD3(CCC_Mask,		"rs_constant_fps",		&psDeviceFlags,		rsConstantFPS);

	CMD3(CCC_Mask,		"rs_no_v_sync",			&psDeviceFlags,		rsNoVSync);
	CMD3(CCC_Mask,		"rs_warm_hzb",			&psDeviceFlags,		rsWarmHZB);
	CMD3(CCC_Mask,		"rs_fullscreen",		&psDeviceFlags,		rsFullscreen);
	CMD3(CCC_Mask,		"rs_refresh_60hz",		&psDeviceFlags,		rsRefresh60hz);
	CMD3(CCC_Mask,		"rs_stats",				&psDeviceFlags,		rsStatistic);
	CMD4(CCC_Float,		"rs_vis_distance",		&psVisDistance,		0.01f,	1.0f);

	CMD1(CCC_Gamma,		"rs_c_gamma"			);
	CMD1(CCC_Gamma,		"rs_c_brightness"		);
	CMD1(CCC_Gamma,		"rs_c_contrast"			);
	CMD4(CCC_Integer,	"rs_skeleton_update",	&psSkeletonUpdate,	2,		128	);
	CMD4(CCC_Integer,	"rs_vb_size",			&rsDVB_Size,		32,		4096);
	CMD4(CCC_Integer,	"rs_ib_size",			&rsDIB_Size,		32,		4096);
	
	// Texture manager	
	CMD4(CCC_Integer,	"texture_lod",			&psTextureLOD,				0,	4	);
	CMD4(CCC_Integer,	"net_dedicated_sleep",	&psNET_DedicatedSleep,		0,	64	);

	// General video control
	CMD3(CCC_Token,		"vid_mode",				&psCurrentMode, vid_mode_token);
	CMD3(CCC_Token,		"vid_bpp",				&psCurrentBPP,	vid_bpp_token);
	CMD1(CCC_VID_Reset, "vid_restart"			);
	
	// Sound
	CMD2(CCC_Float,		"snd_volume_eff",		&psSoundVEffects);
	CMD2(CCC_Float,		"snd_volume_music",		&psSoundVMusic);
	CMD3(CCC_Token,		"snd_freq",				&psSoundFreq,		snd_freq_token		);
	CMD3(CCC_Token,		"snd_model",			&psSoundModel,		snd_model_token		);
	CMD1(CCC_SND_Restart,"snd_restart"			);
	CMD3(CCC_Mask,		"snd_acceleration",		&psSoundFlags,		ssHardware	);
	CMD3(CCC_Mask,		"snd_efx",				&psSoundFlags,		ssEAX		);
	CMD4(CCC_Integer,	"snd_targets",			&psSoundTargets,	4,32		);
	CMD4(CCC_Integer,	"snd_cache_size",		&psSoundCacheSizeMB,4,32		);
	
	// Mouse
	CMD3(CCC_Mask,		"mouse_invert",			&psMouseInvert,1);
	CMD2(CCC_Float,		"mouse_sens",			&psMouseSens);

	// Camera
	CMD2(CCC_Float,		"cam_inert",			&psCamInert);
	CMD2(CCC_Float,		"cam_slide_inert",		&psCamSlideInert);

	psSoundRolloff			= pSettings->r_float	("sound","rolloff");		clamp(psSoundRolloff,			EPS_S,	2.f);
	psSoundOcclusionScale	= pSettings->r_float	("sound","occlusion_scale");clamp(psSoundOcclusionScale,	0.1f,	.5f);
};
 