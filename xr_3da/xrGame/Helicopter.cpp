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
	m_curState = eIdle;

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
	CShootingObject::Load	(section);

	m_sAmmoType = pSettings->r_string(section, "ammo_class");
	m_CurrentAmmo.Load(*m_sAmmoType);

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
	CKinematics* K		= PKinematics(Visual());
	CInifile* pUserData	= K->LL_UserData();
	int id;

	m_rotate_x_bone		= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_rotate_x_bone"));
	m_rotate_y_bone		= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_rotate_y_bone"));
	m_fire_bone			= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_fire_bone"));

	LPCSTR s = pUserData->r_string("helicopter_definition","hit_section");

	if( pUserData->section_exist(s) )
	{
		int lc = pUserData->line_count(s);
		LPCSTR name;
		LPCSTR value;
		s16 boneID;
		for (int i=0 ;i<lc; ++i) 
		{
			pUserData->r_line( s, i, &name, &value);
			boneID=K->LL_BoneID(name);
			m_hitBones.insert( std::make_pair(boneID, atof(value)) );
		}
	}
	
	CBoneInstance& biX		= PKinematics(Visual())->LL_GetBoneInstance(m_rotate_x_bone);	
	biX.set_callback		(BoneMGunCallbackX,this);
	CBoneInstance& biY		= PKinematics(Visual())->LL_GetBoneInstance(m_rotate_y_bone);	
	biY.set_callback		(BoneMGunCallbackY,this);

	CSkeletonAnimated	*A= PSkeletonAnimated(Visual());
	if (A) {
		A->PlayCycle	(*heli->startup_animation);
		A->Calculate	();
	}
	m_engineSound.create(TRUE,*heli->engine_sound);
	m_engineSound.play_at_pos(0,XFORM().c,sm_Looped);

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

	m_movementMngr.onFrame( XFORM(),Device.fTimeDelta );

	m_engineSound.set_position(XFORM().c);

	//weapon
	CKinematics* K	= PKinematics(Visual());
	K->Calculate	();

	m_fire_bone_xform	= K->LL_GetTransform(m_fire_bone);

	m_fire_bone_xform.mulA(XFORM());
	m_fire_pos.set(0,0,0); 
	m_fire_bone_xform.transform_tiny(m_fire_pos);
	m_fire_dir.set(0,0,1); 
	m_fire_bone_xform.transform_dir(m_fire_dir);

	UpdateFire();

	if(Device.dwFrame == 1000)
	{
		FireStart();
	}

	if(Device.dwFrame == 10000)
	{
//		FireEnd();
	}
}

void		
CHelicopter::shedule_Update(u32	time_delta)
{
	if (!getEnabled())	return;
	inherited::shedule_Update	(time_delta);
	
	m_movementMngr.shedule_Update(time_delta);
}

void		
CHelicopter::Hit(	float P, 
					Fvector &dir, 
					CObject* who, 
					s16 element, 
					Fvector position_in_bone_space, 
					float impulse,  
					ALife::EHitType hit_type/* = ALife::eHitTypeWound*/)
{
	if(hit_type != ALife::eHitTypeWound)
		return;

	bonesIt It = m_hitBones.find(element);
	if(It != m_hitBones.end())
	{
	}
	else
		CEntity::Hit(P,dir,who,element,position_in_bone_space,impulse,hit_type );

	CGameObject* GO = dynamic_cast<CGameObject*>(who);
	if (GO){
		switch (GO->SUB_CLS_ID){
			case CLSID_OBJECT_ACTOR: 
				doHunt(who);
			break;
		default:
			break;
		}
	}

}

void					
CHelicopter::doHunt(CObject* dest)
{
	m_movementMngr.buildHuntPath(dest->XFORM().c);
}
