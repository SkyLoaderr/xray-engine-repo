#include "stdafx.h"
#include "x_ray.h"
#include "xr_ioconsole.h"
#include "xr_creator.h"
#include "xr_ioc_cmd.h"
#include "fbasicvisual.h"
#include "cameramanager.h"
#include "environment.h"
#include "xr_input.h"
#include "CustomHUD.h"

#include "xr_object.h"

xr_token							snd_freq_token							[ ]={
	{ "11khz",						0											},
	{ "22khz",						1											},
	{ "44khz",						2											},
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
	{ "320x240",					320											},
	{ "512x384",					512											},
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
class CCC_Quit : public CConsoleCommand
{
public:
	CCC_Quit(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Console.Hide();
		Engine.Event.Defer("KERNEL:disconnect");
		Engine.Event.Defer("KERNEL:quit");
	}
};
//-----------------------------------------------------------------------
class CCC_MemStat : public CConsoleCommand
{
public:
	CCC_MemStat(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Memory.mem_statistic();
	}
};
//-----------------------------------------------------------------------
class CCC_E_Dump : public CConsoleCommand
{
public:
	CCC_E_Dump(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Engine.Event.Dump();
	}
};
class CCC_E_Signal : public CConsoleCommand
{
public:
	CCC_E_Signal(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		char	Event[128],Param[128];
		Event[0]=0; Param[0]=0;
		sscanf	(args,"%[^,],%s",Event,Param);
		Engine.Event.Signal	(Event,(u32)Param);
	}
};
//-----------------------------------------------------------------------
class CCC_Help : public CConsoleCommand
{
public:
	CCC_Help(LPCSTR N) : CConsoleCommand(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Log("- --- Command listing: start ---");
		CConsole::vecCMD_IT it;
		for (it=Console.Commands.begin(); it!=Console.Commands.end(); it++)
		{
			CConsoleCommand &C = *(it->second);
			TStatus _S; C.Status(_S);
			TInfo	_I;	C.Info	(_I);
			
			Msg("%-20s (%-10s) --- %s",	C.Name(), _S, _I);
		}
		Log("- --- Command listing: end ----");
	}
};
//-----------------------------------------------------------------------
class CCC_SaveCFG : public CConsoleCommand
{
public:
	CCC_SaveCFG(LPCSTR N) : CConsoleCommand(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) 
	{
		LPCSTR	c_name		= Console.ConfigFile;
		SetFileAttributes	(c_name,FILE_ATTRIBUTE_NORMAL);
		IWriter* F			= FS.w_open(c_name);
		R_ASSERT			(F);

		CConsole::vecCMD_IT it;
		for (it=Console.Commands.begin(); it!=Console.Commands.end(); it++)
			it->second->Save(F);

		FS.w_close			(F);
	}
};
class CCC_LoadCFG : public CConsoleCommand
{
public:
	CCC_LoadCFG(LPCSTR N) : CConsoleCommand(N) {};
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
				Console.Execute	(str);
			}
			FS.r_close(F)
		} else {
			Log("! Cannot open script file.");
		}
		//RecordCommands	= true;
	}
};
//-----------------------------------------------------------------------
class CCC_Start : public CConsoleCommand
{
	void	parse		(LPSTR dest, LPCSTR args, LPCSTR name)
	{
		dest[0]	= 0;
		if (strstr(args,name))
			sscanf(strstr(args,name)+strlen(name),"(%[^)])",dest);
	}
public:
	CCC_Start(LPCSTR N) : CConsoleCommand(N) {};
	virtual void Execute(LPCSTR args) {
		if (pCreator)	{
			Log		("! Please disconnect/unload first");
			return;
		}
		string256	op_server,op_client;
		
		parse		(op_server,args,"server");	// 1. server
		parse		(op_client,args,"client");	// 2. client

		if (0==strlen(op_client))	
		{
			Log("! Can't start game without client. Arguments: '%s'.",args);
			return;
		}
		Engine.Event.Defer	("KERNEL:start",u32(strlen(op_server)?xr_strdup(op_server):0),u32(xr_strdup(op_client)));
	}
};

