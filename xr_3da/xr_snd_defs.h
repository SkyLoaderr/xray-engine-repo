#pragma once

const	int SND_UNDEFINED = -1;

// refs
class	ENGINE_API	CCDA;
class	ENGINE_API	C3DSound;
class	ENGINE_API	C3DSoundRender;
class	ENGINE_API	C2DSound;
class	ENGINE_API	C2DSoundRender;
class	ENGINE_API	CMusicStream;
class	ENGINE_API	CInifile;
struct	ENGINE_API	xr_token;

struct	sound3D
{
	int			handle;
	C3DSound*	feedback;
	sound3D()	{ handle = SND_UNDEFINED; feedback=0; }
};
struct	sound2D
{
	int			handle;
	C2DSound*	feedback;
	sound2D()	{ handle = SND_UNDEFINED; feedback=0; }
};

