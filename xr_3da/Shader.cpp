// Shader.cpp: implementation of the CShader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Shader.h"

// 
STextureList::~STextureList				()			{	Device.Shader._DeleteTextureList	(this);			}
SMatrixList::~SMatrixList				()			{	Device.Shader._DeleteMatrixList		(this);			}
SConstantList::~SConstantList			()			{	Device.Shader._DeleteConstantList	(this);			}
SPass::~SPass							()			{	Device.Shader._DeletePass			(this);			}
ShaderElement::~ShaderElement			()			{	Device.Shader._DeleteElement		(this);			}
SGeometry::~SGeometry					()			{	Device.Shader.DeleteGeom			(this);			}
Shader::~Shader							()			{	Device.Shader.Delete				(this);			}
																							 
//////////////////////////////////////////////////////////////////////////					 
void	resptrcode_shader::create		(LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
	_set(Device.Shader.Create			(s_shader,s_textures,s_constants,s_matrices));
}
void	resptrcode_shader::create		(IBlender* B, LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
	_set(Device.Shader.Create_B		(B,s_shader,s_textures,s_constants,s_matrices));
}

//////////////////////////////////////////////////////////////////////////
void	resptrcode_geom::create			(u32 FVF , IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib)
{
	_set(Device.Shader.CreateGeom		(FVF,vb,ib));
}
void	resptrcode_geom::create			(D3DVERTEXELEMENT9* decl, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib)
{
	_set(Device.Shader.CreateGeom		(decl,vb,ib));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL	SPass::equal	(SPass& P)
{
	if (state		!= P.state)						return FALSE;
	if (ps			!= P.ps)						return FALSE;
	if (vs			!= P.vs)						return FALSE;
	if (constants	!= P.constants)					return FALSE;	// is this nessesary??? (ps+vs already combines)

	if (T != P.T)									return FALSE;
	if (M != P.M)									return FALSE;
	if (C != P.C)									return FALSE;
	return TRUE;
}

//
ShaderElement::ShaderElement()
{
	ZeroMemory			(this,sizeof(*this));
	Flags.iPriority		= 1;
	Flags.bStrictB2F	= FALSE;
	Flags.bLighting		= FALSE;
}

BOOL ShaderElement::equal	(ShaderElement& S)
{
	if (Flags.iPriority		!= S.Flags.iPriority)	return FALSE;
	if (Flags.bStrictB2F	!= S.Flags.bStrictB2F)	return FALSE;
	if (Flags.bLighting		!= S.Flags.bLighting)	return FALSE;
	if (Passes.size() != S.Passes.size())			return FALSE;
	for (u32 p=0; p<Passes.size(); p++)
		if (Passes[p] != S.Passes[p])				return FALSE;
	return TRUE;
}

BOOL ShaderElement::equal	(ShaderElement* S)
{	return	equal(*S);	}

//
BOOL Shader::equal	(Shader& S)
{
	return
		E[0]->equal(S.E[0]) &&
		E[1]->equal(S.E[1]) &&
		E[2]->equal(S.E[2]);
}
BOOL Shader::equal	(Shader* S)
{	return	equal(*S);	}