class CCC_ServerLoad : public CConsoleCommand
{
public:
	CCC_ServerLoad(LPCSTR N) : CConsoleCommand(N) {};
	virtual void Execute(LPCSTR args) {
		if (pCreator)	{
			Log("! Please disconnect/unload first");
			return;
		}
		string256	fn;
		if (FS.exist(fn,"",args,".save"))
		{
			Engine.Event.Defer("KERNEL:server_load",u32(xr_strdup(fn)));
		} else {
			Log("! Cannot find save-game '%s'.",fn);
		}
	}
};

class CCC_Disconnect : public CConsoleCommand
{
public:
	CCC_Disconnect(LPCSTR N) : CConsoleCommand(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Engine.Event.Defer("KERNEL:disconnect");
	}
};
//-----------------------------------------------------------------------
class CCC_VID_Restart : public CConsoleCommand
{
public:
	CCC_VID_Restart(LPCSTR N) : CConsoleCommand(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		if (Device.bReady) {
			Device.Destroy	();
			Device.Create	();
		}
	}
};
class CCC_VID_Reset : public CConsoleCommand
{
public:
	CCC_VID_Reset(LPCSTR N) : CConsoleCommand(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		if (Device.bReady) {
			Device.Reset	();
		}
	}
};
//-----------------------------------------------------------------------
class CCC_SND_Restart : public CConsoleCommand
{
public:
	CCC_SND_Restart(LPCSTR N) : CConsoleCommand(N) { bEmptyArgsHandled = TRUE; };
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
		CCC_Float::Execute(args);
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
class CCC_Contrast : public CCC_Float
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
extern int			psPhysicsFPS;
extern int			rsDVB_Size;
extern int			rsDIB_Size;
extern int			psNET_ClientUpdate;
extern int			psNET_ClientPending;
extern int			psNET_ServerUpdate;
extern int			psNET_ServerPending;
extern char			psNET_Name[32];
extern int			psNET_Port;
extern int			psSheduler;
extern Flags32		psEnvFlags;

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
	CMD1(CCC_MemStat,	"stat_mem"				);

	CMD3(CCC_Mask,		"mt_sound",				&psDeviceFlags,	mtSound);
	CMD3(CCC_Mask,		"mt_input",				&psDeviceFlags,	mtInput);
	CMD3(CCC_Mask,		"mt_physics",			&psDeviceFlags,	mtPhysics);
	CMD4(CCC_Integer,	"mt_sheduler",			&psSheduler,	1000,	100000	);
	
	// Events
	CMD1(CCC_E_Dump,	"e_list"				);
	CMD1(CCC_E_Signal,	"e_signal"				);

	// Render device states
	CMD3(CCC_Mask,		"rs_no_v_sync",			&psDeviceFlags,rsNoVSync);
	CMD3(CCC_Mask,		"rs_anisotropic",		&psDeviceFlags,rsAnisotropic);
	CMD3(CCC_Mask,		"rs_wireframe",			&psDeviceFlags,rsWireframe);
	CMD3(CCC_Mask,		"rs_renormalize",		&psDeviceFlags,rsNormalize);
	CMD3(CCC_Mask,		"rs_antialias",			&psDeviceFlags,rsAntialias);
	CMD3(CCC_Mask,		"rs_warm_hzb",			&psDeviceFlags,rsWarmHZB);
	CMD3(CCC_Mask,		"rs_triplebuffer",		&psDeviceFlags,rsTriplebuffer);
	CMD3(CCC_Mask,		"rs_fullscreen",		&psDeviceFlags,rsFullscreen);
	CMD3(CCC_Mask,		"rs_clear_bb",			&psDeviceFlags,rsClearBB);
	CMD3(CCC_Mask,		"rs_compress_lmaps",	&psDeviceFlags,rsCompressLMAPs);
	CMD3(CCC_Mask,		"rs_occlusion",			&psDeviceFlags,rsOcclusion);
	CMD3(CCC_Mask,		"rs_stats",				&psDeviceFlags,rsStatistic);
	CMD3(CCC_Mask,		"rs_postprocess",		&psDeviceFlags,rsPostprocess);
	CMD3(CCC_Mask,		"rs_refresh_60hz",		&psDeviceFlags,rsRefresh60hz);
	
