#include "stdafx.h"
#include "x_ray.h"
#include "xr_creator.h"
#include "std_classes.h"
#include "customHUD.h"
#include "xr_ini.h"
#include "render.h"
#include "xr_sndman.h"
#include "gamefont.h"
#include "xrLevel.h"

CCreator::CCreator( ) 
{
	pCreator					= this;
	pLevel						= NULL;
	bReady						= false;
	pCurrentEntity				= NULL;
	pCurrentViewEntity			= NULL;
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

	pSounds->Delete				(Sounds_Ambience);

	// Render-level unload
	Render->OnDeviceDestroy		( );
	Device.seqDevDestroy.Remove	(Render);
	Device.seqDevCreate.Remove	(Render);

	// Unload sounds
	for (u32 i=0; i<Sounds.size(); i++)
		pSounds->Delete	(Sounds[i]);
	Sounds.clear();

	// Unregister
	Device.seqRender.Remove		(this);
	Device.seqFrame.Remove		(this);
}

//-------------------------------------------------------------------------------------------
BOOL CCreator::Load(u32 dwNum) 
{
//	Device.Reset				();

	// Initialize level data
	pApp->Level_Set				( dwNum );
	FILE_NAME					temp;
	if (!Engine.FS.Exist(temp, Path.Current, "level.ltx"))
		Device.Fatal("Can't find level configuration file '%s'.",temp);

	pLevel						= new CInifile	( temp );

	// Read Environment
	FILE_NAME					LDesc;
	strconcat					(LDesc,Path.Current,"level");
	
	pApp->LoadTitle				("Opening virtual stream: ",LDesc);
	LL_Stream					= Engine.FS.Open	(LDesc);
	CStream	&fs					= *LL_Stream;
	
	CStream *chunk = 0;
	u32	count,i;

	Environment.Load			(pLevel, "environment");
	
	// Header
	hdrLEVEL H;
	fs.ReadChunkSafe			(fsL_HEADER,&H,sizeof(H));
	R_ASSERT2					(XRCL_PRODUCTION_VERSION==H.XRLC_version,"Incompatible level version.");
	pApp->LoadTitle				("Description string: ",H.name);
	
	// Textures
	chunk = fs.OpenChunk		(fsL_STRINGS);
	R_ASSERT2					(chunk,"Level doesn't builded correctly.");
	count = chunk->Rdword		();
	LL_strings.resize			(count);
	for(i=0; i<count; i++) {
		LL_strings[i] = LPSTR(chunk->Pointer());
		chunk->SkipStringZ();
	}
	chunk->Close();
	
	// CForms
	pApp->LoadTitle				("Loading CFORM...");
	chunk = fs.OpenChunk		(fsL_CFORM);
	ObjectSpace.Load			(chunk);
	chunk->Close				();
	pApp->LoadSwitch			();

	// Render-level Load
	Device.seqDevDestroy.Add	(Render);
	Device.seqDevCreate.Add		(Render);
	Render->OnDeviceCreate		();
	
	// Objects
	pApp->LoadTitle				("Loading entities...");
	R_ASSERT					(Load_GameSpecific_Before());
	// Objects.Load				(pLevel, "mobileobjects" );
	R_ASSERT					(Load_GameSpecific_After ());
	
	// Load Sounds
	pApp->LoadTitle				("Loading sound and music...");
	{
		CInifile::Sect& S = pLevel->ReadSection		("static_sounds");
		Sounds.reserve			(S.size());
		for (CInifile::SectIt I=S.begin(); I!=S.end(); I++) {
			Fvector				pos;
			string128			fname;
			sscanf				( I->second,"%[^,],%f,%f,%f",fname,&pos.x,&pos.y,&pos.z);
			if (0==stricmp(fname,"ambient"))	continue;
			Sounds.push_back	(sound());
			pSounds->Create		(Sounds.back(),TRUE,fname);
			pSounds->PlayAtPos	(Sounds.back(),0,pos,true);
		}
		if (pLevel->LineExists("static_sounds","ambient"))
		{
			LPCSTR fname		= pLevel->ReadSTRING("static_sounds","ambient");
			pSounds->Create		(Sounds_Ambience,FALSE,fname);
			pSounds->Play		(Sounds_Ambience,0,true);
		} 
	}
	{
		if (pLevel->SectionExists("random_sounds"))	
		{
			CInifile::Sect& S		= pLevel->ReadSection("random_sounds");
			Sounds_Random.reserve	(S.size());
			for (CInifile::SectIt I=S.begin(); I!=S.end(); I++) {
				Sounds_Random.push_back	(sound());
				pSounds->Create			(Sounds_Random.back(),TRUE,I->second);
			}
			Sounds_dwNextTime		= Device.TimerAsync	()	+ 5000;
		}
	}
	Environment.Load_Music		(pLevel);

	// Done
	pApp->LoadTitle				("Syncronizing...");
	bReady						= true;
	iCapture					( );
	Device.seqRender.Add		(this);
	Device.seqFrame.Add			(this);

	return TRUE;
}

Shader* CCreator::LL_CreateShader(int S, int T, int M, int C)
{
	return Device.Shader.Create(getString(S),getString(T),getString(M),getString(C));
}

void CCreator::OnRender() 
{
	if (_abs(Device.fTimeDelta)<EPS_S) return;

	// Level render
	Render->Calculate			();
	Render->Render				();

	// Font
	pApp->pFontSystem->OnRender	();
}

void CCreator::OnFrame	( void ) 
{
	if (_abs(Device.fTimeDelta)<EPS_S) return;

	// Update all objects
	VERIFY						(bReady);
	Engine.Sheduler.Update		( );
	Environment.OnMove			( );
	Objects.OnMove				( );
	pHUD->OnFrame				( );

	// Ambience
	if (Sounds_Random.size() && (Device.dwTimeGlobal > Sounds_dwNextTime))
	{
		Sounds_dwNextTime		= Device.dwTimeGlobal + ::Random.randI	(1000,10000);
		Fvector	pos;
		pos.random_dir			();
		pos.normalize			();
		pos.mul					(::Random.randF(10,50));
		pos.add					(Device.vCameraPosition);
		pSounds->PlayAtPos		(Sounds_Random[::Random.randI(Sounds_Random.size())],0,pos,false);
	}
}
