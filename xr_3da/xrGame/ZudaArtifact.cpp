///////////////////////////////////////////////////////////////
// ZudaArtifact.cpp
// ZudaArtifact - артефакт "зуда"
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZudaArtifact.h"
#include "PhysicsShell.h"


CZudaArtifact::CZudaArtifact(void) 
{
}

CZudaArtifact::~CZudaArtifact(void) 
{
}

void CZudaArtifact::Load(LPCSTR section) 
{
	inherited::Load(section);
}



void CZudaArtifact::UpdateCL() 
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