// Shader.cpp: implementation of the CShader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Shader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShader_Base::CShader_Base()
{
	dwFrame				= 0;
	Flags.iPriority		= 1;
	Flags.bLighting		= FALSE;
	Flags.bStrictB2F	= FALSE;
}

CShader_Base::~CShader_Base()
{

}

