
#ifndef CHARACTER_PHYSICS_SUPPORT
#define CHARACTER_PHYSICS_SUPPORT

#include "alife_space.h"

class CEntityAlive;
class CPhysicsShell;
class CPHMovementControl;
class CCharacterPhysicsSupport
{
public:
enum EType
{
	etActor,
	etStalker,
	etBitting
};

enum EState
{
	esDead,
	esAlive
};

private:
EType				m_eType;
EState				m_eState;

CEntityAlive&		m_EntityAlife;
Fmatrix&			mXFORM;
CPhysicsShell*      &m_pPhysicsShell;
CPhysicsShell*		m_physics_skeleton;
CPHMovementControl& m_PhysicMovementControl;
//skeleton

float					skel_airr_lin_factor;
float					skel_airr_ang_factor;
float					hinge_force_factor1;
float					skel_fatal_impulse_factor;
int						skel_ddelay;
bool					b_death_anim_on;
bool					b_skeleton_in_shell;
///////////////////////////////////////////////////////
float					m_saved_impulse;
Fvector					m_saved_hit_position;
Fvector					m_saved_hit_dir;
s16						m_saved_element;
ALife::EHitType			m_saved_hit_type;
/////////////////////////////////////////////////////////
float					m_shot_up_factor;
float					m_after_death_velocity_factor;

public:
EType Type()
	{
		return m_eType;
	}
EState STate()
	{
		return m_eState;
	}
void	SetState(EState astate)
	{
		m_eState=astate;
	}
IC	bool isDead()
	{
		return m_eState==esDead;
	}
IC	bool isAlive()
	{
		return !m_pPhysicsShell;
	}
public:
void Deactivate();
void Activate();
void Clear();
void Allocate();
void in_UpdateCL();
void in_shedule_Update( u32 DT );
void in_NetSpawn();
void in_NetDestroy();
void in_Init();
void in_Load(LPCSTR section);
void in_Hit(Fvector &dir, s16 element,Fvector p_in_object_space, float impulse,ALife::EHitType hit_type ,bool is_killing=false);
CCharacterPhysicsSupport(EType atype,CEntityAlive* aentity);
~CCharacterPhysicsSupport();
private:
CCharacterPhysicsSupport& operator = (CCharacterPhysicsSupport& /**asup/**/){};
void CreateSkeleton(CPhysicsShell* &pShell);
void CreateSkeleton();
void ActivateShell();
};
#endif  //CHARACTER_PHYSICS_SUPPORT