// Shader.cpp: implementation of the CShader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Shader.h"
#include "ResourceManager.h"

// 
STextureList::~STextureList				()			{	Device.Resources->_DeleteTextureList	(this);			}
SMatrixList::~SMatrixList				()			{	Device.Resources->_DeleteMatrixList		(this);			}
SConstantList::~SConstantList			()			{	Device.Resources->_DeleteConstantList	(this);			}
SPass::~SPass							()			{	Device.Resources->_DeletePass			(this);			}
ShaderElement::~ShaderElement			()			{	Device.Resources->_DeleteElement		(this);			}
SGeometry::~SGeometry					()			{	Device.Resources->DeleteGeom			(this);			}
Shader::~Shader							()			{	Device.Resources->Delete				(this);			}
																							 
//////////////////////////////////////////////////////////////////////////					 
void	resptrcode_shader::create		(LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
	_set(Device.Resources->Create			(s_shader,s_textures,s_constants,s_matrices));
}
void	resptrcode_shader::create		(IBlender* B, LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
	_set(Device.Resources->Create_B		(B,s_shader,s_textures,s_constants,s_matrices));
}

//////////////////////////////////////////////////////////////////////////
void	resptrcode_geom::create			(u32 FVF , IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib)
{
	_set(Device.Resources->CreateGeom		(FVF,vb,ib));
}
void	resptrcode_geom::create			(D3DVERTEXELEMENT9* decl, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib)
{
	_set(Device.Resources->CreateGeom		(decl,vb,ib));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL	SPass::equal	(ref_state& _state, ref_ps& _ps, ref_vs& _vs, ref_ctable& _ctable, ref_texture_list& _T, ref_matrix_list& _M, ref_constant_list& _C)
{
	if (state		!= _state)		return FALSE;
	if (ps			!= _ps)			return FALSE;
	if (vs			!= _vs)			return FALSE;
	if (constants	!= _ctable)		return FALSE;	// is this nessesary??? (ps+vs already combines)

	if (T != _T)					return FALSE;
	if (M != _M)					return FALSE;
	if (C != _C)					return FALSE;
	return TRUE;
}

//
ShaderElement::ShaderElement()
{
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
		E[0]->equal(*S.E[0]) &&
		E[1]->equal(*S.E[1]) &&
		E[2]->equal(*S.E[2]);
}
BOOL Shader::equal	(Shader* S)
{	return	equal(*S);	}
