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

void CSoundRender_Environment::set_default	(bool R, bool E, bool D)
{
	if (R){
		R_InGain   	=	DSFX_WAVESREVERB_INGAIN_DEFAULT;
		R_Mix	   	=	DSFX_WAVESREVERB_REVERBMIX_DEFAULT;
		R_Time	   	=	DSFX_WAVESREVERB_REVERBTIME_DEFAULT;
		R_HFRatio  	=	DSFX_WAVESREVERB_HIGHFREQRTRATIO_DEFAULT;
    }

    if (E){
		E_WetDry   	=	50;
		E_FeedBack 	=	50;
		E_Delay	   	=	500;
    }

    if (D){
		D_Gain	   	=	-18;
		D_Edge	   	=	15;
		D_CenterF  	=	2400;
		D_BandWidth	=	2400;
		D_Cutoff   	=	8000;
    }
}

void CSoundRender_Environment::set_identity	(bool R, bool E, bool D)
{
	set_default		(R,E,D);
	if (R)
	{
		R_InGain	= 0;	
		R_Mix		= -96;
	}
	if (E)
	{
		E_WetDry	= 0;
		E_FeedBack	= 0;
	}
	if (D)
	{
		D_Gain		= 0;
		D_Edge		= 0;
		D_CenterF	= 8*1000;
		D_BandWidth	= 8*1000;
		D_Cutoff	= 8*1000;
	}
	clamp			(R,E,D);
}

void CSoundRender_Environment::lerp			(CSoundRender_Environment& A, CSoundRender_Environment& B, float f)
{
	float	fi	= 1.f-f;

	R_InGain	= fi*A.R_InGain		+	f*B.R_InGain;
	R_Mix		= fi*A.R_Mix		+	f*B.R_Mix;
	R_Time		= fi*A.R_Time		+	f*B.R_Time;
	R_HFRatio	= fi*A.R_HFRatio	+	f*B.R_HFRatio;

	E_WetDry	= fi*A.E_WetDry		+	f*B.E_WetDry;
	E_FeedBack	= fi*A.E_FeedBack	+	f*B.E_FeedBack;
	E_Delay		= fi*A.E_Delay		+	f*B.E_Delay;

	D_Gain		= fi*A.D_Gain		+	f*B.D_Gain;
	D_Edge		= fi*A.D_Edge		+	f*B.D_Edge;
	D_CenterF	= fi*A.D_CenterF	+	f*B.D_CenterF; 
	D_BandWidth	= fi*A.D_BandWidth	+	f*B.D_BandWidth;
	D_Cutoff	= fi*A.D_Cutoff		+	f*B.D_Cutoff;
	clamp		(true,true,true);
}

void CSoundRender_Environment::clamp		(bool R, bool E, bool D)
{
	if (R)
	{
		::clamp(R_InGain,		DSFX_WAVESREVERB_INGAIN_MIN,			DSFX_WAVESREVERB_INGAIN_MAX);
		::clamp(R_Mix,			DSFX_WAVESREVERB_REVERBMIX_MIN,			DSFX_WAVESREVERB_REVERBMIX_MAX);
		::clamp(R_Time,			DSFX_WAVESREVERB_REVERBTIME_MIN,		DSFX_WAVESREVERB_REVERBTIME_MAX);
		::clamp(R_HFRatio,		DSFX_WAVESREVERB_HIGHFREQRTRATIO_MIN,	DSFX_WAVESREVERB_HIGHFREQRTRATIO_MAX);
	}
	if (E)
	{
		::clamp(E_WetDry,		DSFXECHO_WETDRYMIX_MIN,					DSFXECHO_WETDRYMIX_MAX);
		::clamp(E_FeedBack,		DSFXECHO_FEEDBACK_MIN,					DSFXECHO_FEEDBACK_MAX);
		::clamp(E_Delay,		DSFXECHO_LEFTDELAY_MIN,					DSFXECHO_LEFTDELAY_MAX);	
	}
	if (D)
	{
		::clamp(D_Gain,			-60.f,		0.f);
		::clamp(D_Edge,			0.f,		100.f);
		::clamp(D_CenterF,		100.f,		8000.f);
		::clamp(D_BandWidth,	100.f,		8000.f);
		::clamp(D_Cutoff,		100.f,		8000.f);
	}
}

void CSoundRender_Environment::load			(IReader* fs)
{
	version			= fs->r_u32		();
	R_ASSERT		(sdef_env_version	== version);

	fs->r_stringZ	(name);
	R_InGain		= fs->r_float();
	R_Mix			= fs->r_float();
	R_Time			= fs->r_float();
	R_HFRatio		= fs->r_float();

	E_WetDry		= fs->r_float();
	E_FeedBack		= fs->r_float();
	E_Delay			= fs->r_float();

	D_Gain			= fs->r_float();
	D_Edge			= fs->r_float();
	D_CenterF		= fs->r_float();
	D_BandWidth		= fs->r_float();
	D_Cutoff		= fs->r_float();
}

void CSoundRender_Environment::save			(IWriter* fs)
{
	fs->w_u32		(sdef_env_version);
	fs->w_stringZ	(name);

	fs->w_float		(R_InGain	);
	fs->w_float		(R_Mix		);
	fs->w_float		(R_Time		);
	fs->w_float		(R_HFRatio	);

	fs->w_float		(E_WetDry	);
	fs->w_float		(E_FeedBack	);
	fs->w_float		(E_Delay	);

	fs->w_float		(D_Gain		);
	fs->w_float		(D_Edge		);
	fs->w_float		(D_CenterF	);
	fs->w_float		(D_BandWidth);
	fs->w_float		(D_Cutoff	);
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
