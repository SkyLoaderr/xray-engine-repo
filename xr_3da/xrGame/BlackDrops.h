///////////////////////////////////////////////////////////////
// BlackDrops.h
// BlackDrops - ������ �����
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CBlackDrops : public CArtefact 
{
private:
	typedef CArtefact inherited;
public:
	CBlackDrops(void);
	virtual ~CBlackDrops(void);

	virtual void Load				(LPCSTR section);

protected:
};