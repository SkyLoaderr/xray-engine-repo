///////////////////////////////////////////////////////////////
// RustyHairArtifact.cpp
// RustyHairArtifact - артефакт ржавые волосы
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RustyHairArtifact.h"
#include "PhysicsShell.h"


CRustyHairArtifact::CRustyHairArtifact(void) 
{
}

CRustyHairArtifact::~CRustyHairArtifact(void) 
{
}

void CRustyHairArtifact::Load(LPCSTR section) 
{
	inherited::Load(section);
}



void CRustyHairArtifact::UpdateCL() 
{
	inherited::UpdateCL();

	if(getVisible() && m_pPhysicsShell) 
	{
		m_pPhysicsShell->Update	();
		XFORM().set(m_pPhysicsShell->mXFORM);
		Position().set(m_pPhysicsShell->mXFORM.c);
	} 
	else if(H_Parent()) XFORM().set(H_Parent()->XFORM());
}