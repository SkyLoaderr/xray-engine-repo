///////////////////////////////////////////////////////////////
// BlackGraviArtifact.h
// BlackGraviArtefact - гравитационный артефакт, 
// такой же как и обычный, но при получении хита
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

	virtual BOOL net_Spawn			(LPVOID DC);

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

	//гравитационный удар по всем объектам в зоне дос€гаемости
	void GraviStrike();

	//которого артефакт активизируетс€
	float m_fImpulseThreshold;
	//радиус действи€ артефакта
	float m_fRadius;
	//импульс передаваемый окружающим предметам
	float m_fStrikeImpulse;

	//флаг, того что артефакт получил хит 
	//и теперь может совершить бросок
	bool m_bStrike;	

	shared_str	m_sParticleName;
};
