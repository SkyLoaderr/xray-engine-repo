//////////////////////////////////////////////////////////////////////////
// GraviZone.cpp:	�������������� ��������
//					��� ������� ������� (�������� � �����)
//					��������� � ������ � ������ ��� �����-��
//					�����
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gravizone.h"


CGraviZone::CGraviZone(void)
{
}
CGraviZone::~CGraviZone(void)
{
}

BOOL CGraviZone::net_Spawn(LPVOID DC)
{
	TTelekinesis::InitExtern(this, 3.f, 1.5f, int_max);
	return inherited::net_Spawn(DC);
}
void CGraviZone::net_Destroy()
{
	TTelekinesis::Deactivate();
	inherited::net_Destroy();
}

void CGraviZone::Affect(CObject* O)
{
	
	inherited::Affect(O);
}

void CGraviZone::PhDataUpdate(dReal step)
{
	TTelekinesis::UpdateCL(step);	
}

void CGraviZone::shedule_Update		(u32 dt)
{
	inherited::shedule_Update(dt);

	static int i = 0;
	i+=dt;

	if (i > 10000)
	{
		if (!TTelekinesis::IsActive())
			TTelekinesis::Activate();
		else 
			TTelekinesis::Deactivate();
		
		i = 0;

	}

	TTelekinesis::UpdateSched();
}