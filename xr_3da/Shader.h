// Shader.h: interface for the CShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SHADER__INCLUDED_)
#define SHADER__INCLUDED_
#pragma once

class	ENGINE_API	CTexture;
class	ENGINE_API	CMatrix;
class	ENGINE_API	CConstant;

typedef svector<CTexture*,8>	STextureList;
typedef svector<CMatrix*,8>		SMatrixList;
typedef svector<CConstant*,8>	SConstantList;

struct	ENGINE_API		CPass 
{
	DWORD				dwStateBlock;
	STextureList*		T;
	SMatrixList*		M;
	SConstantList*		C;
};

struct ENGINE_API		Shader
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
	SFlags				Flags;
	svector<CPass,8>	Passes;
};

#endif // !defined(AFX_SHADER_H__9CBD70DD_E147_446B_B4EE_5DA321EB726F__INCLUDED_)
