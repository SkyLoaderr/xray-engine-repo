///////////////////////////////////////////////////////////////
// ThornArtifact.cpp
// ThornArtifact - артефакт колючка
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ThornArtifact.h"
#include "PhysicsShell.h"


CThornArtifact::CThornArtifact(void) 
{
}

CThornArtifact::~CThornArtifact(void) 
{
}

void CThornArtifact::Load(LPCSTR section) 
{
	inherited::Load(section);
}



void CThornArtifact::UpdateCL() 
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