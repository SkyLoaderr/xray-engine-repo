#pragma once
#include "gameobject.h"

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

	BOOL net_Spawn(LPVOID DC);
};
