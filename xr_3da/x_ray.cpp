//-----------------------------------------------------------------------------
// File: x_ray.cpp
//
// Programmers:
//	Oles		- Oles Shishkovtsov
//	AlexMX		- Alexander Maksimchuk
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "igame_level.h"
#include "igame_persistent.h"

#include "xr_input.h"
#include "xr_ioconsole.h"
#include "x_ray.h"
#include "std_classes.h"
#include "GameFont.h"
#include "resource.h"
#include "LightAnimLibrary.h"
#include "ispatial.h"

//////////////////////////////////////////////////////////////////////////
struct _SoundProcessor	: public pureFrame
{
	virtual void OnFrame	( )
	{
		// Msg							("------------- sound: %d",u32(Device.dwFrame));
		Device.Statistic.Sound.Begin();
		::Sound->update				(Device.vCameraPosition,Device.vCameraDirection,Device.vCameraTop,Device.fTimeDelta);
		Device.Statistic.Sound.End	();
	}
}	SoundProcessor;

//////////////////////////////////////////////////////////////////////////
// global variables
ENGINE_API	CApplication*	pApp			= NULL;
static		HWND			logoWindow		= NULL;

float	frnd			()						{
	float	v			= float(rand())/float(RAND_MAX);
	return	(v-0.5f)*2.f;
}
u8		fpack			(float v)				{
	s32	_v	= iFloor	(((v+1)*.5f)*255.f + .5f);
	clamp	(_v,0,255);
	return	u8(_v);
}
u8		fpackZ			(float v)				{
	s32	_v	= iFloor	(-v*255.f + .5f);
	clamp	(_v,0,255);
	return	u8(_v);
}
Fvector	vunpack			(s32 x, s32 y, s32 z)	{
	Fvector	pck;
	pck.x	= (float(x)/255.f - .5f)*2.f;
	pck.y	= (float(y)/255.f - .5f)*2.f;
	pck.z	= -float(z)/255.f;
	return	pck;
}
Fvector	vunpack			(Ivector src)			{
	return	vunpack	(src.x,src.y,src.z);
}
Ivector	vpack			(Fvector src)
{
	Fvector			_v;
	int	bx			= fpack	(src.x);
	int by			= fpack	(src.y);
	int bz			= fpackZ(src.z);
	// dumb test
	float	e_best	= flt_max;
	int		r=bx,g=by,b=bz;
	int		d=17;
	for (int x=_max(bx-d,0); x<=_min(bx+d,255); x++)
		for (int y=_max(by-d,0); y<=_min(by+d,255); y++)
			for (int z=_max(bz-d,0); z<=_min(bz+d,255); z++)
			{
				_v				= vunpack(x,y,z);
				float	m		= _v.magnitude();
				float	me		= _abs(m-1.f);
				if	(me>0.03f)	continue;
				_v.div	(m);
				float	e		= _abs(src.dotproduct(_v)-1.f);
				if (e<e_best)	{
					e_best		= e;
					r=x,g=y,b=z;
				}
			}
			Ivector		ipck;
			ipck.set	(r,g,b);
			return		ipck;
}
float	_e	(float e)
{
	return	e;
	float	a = acosf(e);
	return	deg2rad(_abs(a));
}
float	_m	(float m)
{
	return _abs(m-1.f)*100.f;
}

const	u32		tcnt	= 100000;
void	test_vec3norm	()
{
	float	_1_e=0,_1_m=0;
	float	_2_e=0,_2_m=0;
	FPU::m64r	();
	for (u32 test=0; test<tcnt; test++)
	{
		CRandom			Random;

		// src
		Fvector			src;
		for(;;)
		{
			src.set			(frnd(),frnd(),frnd());
			if	(src.z<=0)	break;
		}
		src.normalize	();

		// pack + base error
		Fvector			_v;
		Ivector			ipck;
		{
			ipck.set	(fpack(src.x),fpack(src.y),fpackZ(src.z));
			_v			= vunpack(ipck);
			float	m	= _v.magnitude();	_v.normalize();
			float	e	= _abs(src.dotproduct(_v)-1.f);
			//Msg				("%4d: [%+1.3f,%+1.3f,%+1.3f] => [%3d,%3d,%3d] => [%+1.3f,%+1.3f,%+1.3f] => %.8f, %2.2f%%",
			//	test,VPUSH(src),VPUSH(ipck),VPUSH(_v),_e(e),_m(m));
			_1_e+=e,_1_m+=m;
		}

		// pack + error
		{
			ipck	= vpack(src);
			_v		= vunpack(ipck);
			float	m	= _v.magnitude();	_v.normalize();
			float	e	= _abs(src.dotproduct(_v)-1.f);
			//Msg			("%28s => [%3d,%3d,%3d] => [%+1.3f,%+1.3f,%+1.3f] => %.8f, %2.2f%%",
			//	" ",VPUSH(ipck),VPUSH(_v),_e(e),_m(m));
			_2_e+=e,_2_m+=m;
		}
	}

	_1_e/=float(tcnt); _1_m/=float(tcnt); float _1_a = _abs(rad2deg(acosf(_1_e))-90);
	_2_e/=float(tcnt); _2_m/=float(tcnt); float _2_a = _abs(rad2deg(acosf(_2_e))-90);
	Msg	("*** 1: %0.9f(%0.9f), %1.5f%%", _e(_1_e),	_1_a,		_m(_1_m));
	Msg	("*** 2: %0.9f(%0.9f), %1.5f%%", _e(_2_e),	_2_a,		_m(_2_m));
	Msg	("*****: %1.9f(%1.9f), %1.5f% ", _1_e/_2_e,	_1_a/_2_a,	_m(_2_m)/_m(_1_m));
	FlushLog			();
}

