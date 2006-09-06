#pragma once
#include "inventory_space.h"
#include "GameObject.h"

class CInventoryBox :public CGameObject
{
	typedef CGameObject									inherited;
	xr_vector<u16>										m_items;
public:
	virtual		void	OnEvent							(NET_Packet& P, u16 type);
	virtual		BOOL	net_Spawn						(CSE_Abstract* DC);
	virtual		void	net_Relcase						(CObject* O	);
				void	AddAvailableItems				(TIItemContainer& items_container) const;
};