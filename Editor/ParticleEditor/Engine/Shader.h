// Shader.h: interface for the CShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SHADER__INCLUDED_)
#define SHADER__INCLUDED_
#pragma once

class	ENGINE_API	CBlender;
class	ENGINE_API	CTexture;
class	ENGINE_API	CMatrix;
class	ENGINE_API	CConstant;
class	ENGINE_API	CRT;
class	ENGINE_API	CVS;
class	ENGINE_API	CPS;

#pragma warning(disable : 4275)
struct	ENGINE_API	STextureList	: public svector<CTexture*,8>
{
	DWORD	dwReference;
};
struct	ENGINE_API	SMatrixList		: public svector<CMatrix*,8>
{
	DWORD	dwReference;
};
struct	ENGINE_API	SConstantList	: public svector<CConstant*,8>
{
	DWORD	dwReference;
};
#pragma warning(default : 4275)

typedef svector<string64,8>			sh_list;

struct	ENGINE_API		CPass 
{
	DWORD				dwStateBlock;
	STextureList*		T;
	SMatrixList*		M;
	SConstantList*		C;

	IC BOOL				equal	(CPass& P)	
	{
		if (dwStateBlock != P.dwStateBlock)	return FALSE;
		if (T != P.T)						return FALSE;
		if (M != P.M)						return FALSE;
		if (C != P.C)						return FALSE;
		return TRUE;
	}
};

struct ENGINE_API		ShaderElement
{
public:
	struct SFlags
	{
		DWORD	iPriority	:	2;
		DWORD	bStrictB2F	:	1;
		DWORD	bLighting	:	1;
		DWORD	bPixelShader:	1;
	};
public:
	Shader()	{
		ZeroMemory			(this,sizeof(*this));
		Flags.iPriority		= 1;
		Flags.bStrictB2F	= FALSE;
		Flags.bLighting		= FALSE;
		Flags.bPixelShader	= FALSE;
	}
public:
	DWORD					dwReference;
	SFlags					Flags;
	svector<CPass,8>		Passes;
	
	IC BOOL					equal	(Shader& S)
	{
		if (Flags.iPriority != S.Flags.iPriority)	return FALSE;
		if (Flags.bStrictB2F != S.Flags.bStrictB2F)	return FALSE;
		if (Flags.bLighting	!= S.Flags.bLighting)	return FALSE;
		if (Passes.size() != S.Passes.size())		return FALSE;
		for (DWORD p=0; p<Passes.size(); p++)
			if (!Passes[p].equal(S.Passes[p]))		return FALSE;
		return TRUE;
	}
};

struct ENGINE_API		Shader 
{
public:
	ShaderElement*		lod0;
	ShaderElement*		lod1;
	ShaderElement*		lighting;
};

#endif // !defined(AFX_SHADER_H__9CBD70DD_E147_446B_B4EE_5DA321EB726F__INCLUDED_)
