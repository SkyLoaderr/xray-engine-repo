#include "stdafx.h"
#pragma hdrstop

#include "soundrender.h"
#include "soundrender_environment.h"

CSoundRender_Environment::CSoundRender_Environment(void)
{
	version			= sdef_env_version;
	set_default		();
}

CSoundRender_Environment::~CSoundRender_Environment(void)
{
}

void CSoundRender_Environment::set_default	()
{
    Room                    = EAXLISTENER_DEFAULTROOM;
    RoomHF                  = EAXLISTENER_DEFAULTROOMHF;
    RoomRolloffFactor       = EAXLISTENER_DEFAULTROOMROLLOFFFACTOR;
    DecayTime               = EAXLISTENER_DEFAULTDECAYTIME;
    DecayHFRatio            = EAXLISTENER_DEFAULTDECAYHFRATIO;
    Reflections             = EAXLISTENER_DEFAULTREFLECTIONS;
    ReflectionsDelay        = EAXLISTENER_DEFAULTREFLECTIONSDELAY;
    Reverb                  = EAXLISTENER_DEFAULTREVERB;
    ReverbDelay             = EAXLISTENER_DEFAULTREVERBDELAY;
    EnvironmentSize         = EAXLISTENER_DEFAULTENVIRONMENTSIZE;
    EnvironmentDiffusion    = EAXLISTENER_DEFAULTENVIRONMENTDIFFUSION;
    AirAbsorptionHF         = EAXLISTENER_DEFAULTAIRABSORPTIONHF;
}

void CSoundRender_Environment::set_identity	()
{
	set_default				();
    Room                    = EAXLISTENER_MINROOM;
	clamp				  	();
}

void CSoundRender_Environment::lerp			(CSoundRender_Environment& A, CSoundRender_Environment& B, float f)
{
	float	fi				= 1.f-f;

    Room                    = fi*A.Room                	+ f*B.Room;                
    RoomHF                  = fi*A.RoomHF              	+ f*B.RoomHF;              
    RoomRolloffFactor       = fi*A.RoomRolloffFactor   	+ f*B.RoomRolloffFactor;
    DecayTime               = fi*A.DecayTime           	+ f*B.DecayTime;           
    DecayHFRatio            = fi*A.DecayHFRatio        	+ f*B.DecayHFRatio;        
    Reflections             = fi*A.Reflections         	+ f*B.Reflections;         
    ReflectionsDelay        = fi*A.ReflectionsDelay    	+ f*B.ReflectionsDelay;    
    Reverb                  = fi*A.Reverb              	+ f*B.Reverb;              
    ReverbDelay             = fi*A.ReverbDelay         	+ f*B.ReverbDelay;         
    EnvironmentSize         = fi*A.EnvironmentSize     	+ f*B.EnvironmentSize;     
    EnvironmentDiffusion    = fi*A.EnvironmentDiffusion	+ f*B.EnvironmentDiffusion;
    AirAbsorptionHF         = fi*A.AirAbsorptionHF     	+ f*B.AirAbsorptionHF;     

	clamp					();
}

void CSoundRender_Environment::clamp		()
{
    ::clamp(Room,             		(float)EAXLISTENER_MINROOM, 	  	(float)EAXLISTENER_MAXROOM			);
    ::clamp(RoomHF,              	(float)EAXLISTENER_MINROOMHF, 	  	(float)EAXLISTENER_MAXROOMHF		);
    ::clamp(RoomRolloffFactor,   	EAXLISTENER_MINROOMROLLOFFFACTOR, 	EAXLISTENER_MAXROOMROLLOFFFACTOR	);
    ::clamp(DecayTime,           	EAXLISTENER_MINDECAYTIME, 			EAXLISTENER_MAXDECAYTIME			);
    ::clamp(DecayHFRatio,        	EAXLISTENER_MINDECAYHFRATIO, 		EAXLISTENER_MAXDECAYHFRATIO			);
    ::clamp(Reflections,         	(float)EAXLISTENER_MINREFLECTIONS,	(float)EAXLISTENER_MAXREFLECTIONS	);
    ::clamp(ReflectionsDelay,    	EAXLISTENER_MINREFLECTIONSDELAY, 	EAXLISTENER_MAXREFLECTIONSDELAY		);
    ::clamp(Reverb,              	(float)EAXLISTENER_MINREVERB, 	  	(float)EAXLISTENER_MAXREVERB		);
    ::clamp(ReverbDelay,         	EAXLISTENER_MINREVERBDELAY, 		EAXLISTENER_MAXREVERBDELAY			);
    ::clamp(EnvironmentSize,     	EAXLISTENER_MINENVIRONMENTSIZE, 	EAXLISTENER_MAXENVIRONMENTSIZE		);
    ::clamp(EnvironmentDiffusion,	EAXLISTENER_MINENVIRONMENTDIFFUSION,EAXLISTENER_MAXENVIRONMENTDIFFUSION	);
    ::clamp(AirAbsorptionHF,     	EAXLISTENER_MINAIRABSORPTIONHF, 	EAXLISTENER_MAXAIRABSORPTIONHF		);
}

