
#ifndef CHARACTER_PHYSICS_SUPPORT
#define CHARACTER_PHYSICS_SUPPORT

#include "alife_space.h"
#include "PHSkeleton.h"
#include "Entity_Alive.h"
#include "PHDestroyable.h"

class CPhysicsShell;
class CPHMovementControl;
class CCharacterPhysicsSupport :
public CPHSkeleton,
public CPHDestroyable
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
	esAlive,
	esRemoved
};

private:
	EType								m_eType;
	EState								m_eState;

	CEntityAlive						&m_EntityAlife																																		;
	Fmatrix								&mXFORM																																				;
	CPhysicsShell						*&m_pPhysicsShell																																	;
	CPhysicsShell						*m_physics_skeleton																																	;
	CPHMovementControl					&m_PhysicMovementControl																															;
//skeleton
	float								skel_airr_lin_factor																																;
	float								skel_airr_ang_factor																																;
	float								hinge_force_factor1																																	;
	float								skel_fatal_impulse_factor																															;
	int									skel_ddelay																																			;
	bool								b_death_anim_on																																		;
	bool								b_skeleton_in_shell																																	;
///////////////////////////////////////////////////////////////////////////
	float								m_saved_impulse																																		;
	Fvector								m_saved_hit_position																																;
	Fvector								m_saved_hit_dir																																		;
	s16									m_saved_element																																		;
	ALife::EHitType						m_saved_hit_type																																	;
///////////////////////////////////////////////////////////////////////////
	float								m_shot_up_factor																																	;
	float								m_after_death_velocity_factor																														;

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
protected:
virtual void							SpawnInitPhysics				(CSE_Abstract	*D)																									;
virtual CPhysicsShellHolder*			PPhysicsShellHolder				()	{return m_EntityAlife.PhysicsShellHolder();}	
virtual bool							CanRemoveObject					();
public:

		void							SetRemoved						();
		bool							IsRemoved						(){return m_eState==esRemoved;}

//////////////////base hierarchi methods///////////////////////////////////////////////////
		void 							in_UpdateCL()																																		;
		void 							in_shedule_Update				( u32 DT )																											;
		void 							in_NetSpawn						(CSE_Abstract* e)																									;
		void 							in_NetDestroy					()																													;
		void 							in_Init							()																													;
		void 							in_Load							(LPCSTR section)																									;
		void 							in_Hit							(float P,Fvector &dir, CObject *who, s16 element,Fvector p_in_object_space, float impulse,ALife::EHitType hit_type ,bool is_killing=false);
		void							in_NetSave						(NET_Packet& P)																										;
		void							in_ChangeVisual					();
		void							PHGetLinearVell					(Fvector& velocity);
/////////////////////////////////////////////////////////////////
		CCharacterPhysicsSupport& operator = (CCharacterPhysicsSupport& /**asup/**/){R_ASSERT2(false,"Can not assign it");}
								CCharacterPhysicsSupport				(EType atype,CEntityAlive* aentity)																					;
virtual							~CCharacterPhysicsSupport				()																													;
private:
		void 							CreateSkeleton					(CPhysicsShell* &pShell)																							;
		void 							CreateSkeleton					();
		void 							ActivateShell					(CObject* who)																										;
};
#endif  //CHARACTER_PHYSICS_SUPPORT