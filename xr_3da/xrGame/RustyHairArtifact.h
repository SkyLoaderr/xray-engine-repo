///////////////////////////////////////////////////////////////
// RustyHairArtifact.h
// RustyHairArtefact - �������� ������ ������
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CRustyHairArtefact : public CArtefact 
{
private:
	typedef CArtefact inherited;
public:
	CRustyHairArtefact(void);
	virtual ~CRustyHairArtefact(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

protected:
};