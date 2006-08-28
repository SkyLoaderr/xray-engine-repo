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
#include "SkeletonAnimated.h"
#include "ResourceManager.h"

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
	{ "1280x960",					1280										},
	{ "1280x1024",					1280+1										},
	{ "1600x900",					1600-1										},
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
//		TerminateProcess(GetCurrentProcess(),0);
		Console->Hide();
		Engine.Event.Defer("KERNEL:disconnect");
		Engine.Event.Defer("KERNEL:quit");
	}
};
//-----------------------------------------------------------------------
extern __declspec(dllimport)	size_t	__cdecl lua_memusage	();
class CCC_MemStat : public IConsole_Command
{
public:
	CCC_MemStat(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		string_path fn;
		if (args&&args[0])	sprintf	(fn,"%s.dump",args);
		else				strcpy	(fn,"x:\\$memory$.dump");
		Memory.mem_statistic				(fn);
//		g_pStringContainer->dump			();
//		g_pSharedMemoryContainer->dump		();
	}
};
static void vminfo (size_t *_free, size_t *reserved, size_t *committed) {
	MEMORY_BASIC_INFORMATION memory_info;
	memory_info.BaseAddress = 0;
	*_free = *reserved = *committed = 0;
	while (VirtualQuery (memory_info.BaseAddress, &memory_info, sizeof (memory_info))) {
		switch (memory_info.State) {
		case MEM_FREE:
			*_free		+= memory_info.RegionSize;
			break;
		case MEM_RESERVE:
			*reserved	+= memory_info.RegionSize;
			break;
		case MEM_COMMIT:
			*committed += memory_info.RegionSize;
			break;
		}
		memory_info.BaseAddress = (char *) memory_info.BaseAddress + memory_info.RegionSize;
	}
}
class CCC_MemStats : public IConsole_Command
{
public:
	CCC_MemStats(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Memory.mem_compact		();
		size_t  w_free, w_reserved, w_committed;
		vminfo	(&w_free, &w_reserved, &w_committed);
		u32		_total			= Memory.mem_usage	();
		u32		_lua			= 0; // u32 (lua_memusage());
		u32		_eco_strings	= g_pStringContainer->stat_economy			();
		u32		_eco_smem		= g_pSharedMemoryContainer->stat_economy	();
		u32	m_base=0,c_base=0,m_lmaps=0,c_lmaps=0;
		if (Device.Resources)	Device.Resources->_GetMemoryUsage	(m_base,c_base,m_lmaps,c_lmaps);
		Msg		("* [win32]: free[%d K], reserved[%d K], committed[%d K]",w_free/1024,w_reserved/1024,w_committed/1024);
		Msg		("* [ D3D ]: textures[%d K]", (m_base+m_lmaps)/1024);
		Msg		("* [x-ray]: total[%d K], lua[%d K]",_total/1024,_lua/(1024*1024));
		Msg		("* [x-ray]: economy: strings[%d K], smem[%d K]",_eco_strings/1024,_eco_smem);
	}
};
class CCC_DbgMemCheck : public IConsole_Command
{
public:
	CCC_DbgMemCheck(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) { if (Memory.debug_mode){ Memory.dbg_check();}else{Msg("! Run with -mem_debug options.");} }
};
class CCC_DbgStrCheck : public IConsole_Command
{
public:
	CCC_DbgStrCheck(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) { g_pStringContainer->verify(); }
};
class CCC_DbgStrDump : public IConsole_Command
{
public:
	CCC_DbgStrDump(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) { g_pStringContainer->dump();}
};

