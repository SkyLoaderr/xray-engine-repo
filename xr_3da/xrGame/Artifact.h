#pragma once
#include "inventory.h"

class CArtifact :
	public CInventoryItem
{
typedef	CInventoryItem	inherited;
public:
	CArtifact(void);
	virtual ~CArtifact(void);

	virtual BOOL net_Spawn(LPVOID DC);
	virtual void OnH_A_Chield();
	virtual void OnH_B_Independent();
	virtual void UpdateCL();
};
