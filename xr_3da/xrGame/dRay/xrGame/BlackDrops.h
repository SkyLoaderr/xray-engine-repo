///////////////////////////////////////////////////////////////
// BlackDrops.h
// BlackDrops - ������ �����
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CBlackDrops : public CArtifact 
{
private:
	typedef CArtifact inherited;
public:
	CBlackDrops(void);
	virtual ~CBlackDrops(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

protected:
};