///////////////////////////////////////////////////////////////
// DummyArtifact.cpp
// DummyArtifact - �������� ��������
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
}