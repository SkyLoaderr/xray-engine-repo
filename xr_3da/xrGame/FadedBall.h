///////////////////////////////////////////////////////////////
// FadedBall.h
// FadedBall - �������� ������� ���
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CFadedBall : public CArtefact 
{
private:
	typedef CArtefact inherited;
public:
	CFadedBall(void);
	virtual ~CFadedBall(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

protected:
};