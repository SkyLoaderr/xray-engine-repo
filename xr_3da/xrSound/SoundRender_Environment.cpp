#include "stdafx.h"
#pragma hdrstop

#include "soundrender.h"
#include "soundrender_environment.h"

CSoundRender_Environment::CSoundRender_Environment(void)
{
	version			= sdef_env_version;
	strcpy			(name,"");
	set_default		(true,true,true);
}

CSoundRender_Environment::~CSoundRender_Environment(void)
{
}

void CSoundRender_Environment::set_default	(bool R, bool E, bool L)
{
	if (L){
        L_Room			   	= DSFX_I3DL2REVERB_ROOM_DEFAULT;
        L_RoomHF		   	= DSFX_I3DL2REVERB_ROOMHF_DEFAULT;
        L_RoomRolloffFactor	= DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_DEFAULT;
        L_DecayTime			= DSFX_I3DL2REVERB_DECAYTIME_DEFAULT;
        L_DecayHFRatio		= DSFX_I3DL2REVERB_DECAYHFRATIO_DEFAULT;
        L_Reflections	   	= DSFX_I3DL2REVERB_REFLECTIONS_DEFAULT;
        L_ReflectionsDelay	= DSFX_I3DL2REVERB_REFLECTIONSDELAY_DEFAULT;
        L_Reverb		   	= DSFX_I3DL2REVERB_REVERB_DEFAULT;
        L_ReverbDelay	   	= DSFX_I3DL2REVERB_REVERBDELAY_DEFAULT;
        L_Diffusion			= DSFX_I3DL2REVERB_DIFFUSION_DEFAULT;
        L_Density		   	= DSFX_I3DL2REVERB_DENSITY_DEFAULT;
        L_HFReference	   	= DSFX_I3DL2REVERB_HFREFERENCE_DEFAULT;
    }

	if (R){
		R_InGain   	        = DSFX_WAVESREVERB_INGAIN_DEFAULT;
		R_Mix	   	        = DSFX_WAVESREVERB_REVERBMIX_DEFAULT;
		R_Time	   	        = DSFX_WAVESREVERB_REVERBTIME_DEFAULT;
		R_HFRatio  	        = DSFX_WAVESREVERB_HIGHFREQRTRATIO_DEFAULT;
    }

    if (E){
		E_WetDry   	        = 50;
		E_FeedBack 	        = 50;
		E_Delay	   	        = 500;
    }
}

void CSoundRender_Environment::set_identity	(bool R, bool E, bool L)
{
	set_default				(R,E,L);
	if (L)
	{
        L_Room				= DSFX_I3DL2REVERB_ROOM_DEFAULT;
        L_RoomHF			= DSFX_I3DL2REVERB_ROOMHF_DEFAULT;
        L_RoomRolloffFactor	= DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_DEFAULT;
        L_DecayTime			= DSFX_I3DL2REVERB_DECAYTIME_DEFAULT;
        L_DecayHFRatio		= DSFX_I3DL2REVERB_DECAYHFRATIO_DEFAULT;
        L_Reflections		= DSFX_I3DL2REVERB_REFLECTIONS_DEFAULT;
        L_ReflectionsDelay	= DSFX_I3DL2REVERB_REFLECTIONSDELAY_DEFAULT;
        L_Reverb			= DSFX_I3DL2REVERB_REVERB_DEFAULT;
        L_ReverbDelay		= DSFX_I3DL2REVERB_REVERBDELAY_DEFAULT;
        L_Diffusion			= DSFX_I3DL2REVERB_DIFFUSION_DEFAULT;
        L_Density			= DSFX_I3DL2REVERB_DENSITY_DEFAULT;
        L_HFReference		= DSFX_I3DL2REVERB_HFREFERENCE_DEFAULT;
	}
	if (R)
	{
		R_InGain		  	= 0;	
		R_Mix			  	= -96;
	}
	if (E)
	{
		E_WetDry		  	= 0;
		E_FeedBack		  	= 0;
	}
	clamp				  	(R,E,L);
}

