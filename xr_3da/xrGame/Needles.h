///////////////////////////////////////////////////////////////
// Needles.h
// Needles - артефакт иголки
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CNeedles: public CArtefact 
{
private:
	typedef CArtefact inherited;
public:
	CNeedles(void);
	virtual ~CNeedles(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

protected:
};