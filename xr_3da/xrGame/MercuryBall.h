///////////////////////////////////////////////////////////////
// MercuryBall.h
// MercuryBall - переливающийс€ и колыхающийс€ шар
// перекатываетс€ с места на место
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CMercuryBall : public CArtefact 
{
private:
	typedef CArtefact inherited;
public:
	CMercuryBall(void);
	virtual ~CMercuryBall(void);

	virtual void Load				(LPCSTR section);
	virtual void UpdateCL			();

protected:
	//врем€ последнего обновлени€ поведени€ шара
	ALife::_TIME_ID m_timeLastUpdate;
	//врем€ между апдейтами
	ALife::_TIME_ID m_timeToUpdate;

	//диапазон импульсов катани€ шара
	float m_fImpulseMin;
	float m_fImpulseMax;
};

/*

#pragma once
#include "gameobject.h"
#include "PhysicsShell.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// –тутный шар
// ѕо€вл€етс€ после выброса, держитс€ недолго, после чего испар€етс€.
// ÷ены:  от 50 до 200 рублей, в зависимости от размера 
// —пецифика: опасное аномальное образование, хранить только в защищенном контейнере,
// например в капсуле R1.
class CMercuryBall : public CGameObject {
typedef	CGameObject	inherited;
public:
	CMercuryBall(void);
	virtual ~CMercuryBall(void);

	virtual void OnH_A_Chield();
	virtual void OnH_B_Independent();

	//virtual	void Hit					(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse){};
	virtual BOOL			net_Spawn			(LPVOID DC);
};
*/