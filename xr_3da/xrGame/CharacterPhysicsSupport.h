
#ifndef CHARACTER_PHYSICS_SUPPORT
#define CHARACTER_PHYSICS_SUPPORT
class CEntityAlive;
class CPhysicsShell;
class CPHMovementControl;
class CCharacterPhysicsSupport
{
public:
enum EType
{
	etActor,
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

CEntityAlive*		m_pEntityAlife;
CPhysicsShell*		m_pPhysicsShell;
CPHMovementControl& Movement;
//skeleton
float					skel_density_factor;
float					skel_airr_lin_factor;
float					skel_airr_ang_factor;
float					hinge_force_factor;
float					hinge_force_factor1;
float					hinge_force_factor2;
float					hinge_vel;
float					skel_fatal_impulse_factor;
int						skel_ddelay;
bool					b_death_anim_on;

float					m_saved_impulse;
Fvector					m_saved_hit_position;
Fvector					m_saved_hit_dir;
s16						m_saved_element;
//PHYS
float					m_phMass;
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
		return m_eState==esAlive;
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
void in_Hit(float P, Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse);
CCharacterPhysicsSupport(EType atype,CEntityAlive* aentity);
private:
CCharacterPhysicsSupport& operator = (CCharacterPhysicsSupport& asup){};
void CreateSkeleton();
};
#endif  //CHARACTER_PHYSICS_SUPPORT