//////////////////////////////////////////////////////////////////////////
// GraviZone.cpp:	�������������� ��������
//////////////////////////////////////////////////////////////////////////
//					������� ��� �� �� 2� ���
//					���� ���������� ������, ������ �������� � 
//					��� ������� ������� (�������� � �����)
//					��������� � ������ � ������ ��� �����-��
//					�����
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gravizone.h"

#include "PhysicsShell.h"
#include "entity_alive.h"
#include "phmovementcontrol.h"

CGraviZone::CGraviZone(void)
{
}
CGraviZone::~CGraviZone(void)
{
}

void CGraviZone::Load(LPCSTR section)
{
	inherited::Load(section);

	m_dwThrowInTime = 5000;
	m_fThrowInImpulse = 760.f;
}

BOOL CGraviZone::net_Spawn(LPVOID DC)
{
	return inherited::net_Spawn(DC);
}
void CGraviZone::net_Destroy()
{
	TTelekinesis::Deactivate();
	inherited::net_Destroy();
}

void CGraviZone::Affect(CObject* O)
{
	CGameObject* GO = dynamic_cast<CGameObject*>(O);
	if(!GO) return;
	CEntityAlive* EA = dynamic_cast<CEntityAlive*>(GO);

	//���������� ����� �� ����������� � ������ ����
	Fvector throw_in_dir;
	Fvector zone_center;
	Center(zone_center);
	throw_in_dir.sub(zone_center, GO->Position());

	float dist = throw_in_dir.magnitude();
	if(fis_zero(dist)) return;
	
	float throw_power = m_fThrowInImpulse*RelativePower(dist);
	throw_in_dir.normalize();

	static int i = 0;
	i+=Device.dwTimeDelta;
	if (i > 250)
	{
		i = 0;
		if(EA && EA->g_Alive())
		{
			EA->m_PhysicMovementControl->ApplyImpulse(throw_in_dir, throw_power);
		}
		else
			GO->m_pPhysicsShell->applyImpulse(throw_in_dir, throw_power);
	}
	
	
	float dist_to_radius = dist/Radius();
	if(dist_to_radius<0.2)
		inherited::Affect(O);
}

void CGraviZone::shedule_Update		(u32 dt)
{
	inherited::shedule_Update(dt);

//	static int i = 0;
//	i+=dt;
//
//	if (i > 10000)
//	{
//		if (!TTelekinesis::IsActive())
//			TTelekinesis::Activate(Position());
//		else 
//			TTelekinesis::Deactivate();
//		
//		i = 0;
//
//	}

	TTelekinesis::schedule_update();
}