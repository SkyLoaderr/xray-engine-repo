#include "stdafx.h"
#include "x_ray.h"
#include "xr_creator.h"
#include "std_classes.h"
#include "customHUD.h"
#include "xr_ini.h"
#include "fstaticrender.h"
#include "xr_sndman.h"
#include "xr_smallfont.h"
#include "xrLevel.h"

CCreator::CCreator( ) 
{
	pCreator					= this;
	pLevel						= NULL;
	bReady						= false;
	pCurrentEntity				= NULL;
	LL_Stream					= NULL;
	Sounds_Ambience				= -1;

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

	if (Sounds_Ambience>=0)		pSounds->Delete2D(Sounds_Ambience);

	// Render-level unload
	Render.OnDeviceDestroy		( );
	Device.seqDevDestroy.Remove	(&Render);
	Device.seqDevCreate.Remove	(&Render);

	// Unload sounds
	for (DWORD i=0; i<Sounds.size(); i++)
		pSounds->Delete3D(Sounds[i]);
	Sounds.clear();

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
//	Device.Reset				();

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

	// Render-level Load
	Device.seqDevDestroy.Add	(&Render);
	Device.seqDevCreate.Add		(&Render);
	Render.OnDeviceCreate		();
	
	// Objects
	pApp->LoadTitle				("Loading entities...");
	R_ASSERT					(Load_GameSpecific_Before());
	Objects.Load				(pLevel, "mobileobjects" );
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
			Sounds.push_back	(sound3D());
			pSounds->Create3D	(Sounds.back(),fname);
			pSounds->Play3DAtPos(Sounds.back(),pos,true);
		}
		if (pLevel->LineExists("static_sounds","ambient"))
		{
			LPCSTR fname		= pLevel->ReadSTRING("static_sounds","ambient");
			Sounds_Ambience		= pSounds->Create2D(fname);
			pSounds->Play2D		(Sounds_Ambience,true);
		}
	}
	{
		if (pLevel->SectionExists("random_sounds"))	
		{
			CInifile::Sect& S		= pLevel->ReadSection("random_sounds");
			Sounds_Random.reserve	(S.size());
			for (CInifile::SectIt I=S.begin(); I!=S.end(); I++) {
				Sounds_Random.push_back	(sound3D());
				pSounds->Create3D		(Sounds_Random.back(),I->second);
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
	if (fabsf(Device.fTimeDelta)<EPS_S) return;

	// Environment render
	Environment.RenderFirst		();

	// Level render
	Render.Calculate			();
	Render.Render				();

	// Environment render
	Environment.RenderLast		();

	// Font
	pApp->pFont->OnRender		();
}

void CCreator::OnFrame	( void ) 
{
	mmgrMessage					("*** Frame");
	if (fabsf(Device.fTimeDelta)<EPS_S) return;

	// Update all objects
	VERIFY						(bReady);
	Environment.OnMove			( );
	Objects.OnMove				( );
	pHUD->OnMove				( );

	// Ambience
	if (Sounds_Random.size() && (Device.dwTimeGlobal > Sounds_dwNextTime))
	{
		Sounds_dwNextTime		= Device.dwTimeGlobal + ::Random.randI	(200,5000);
		Fvector	pos;
		pos.random_dir			();
		pos.normalize			();
		pos.mul					(::Random.randF(10,40));
		pos.add					(Device.vCameraPosition);
		pSounds->Play3DAtPos	(Sounds_Random[::Random.randI(Sounds_Random.size())],pos,false);
	}
}
