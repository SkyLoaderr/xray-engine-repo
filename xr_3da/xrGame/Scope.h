///////////////////////////////////////////////////////////////
// Scope.h
// Scope - апгрейд оружия снайперский прицел
///////////////////////////////////////////////////////////////

#pragma once
#include "inventory_item_object.h"

class CScope : public CInventoryItemObject {
private:
	typedef CInventoryItemObject inherited;
public:
			CScope ();
	virtual ~CScope();

	virtual BOOL net_Spawn			(CSE_Abstract* DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();

	virtual void UpdateCL			();
	virtual void renderable_Render	();

protected:
};