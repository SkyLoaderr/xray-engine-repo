#pragma once
#include "gameobject.h"

// ������� ���
class CMercuryBall : public CGameObject {
typedef	CGameObject	inherited;
public:
	CMercuryBall(void);
	~CMercuryBall(void);

	BOOL net_Spawn(LPVOID DC);
};
