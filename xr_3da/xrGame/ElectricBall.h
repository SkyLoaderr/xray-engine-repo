///////////////////////////////////////////////////////////////
// ElectricBall.h
// ElectricBall - артефакт электрический шар
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CElectricBall : public CArtifact 
{
private:
	typedef CArtifact inherited;
public:
	CElectricBall(void);
	virtual ~CElectricBall(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

protected:
};