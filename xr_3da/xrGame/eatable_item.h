////////////////////////////////////////////////////////////////////////////
//	Module 		: eatable_item.h
//	Created 	: 24.03.2003
//  Modified 	: 29.01.2004
//	Author		: Yuri Dobronravin
//	Description : Eatable item
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "inventory_item.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// —ъедобна€ вещь
///////////////////////////////////////////////////////////////////////////////////////////////////

class CEatableItem : virtual public CInventoryItem 
{
private:
	typedef CInventoryItem	inherited;
public:
	CEatableItem();
	virtual ~CEatableItem();

	virtual void Load(LPCSTR section);
	virtual bool Useful() const;

	virtual void OnH_B_Independent();
	virtual void OnH_A_Independent();

	//вли€ние при поедании вещи на параметры игрока
	float	m_fHealthInfluence;
	float	m_fPowerInfluence;
	float	m_fSatietyInfluence;
	float	m_fRadiationInfluence;

	//количество порций еды, 
	//-1 - порци€ одна и больше не бывает (чтоб не выводить надпись в меню)
	int		m_iPortionsNum;
};

