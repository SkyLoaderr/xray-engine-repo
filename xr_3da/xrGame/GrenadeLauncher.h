///////////////////////////////////////////////////////////////
// GrenadeLauncher.h
// GrenadeLauncher - ������� ������ ����������� ����������
///////////////////////////////////////////////////////////////

#pragma once
#include "inventory.h"

class CGrenadeLauncher : public CInventoryItem
{
private:
	typedef CInventoryItem inherited;
public:
	CGrenadeLauncher (void);
	virtual ~CGrenadeLauncher(void);

	virtual BOOL net_Spawn			(LPVOID DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();

	virtual void UpdateCL			();
	virtual void renderable_Render	();

	float	GetGrenadeVel() {return m_fGrenadeVel;}

protected:
	bool m_bForPistol;
	//��������� �������� ������ ������������ �������
	float m_fGrenadeVel;
};