void CSoundRender_Environment::load			(IReader* fs)
{
	version							= fs->r_u32();

	if (sdef_env_version == version){
        fs->r_stringZ			    (name);

        Room                		= fs->r_float();
        RoomHF              		= fs->r_float();
        RoomRolloffFactor   		= fs->r_float();
        DecayTime           		= fs->r_float();
        DecayHFRatio        		= fs->r_float();
        Reflections         		= fs->r_float();
        ReflectionsDelay    		= fs->r_float();
        Reverb              		= fs->r_float();
        ReverbDelay         		= fs->r_float();
        EnvironmentSize     		= fs->r_float();
        EnvironmentDiffusion		= fs->r_float();
        AirAbsorptionHF     		= fs->r_float();
    }
}

void CSoundRender_Environment::save	(IWriter* fs)
{
	fs->w_u32 	                    (sdef_env_version);
	fs->w_stringZ                   (name);

    fs->w_float	                    (Room                );
    fs->w_float	                    (RoomHF              );
    fs->w_float	                    (RoomRolloffFactor   );
    fs->w_float	                    (DecayTime           );
    fs->w_float	                    (DecayHFRatio        );
    fs->w_float	                    (Reflections         );
    fs->w_float	                    (ReflectionsDelay    );
    fs->w_float	                    (Reverb              );
    fs->w_float	                    (ReverbDelay         );
    fs->w_float	                    (EnvironmentSize     );
    fs->w_float	                    (EnvironmentDiffusion);
    fs->w_float	                    (AirAbsorptionHF     );
}

//////////////////////////////////////////////////////////////////////////
void	SoundEnvironment_LIB::Load	(LPCSTR name)
{
	R_ASSERT			(library.empty());
	IReader* F			= FS.r_open(name);
	IReader* C;
	library.reserve		(256);
	for (u32 chunk=0; 0!=(C=F->open_chunk(chunk)); chunk++)
	{
		CSoundRender_Environment*	E	= xr_new<CSoundRender_Environment>	();
		E->load							(C);
		library.push_back				(E);
        C->close		();
	}
	FS.r_close			(F);
}
void	SoundEnvironment_LIB::Save	(LPCSTR name)
{
	IWriter* F			= FS.w_open(name);
	for (u32 chunk=0; chunk<library.size(); chunk++)
	{
		F->open_chunk		(chunk);
		library[chunk]->save(F);
		F->close_chunk		();
	}
	FS.w_close			(F);
}
void	SoundEnvironment_LIB::Unload	()
{
	for (u32 chunk=0; chunk<library.size(); chunk++)
		xr_delete(library[chunk]);
	library.clear		();
}
int		SoundEnvironment_LIB::GetID		(LPCSTR name)
{
	for (SE_IT it=library.begin(); it!=library.end(); it++)
		if (0==stricmp(name,*(*it)->name)) return int(it-library.begin());
	return -1;
}
CSoundRender_Environment*	SoundEnvironment_LIB::Get		(LPCSTR name)
{
	for (SE_IT it=library.begin(); it!=library.end(); it++)
		if (0==stricmp(name,*(*it)->name)) return *it;
	return NULL;
}
CSoundRender_Environment*	SoundEnvironment_LIB::Get		(int id)
{
	return library[id];
}
CSoundRender_Environment*	SoundEnvironment_LIB::Append	(CSoundRender_Environment* parent)
{
	library.push_back	(parent?xr_new<CSoundRender_Environment>(*parent):xr_new<CSoundRender_Environment>());
	return library.back	();
}
void						SoundEnvironment_LIB::Remove	(LPCSTR name)
{
	for (SE_IT it=library.begin(); it!=library.end(); it++)
		if (0==stricmp(name,*(*it)->name))
		{
			xr_delete		(*it);
			library.erase	(it);
			break;
		}
}
void						SoundEnvironment_LIB::Remove	(int id)
{
	xr_delete		(library[id]);
	library.erase	(library.begin()+id);
}
SoundEnvironment_LIB::SE_VEC& SoundEnvironment_LIB::Library	()	
{ 
	return library;
}
