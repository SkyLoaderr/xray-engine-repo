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
	DWORD						dwFrame;
	RFlags						Flags;		

	IC void						Activate	()
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

	CShader	();
	virtual ~CShader();
};

class	ENGINE_API	CShaderArray
{
public:
	CShader*		shaders		[COMBINATIONS_SH];
	
	CShaderCombination()	
	{
		ZeroMemory	(this,sizeof(*this));
	}
	IC CShader*		select		(DWORD num)	{
		return shaders			[num];
	}
};

// 
struct ENGINE_API	Shader
{
	CShader*		S;
	tex_handles*	T;
	DWORD			c[4];		// user-defined constants

	IC void			set_value	(DWORD id, DWORD val)	{ c[id]=val; }
};

#endif // !defined(AFX_SHADER_H__9CBD70DD_E147_446B_B4EE_5DA321EB726F__INCLUDED_)