void CSoundRender_Environment::lerp			(CSoundRender_Environment& A, CSoundRender_Environment& B, float f)
{
	float	fi				= 1.f-f;

    L_Room					= fi*A.L_Room				+ f*B.L_Room;
    L_RoomHF				= fi*A.L_RoomHF			 	+ f*B.L_RoomHF;
    L_RoomRolloffFactor		= fi*A.L_RoomRolloffFactor 	+ f*B.L_RoomRolloffFactor;
    L_DecayTime				= fi*A.L_DecayTime		 	+ f*B.L_DecayTime;
    L_DecayHFRatio			= fi*A.L_DecayHFRatio	 	+ f*B.L_DecayHFRatio;
    L_Reflections			= fi*A.L_Reflections		+ f*B.L_Reflections;
    L_ReflectionsDelay		= fi*A.L_ReflectionsDelay 	+ f*B.L_ReflectionsDelay;
    L_Reverb				= fi*A.L_Reverb			 	+ f*B.L_Reverb;
    L_ReverbDelay			= fi*A.L_ReverbDelay		+ f*B.L_ReverbDelay;
    L_Diffusion				= fi*A.L_Diffusion		 	+ f*B.L_Diffusion;
    L_Density				= fi*A.L_Density			+ f*B.L_Density;
    L_HFReference			= fi*A.L_HFReference		+ f*B.L_HFReference;

	R_InGain	            = fi*A.R_InGain		        + f*B.R_InGain;
	R_Mix		            = fi*A.R_Mix		        + f*B.R_Mix;
	R_Time		            = fi*A.R_Time		        + f*B.R_Time;
	R_HFRatio	            = fi*A.R_HFRatio	        + f*B.R_HFRatio;

	E_WetDry	            = fi*A.E_WetDry		        + f*B.E_WetDry;
	E_FeedBack	            = fi*A.E_FeedBack	        + f*B.E_FeedBack;
	E_Delay		            = fi*A.E_Delay		        + f*B.E_Delay;

	clamp					(true,true,true);
}

void CSoundRender_Environment::clamp		(bool R, bool E, bool L)
{
	if (L)
    {
		::clamp(L_Room,				(float)DSFX_I3DL2REVERB_ROOM_MIN, 		(float)DSFX_I3DL2REVERB_ROOM_MAX);
		::clamp(L_RoomHF,			(float)DSFX_I3DL2REVERB_ROOMHF_MIN, 	(float)DSFX_I3DL2REVERB_ROOMHF_MAX);
		::clamp(L_RoomRolloffFactor,DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_MIN, DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_MAX);
		::clamp(L_DecayTime,		DSFX_I3DL2REVERB_DECAYTIME_MIN, 		DSFX_I3DL2REVERB_DECAYTIME_MAX);
		::clamp(L_DecayHFRatio,		DSFX_I3DL2REVERB_DECAYHFRATIO_MIN, 		DSFX_I3DL2REVERB_DECAYHFRATIO_MAX);
		::clamp(L_Reflections,		(float)DSFX_I3DL2REVERB_REFLECTIONS_MIN,(float)DSFX_I3DL2REVERB_REFLECTIONS_MAX);
		::clamp(L_ReflectionsDelay,	DSFX_I3DL2REVERB_REFLECTIONSDELAY_MIN, 	DSFX_I3DL2REVERB_REFLECTIONSDELAY_MAX);
		::clamp(L_Reverb,			(float)DSFX_I3DL2REVERB_REVERB_MIN, 	(float)DSFX_I3DL2REVERB_REVERB_MAX);
		::clamp(L_ReverbDelay,		DSFX_I3DL2REVERB_REVERBDELAY_MIN, 		DSFX_I3DL2REVERB_REVERBDELAY_MAX);
		::clamp(L_Diffusion,		DSFX_I3DL2REVERB_DIFFUSION_MIN, 		DSFX_I3DL2REVERB_DIFFUSION_MAX);
		::clamp(L_Density,			DSFX_I3DL2REVERB_DENSITY_MIN, 			DSFX_I3DL2REVERB_DENSITY_MAX);
		::clamp(L_HFReference,		DSFX_I3DL2REVERB_HFREFERENCE_MIN, 		DSFX_I3DL2REVERB_HFREFERENCE_MAX);       
    }
	if (R)
	{
		::clamp(R_InGain,			DSFX_WAVESREVERB_INGAIN_MIN,			DSFX_WAVESREVERB_INGAIN_MAX);
		::clamp(R_Mix,				DSFX_WAVESREVERB_REVERBMIX_MIN,			DSFX_WAVESREVERB_REVERBMIX_MAX);
		::clamp(R_Time,				DSFX_WAVESREVERB_REVERBTIME_MIN,		DSFX_WAVESREVERB_REVERBTIME_MAX);
		::clamp(R_HFRatio,			DSFX_WAVESREVERB_HIGHFREQRTRATIO_MIN,	DSFX_WAVESREVERB_HIGHFREQRTRATIO_MAX);
	}
	if (E)
	{
		::clamp(E_WetDry,			DSFXECHO_WETDRYMIX_MIN,					DSFXECHO_WETDRYMIX_MAX);
		::clamp(E_FeedBack,			DSFXECHO_FEEDBACK_MIN,					DSFXECHO_FEEDBACK_MAX);
		::clamp(E_Delay,			DSFXECHO_LEFTDELAY_MIN,					DSFXECHO_LEFTDELAY_MAX);	
	}
}

