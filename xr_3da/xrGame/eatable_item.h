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
// ��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////

class CEatableItem : virtual public CInventoryItem 
{
private:
	typedef CInventoryItem	inherited;
public:
	CEatableItem();
	virtual ~CEatableItem();

	virtual void Load(LPCSTR section);
	virtual bool Useful();

	//������� ��� �������� ���� �� ��������� ������
	float	m_fHealthInfluence;
	float	m_fPowerInfluence;
	float	m_fSatietyInfluence;
	float	m_fRadiationInfluence;

	//���������� ������ ���, 
	//-1 - ������ ���� � ������ �� ������ (���� �� �������� ������� � ����)
	int		m_iPortionsNum;
};

