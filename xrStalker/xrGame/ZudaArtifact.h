///////////////////////////////////////////////////////////////
// ZudaArtifact.h
// ZudaArtifact - артефакт зуда
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CZudaArtifact : public CArtifact 
{
private:
	typedef CArtifact inherited;
public:
	CZudaArtifact(void);
	virtual ~CZudaArtifact(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

protected:
};