///////////////////////////////////////////////////////////////
// GraviArtifact.h
// GraviArtifact - �������������� ��������, ������� �� �����
// � ����� ��� ������
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CGraviArtifact : public CArtifact 
{
private:
	typedef CArtifact inherited;
public:
	CGraviArtifact(void);
	virtual ~CGraviArtifact(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

protected:
	//��������� ���������
	float m_fJumpHeight;
	float m_fEnergy;
};
