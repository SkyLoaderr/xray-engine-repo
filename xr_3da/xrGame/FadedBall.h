///////////////////////////////////////////////////////////////
// FadedBall.h
// FadedBall - артефакт блеклый шар
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CFadedBall : public CArtifact 
{
private:
	typedef CArtifact inherited;
public:
	CFadedBall(void);
	virtual ~CFadedBall(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

protected:
};