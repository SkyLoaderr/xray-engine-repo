///////////////////////////////////////////////////////////////
// BottleItem.h
// BottleItem - ������� � ��������, ������� ����� �������
///////////////////////////////////////////////////////////////


#pragma once

#include "fooditem.h"


class CBottleItem: public CFoodItem
{
private:
    typedef	CFoodItem inherited;
public:
	CBottleItem(void);
	virtual ~CBottleItem(void);


	virtual void Load				(LPCSTR section);
	
	virtual BOOL net_Spawn			(CSE_Abstract* DC);
	virtual void net_Destroy		();
	
	virtual void UpdateCL			();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();
	virtual void OnH_A_Independent	(){inherited::OnH_A_Independent();}
	virtual void OnH_B_Chield		(){inherited::OnH_B_Chield();}

	void	OnEvent					(NET_Packet& P, u16 type);


	virtual	void	Hit				(SHit* pHDS);
	

			void					BreakToPieces		();
	virtual void					UseBy				(CEntityAlive* entity_alive);
protected:
	float		m_alcohol;
	//�������� ���������� �������
	shared_str m_sBreakParticles;
	ref_sound sndBreaking;
};