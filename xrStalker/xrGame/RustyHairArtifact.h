///////////////////////////////////////////////////////////////
// RustyHairArtifact.h
// RustyHairArtifact - артефакт ржавые волосы
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CRustyHairArtifact : public CArtifact 
{
private:
	typedef CArtifact inherited;
public:
	CRustyHairArtifact(void);
	virtual ~CRustyHairArtifact(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

protected:
};