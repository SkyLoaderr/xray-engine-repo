#include "stdafx.h"
#include "x_ray.h"
#include "xr_ioconsole.h"
#include "xr_creator.h"
#include "xr_ioc_cmd.h"
#include "fdemorecord.h"
#include "fdemoplay.h"
#include "fbasicvisual.h"
#include "xr_tokens.h"
#include "xr_sndman.h"
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
		Engine.Event.Signal	(Event,(DWORD)Param);
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
	virtual void Execute(LPCSTR args) {
		SetFileAttributes("user.ltx",FILE_ATTRIBUTE_NORMAL);
		FILE *F = fopen("user.ltx","wt");
		R_ASSERT(F);

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
class CCC_Server : public CConsoleCommand
{
public:
	CCC_Server(LPCSTR N) : CConsoleCommand(N) {};
	virtual void Execute(LPCSTR args) {
		if (pCreator)	{
			Log("! Please disconnect/unload first");
			return;
		}
		int id		= pApp->Level_ID(args);
		if (id>=0) {
			Engine.Event.Defer("KERNEL:server",DWORD(strdup(args)));
		} else {
			Log("! Cannot find level '%s'.",args);
		}
	}
};

class CCC_Client : public CConsoleCommand
{
public:
	CCC_Client(LPCSTR N) : CConsoleCommand(N) {};
	virtual void Execute(LPCSTR args) {
		if (pCreator)	{
			Log("! Please disconnect/unload first");
			return;
		}
		Engine.Event.Defer("KERNEL:client",DWORD(strdup(args)));
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
			Device.Destroy();
			Device.Create();
		}
	}
};
//-----------------------------------------------------------------------
class CCC_SND_Restart : public CConsoleCommand
{
public:
	CCC_SND_Restart(LPCSTR N) : CConsoleCommand(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		pSounds->Restart();
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
	CCC_Gamma(LPCSTR N) : CCC_Float(N, &gamma, 0, 5), gamma(1) {};
	virtual void Execute(LPCSTR args)
	{
		CCC_Float::Execute(args);
		Device.Gamma.Gamma(gamma);
		Device.Gamma.Update();
	}
};
//-----------------------------------------------------------------------
/*
class CCC_Health : public CConsoleCommand
{
public:
	CCC_Health(LPCSTR N) : CConsoleCommand(N) {};
	virtual void Execute(LPCSTR args) {
		pCreator->CurrentEntity()->Cure(atoi(args.c_str()));
	}
};
//-----------------------------------------------------------------------
class CCC_Respawn: public CConsoleCommand
{
public:
	CCC_Respawn(LPCSTR N) : CConsoleCommand(N) {bEmptyArgsHandled = TRUE;};
	virtual void Execute(LPCSTR args) {
		pCreator->CurrentEntity()->Respawn();
	}
};
*/
//-----------------------------------------------------------------------
ENGINE_API float	psHUD_FOV=0.5f;

extern float	psOSSR;
extern int		psSkeletonUpdate;
extern int		psPhysicsFPS;
extern int		psGlowsPerFrame;
extern int		rsDVB_Size;
extern int		rsDIB_Size;
extern int		psNET_ClientUpdate;
extern int		psNET_ClientPending;
extern int		psNET_ServerUpdate;
extern int		psNET_ServerPending;
extern char		psNET_Name[32];
extern int		psNET_Port;
extern float	ssaLIMIT;
extern float	ssaDONTSORT;
extern float	psDetailDensity;


void CCC_Register()
{
	// Input
	extern void CCC_RegisterInput();
	CCC_RegisterInput	();

	// General
	CMD1(CCC_Help,		"help"					);
	CMD1(CCC_Quit,		"quit"					);
	CMD1(CCC_Server,	"server"				);
	CMD1(CCC_Client,	"client"				);
	CMD1(CCC_Disconnect,"disconnect"			);
	CMD1(CCC_SaveCFG,	"cfg_save"				);
	CMD1(CCC_LoadCFG,	"cfg_load"				);

	CMD3(CCC_Mask,		"mt_sound",				&psDeviceFlags,mtSound);
	CMD3(CCC_Mask,		"mt_input",				&psDeviceFlags,mtInput);
	
	// Events
	CMD1(CCC_E_Dump,	"e_list"				);
	CMD1(CCC_E_Signal,	"e_signal"				);
 		
	// Render device states
	CMD3(CCC_Mask,		"rs_no_v_sync",			&psDeviceFlags,rsNoVSync);
	CMD3(CCC_Mask,		"rs_anisotropic",		&psDeviceFlags,rsAnisotropic);
	CMD3(CCC_Mask,		"rs_wireframe",			&psDeviceFlags,rsWireframe);
	CMD3(CCC_Mask,		"rs_renormalize",		&psDeviceFlags,rsNormalize);
	CMD3(CCC_Mask,		"rs_antialias",			&psDeviceFlags,rsAntialias);
	CMD3(CCC_Mask,		"rs_triplebuffer",		&psDeviceFlags,rsTriplebuffer);
	CMD3(CCC_Mask,		"rs_fullscreen",		&psDeviceFlags,rsFullscreen);
	CMD3(CCC_Mask,		"rs_clear_bb",			&psDeviceFlags,rsClearBB);
	CMD3(CCC_Mask,		"rs_overdraw",			&psDeviceFlags,rsOverdrawView);
	CMD3(CCC_Mask,		"rs_occlusion",			&psDeviceFlags,rsOcclusion);
	CMD3(CCC_Mask,		"rs_depth_enhance",		&psDeviceFlags,rsDepthEnhance);
	CMD3(CCC_Mask,		"rs_stats",				&psDeviceFlags,rsStatistic);
	
	CMD4(CCC_Float,		"rs_geometry_lod",		&QualityControl.fGeometryLOD, 0,2);
	CMD4(CCC_Float,		"rs_min_fps",			&QualityControl.fMinFPS, 0, 512);
	CMD4(CCC_Float,		"rs_max_fps",			&QualityControl.fMaxFPS, 0, 512);
	CMD4(CCC_Float,		"rs_occ_reject",		&psOSSR,		0, 1);
	CMD4(CCC_Float,		"rs_ssa_discard",		&ssaLIMIT,		1, 16);
	CMD4(CCC_Float,		"rs_ssa_dontsort",		&ssaDONTSORT,	16,65536);
	CMD4(CCC_Float,		"rs_detail_density",	&psDetailDensity,.05f,0.3f);
	
	CMD1(CCC_Gamma,		"rs_gamma"				);
	CMD4(CCC_Integer,	"rs_skeleton_update",	&psSkeletonUpdate,	2,		128	);
	CMD4(CCC_Integer,	"rs_glows_per_frame",	&psGlowsPerFrame,	2,		32	);
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
	CMD4(CCC_Integer,	"texture_lod",			&psTextureLOD,0,4);
	CMD3(CCC_Mask,		"texture_show_mips",	&psTextureFlags, TF_ShowMipmaps);

	// General video control
	CMD3(CCC_Token,		"vid_mode",				&psCurrentMode, vid_mode_token);
	CMD3(CCC_Token,		"vid_bpp",				&psCurrentBPP,	vid_bpp_token);
	CMD1(CCC_VID_Restart,"vid_restart"			);

	// Sound
	CMD2(CCC_Float,		"snd_volume_eff",		&psSoundVEffects);
	CMD2(CCC_Float,		"snd_volume_music",		&psSoundVMusic);
	CMD2(CCC_Float,		"snd_volume_master",	&psSoundVMaster);
	CMD3(CCC_Token,		"snd_freq",				&psSoundFreq,	snd_freq_token);
	CMD3(CCC_Token,		"snd_model",			&psSoundModel,	snd_model_token);
	CMD1(CCC_SND_Restart,"snd_restart"			);

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
	CMD4(CCC_Integer,	"cl_act_depth",			&psCollideActDepth,			4, 32);
	CMD4(CCC_Integer,	"cl_act_stuck_depth",	&psCollideActStuckDepth,	2, 32);

	// Physic
	CMD4(CCC_Float,		"ph_gravity",			&psGravity,					1,100);
};
