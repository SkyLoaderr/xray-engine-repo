#pragma once
#include "gameobject.h"
#include "PhysicsShell.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Ртутный шар
// Появляется после выброса, держится недолго, после чего испаряется.
// Цены:  от 50 до 200 рублей, в зависимости от размера 
// Специфика: опасное аномальное образование, хранить только в защищенном контейнере,
// например в капсуле R1.
class CMercuryBall : public CGameObject {
typedef	CGameObject	inherited;
public:
	CMercuryBall(void);
	~CMercuryBall(void);

	virtual void OnH_A_Chield();
	virtual void OnH_B_Independent();
	virtual void OnDeviceCreate();
	virtual void OnDeviceDestroy();
	virtual	void Hit					(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space){};
	virtual BOOL			net_Spawn			(LPVOID DC);
};