// startup point
void Startup				( )
{
	CTimer				T;
	//SetPriorityClass	(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	T.Start				();
	test_vec3norm		();
	float	t			= T.GetElapsed_sec	()*1000.f;
	//SetPriorityClass	(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
	Msg					("%2.5f ms",float(tcnt)/t);

	// initialization
	Engine.Initialize			( );
	Device.Initialize			( );

	// Creation
	string256					fname; 
	FS.update_path				(fname,"$game_data$","system.ltx");
	pSettings					= xr_new<CInifile>	(fname,TRUE);

	Console						= xr_new<CConsole>	();
	Console->Initialize			( );
	Engine.External.Initialize	( );
	
	// Execute script
	{
		strcpy						(Console->ConfigFile,"user.ltx");
		if (strstr(Core.Params,"-ltx ")) {
			string64				c_name;
			sscanf					(strstr(Core.Params,"-ltx ")+5,"%[^ ] ",c_name);
			strcpy					(Console->ConfigFile,c_name);
		}
		if (!FS.exist(Console->ConfigFile))
			strcpy					(Console->ConfigFile,"user.ltx");
		Console->ExecuteScript		(Console->ConfigFile);
	}

	BOOL bCaptureInput			= !strstr(Core.Params,"-i");

	pInput						= xr_new<CInput>		(bCaptureInput);
	Sound->_initialize			(u64(Device.m_hWnd));

	// ...command line for auto start
	LPCSTR	pStartup			= strstr		(Core.Params,"-start ");
	if (pStartup)				Console->Execute	(pStartup+1);

	// Initialize APP
	Device.Create				( );
	LALib.OnCreate				( );
	pApp						= xr_new<CApplication>	();
	g_pGamePersistent			= (IGame_Persistent*)	NEW_INSTANCE (CLSID_GAME_PERSISTANT);
	g_SpatialSpace				= xr_new<ISpatial_DB>	();

	// Destroy LOGO
	DestroyWindow				(logoWindow);
	logoWindow					= NULL;

	// Main cycle
	Device.Run					( );

	// Destroy APP
	xr_delete					( g_SpatialSpace	);
	DEL_INSTANCE				( g_pGamePersistent );
	xr_delete					( pApp				);
	Engine.Event.Dump			( );

	// Destroying
	Sound->_destroy				( );
	xr_delete					( pInput		);
	xr_delete					( pSettings		);

	LALib.OnDestroy				( );
	Console->Destroy				( );
	xr_delete					(Console);
	Device.Destroy				( );
	Engine.Destroy				( );
}

