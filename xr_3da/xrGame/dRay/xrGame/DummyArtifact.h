///////////////////////////////////////////////////////////////
// DummyArtifact.h
// DummyArtifact - артефакт пустышка
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CDummyArtifact : public CArtifact 
{
private:
	typedef CArtifact inherited;
public:
	CDummyArtifact(void);
	virtual ~CDummyArtifact(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

protected:
};