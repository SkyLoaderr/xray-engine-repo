///////////////////////////////////////////////////////////////
// ThornArtifact.h
// ThornArtifact - артефакт колючка
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CThornArtifact : public CArtifact 
{
private:
	typedef CArtifact inherited;
public:
	CThornArtifact(void);
	virtual ~CThornArtifact(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

protected:
};