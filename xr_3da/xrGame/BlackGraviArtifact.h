///////////////////////////////////////////////////////////////
// BlackGraviArtifact.h
// BlackGraviArtefact - �������������� ��������, 
// ����� �� ��� � �������, �� ��� ��������� ����
///////////////////////////////////////////////////////////////

#pragma once
#include "GraviArtifact.h"
#include "../feel_touch.h"
#include "PhysicsShellHolder.h"
DEFINE_VECTOR (CPhysicsShellHolder*, GAME_OBJECT_LIST, GAME_OBJECT_LIST_it);

class CBlackGraviArtefact: public CGraviArtefact,
						   public Feel::Touch
{
private:
	typedef CGraviArtefact inherited;
public:
	CBlackGraviArtefact(void);
	virtual ~CBlackGraviArtefact(void);

	virtual BOOL net_Spawn			(CSE_Abstract* DC);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

	virtual	void Hit				(float P, Fvector &dir,	
									CObject* who, s16 element,
									Fvector position_in_object_space, 
									float impulse, 
									ALife::EHitType hit_type = ALife::eHitTypeWound);

	virtual void feel_touch_new	(CObject* O);
	virtual void feel_touch_delete	(CObject* O);
	virtual BOOL feel_touch_contact	(CObject* O);

protected:
	GAME_OBJECT_LIST m_GameObjectList;

	//�������������� ���� �� ���� �������� � ���� ������������
	void GraviStrike();

	//�������� �������� ��������������
	float m_fImpulseThreshold;
	//������ �������� ���������
	float m_fRadius;
	//������� ������������ ���������� ���������
	float m_fStrikeImpulse;

	//����, ���� ��� �������� ������� ��� 
	//� ������ ����� ��������� ������
	bool m_bStrike;	

	shared_str	m_sParticleName;
};
