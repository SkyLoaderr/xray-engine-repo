
#ifndef CHARACTER_PHYSICS_SUPPORT
#define CHARACTER_PHYSICS_SUPPORT

#include "alife_space.h"
#include "PHSkeleton.h"
#include "Entity_Alive.h"
#include "PHSoundPlayer.h"
#include "Phdestroyable.h"
class CPhysicsShell;
class CPHMovementControl;
class CIKLimbsController;
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
	Flags8								m_flags;
	enum Fags 
	{
		fl_death_anim_on			=1<<0,
		fl_skeleton_in_shell		=1<<1,
		fl_specific_bonce_demager	=1<<2
	};
	CEntityAlive						&m_EntityAlife																																		;
	Fmatrix								&mXFORM																																				;
	CPhysicsShell						*&m_pPhysicsShell																																	;
	CPhysicsShell						*m_physics_skeleton																																	;
	CPHMovementControl					*m_PhysicMovementControl																																;
	CPHSoundPlayer						m_ph_sound_player																																	;
	CIKLimbsController					*m_ik_controller																																	;
	SCollisionHitCallback				*m_collision_hit_callback;
//skeleton modell(!share?)
	float								skel_airr_lin_factor																																;
	float								skel_airr_ang_factor																																;
	float								hinge_force_factor1																																	;
	float								skel_fatal_impulse_factor																															;
	int									skel_ddelay																																			;
/////////////////////////////////////////////////
	//bool								b_death_anim_on																																		;
	//bool								b_skeleton_in_shell																																	;
///////////////////////////////////////////////////////////////////////////
	float								m_shot_up_factor																																	;
	float								m_after_death_velocity_factor																														;
	float								m_BonceDamageFactor																																	;
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
IC		CPHMovementControl				*movement						()	{return m_PhysicMovementControl;}
		CPHSoundPlayer					*ph_sound_player				()	{return &m_ph_sound_player;}
		void							SetRemoved						();
		bool							IsRemoved						(){return m_eState==esRemoved;}
		bool							IsSpecificDamager				()																{return !!m_flags.test(fl_specific_bonce_demager)	;}
		float							BonceDamageFactor				(){return m_BonceDamageFactor;}
//////////////////base hierarchi methods///////////////////////////////////////////////////
		void 							in_UpdateCL()																																		;
		void 							in_shedule_Update				( u32 DT )																											;
		void 							in_NetSpawn						(CSE_Abstract* e)																									;
		void 							in_NetDestroy					()																													;
		void							in_NetRelcase					(CObject* O)																										;
		void 							in_Init							()																													;
		void 							in_Load							(LPCSTR section)																									;
		void 							in_Hit							(float P,Fvector &dir, CObject *who, s16 element,Fvector p_in_object_space, float impulse,ALife::EHitType hit_type ,bool is_killing=false);
		void							in_NetSave						(NET_Packet& P)																										;
		void							in_ChangeVisual					();
		void							PHGetLinearVell					(Fvector& velocity);
		SCollisionHitCallback*			get_collision_hit_callback		();
		bool							set_collision_hit_callback		(SCollisionHitCallback* cc);
/////////////////////////////////////////////////////////////////
		CCharacterPhysicsSupport& operator = (CCharacterPhysicsSupport& /**asup/**/){R_ASSERT2(false,"Can not assign it");}
								CCharacterPhysicsSupport				(EType atype,CEntityAlive* aentity)																					;
virtual							~CCharacterPhysicsSupport				()																													;
private:
		void 							CreateSkeleton					(CPhysicsShell* &pShell)																							;
		void 							CreateSkeleton					();
		void 							ActivateShell					(CObject* who)																										;
static void _stdcall 					IKVisualCallback				(CKinematics* K)																									;
		void							CreateIKController				()																													;
		void							DestroyIKController				()																													;
		void							CalculateIK						(CKinematics* K)																									;
		void							CollisionCorrectObjPos			(const Fvector& start_from);
};
#endif  //CHARACTER_PHYSICS_SUPPORT