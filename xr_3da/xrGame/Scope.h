///////////////////////////////////////////////////////////////
// Scope.h
// Scope - ������� ������ ����������� ������
///////////////////////////////////////////////////////////////

#pragma once
#include "inventory_item.h"

class CScope : public CInventoryItem
{
private:
	typedef CInventoryItem inherited;
public:
	CScope (void);
	virtual ~CScope(void);

	virtual BOOL net_Spawn			(LPVOID DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();

	virtual void UpdateCL			();
	virtual void renderable_Render	();

protected:
};