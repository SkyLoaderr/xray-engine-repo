//////////////////////////////////////////////////////////////////////
// ExplosiveItem.h: ����� ��� ���� ������� ���������� ��� 
//					��������� ��������� ����� (��������,
//					������ � ����� � �.�.)
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Explosive.h"

class CExplosiveItem: public CExplosive
{
private:
	typedef CExplosive inherited;
public:
	CExplosiveItem(void);
	virtual ~CExplosiveItem(void);

	virtual void				Load				(LPCSTR section);

	virtual BOOL				net_Spawn			(LPVOID DC);
	virtual void				net_Destroy			();
	virtual void				net_Export			(NET_Packet& P);
	virtual void				net_Import			(NET_Packet& P);

	virtual	void Hit			(float P, Fvector &dir,	CObject* who, s16 element,
										  Fvector position_in_object_space, float impulse, 
										  ALife::EHitType hit_type = ALife::eHitTypeWound);

	virtual void renderable_Render(); 
protected:
	//��������� ��������, ���� 0, �� �� ����������
	float m_fHealth;
	//������������ ����
	HitTypeSVec m_HitTypeK;
};