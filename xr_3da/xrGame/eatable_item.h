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
// ��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////

class CEatableItem : public CInventoryItem {
private:
	typedef CInventoryItem	inherited;

private:
	CPhysicItem		*m_physic_item;

public:
	CEatableItem();
	virtual ~CEatableItem();
	virtual DLL_Pure *_construct();

	virtual void Load(LPCSTR section);
	virtual bool Useful() const;

	virtual void OnH_B_Independent();
	virtual void OnH_A_Independent();

	//������� ��� �������� ���� �� ��������� ������
	float	m_fHealthInfluence;
	float	m_fPowerInfluence;
	float	m_fSatietyInfluence;
	float	m_fRadiationInfluence;
	//���������� ��� �� ���-�� ���������
	float	m_fWoundsHealPerc;

	//���������� ������ ���, 
	//-1 - ������ ���� � ������ �� ������ (���� �� �������� ������� � ����)
	int		m_iPortionsNum;
};