static BOOL CALLBACK logDlgProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
		case WM_DESTROY:
			break;
		case WM_CLOSE:
			DestroyWindow( hw );
			break;
		case WM_COMMAND:
			if( LOWORD(wp)==IDCANCEL )
				DestroyWindow( hw );
			break;
		default:
			return FALSE;
	}
	return TRUE;
}
/*
void	test_rtc	()
{
	CStatTimer		tMc,tM,tC,tD;
	u32				bytes=0;
	tMc.FrameStart	();
	tM.FrameStart	();
	tC.FrameStart	();
	tD.FrameStart	();
	::Random.seed	(0x12071980);
	for		(u32 test=0; test<10000; test++)
	{
		u32			in_size			= ::Random.randI(1024,256*1024);
		u32			out_size_max	= rtc_csize		(in_size);
		u8*			p_in			= xr_alloc<u8>	(in_size);
		u8*			p_in_tst		= xr_alloc<u8>	(in_size);
		u8*			p_out			= xr_alloc<u8>	(out_size_max);
		for (u32 git=0; git<in_size; git++)			p_in[git] = (u8)::Random.randI	(8);	// garbage
		bytes		+= in_size;

		tMc.Begin	();
		memcpy		(p_in_tst,p_in,in_size);
		tMc.End		();

		tM.Begin	();
		Memory.mem_copy(p_in_tst,p_in,in_size);
		tM.End		();

		tC.Begin	();
		u32			out_size		= rtc_compress	(p_out,out_size_max,p_in,in_size);
		tC.End		();

		tD.Begin	();
		u32			in_size_tst		= rtc_decompress(p_in_tst,in_size,p_out,out_size);
		tD.End		();

		// sanity check
		R_ASSERT	(in_size == in_size_tst);
		for (u32 tit=0; tit<in_size; tit++)			R_ASSERT(p_in[tit] == p_in_tst[tit]);	// garbage

		xr_free		(p_out);
		xr_free		(p_in_tst);
		xr_free		(p_in);
	}
	tMc.FrameEnd	();	float rMc		= 1000.f*(float(bytes)/tMc.result)/(1024.f*1024.f);
	tM.FrameEnd		(); float rM		= 1000.f*(float(bytes)/tM.result)/(1024.f*1024.f);
	tC.FrameEnd		(); float rC		= 1000.f*(float(bytes)/tC.result)/(1024.f*1024.f);
	tD.FrameEnd		(); float rD		= 1000.f*(float(bytes)/tD.result)/(1024.f*1024.f);
	Msg				("* memcpy:        %5.2f M/s (%3.1f%%)",rMc,100.f*rMc/rMc);
	Msg				("* mm-memcpy:     %5.2f M/s (%3.1f%%)",rM,100.f*rM/rMc);
	Msg				("* compression:   %5.2f M/s (%3.1f%%)",rC,100.f*rC/rMc);
	Msg				("* decompression: %5.2f M/s (%3.1f%%)",rD,100.f*rD/rMc);
}
*/

class _A 
{
	int _a;
public:
	virtual ~_A()	{}
};

class _B : public _A
{
	int _b;
public:
};

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     char *    lpCmdLine,
                     int       nCmdShow)
{
	// Title window
	logoWindow = CreateDialog(
		GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDD_STARTUP),
		0, logDlgProc );

	Core._initialize		("xray",NULL);
	FPU::m24r				();

	// test_rtc				();

	Startup					();
	
	Core._destroy			();

	// check for need to execute something external
	if (strstr(lpCmdLine,"-exec ")) 
	{
		char *N = strstr(lpCmdLine,"-exec ")+6;
		return (int)_execl(N,N,0);
	}
	return 0;
}

CApplication::CApplication()
{
	ll_dwReference	= 0;

	// events
	eQuit						= Engine.Event.Handler_Attach("KERNEL:quit",this);
	eStart						= Engine.Event.Handler_Attach("KERNEL:start",this);
	eStartLoad					= Engine.Event.Handler_Attach("KERNEL:load",this);
	eDisconnect					= Engine.Event.Handler_Attach("KERNEL:disconnect",this);

	// levels
	Level_Current				= 0;
	Level_Scan					( );

	// Font
	pFontSystem					= xr_new<CGameFont>	("startup_font",CGameFont::fsGradient|CGameFont::fsDeviceIndependent);
	Device.seqRender.Add		( pFontSystem, REG_PRIORITY_LOW-1000 );

	// Register us
	Device.seqFrame.Add			(this, REG_PRIORITY_HIGH+1000);
	if (psDeviceFlags.test(mtSound))	Device.seqFrameMT.Add		(&SoundProcessor);
	else								Device.seqFrame.Add			(&SoundProcessor);
	Console->Show				( );
}

CApplication::~CApplication()
{
	Console->Hide				( );

	Device.seqFrameMT.Remove	(&SoundProcessor);
	Device.seqFrame.Remove		(&SoundProcessor);
	Device.seqFrame.Remove		(this);

	// font
	Device.seqRender.Remove		( pFontSystem		);
	xr_delete					( pFontSystem		);

	// events
	Engine.Event.Handler_Detach	(eDisconnect,this);
	Engine.Event.Handler_Detach	(eStartLoad,this);
	Engine.Event.Handler_Detach	(eStart,this);
	Engine.Event.Handler_Detach	(eQuit,this);
}

