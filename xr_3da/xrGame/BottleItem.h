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
	
	virtual BOOL net_Spawn			(LPVOID DC);
	virtual void net_Destroy		();
	
	virtual void UpdateCL			();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();

	void	OnEvent					(NET_Packet& P, u16 type);


	virtual	void Hit				(float P, Fvector &dir,	
									 CObject* who, s16 element,
									 Fvector position_in_object_space, 
									 float impulse, 
									 ALife::EHitType hit_type = eHitTypeWound);
	

	virtual void BreakToPieces();

protected:
	//�������� ���������� �������
	ref_str m_sBreakParticles;
	ref_sound sndBreaking;
};