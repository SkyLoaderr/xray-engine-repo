#pragma once
#include "gameobject.h"
#include "PhysicsShell.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// ������� ���
// ���������� ����� �������, �������� �������, ����� ���� ����������.
// ����:  �� 50 �� 200 ������, � ����������� �� ������� 
// ���������: ������� ���������� �����������, ������� ������ � ���������� ����������,
// �������� � ������� R1.
class CMercuryBall : public CGameObject {
typedef	CGameObject	inherited;
public:
	CMercuryBall(void);
	~CMercuryBall(void);

	virtual void OnH_A_Chield();
	virtual void OnH_B_Independent();
	virtual void OnDeviceCreate();
	virtual void OnDeviceDestroy();

	virtual BOOL			net_Spawn			(LPVOID DC);
};
