// Shader.h: interface for the CShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SHADER__INCLUDED_)
#define SHADER__INCLUDED_
#pragma once

class	ENGINE_API	CBlender_Compile;
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

	BOOL				equal	(CPass& P);
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
	ShaderElement	();
public:
	DWORD					dwReference;
	SFlags					Flags;
	svector<CPass,8>		Passes;
	
	BOOL					equal	(ShaderElement& S);
	BOOL					equal	(ShaderElement* S);
};

struct ENGINE_API		Shader 
{
public:
	DWORD					dwReference;
	union {
		struct {
			ShaderElement*		lod0;
			ShaderElement*		lod1;
			ShaderElement*		lighting;
		};
		ShaderElement*		E	[3];
	};
	
	BOOL					equal	(Shader& S);
	BOOL					equal	(Shader* S);
};

#endif // !defined(AFX_SHADER_H__9CBD70DD_E147_446B_B4EE_5DA321EB726F__INCLUDED_)
