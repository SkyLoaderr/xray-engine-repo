///////////////////////////////////////////////////////////////
// GalantineArtifact.cpp
// GalantineArtifact - артефакт ведбмин студень
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GalantineArtifact.h"
#include "PhysicsShell.h"


CGalantineArtifact::CGalantineArtifact(void) 
{
}

CGalantineArtifact::~CGalantineArtifact(void) 
{
}

void CGalantineArtifact::Load(LPCSTR section) 
{
	inherited::Load(section);
}



void CGalantineArtifact::UpdateCL() 
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