///////////////////////////////////////////////////////////////
// GalantineArtifact.h
// GalantineArtifact - артефакт ведбмин студень
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CGalantineArtifact : public CArtifact 
{
private:
	typedef CArtifact inherited;
public:
	CGalantineArtifact(void);
	virtual ~CGalantineArtifact(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

protected:
};