void CApplication::OnEvent(EVENT E, u64 P1, u64 P2)
{
	if (E==eQuit)	{
		PostQuitMessage	(0);
		
		for (u32 i=0; i<Levels.size(); i++)
		{
			xr_free(Levels[i].folder	);
			xr_free(Levels[i].name	);
		}
	} else if (E==eStart) {
		Console->Hide();
		LPSTR		op_server		= LPSTR	(P1);
		LPSTR		op_client		= LPSTR	(P2);
		R_ASSERT	(0==g_pGameLevel);
		g_pGameLevel= (IGame_Level*)	NEW_INSTANCE(CLSID_GAME_LEVEL);
		R_ASSERT	(g_pGameLevel->net_Start(op_server,op_client));
		xr_free		(op_server);
		xr_free		(op_client);

		// start any console command
		if (strstr(Core.Params,"-$")) {
			string128			buf,cmd,param;
			sscanf				(strstr(Core.Params,"-$")+2,"%[^ ] %[^ ] ",cmd,param);
			strconcat			(buf,cmd," ",param);
			Console->Execute		(buf);
		}
	} else if (E==eDisconnect) {
		if (g_pGameLevel) {
			Console->Hide			();
			g_pGameLevel->net_Stop	();
			DEL_INSTANCE			(g_pGameLevel);
			Console->Show			();
		}
	}
}

void CApplication::LoadBegin()
{
	ll_dwReference++;
	if (1==ll_dwReference) {
		ll_hGeom.create	(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
		ll_hLogo1.create("font","ui\\logo");
		ll_hLogo2.create("font","ui\\ui_logo_nv");
		ll_hLogo		= ll_hLogo2;
	}
}

void CApplication::LoadEnd()
{
	ll_dwReference--;
}

void CApplication::LoadTitle	(char *S, char *S2)
{
	VERIFY(ll_dwReference);

	Device.Begin();

	// Draw logo
	u32	Offset;
	u32	C						= 0xffffffff;
	u32	_w						= Device.dwWidth;
	u32	_h						= Device.dwHeight;
	FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock(4,ll_hGeom.stride(),Offset);
	pv->set						(EPS_S,				float(_h+EPS_S),	0+EPS_S, 1, C, 0, 1);	pv++;
	pv->set						(EPS_S,				EPS_S,				0+EPS_S, 1, C, 0, 0);	pv++;
	pv->set						(float(_w+EPS_S),	float(_h+EPS_S),	0+EPS_S, 1, C, 1, 1);	pv++;
	pv->set						(float(_w+EPS_S),	EPS_S,				0+EPS_S, 1, C, 1, 0);	pv++;
	RCache.Vertex.Unlock		(4,ll_hGeom.stride());
	
	RCache.set_Shader			(ll_hLogo);
	RCache.set_Geometry			(ll_hGeom);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);

	// Draw title
	Log							(S,S2);
	R_ASSERT					(pFontSystem);
	pFontSystem->Clear			();
	pFontSystem->SetColor		(color_rgba(192,192,192,255));
	pFontSystem->SetAligment	(CGameFont::alCenter);
	char *F = "%s";
	if (S2) F="%s%s";
	pFontSystem->Out			(0.f,0.93f,F,S,S2);
	pFontSystem->OnRender		();

	Device.End	();
}

void CApplication::LoadSwitch()
{
	if (ll_hLogo == ll_hLogo1)	ll_hLogo = ll_hLogo2;
	else						ll_hLogo = ll_hLogo1;
}

void CApplication::OnFrame	( )
{
	/*
	CTimer	T;
	T.Start	();
	while	(T.GetElapsed_ms()<10);
	*/
	Engine.Event.OnFrame	();
	g_SpatialSpace->update	();
}

void CApplication::Level_Scan()
{
	xr_vector<char*>*	folder	= FS.file_list_open	("$game_levels$","",FS_ListFolders|FS_RootOnly);
	R_ASSERT			(!folder->empty());
	for (u32 i=0; i<folder->size(); i++)
	{
		string256	N1,N2,N3,N4;
		strconcat	(N1,(*folder)[i],"level");
		strconcat	(N2,(*folder)[i],"level.ltx");
		strconcat	(N3,(*folder)[i],"level.game");
		strconcat	(N4,(*folder)[i],"level.cform");
		if	(
			FS.exist("$game_levels$",N1)		&&
			FS.exist("$game_levels$",N2)		&&
			FS.exist("$game_levels$",N3)		&&
			FS.exist("$game_levels$",N4)	
			)
		{
			sLevelInfo			LI;
			LI.folder			= xr_strdup((*folder)[i]);
			LI.name				= 0;
			Levels.push_back	(LI);
		} else {
			Msg		("! Level not compiled: %s",(*folder)[i]);
		}
	}
	FS.file_list_close	(folder);
}

void CApplication::Level_Set(u32 L)
{
	if (L>=Levels.size()) return;
	Level_Current = L;
	FS.get_path	("$level$")->_set	(Levels[L].folder);
}

int CApplication::Level_ID(LPCSTR name)
{
	char buffer	[256];
	strconcat	(buffer,name,"\\");
	for (u32 I=0; I<Levels.size(); I++)
	{
		if (0==stricmp(buffer,Levels[I].folder))	return int(I);
	}
	return -1;
}
