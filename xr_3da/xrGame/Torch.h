#pragma once
#include "inventory.h"

class CTorch :
	public CInventoryItem
{
typedef	CInventoryItem	inherited;
public:
	CTorch(void);
	virtual ~CTorch(void);

	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();

	virtual void OnH_A_Chield();
	virtual void OnH_B_Independent();

	virtual void UpdateCL();
	virtual void OnVisible();

	Fvector m_pos;
	IRender_Light*	light_render;
};