//-----------------------------------------------------------------------
class CCC_MotionsStat : public IConsole_Command
{
public:
	CCC_MotionsStat(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		g_pMotionsContainer->dump();
	}
};
class CCC_TexturesStat : public IConsole_Command
{
public:
	CCC_TexturesStat(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Device.Resources->_DumpMemoryUsage();
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
void 			crashthread			( void* )
{
	Sleep		(1000);
	Msg			("! crash thread activated")	;
	u64			clk		= CPU::GetCLK		()	;
	CRandom		rndg;
	rndg.seed	(s32(clk));
	for (;;)	{
		Sleep	(1);
		__try	{
			//try {
				union	{
					struct {
						u8	_b0;
						u8	_b1;
						u8	_b2;
						u8	_b3;
					};
					uintptr_t	_ptri;
					u32*		_ptr;
				}		rndptr;
				rndptr._b0		=	u8(rndg.randI(0,256));
				rndptr._b1		=	u8(rndg.randI(0,256));
				rndptr._b2		=	u8(rndg.randI(0,256));
				rndptr._b3		=	u8(rndg.randI(0,256));
				rndptr._ptri	&=  (1ul<31ul)-1;
				*rndptr._ptr	=	0xBAADF00D;
			//} catch(...) {
			//	// OK
			//}
		} __except	(EXCEPTION_EXECUTE_HANDLER)	{
			// OK
		}
	}
}
class CCC_Crash : public IConsole_Command
{
public:
	CCC_Crash(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		thread_spawn	(crashthread,"crash",0,0);
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
		if (SetFileAttributes(c_name,FILE_ATTRIBUTE_NORMAL)){
			IWriter* F			= FS.w_open(c_name);
			if (F){
				CConsole::vecCMD_IT it;
				for (it=Console->Commands.begin(); it!=Console->Commands.end(); it++)
					it->second->Save(F);
				FS.w_close			(F);
			}
		}
	}
};
class CCC_LoadCFG : public IConsole_Command
{
public:
	CCC_LoadCFG(LPCSTR N) : IConsole_Command(N) {};
	virtual void Execute(LPCSTR args) {
		Msg("Executing config-script \"%s\"...",args);
		string1024 str;

		//RecordCommands	= false;
		strcpy	(str,args);
		if (strext(str))	*strext(str) = 0;
		strcat	(str,".ltx");
		
		IReader* F = FS.r_open(str);
		if (F!=NULL) {
			while (!F->eof()) {
				F->r_string			(str,sizeof(str));
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
/*		if (g_pGameLevel)	{
			Log		("! Please disconnect/unload first");
			return;
		}
*/
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
float	ps_gamma=1.f,ps_brightness=1.f,ps_contrast=1.f;
class CCC_Gamma : public CCC_Float
{
public:
	CCC_Gamma	(LPCSTR N, float* V) : CCC_Float(N,V,0.5f,1.5f)	{}

	virtual void Execute(LPCSTR args)
	{
		CCC_Float::Execute		(args);
		Device.Gamma.Gamma		(ps_gamma);
		Device.Gamma.Brightness	(ps_brightness);
		Device.Gamma.Contrast	(ps_contrast);
		Device.Gamma.Update		();
	}
};

//-----------------------------------------------------------------------
#ifdef	DEBUG
extern  INT	g_bDR_LM_UsePointsBBox;
extern	Fvector	g_DR_LM_Min, g_DR_LM_Max;

class CCC_DR_ClearPoint : public IConsole_Command
{
public:
	CCC_DR_ClearPoint(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		g_DR_LM_Min.x = 1000000.0f;
		g_DR_LM_Min.z = 1000000.0f;

		g_DR_LM_Max.x = -1000000.0f;
		g_DR_LM_Max.z = -1000000.0f;

		Msg("Local BBox (%f, %f) - (%f, %f)", g_DR_LM_Min.x, g_DR_LM_Min.z, g_DR_LM_Max.x, g_DR_LM_Max.z);
	}
};

class CCC_DR_TakePoint : public IConsole_Command
{
public:
	CCC_DR_TakePoint(LPCSTR N) : IConsole_Command(N)	{ bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		Fvector CamPos =  Device.vCameraPosition;

		if (g_DR_LM_Min.x > CamPos.x)	g_DR_LM_Min.x = CamPos.x;
		if (g_DR_LM_Min.z > CamPos.z)	g_DR_LM_Min.z = CamPos.z;

		if (g_DR_LM_Max.x < CamPos.x)	g_DR_LM_Max.x = CamPos.x;
		if (g_DR_LM_Max.z < CamPos.z)	g_DR_LM_Max.z = CamPos.z;

		Msg("Local BBox (%f, %f) - (%f, %f)", g_DR_LM_Min.x, g_DR_LM_Min.z, g_DR_LM_Max.x, g_DR_LM_Max.z);
	}
};

class CCC_DR_UsePoints : public CCC_Integer
{
public:
	CCC_DR_UsePoints(LPCSTR N, int* V, int _min=0, int _max=999) : CCC_Integer(N, V, _min, _max)	{};
	virtual void	Save	(IWriter *F)	{};
};
#endif
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
extern Flags32		psEnvFlags;
extern float		r__dtex_range;

extern int			g_ErrorLineCount;

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
	CMD1(CCC_Crash,		"crash"					);
#endif

	CMD1(CCC_MotionsStat,	"stat_motions"		);
	CMD1(CCC_MemStats,		"stat_memory"		);
	CMD1(CCC_TexturesStat,	"stat_textures"		);

	CMD1(CCC_MemStat,		"dbg_mem_dump"		);
	CMD1(CCC_DbgMemCheck,	"dbg_mem_check"		);
	CMD1(CCC_DbgStrCheck,	"dbg_str_check"		);
	CMD1(CCC_DbgStrDump,	"dbg_str_dump"		);

	CMD3(CCC_Mask,		"mt_particles",			&psDeviceFlags,			mtParticles);
#ifdef DEBUG

	CMD3(CCC_Mask,		"mt_sound",				&psDeviceFlags,			mtSound);
	CMD3(CCC_Mask,		"mt_physics",			&psDeviceFlags,			mtPhysics);
	CMD3(CCC_Mask,		"mt_network",			&psDeviceFlags,			mtNetwork);
	
	// Events
	CMD1(CCC_E_Dump,	"e_list"				);
	CMD1(CCC_E_Signal,	"e_signal"				);

	CMD3(CCC_Mask,		"rs_wireframe",			&psDeviceFlags,		rsWireframe);
	CMD3(CCC_Mask,		"rs_clear_bb",			&psDeviceFlags,		rsClearBB);
	CMD3(CCC_Mask,		"rs_occlusion",			&psDeviceFlags,		rsOcclusion);

	CMD3(CCC_Mask,		"rs_detail",			&psDeviceFlags,		rsDetails	);
	CMD4(CCC_Float,		"r__dtex_range",		&r__dtex_range,		5,		75	);

	CMD3(CCC_Mask,		"rs_constant_fps",		&psDeviceFlags,		rsConstantFPS			);
	CMD3(CCC_Mask,		"rs_render_statics",	&psDeviceFlags,		rsDrawStatic			);
	CMD3(CCC_Mask,		"rs_render_dynamics",	&psDeviceFlags,		rsDrawDynamic			);
#endif

	// Render device states
	CMD4(CCC_Integer,	"r__supersample",		&ps_r__Supersample,			1,		4		);


	CMD3(CCC_Mask,		"rs_no_v_sync",			&psDeviceFlags,		rsNoVSync				);
//	CMD3(CCC_Mask,		"rs_disable_objects_as_crows",&psDeviceFlags,	rsDisableObjectsAsCrows	);
	CMD3(CCC_Mask,		"rs_fullscreen",		&psDeviceFlags,		rsFullscreen			);
	CMD3(CCC_Mask,		"rs_refresh_60hz",		&psDeviceFlags,		rsRefresh60hz			);
	CMD3(CCC_Mask,		"rs_stats",				&psDeviceFlags,		rsStatistic				);
	CMD4(CCC_Float,		"rs_vis_distance",		&psVisDistance,		0.01f,	1.0f			);
	CMD3(CCC_Mask,		"rs_cam_pos",			&psDeviceFlags,		rsCameraPos				);

	CMD3(CCC_Mask,		"rs_occ_draw",			&psDeviceFlags,		rsOcclusionDraw			);
	CMD3(CCC_Mask,		"rs_occ_stats",			&psDeviceFlags,		rsOcclusionStats		);

	CMD2(CCC_Gamma,		"rs_c_gamma"			,&ps_gamma			);
	CMD2(CCC_Gamma,		"rs_c_brightness"		,&ps_brightness		);
	CMD2(CCC_Gamma,		"rs_c_contrast"			,&ps_contrast		);
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
	CMD3(CCC_Mask,		"snd_acceleration",		&psSoundFlags,		ss_Hardware	);
	CMD3(CCC_Mask,		"snd_efx",				&psSoundFlags,		ss_EAX		);
	CMD4(CCC_Integer,	"snd_targets",			&psSoundTargets,	4,32		);
	CMD4(CCC_Integer,	"snd_cache_size",		&psSoundCacheSizeMB,4,32		);
	CMD3(CCC_Mask,		"snd_stats",			&g_stats_flags,		st_sound	);
	CMD3(CCC_Mask,		"snd_stats_min_dist",	&g_stats_flags,		st_sound_min_dist );
	CMD3(CCC_Mask,		"snd_stats_max_dist",	&g_stats_flags,		st_sound_max_dist );
	CMD3(CCC_Mask,		"snd_stats_ai_dist",	&g_stats_flags,		st_sound_ai_dist );
	CMD3(CCC_Mask,		"snd_stats_info_name",	&g_stats_flags,		st_sound_info_name );
	CMD3(CCC_Mask,		"snd_stats_info_object",&g_stats_flags,		st_sound_info_object );

	#ifdef DEBUG
	CMD4(CCC_Integer,	"error_line_count",		&g_ErrorLineCount,	6,	1024	);
#endif

	// Mouse
	CMD3(CCC_Mask,		"mouse_invert",			&psMouseInvert,1);
	psMouseSens = 0.2f;
	CMD2(CCC_Float,		"mouse_sens",			&psMouseSens);

	// Camera
	CMD2(CCC_Float,		"cam_inert",			&psCamInert);
	CMD2(CCC_Float,		"cam_slide_inert",		&psCamSlideInert);

	//psSoundRolloff			= pSettings->r_float	("sound","rolloff");		clamp(psSoundRolloff,			EPS_S,	2.f);
	psSoundOcclusionScale	= pSettings->r_float	("sound","occlusion_scale");clamp(psSoundOcclusionScale,	0.1f,	.5f);

	CMD1(CCC_Pause,		"pause"			);
#ifdef DEBUG
	extern	INT	g_Dump_Export_Obj;
	extern	INT	g_Dump_Import_Obj;
	CMD4(CCC_Integer,	"net_dbg_dump_export_obj",	&g_Dump_Export_Obj, 0, 1);
	CMD4(CCC_Integer,	"net_dbg_dump_import_obj",	&g_Dump_Import_Obj, 0, 1);
#endif

#ifdef DEBUG	
	CMD1(CCC_DR_TakePoint,		"demo_record_take_point");
	CMD1(CCC_DR_ClearPoint,		"demo_record_clear_points");
	CMD4(CCC_DR_UsePoints,		"demo_record_use_points",	&g_bDR_LM_UsePointsBBox, 0, 1);
#endif


};
 