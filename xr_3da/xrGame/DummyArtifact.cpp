///////////////////////////////////////////////////////////////
// DummyArtifact.cpp
// DummyArtifact - артефакт пустышка
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DummyArtifact.h"
#include "PhysicsShell.h"


CDummyArtifact::CDummyArtifact(void) 
{
}

CDummyArtifact::~CDummyArtifact(void) 
{
}

void CDummyArtifact::Load(LPCSTR section) 
{
	inherited::Load(section);
}



void CDummyArtifact::UpdateCL() 
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