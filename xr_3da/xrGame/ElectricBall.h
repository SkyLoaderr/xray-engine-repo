///////////////////////////////////////////////////////////////
// ElectricBall.h
// ElectricBall - �������� ������������� ���
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CElectricBall : public CArtefact 
{
private:
	typedef CArtefact inherited;
public:
	CElectricBall(void);
	virtual ~CElectricBall(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

protected:
};