	/*
	CMD4(CCC_Float,		"rs_geometry_lod",		&QualityControl.fGeometryLOD, 0,2);
	CMD4(CCC_Float,		"rs_min_fps",			&QualityControl.fMinFPS, 0, 512	);
	CMD4(CCC_Float,		"rs_max_fps",			&QualityControl.fMaxFPS, 0, 512	);
	*/

	CMD3(CCC_Mask,		"rs_detail",			&psDeviceFlags,		rsDetails	);

	CMD1(CCC_Gamma,		"rs_c_gamma"			);
	CMD1(CCC_Gamma,		"rs_c_brightness"		);
	CMD1(CCC_Gamma,		"rs_c_contrast"			);
	CMD4(CCC_Integer,	"rs_skeleton_update",	&psSkeletonUpdate,	2,		128	);
	CMD4(CCC_Integer,	"rs_vb_size",			&rsDVB_Size,		32,		4096);
	CMD4(CCC_Integer,	"rs_ib_size",			&rsDIB_Size,		32,		4096);
	
	// Network
	CMD4(CCC_Integer,	"net_cl_update_rate",	&psNET_ClientUpdate,3,		100	);
	CMD4(CCC_Integer,	"net_cl_pending_lim",	&psNET_ClientPending,0,		10	);
	CMD4(CCC_Integer,	"net_sv_update_rate",	&psNET_ClientUpdate,3,		100	);
	CMD4(CCC_Integer,	"net_sv_pending_lim",	&psNET_ClientPending,0,		10	);
	CMD4(CCC_Integer,	"net_port",				&psNET_Port,		5400,	5500);
	CMD3(CCC_String,	"net_name",				psNET_Name,			32			);

	// Texture manager	
	CMD4(CCC_Integer,	"texture_lod",			&psTextureLOD,		0,			4	);

	// General video control
	CMD3(CCC_Token,		"vid_mode",				&psCurrentMode, vid_mode_token);
	CMD3(CCC_Token,		"vid_bpp",				&psCurrentBPP,	vid_bpp_token);
	CMD1(CCC_VID_Restart,"vid_restart"			);
	CMD1(CCC_VID_Reset, "vid_reset"				);
	
	// Sound
	CMD4(CCC_Float,		"snd_rolloff",			&psSoundRolloff,	EPS_S, 10);
	CMD4(CCC_Float,		"snd_doppler",			&psSoundDoppler,	EPS_S, 10);
	CMD2(CCC_Float,		"snd_volume_eff",		&psSoundVEffects);
	CMD2(CCC_Float,		"snd_volume_music",		&psSoundVMusic);
	CMD2(CCC_Float,		"snd_volume_master",	&psSoundVMaster);
	CMD4(CCC_Float,		"snd_occlusion_scale",	&psSoundOcclusionScale, 0.f,1.f		);
	CMD4(CCC_Float,		"snd_cull",				&psSoundCull,			0.01f,0.20f	);
	CMD3(CCC_Token,		"snd_freq",				&psSoundFreq,	snd_freq_token		);
	CMD3(CCC_Token,		"snd_model",			&psSoundModel,	snd_model_token		);
	CMD1(CCC_SND_Restart,"snd_restart"			);
	CMD3(CCC_Mask,		"snd_occlusion",		&psSoundFlags,	ssWaveTrace);
	CMD4(CCC_Integer,	"snd_targets",			&psSoundTargets,4,24);
	
	// Mouse
	CMD3(CCC_Mask,		"mouse_invert",			&psMouseInvert,1);
	CMD2(CCC_Float,		"mouse_sens",			&psMouseSens);

	// Camera
	CMD2(CCC_Float,		"cam_inert",			&psCamInert);
	CMD2(CCC_Float,		"cam_slide_inert",		&psCamSlideInert);

	// Collision
	CMD4(CCC_Integer,	"cl_act_depth",			&psCollideActDepth,			4,	32);
	CMD4(CCC_Integer,	"cl_act_stuck_depth",	&psCollideActStuckDepth,	2,	32);

	// Physic
	CMD4(CCC_Float,		"ph_gravity",			&psGravity,					1,	100);

	CMD3(CCC_Mask,		"eff_sun_glare",		&psEnvFlags,	effSunGlare);
	CMD3(CCC_Mask,		"eff_glow",				&psEnvFlags,	effGlows);
};
