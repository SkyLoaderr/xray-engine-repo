// Shader.h: interface for the CShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SHADER__INCLUDED_)
#define SHADER__INCLUDED_
#pragma once

class	ENGINE_API	CShader;
class	ENGINE_API	CTextureArray;
class	ENGINE_API	CMatrixArray;
class	ENGINE_API	CConstantArray;

struct ENGINE_API	Shader
{
	CShader*		S;
	CTextureArray*	T;
	CMatrixArray*	M;
	CConstantArray* C;
};

#endif // !defined(AFX_SHADER_H__9CBD70DD_E147_446B_B4EE_5DA321EB726F__INCLUDED_)
