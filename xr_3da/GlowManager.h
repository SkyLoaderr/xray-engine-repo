// GlowManager.h: interface for the CGlowManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOWMANAGER_H__EC35911F_479B_469A_845C_1A64D81D0326__INCLUDED_)
#define AFX_GLOWMANAGER_H__EC35911F_479B_469A_845C_1A64D81D0326__INCLUDED_
#pragma once

#include "xr_collide_defs.h"

class CGlow  
{
public:
	Fvector	C;
	float	R;
	float	fade;
	Shader*	hShader;
	DWORD	dwFrame;

	// Ray-testing cache
	BOOL				bTestResult;
	Collide::ray_cache	RayCache;
};

#define MAX_GlowsPerFrame	512

class ENGINE_API CGlowManager {
	vector<CGlow>		Glows;
	CGlow*				Selected[MAX_GlowsPerFrame];
	DWORD				Selected_Count;
	CVertexStream*		Stream;

	DWORD				dwTestID;
public:
	void	add			(vector<WORD> &V);

	void	Load		(CStream *fs);
	void	Unload		();

	void	Render		();

	CGlowManager		();
	~CGlowManager		();
};

#endif // !defined(AFX_GLOWMANAGER_H__EC35911F_479B_469A_845C_1A64D81D0326__INCLUDED_)
