// Shader.h: interface for the CShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SHADER__INCLUDED_)
#define SHADER__INCLUDED_
#pragma once

#define	COMBINATIONS_SH		4
#define	COMBINATIONS_TEX	16

//-----------------------------------------------------------------------------------------------
class	ENGINE_API	CShader
{
protected:
	struct RFlags
	{
		DWORD	iPriority	:	2;
		DWORD	bStrictB2F	:	1;
		DWORD	bLighting	:	1;
	};
protected:
	virtual void				internal_Activate	()				= 0;
	virtual void				internal_Apply		(DWORD dwPass)	= 0;
	virtual void				internal_Release	()				= 0;
public:
	LPSTR						Name;
	DWORD						dwRefCount;
	DWORD						dwFrame;
	DWORD						dwPasses;
	RFlags						Flags;		

	IC DWORD					Activate	()
	{
		if (Device.dwFrame != dwFrame) {
			dwFrame = Device.dwFrame;
			internal_Activate	();
		}
	}
	IC void						Apply		(DWORD dwPass)
	{	internal_Apply	(dwPass);	}
	IC void						Release		()
	{	internal_Release();			}

	CShader						();
	virtual ~CShader			();
};

class	ENGINE_API	CTextureArray
{
public:
	svector<svector<CTexture*,8>,8>		textures;

	CTextureArray()
	{
		ZeroMemory	(this,sizeof(*this)));
	}
};

class	ENGINE_API	CConstant
{
public:
	LPSTR			name;
	Fcolor			const_float;
	DWORD			const_dword;

	void			set_float	(float r, float g, float b, float a)
	{
		const_float.set	(r,g,b,a);
		const_dword		= const_float.get();
	}
	void			set_float	(Fcolor& c)
	{
		const_float.set	(c);
		const_dword		= const_float.get();
	}
	void			set_dword	(DWORD c)
	{
		const_float.set(c);
		const_dword		= c;
	}
};

class	ENGINE_API	CConstantArray
{
public:
	svector<svector<CConstant*,8>,8>	constants;

	CConstantArray()
	{
		ZeroMemory	(this,sizeof(*this)));
	}
};

// 
struct ENGINE_API	Shader
{
	CShader_Base*	S;
	CTextureArray*	T;
	CConstantArray* C;
};

#endif // !defined(AFX_SHADER_H__9CBD70DD_E147_446B_B4EE_5DA321EB726F__INCLUDED_)
