///////////////////////////////////////////////////////////////
// Needles.h
// Needles - �������� ������
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CNeedles: public CArtifact 
{
private:
	typedef CArtifact inherited;
public:
	CNeedles(void);
	virtual ~CNeedles(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

protected:
};