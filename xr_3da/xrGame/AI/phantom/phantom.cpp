#include "stdafx.h"
#include "phantom.h"
#include "../../NET_utils.h"
#include "../level.h"

CPhantom::CPhantom()
{
}

CPhantom::~CPhantom()
{
}

void CPhantom::Load( LPCSTR section )
{
	inherited::Load				(section);
	//////////////////////////////////////////////////////////////////////////
	ISpatial*			self = smart_cast<ISpatial*> (this);
	if (self) {
		self->spatial.type &=~STYPE_VISIBLEFORAI;
		self->spatial.type &=~STYPE_REACTTOSOUND;
	}
	//////////////////////////////////////////////////////////////////////////

}

BOOL CPhantom::net_Spawn		(CSE_Abstract* DC)
{
	if (!inherited::net_Spawn(DC)) return FALSE;
	
	setVisible	(TRUE);
	setEnabled	(TRUE);

	return	TRUE;
}

void CPhantom::shedule_Update(u32 DT)
{
	spatial.type &=~STYPE_VISIBLEFORAI;

	inherited::shedule_Update(DT);
}


// Animation Callbacks
void __stdcall CPhantom::animation_callback(CBlend* B)
{
	CPhantom *phantom = (CPhantom*)B->CallbackParam;
	phantom->m_motion.invalidate	();
}


void CPhantom::UpdateCL()
{
	inherited::UpdateCL();

	// назначить глобальную анимацию
	if (!m_motion) {
		// выбор анимации
		CSkeletonAnimated *skeleton_animated = smart_cast<CSkeletonAnimated*>(Visual());
		
		m_motion = skeleton_animated->ID_Cycle("stand_idle_0");
		skeleton_animated->PlayCycle(m_motion, TRUE, animation_callback, this);
	}
}



// Core events
void CPhantom::net_Export	(NET_Packet& P)					// export to server
{
	// export 
	R_ASSERT			(Local());

	u8					flags = 0;
	P.w_float_q16		(g_Health(),-500,1000);

	P.w_float			(0);
	P.w_u32				(0);
	P.w_u32				(0);

	P.w_u32				(Device.dwTimeGlobal);
	P.w_u8				(flags);

	float				yaw, pitch, bank;
	XFORM().getHPB		(yaw,pitch,bank);
	P.w_angle8			(yaw);
	P.w_angle8			(yaw);
	P.w_angle8			(pitch);
	P.w_angle8			(0);
	P.w_u8				(u8(g_Team()));
	P.w_u8				(u8(g_Squad()));
	P.w_u8				(u8(g_Group()));
}

//---------------------------------------------------------------------
void CPhantom::net_Import	(NET_Packet& P)
{
	// import
	R_ASSERT			(Remote());

	u8					flags;

	float health;
	P.r_float_q16		(health,-500,1000);
	fEntityHealth = health;

	float fDummy;
	u32 dwDummy;
	P.r_float			(fDummy);
	P.r_u32				(dwDummy);
	P.r_u32				(dwDummy);

	P.r_u32				(dwDummy);
	P.r_u8				(flags);

	float				yaw, pitch, bank = 0, roll = 0;

	P.r_angle8			(yaw);
	P.r_angle8			(yaw);
	P.r_angle8			(pitch);
	P.r_angle8			(roll);

	id_Team				= P.r_u8();
	id_Squad			= P.r_u8();
	id_Group			= P.r_u8();

	XFORM().setHPB		(yaw,pitch,bank);
}

//---------------------------------------------------------------------
void CPhantom::HitSignal	(float /**HitAmount/**/, Fvector& /**local_dir/**/, CObject* who, s16 /**element/**/)
{
	setVisible	(FALSE);
	setEnabled	(FALSE);
}
//---------------------------------------------------------------------


