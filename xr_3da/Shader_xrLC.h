// exxZERO Time Stamp AddIn. Document modified at : Monday, March 11, 2002 11:58:10 , by user : Oles , from computer : OLES
#pragma once

struct Shader_xrLC
{
public:
	struct Flags {
		DWORD bCollision	: 1;
		DWORD bOptimizeUV	: 1;
		DWORD bLIGHT_Vertex	: 1;
		DWORD bLIGHT_Sharp	: 1;
	};
public:
	char		Name		[128];
	Flags		flags;
	float		vert_translucency;
	float		vert_ambient;
	float		lm_density;

	Shader_xrLC()	{
		ZeroMemory	(this,sizeof(*this));
		strcpy		(Name,"unknown");
		flags.bCollision	= TRUE;
		flags.bOptimizeUV	= TRUE;
		flags.bLIGHT_Vertex	= FALSE;
		flags.bLIGHT_Sharp	= TRUE;
		vert_translucency	= .5f;
		vert_ambient		= .0f;
		lm_density			= 1.f;
	}
};
