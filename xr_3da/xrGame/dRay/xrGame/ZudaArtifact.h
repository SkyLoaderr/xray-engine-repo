///////////////////////////////////////////////////////////////
// ZudaArtifact.h
// ZudaArtefact - �������� ����
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CZudaArtefact : public CArtefact 
{
private:
	typedef CArtefact inherited;
public:
	CZudaArtefact(void);
	virtual ~CZudaArtefact(void);

	virtual void Load				(LPCSTR section);

protected:
};