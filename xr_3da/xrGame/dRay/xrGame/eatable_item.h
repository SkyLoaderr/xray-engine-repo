////////////////////////////////////////////////////////////////////////////
//	Module 		: eatable_item.h
//	Created 	: 24.03.2003
//  Modified 	: 29.01.2004
//	Author		: Yuri Dobronravin
//	Description : Eatable item
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "inventory_item.h"

class CPhysicItem;

///////////////////////////////////////////////////////////////////////////////////////////////////
// —ъедобна€ вещь
///////////////////////////////////////////////////////////////////////////////////////////////////

class CEatableItem : public CInventoryItem {
private:
	typedef CInventoryItem	inherited;

private:
	CPhysicItem		*m_physic_item;

public:
							CEatableItem				();
	virtual					~CEatableItem				();
	virtual	DLL_Pure*		_construct					();
	virtual CEatableItem	*cast_eatable_item			()	{return this;}

	virtual void			Load						(LPCSTR section);
	virtual bool			Useful						() const;

	virtual BOOL			net_Spawn					(CSE_Abstract* DC);

	virtual void			OnH_B_Independent			();
	virtual void			OnH_A_Independent			();
	virtual	void			UseBy						(CEntityAlive* npc);
	virtual	bool			Empty						()						{return PortionsNum()==0;};
			int				PortionsNum					()	const				{return m_iPortionsNum;}
protected:	
	//вли€ние при поедании вещи на параметры игрока
	float					m_fHealthInfluence;
	float					m_fPowerInfluence;
	float					m_fSatietyInfluence;
	float					m_fRadiationInfluence;
	float					m_fMaxPowerUpInfluence;
	//заживление ран на кол-во процентов
	float					m_fWoundsHealPerc;

	//количество порций еды, 
	//-1 - порци€ одна и больше не бывает (чтоб не выводить надпись в меню)
	int						m_iPortionsNum;
	int						m_iStartPortionsNum;
};

