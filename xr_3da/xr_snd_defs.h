#pragma once

const	int SND_UNDEFINED = -1;

// refs
class	ENGINE_API	CCDA;
class	ENGINE_API	CSound;
class	ENGINE_API	CSoundRender;
class	ENGINE_API	CMusicStream;
class	ENGINE_API	CInifile;
class	ENGINE_API	CObject;
struct	ENGINE_API	xr_token;

// t-defs
struct	sound
{
	int			handle;
	CSound*		feedback;
	int			g_type;
	CObject*	g_object;
	sound()		{ handle = SND_UNDEFINED; feedback=0; g_type=0; g_object=0; }
};
struct	soundListener
{
	virtual		void	soundEvent	(CObject* who, int type, Fvector& Position, float power) {};
};

const DWORD soundEventPulse	= 500;	// ms

typedef Fvector soundOccluder[3];
