// Shader.cpp: implementation of the CShader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Shader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


BOOL	CPass::equal	(CPass& P)	
{
	if (dwStateBlock != P.dwStateBlock)	return FALSE;
	if (T != P.T)						return FALSE;
	if (M != P.M)						return FALSE;
	if (C != P.C)						return FALSE;
	return TRUE;
}
//
ShaderElement::ShaderElement()
{
		ZeroMemory			(this,sizeof(*this));
		Flags.iPriority		= 1;
		Flags.bStrictB2F	= FALSE;
		Flags.bLighting		= FALSE;
		Flags.bPixelShader	= FALSE;
}

BOOL ShaderElement::equal	(ShaderElement& S)
{
	if (Flags.iPriority != S.Flags.iPriority)	return FALSE;
	if (Flags.bStrictB2F != S.Flags.bStrictB2F)	return FALSE;
	if (Flags.bLighting	!= S.Flags.bLighting)	return FALSE;
	if (Passes.size() != S.Passes.size())		return FALSE;
	for (u32 p=0; p<Passes.size(); p++)
		if (!Passes[p].equal(S.Passes[p]))		return FALSE;
	return TRUE;
}
BOOL ShaderElement::equal	(ShaderElement* S)
{	return	equal(*S);	}

//
BOOL Shader::equal	(Shader& S)
{
	return 
		lod0->equal(S.lod0) && 
		lod1->equal(S.lod1) && 
		lighting->equal(S.lighting);
}
BOOL Shader::equal	(Shader* S)
{	return	equal(*S);	}
