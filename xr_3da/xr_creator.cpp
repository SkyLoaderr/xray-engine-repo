#include "stdafx.h"
#include "x_ray.h"
#include "xr_creator.h"
#include "std_classes.h"
#include "customHUD.h"
#include "xr_ini.h"
#include "fstaticrender.h"
#include "xr_sndman.h"
#include "xr_smallfont.h"

CCreator::CCreator( ) 
{
	pCreator					= this;
	pLevel						= NULL;
	bReady						= false;
	pCurrentEntity				= NULL;
	LL_Stream					= NULL;

	pHUD						= (CCustomHUD*)NEW_INSTANCE(CLSID_HUDMANAGER);
}

CCreator::~CCreator	( ) 
{
	delete						LL_Stream;
	iRelease					( );

	// Destroy all objects
	Objects.Unload				( );

	DEL_INSTANCE				( pHUD );

	bReady						= false;
	_DELETE						( pLevel );

	// Render-level unload
	Render.level_Unload			( );
	Device.seqDevDestroy.Remove	(&Render);
	Device.seqDevCreate.Remove	(&Render);

	// Unload sounds
	for (DWORD i=0; i<Sounds.size(); i++)
		pSounds->Delete3D(Sounds[i]);
	Sounds.clear();
	pSounds->UnloadEnvironment();

	// Unregister
	Device.seqRender.Remove(this);
	Device.seqFrame.Remove(this);
}

BOOL CCreator::net_Server		( LPCSTR name_of_level	)
{
	return FALSE;
}
BOOL CCreator::net_Client		( LPCSTR name_of_server )
{
	return FALSE;
}
void CCreator::net_Disconnect	( )
{
}

//-------------------------------------------------------------------------------------------
BOOL CCreator::Load(DWORD dwNum) 
{
	// Initialize level data
	pApp->Level_Set				( dwNum );
	FILE_NAME					temp;
	if (!Engine.FS.Exist(temp, Path.Current, "level.ltx"))
		Device.Fatal("Can't find level configuration file '%s'.",temp);

	pLevel						= new CInifile	( temp );

	// Read Environment
	FILE_NAME					LDesc;
	strconcat					(LDesc,Path.Current,"level.");
	
	pApp->LoadTitle				("Opening virtual stream: ",LDesc);
	LL_Stream					= new CVirtualFileStream (LDesc);
	CStream	&fs					= *LL_Stream;
	
	CStream *chunk = 0;
	DWORD	count,i;

	Environment.Load			(pLevel, "environment");
	
	// Header
	hdrLEVEL H;
	fs.ReadChunk(fsL_HEADER,&H);
	R_ASSERT(XRCL_PRODUCTION_VERSION==H.XRLC_version);
	pApp->LoadTitle("Description string: ",H.name);
	
	// Textures
	chunk = fs.OpenChunk(fsL_TEXTURES);
	count = chunk->Rdword();
	LL_textures.resize(count);
	for(i=0; i<count; i++) {
		LL_textures[i] = (char *) chunk->Pointer();
		chunk->SkipStringZ();
	}
	chunk->Close();
	
	// Shaders
	chunk = fs.OpenChunk(fsL_SHADERS);
	count = chunk->Rdword();
	LL_shaders.resize(count);
	for(i=0; i<count; i++) {
		LL_shaders[i] = (char *) chunk->Pointer();
		chunk->SkipStringZ();
	}
	chunk->Close();
	
	// CForms
	pApp->LoadTitle("Loading CFORM...");
	chunk = fs.OpenChunk(fsL_CFORM);
	ObjectSpace.Load(chunk);
	chunk->Close();

	// Render-level Load
	Render.level_Load();
	Device.seqDevDestroy.Add	(&Render);
	Device.seqDevCreate.Add		(&Render);

	// AI space
	pApp->LoadTitle("Loading AI space...");
	AI.Load(Path.Current);
	
	// Objects
	pApp->LoadTitle	("Loading entities...");
	R_ASSERT(Load_GameSpecific_Before());
	Objects.Load	(pLevel, "mobileobjects" );
	R_ASSERT(Load_GameSpecific_After ());
	
	// Load Sounds
	pApp->LoadTitle("Loading sound and music...");
	{
		CInifile::Sect& S = pLevel->ReadSection("static_sounds");
		Sounds.reserve	(S.size());
		for (CInifile::SectIt I=S.begin(); I!=S.end(); I++) {
			Fvector		pos;
			sscanf		( I->second,"%f,%f,%f",&pos.x,&pos.y,&pos.z);
			Sounds.push_back	(sound3D());
			pSounds->Create3D	(Sounds.back(),I->first);
			pSounds->Play3DAtPos(Sounds.back(),pos,true);
		}
		pSounds->LoadEnvironment(pLevel,"sound_environment");
	}
	Environment.Load_Music(pLevel);

	// Done
	pApp->LoadTitle				("Syncronizing...");
	bReady						= true;
	iCapture					( );
	Device.seqRender.Add		(this);
	Device.seqFrame.Add			(this);

	return TRUE;
}

Shader* CCreator::LL_CreateShader(DWORD S, DWORD T)
{
	R_ASSERT(S<LL_shaders.size() );
	R_ASSERT(T<LL_textures.size());
	return Device.Shader.Create(LL_shaders[S],LL_textures[T]);
}

void CCreator::OnRender() 
{
	if (fabsf(Device.fTimeDelta)<EPS_S) return;

	// Environment render
	Environment.RenderFirst		();

	// Level render
	Render.Calculate			();
	Render.Render				();

	if (bDebug) {
		ObjectSpace.dbgRender	();
		AI.Render				();
	}

	// Environment render
	Environment.RenderLast		();

	// Font
	pApp->pFont->OnRender		();
}

void CCreator::OnFrame	( void ) 
{
	mmgrMessage("*** Frame");
	if (fabsf(Device.fTimeDelta)<EPS_S) return;

	// Verify all objects
	VERIFY						(bReady);
	Environment.OnMove			( );
	Objects.OnMove				( );
	pHUD->OnMove				( );
}

