#include "stdafx.h"
#include "x_ray.h"
#include "xr_ioconsole.h"
#include "xr_creator.h"
#include "xr_ioc_cmd.h"
#include "fdemorecord.h"
#include "fdemoplay.h"
#include "fbasicvisual.h"
#include "xr_tokens.h"
#include "cameramanager.h"
#include "environment.h"
#include "xr_input.h"
#include "fstaticrender_types.h"
#include "xr_level_controller.h"
#include "CustomHUD.h"

#include "xr_object.h"

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
		Log("- --- Command list start ---");
		CConsole::vecCMD_IT it;
		for (it=Console.Commands.begin(); it!=Console.Commands.end(); it++)
		{
			CConsoleCommand &C = *(it->second);
			TStatus _S; C.Status(_S);
			TInfo	_I;	C.Info	(_I);
			
			Msg("%-20s (%-10s) --- %s",	C.Name(), _S, _I);
		}
		Log("- --- Command list end ----");
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
		FILE *F = fopen		(c_name,"wt");
		R_ASSERT			(F);

		CConsole::vecCMD_IT it;
		for (it=Console.Commands.begin(); it!=Console.Commands.end(); it++)
			it->second->Save(F);

		fclose(F);
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
		
		FILE *f = fopen(str,"rt");
		if (f!=NULL) {
			while (fgets(str,1024,f)) {
				Console.Execute(str);
			}
			fclose(f);
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
		if (Engine.FS.Exist(fn,"",args,".save"))
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
		Sound->Restart();
	}
};
//-----------------------------------------------------------------------
class CCC_DemoRecord : public CConsoleCommand
{
public:
	CCC_DemoRecord(LPCSTR N) : CConsoleCommand(N) {};
	virtual void Execute(LPCSTR args) {
		Console.Hide	();
		char fn[256]; strconcat(fn,args,".xrdemo");
		pCreator->Cameras.AddEffector(new CDemoRecord(fn));
	}
};
class CCC_DemoPlay : public CConsoleCommand
{
public:
	CCC_DemoPlay(LPCSTR N) : 
		CConsoleCommand(N) 
		{ bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Console.Hide();
		char fn[256]; strconcat(fn,args,".xrdemo");
		pCreator->Cameras.AddEffector(new CDemoPlay(fn,1.3f));
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
extern int			psGlowsPerFrame;
extern int			rsDVB_Size;
extern int			rsDIB_Size;
extern int			psNET_ClientUpdate;
extern int			psNET_ClientPending;
extern int			psNET_ServerUpdate;
extern int			psNET_ServerPending;
extern char			psNET_Name[32];
extern int			psNET_Port;
extern int			psSH_Blur;
extern float		ssaDISCARD;
extern float		ssaDONTSORT;
extern float		ssaLOD_A;
extern float		ssaLOD_B;
extern float		ssaHZBvsTEX;
extern float		psDetailDensity;
extern int			psSheduler;
extern float		psDetail_w_rot1;
extern float		psDetail_w_rot2;
extern float		psDetail_w_speed;
extern float		psDetail_l_ambient;
extern float		psDetail_l_aniso;
extern int			psSupersample;
extern u32		psEnvFlags;

void CCC_Register()
{
	// Input
	extern void CCC_RegisterInput();
	CCC_RegisterInput	();

	// General
	CMD1(CCC_Help,		"help"					);
	CMD1(CCC_Quit,		"quit"					);
	CMD1(CCC_Start,		"start"					);
	CMD1(CCC_ServerLoad,"server_load"			);
	CMD1(CCC_Disconnect,"disconnect"			);
	CMD1(CCC_SaveCFG,	"cfg_save"				);
	CMD1(CCC_LoadCFG,	"cfg_load"				);

	CMD3(CCC_Mask,		"mt_sound",				&psDeviceFlags,	mtSound);
	CMD3(CCC_Mask,		"mt_input",				&psDeviceFlags,	mtInput);
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
	
	CMD4(CCC_Float,		"rs_geometry_lod",		&QualityControl.fGeometryLOD, 0,2);
	CMD4(CCC_Float,		"rs_min_fps",			&QualityControl.fMinFPS, 0, 512	);
	CMD4(CCC_Float,		"rs_max_fps",			&QualityControl.fMaxFPS, 0, 512	);
	CMD4(CCC_Float,		"rs_ssa_lod_a",			&ssaLOD_A,			16, 96		);
	CMD4(CCC_Float,		"rs_ssa_lod_b",			&ssaLOD_B,			32, 64		);
	CMD4(CCC_Float,		"rs_ssa_discard",		&ssaDISCARD,		1,  16		);
	CMD4(CCC_Float,		"rs_ssa_dontsort",		&ssaDONTSORT,		16, 128		);
	CMD4(CCC_Float,		"rs_ssa_hzb_vs_tex",	&ssaHZBvsTEX,		16,	512		);
	CMD3(CCC_Mask,		"rs_detail",			&psDeviceFlags,		rsDetails	);
	CMD4(CCC_Float,		"rs_detail_density",	&psDetailDensity,	.05f,0.3f	);
	CMD4(CCC_Float,		"rs_detail_w_rot1",		&psDetail_w_rot1,	1.f,180.f	);
	CMD4(CCC_Float,		"rs_detail_w_rot2",		&psDetail_w_rot2,	1.f,180.f	);
	CMD4(CCC_Float,		"rs_detail_w_speed",	&psDetail_w_speed,	1.f,4.f		);
	CMD4(CCC_Float,		"rs_detail_l_ambient",	&psDetail_l_ambient,.5f,.95f	);
	CMD4(CCC_Float,		"rs_detail_l_aniso",	&psDetail_l_aniso,	.1f,.5f		);
	
	CMD1(CCC_Gamma,		"rs_c_gamma"			);
	CMD1(CCC_Gamma,		"rs_c_brightness"		);
	CMD1(CCC_Gamma,		"rs_c_contrast"			);
	CMD4(CCC_Integer,	"rs_skeleton_update",	&psSkeletonUpdate,	2,		128	);
	CMD4(CCC_Integer,	"rs_glows_per_frame",	&psGlowsPerFrame,	2,		32	);
	CMD4(CCC_Integer,	"rs_vb_size",			&rsDVB_Size,		32,		4096);
	CMD4(CCC_Integer,	"rs_ib_size",			&rsDIB_Size,		32,		4096);
	CMD4(CCC_Integer,	"rs_sh_blur",			&psSH_Blur,			0,		2	);
	CMD4(CCC_Integer,	"rs_supersample",		&psSupersample,		0,		4	);
	
	// Network
	CMD4(CCC_Integer,	"net_cl_update_rate",	&psNET_ClientUpdate,3,		100	);
	CMD4(CCC_Integer,	"net_cl_pending_lim",	&psNET_ClientPending,0,		10	);
	CMD4(CCC_Integer,	"net_sv_update_rate",	&psNET_ClientUpdate,3,		100	);
	CMD4(CCC_Integer,	"net_sv_pending_lim",	&psNET_ClientPending,0,		10	);
	CMD4(CCC_Integer,	"net_port",				&psNET_Port,		5400,	5500);
	CMD3(CCC_String,	"net_name",				psNET_Name,			32			);

	// Texture manager	
	CMD4(CCC_Integer,	"texture_lod",			&psTextureLOD,		0,			4	);
	CMD3(CCC_Mask,		"texture_show_mips",	&psTextureFlags,	TF_ShowMipmaps	);

	// General video control
	CMD3(CCC_Token,		"vid_mode",				&psCurrentMode, vid_mode_token);
	CMD3(CCC_Token,		"vid_bpp",				&psCurrentBPP,	vid_bpp_token);
	CMD1(CCC_VID_Restart,"vid_restart"			);
	CMD1(CCC_VID_Reset, "vid_reset"				);
	
	// Sound
	CMD4(CCC_Float,		"snd_rolloff",			&psSoundRolloff,	DS3D_MINROLLOFFFACTOR, DS3D_MAXROLLOFFFACTOR);
	CMD4(CCC_Float,		"snd_doppler",			&psSoundDoppler,	DS3D_MINDOPPLERFACTOR, DS3D_MAXDOPPLERFACTOR );
	CMD2(CCC_Float,		"snd_volume_eff",		&psSoundVEffects);
	CMD2(CCC_Float,		"snd_volume_music",		&psSoundVMusic);
	CMD2(CCC_Float,		"snd_volume_master",	&psSoundVMaster);
	CMD4(CCC_Float,		"snd_occlusion_scale",	&psSoundOcclusionScale, 0.f,1.f		);
	CMD4(CCC_Float,		"snd_cull",				&psSoundCull,			0.01f,0.20f	);
	CMD3(CCC_Token,		"snd_freq",				&psSoundFreq,	snd_freq_token		);
	CMD3(CCC_Token,		"snd_model",			&psSoundModel,	snd_model_token		);
	CMD1(CCC_SND_Restart,"snd_restart"			);
	CMD3(CCC_Mask,		"snd_occlusion",		&psSoundFlags,	ssWaveTrace);
	CMD4(CCC_Integer,	"snd_relaxtime",		&psSoundRelaxTime,1,10);
	
	// Mouse
	CMD3(CCC_Mask,		"mouse_invert",			&psMouseInvert,1);
	CMD2(CCC_Float,		"mouse_sens",			&psMouseSens);

	// Camera
	CMD2(CCC_Float,		"cam_inert",			&psCamInert);
	CMD2(CCC_Float,		"cam_slide_inert",		&psCamSlideInert);

	// Demo
	CMD1(CCC_DemoRecord,"demo_record"			);
	CMD1(CCC_DemoPlay,	"demo_play"				);

	// Collision
	CMD4(CCC_Integer,	"cl_act_depth",			&psCollideActDepth,			4,	32);
	CMD4(CCC_Integer,	"cl_act_stuck_depth",	&psCollideActStuckDepth,	2,	32);

	// Physic
	CMD4(CCC_Float,		"ph_gravity",			&psGravity,					1,	100);

	CMD3(CCC_Mask,		"eff_sun_glare",		&psEnvFlags,	effSunGlare);
	CMD3(CCC_Mask,		"eff_glow",				&psEnvFlags,	effGlows);
};
