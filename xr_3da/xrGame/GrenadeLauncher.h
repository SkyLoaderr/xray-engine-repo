///////////////////////////////////////////////////////////////
// GrenadeLauncher.h
// GrenadeLauncher - апгрейд оружия поствольный гранатомет
///////////////////////////////////////////////////////////////

#pragma once
#include "inventory_item.h"

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
	//стартовая скорость вылета подствольной гранаты
	float m_fGrenadeVel;
};