void CSoundRender_Environment::load			(IReader* fs)
{
	version							= fs->r_u32();
    if (1 == version){
    	fs->r_stringZ				(name);
        R_InGain				    = fs->r_float();
        R_Mix					    = fs->r_float();
        R_Time					    = fs->r_float();
        R_HFRatio				    = fs->r_float();

        E_WetDry				    = fs->r_float();
        E_FeedBack				    = fs->r_float();
        E_Delay					    = fs->r_float();
    }else{
		R_ASSERT				    (sdef_env_version	== version);
        fs->r_stringZ			    (name);

        L_Room						= fs->r_float();
        L_RoomHF					= fs->r_float();
        L_RoomRolloffFactor			= fs->r_float();
        L_DecayTime					= fs->r_float();
        L_DecayHFRatio				= fs->r_float();
        L_Reflections				= fs->r_float();
        L_ReflectionsDelay			= fs->r_float();
        L_Reverb					= fs->r_float();
        L_ReverbDelay				= fs->r_float();
        L_Diffusion					= fs->r_float();
        L_Density					= fs->r_float();
        L_HFReference				= fs->r_float();

        R_InGain				    = fs->r_float();
        R_Mix					    = fs->r_float();
        R_Time					    = fs->r_float();
        R_HFRatio				    = fs->r_float();

        E_WetDry				    = fs->r_float();
        E_FeedBack				    = fs->r_float();
        E_Delay					    = fs->r_float();
    }
}

void CSoundRender_Environment::save	(IWriter* fs)
{
	fs->w_u32 	                    (sdef_env_version);
	fs->w_stringZ                   (name);

    fs->w_float	                    (L_Room				);
    fs->w_float	                    (L_RoomHF			);
    fs->w_float	                    (L_RoomRolloffFactor);
    fs->w_float	                    (L_DecayTime		);
    fs->w_float	                    (L_DecayHFRatio		);
    fs->w_float	                    (L_Reflections		);
    fs->w_float	                    (L_ReflectionsDelay	);
    fs->w_float	                    (L_Reverb			);
    fs->w_float	                    (L_ReverbDelay		);
    fs->w_float	                    (L_Diffusion		);
    fs->w_float	                    (L_Density			);
    fs->w_float	                    (L_HFReference		);

	fs->w_float	                    (R_InGain			);
	fs->w_float                     (R_Mix				);
	fs->w_float                     (R_Time				);
	fs->w_float                     (R_HFRatio			);

	fs->w_float                     (E_WetDry			);
	fs->w_float                     (E_FeedBack			);
	fs->w_float                     (E_Delay			);
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
		if (0==stricmp(name,(*it)->name)) return int(it-library.begin());
	return -1;
}
CSoundRender_Environment*	SoundEnvironment_LIB::Get		(LPCSTR name)
{
	for (SE_IT it=library.begin(); it!=library.end(); it++)
		if (0==stricmp(name,(*it)->name)) return *it;
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
		if (0==stricmp(name,(*it)->name))
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
