// Shader.cpp: implementation of the CShader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Shader.h"

ref_shader::~ref_shader()
{
	Device.Shader.Delete	(_object);
}
ref_geom::~ref_geom()
{
	Device.Shader.DeleteGeom(_object);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


BOOL	SPass::equal	(SPass& P)
{
	if (state		!= P.state)				return FALSE;
	if (ps			!= P.ps)				return FALSE;
	if (vs			!= P.vs)				return FALSE;
	if (constants	!= P.constants)			return FALSE;	// is this nessesary??? (ps+vs already combines)

	if (T != P.T)							return FALSE;
	if (M != P.M)							return FALSE;
	if (C != P.C)							return FALSE;
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
	if (Passes.size() != S.Passes.size())		return FALSE;
	for (u32 p=0; p<Passes.size(); p++)
		if (Passes[p] != S.Passes[p])			return FALSE;
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
