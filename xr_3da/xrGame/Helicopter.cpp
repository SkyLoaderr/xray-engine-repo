#include "stdafx.h"
#include "helicopter.h"
#include "xrserver_objects_alife.h"

CHelicopter::CHelicopter()
{
	init();
}

CHelicopter::~CHelicopter()
{
}

//CAI_ObjectLocation
void				
CHelicopter::init()
{
	m_movementMngr.init(this);
}

void		
CHelicopter::reinit()
{
	inherited::reinit	();
}


//CGameObject
void		
CHelicopter::Load(LPCSTR section)
{
	inherited::Load		(section);
}

void		
CHelicopter::reload(LPCSTR section)
{
	inherited::reload	(section);
}

BOOL		
CHelicopter::net_Spawn(LPVOID	DC)
{
	if (!inherited::net_Spawn(DC))
		return			(FALSE);

	// assigning m_animator here
	CSE_Abstract		*abstract=(CSE_Abstract*)(DC);
	CSE_ALifeHelicopter	*heli	= dynamic_cast<CSE_ALifeHelicopter*>(abstract);
	VERIFY				(heli);

	R_ASSERT			(Visual()&&PKinematics(Visual()));
	CSkeletonAnimated	*A= PSkeletonAnimated(Visual());
	if (A) {
		A->PlayCycle	(*heli->startup_animation);
		A->Calculate	();
	}
/*
	m_animator->Load	(heli->get_motion());
	m_animator->Play	(true);
*/
	m_engine_sound.create(TRUE,*heli->engine_sound);
	m_engine_sound.play_at_pos(0,XFORM().c,sm_Looped);

	setVisible			(true);
	setEnabled			(true);

	return				(TRUE);
}

void		
CHelicopter::net_Destroy()
{
	inherited::net_Destroy();
}

void		
CHelicopter::net_Export(NET_Packet &P)
{
}

void		
CHelicopter::net_Import (NET_Packet &P)
{
}

void		
CHelicopter::renderable_Render()
{
	inherited::renderable_Render();
}

void		
CHelicopter::UpdateCL()
{
	inherited::UpdateCL	();

	m_movementMngr.onFrame( XFORM() );

	float td = Device.fTimeDelta;
	u32 tg = Device.dwTimeGlobal;

	m_engine_sound.set_position(XFORM().c);
}

void		
CHelicopter::shedule_Update(u32	time_delta)
{
	if (!getEnabled())	return;
	inherited::shedule_Update	(time_delta);
	
	m_movementMngr.shedule_Update(time_delta);
}

