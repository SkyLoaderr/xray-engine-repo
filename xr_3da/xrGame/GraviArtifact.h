///////////////////////////////////////////////////////////////
// GraviArtifact.h
// GraviArtifact - гравитационный артефакт, прыгает на месте
// и парит над землей
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CGraviArtifact : public CArtifact 
{
private:
	typedef CArtifact inherited;
public:
	CGraviArtifact(void);
	virtual ~CGraviArtifact(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

protected:
	//параметры артефакта
	float m_fJumpHeight;
	float m_fEnergy;
};
