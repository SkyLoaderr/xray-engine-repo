///////////////////////////////////////////////////////////////
// GraviArtifact.h
// GraviArtefact - �������������� ��������, ������� �� �����
// � ����� ��� ������
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CGraviArtefact : public CArtefact 
{
private:
	typedef CArtefact inherited;
public:
	CGraviArtefact(void);
	virtual ~CGraviArtefact(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

protected:
	//��������� ���������
	float m_fJumpHeight;
	float m_fEnergy;
};
