///////////////////////////////////////////////////////////////
// Needles.h
// Needles - �������� ������
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

protected:
};