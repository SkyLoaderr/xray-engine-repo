//////////////////////////////////////////////////////////////////////////
// GraviZone.cpp:	гравитационная аномалия
//					все неживые объекты (предметы и трупы)
//					поднимает в воздух и качает там какое-то
//					время
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