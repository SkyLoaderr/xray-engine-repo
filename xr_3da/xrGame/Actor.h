// Actor.h: interface for the CActor class.
//
//////////////////////////////////////////////////////////////////////
 
#if !defined(AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_)
#define AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_
#pragma once

#include "actor_flags.h"
#include "actor_defs.h"

#include "entity_alive.h"
#include "../feel_touch.h"
#include "PHMovementControl.h"
#include "PhysicsShell.h"
#include "InventoryOwner.h"
#include "ActorCondition.h"
#include "damage_manager.h"
#include "material_manager.h"
#include "StatGraph.h"

using namespace ACTOR_DEFS;


// refs
class ENGINE_API CCameraBase;
class ENGINE_API CBoneInstance;
class ENGINE_API CMotionDef;
class ENGINE_API CBlend;
class CWeaponList;
class CEffectorBobbing;
class CTargetCS;
class CHolderCustom;

struct SShootingEffector;
struct SSleepEffector;
class  CSleepEffectorPP;

class  CActorEffector;

class	CHudItem;

class CActor: 
	public CEntityAlive, 
	public Feel::Touch,
	public CInventoryOwner,
	public CActorCondition,
	public CDamageManager,
	public CMaterialManager
#ifdef DEBUG
	,public pureRender
#endif
{
private:
	typedef CEntityAlive	inherited;
	//////////////////////////////////////////////////////////////////////////
	// General fucntions
	//////////////////////////////////////////////////////////////////////////
public:
										CActor				();
	virtual								~CActor				();

	virtual void						Load				( LPCSTR section );

	virtual void						shedule_Update		( u32 T ); 
	virtual void						UpdateCL			( );
	
	virtual void						OnEvent				( NET_Packet& P, u16 type		);

	// Render
	virtual void						renderable_Render			();
	virtual BOOL						renderable_ShadowGenerate	();

#ifdef DEBUG
	virtual void						OnRender			();
#endif


	/////////////////////////////////////////////////////////////////
	// Inventory Owner 

	//information receive
	virtual void OnReceiveInfo		(int info_index);
	virtual void ReceivePdaMessage	(u16 who, EPdaMsg msg, int info_index);

	virtual void reinit			();
	virtual void reload			(LPCSTR section);
	virtual bool use_bolts		() const;

	virtual void OnItemTake		(CInventoryItem *inventory_item);
	virtual void OnItemDrop		(CInventoryItem *inventory_item);

	/////////////////////////////////////////////////////////////////
	// condition and hits
	virtual void						Die				( );
	virtual	void						Hit				(float P, Fvector &dir,			CObject* who, s16 element, Fvector position_in_bone_space, float impulse,  ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual void						HitSignal		(float P, Fvector &vLocalDir,	CObject* who, s16 element);
	virtual	float						HitScale		(int element);
	
	/////////////////////////////////////////////////////////////////
	// misc properties
	virtual float						GetMass				() { return g_Alive()?m_PhysicMovementControl->GetMass():m_pPhysicsShell?m_pPhysicsShell->getMass():0; }
	virtual float						Radius				() const;
	virtual void						g_PerformDrop		();


	virtual bool						NeedToDestroyObject()  const;
	virtual ALife::_TIME_ID				TimePassedAfterDeath() const;


	////////////////////////////////////////////////////////////////////
	//Actor condition
	////////////////////////////////////////////////////////////////////
public:
	virtual CWound* ConditionHit(CObject* who, float hit_power, ALife::EHitType hit_type, s16 element = 0);
	virtual void	UpdateCondition();

	//сон
	virtual void	GoSleep(u32	sleep_time);
	virtual void	Awoke();
			void	UpdateSleep();

protected:
	//Sleep params
	//время когда актера надо разбудить
	ALife::_TIME_ID			m_dwWakeUpTime;
	float					m_fOldTimeFactor;
	float					m_fOldOnlineRadius;
	float					m_fSleepTimeFactor;

	/////////////////////////////////////////////////////////////////
	// misc properties
protected:
	// Weapons and Items
	CTargetCS*				m_pArtifact;

	// Death
	float					hit_slowmo;
	float					hit_factor;
	bool					bDeathInit;

	// media
	BOOL					bStep;
	ref_sound				sndLanding;
	ref_sound				sndZoneHeart;
	ref_sound				sndZoneDetector;
	ref_sound				sndHit[SND_HIT_COUNT];
	ref_sound				sndDie[SND_DIE_COUNT];


	float					m_fLandingTime;
	float					m_fJumpTime;
	float					m_fFallTime;
	float					m_fCamHeightFactor;

	// Dropping
	BOOL					b_DropActivated;
	float					f_DropPower;

	//random seed для Zoom mode
	s32						m_ZoomRndSeed;

	//разрешения на удаление трупа актера 
	//после того как контролирующий его игрок зареспавнился заново. 
	//устанавливается в game
public:
	bool					m_bAllowDeathRemove;

	////////////////////////////////////////////////////////
	void					SetZoomRndSeed			(s32 Seed = 0);
	s32						GetZoomRndSeed			()	{ return m_ZoomRndSeed;	};
	/////////////////////////////////////////////////////////
	// car usage
	/////////////////////////////////////////////////////////
public:
	void					detach_Vehicle			();
	void					steer_Vehicle			(float angle);
protected:
	CHolderCustom*			m_holder;
	void					attach_Vehicle			(CHolderCustom* vehicle);
	bool					use_Vehicle				(CGameObject* object);
	bool					use_MountedWeapon		(CGameObject* object);
	void					ActorUse				();


	/////////////////////////////////////////////////////////
	// actor model & animations
	/////////////////////////////////////////////////////////
protected:
	BOOL					m_bAnimTorsoPlayed;
	static void				AnimTorsoPlayCallBack(CBlend* B)
	{
		CActor* actor		= (CActor*)B->CallbackParam;
		actor->m_bAnimTorsoPlayed = FALSE;
	}

	// skeleton
	static	float			skel_airr_lin_factor;
	static	float			skel_airr_ang_factor;
	static	float			hinge_force_factor1;
	static	float			skel_fatal_impulse_factor;

	// Rotation
	SRotation				r_torso;
	
	//положение торса без воздействия эффекта отдачи оружия
	float					unaffected_r_torso_yaw;
	float					unaffected_r_torso_pitch;

	//ориентация модели
	float					r_model_yaw_dest;
	float					r_model_yaw;			// orientation of model
	float					r_model_yaw_delta;		// effect on multiple "strafe"+"something"

	void					create_Skeleton			();
public:
	SActorMotions			m_anims;

	CBlend*					m_current_legs_blend;
	CBlend*					m_current_torso_blend;
	CBlend*					m_current_jump_blend;
	CMotionDef*				m_current_legs;
	CMotionDef*				m_current_torso;

	// callback на анимации модели актера
	static void	__stdcall	SpinCallback		(CBoneInstance*);
	static void	__stdcall	ShoulderCallback	(CBoneInstance*);
	static void	__stdcall	HeadCallback		(CBoneInstance*);
	static void __stdcall	VehicleHeadCallback	(CBoneInstance*);

	virtual const SRotation	Orientation			()	const	{ return r_torso; };
	SRotation				&Orientation		()			 { return r_torso; };

	void					g_SetAnimation		(u32 mstate_rl);
	//////////////////////////////////////////////////////////////////////////
	// HUD
	//////////////////////////////////////////////////////////////////////////
public:
	virtual void			OnHUDDraw			(CCustomHUD* hud);
	IC		BOOL			HUDview				( )const 
	{ 
		return IsFocused()&&(cam_active==eacFirstEye)&&(!m_holder); 
	}

	//visiblity 
	virtual	float			ffGetFov			()	const	{ return 90.f;		}	
	virtual	float			ffGetRange			()	const	{ return 500.f;		}

	
	//////////////////////////////////////////////////////////////////////////
	// Cameras and effectors
	//////////////////////////////////////////////////////////////////////////
public:
	CActorEffector&			EffectorManager		() 	{return *m_pActorEffector;}
	IC CCameraBase*			cam_Active			()	{return cameras[cam_active];}
protected:
	void					cam_Set					(EActorCameras style);
	void					cam_Update				(float dt, float fFOV);

	// Cameras
	CCameraBase*			cameras[eacMaxCam];
	EActorCameras			cam_active;
	float					cam_gray;				// for zone-effects
	float					cam_shift;				// for zone-effects
	float					fPrevCamPos;
	CEffectorBobbing*		pCamBobbing;

	void					LoadShootingEffector	(LPCSTR section);
	SShootingEffector*		m_pShootingEffector;

	void					LoadSleepEffector		(LPCSTR section);
	SSleepEffector*			m_pSleepEffector;
	CSleepEffectorPP*		m_pSleepEffectorPP;

	//менеджер эффекторов, есть у каждого актрера
	CActorEffector*			m_pActorEffector;

	////////////////////////////////////////////
	// для взаимодействия с другими персонажами 
	// или предметами
	///////////////////////////////////////////
public:
	virtual void			feel_touch_new				(CObject* O);
	virtual void			feel_touch_delete			(CObject* O);
	virtual BOOL			feel_touch_contact			(CObject* O);

	CGameObject*			ObjectWeLookingAt			() {return m_pObjectWeLookingAt;}
	EActorAction			GetDefaultActionForObject	() {return m_eDefaultObjAction;}

protected:

	// Person we're looking at
	CInventoryOwner*		m_pPersonWeLookingAt;
	// Vehicle or lorry we're looking at
	CHolderCustom*			m_pVehicleWeLookingAt;
	CGameObject*			m_pObjectWeLookingAt;
	// Default action for object we're looking at
	EActorAction			m_eDefaultObjAction;

	//режим подбирания предметов
	bool					m_bPickupMode;
	//расстояние подсветки предметов
	float					m_fPickupInfoRadius;

	void					PickupModeOn		();
	void					PickupModeOff		();
	void					PickupModeUpdate	();
	void					PickupInfoDraw		(CObject* object);


	//////////////////////////////////////////////////////////////////////////
	// Motions (передвижения актрера)
	//////////////////////////////////////////////////////////////////////////
public:
	void					g_cl_CheckControls		(u32 mstate_wf, Fvector &vControlAccel, float &Jump, float dt);
	void					g_cl_ValidateMState		(float dt, u32 mstate_wf);
	void					g_cl_Orientate			(u32 mstate_rl, float dt);
	void					g_sv_Orientate			(u32 mstate_rl, float dt);
	void					g_Orientate				(u32 mstate_rl, float dt);

	static bool				isAccelerated			(u32 mstate);
	bool					CanAccelerate			();
	bool					CanJump					();

protected:
	u32						mstate_wishful;	
	u32						mstate_real;

	BOOL					m_bJumpKeyPressed;

	float					m_fWalkAccel;
	float					m_fJumpSpeed;
	float					m_fRunFactor;
	float					m_fCrouchFactor;
	float					m_fClimbFactor;
	
	//////////////////////////////////////////////////////////////////////////
	// User input/output
	//////////////////////////////////////////////////////////////////////////
public:
	virtual void			IR_OnMouseMove			(int x, int y);
	virtual void			IR_OnKeyboardPress		(int dik);
	virtual void			IR_OnKeyboardRelease	(int dik);
	virtual void			IR_OnKeyboardHold		(int dik);


	//////////////////////////////////////////////////////////////////////////
	// Weapon fire control (оружие актрера)
	//////////////////////////////////////////////////////////////////////////
public:
	virtual void						g_WeaponBones		(int &L, int &R1, int &R2);
	virtual void						g_fireParams		(const CHudItem* pHudItem, Fvector& P, Fvector& D);
	virtual BOOL						g_State				(SEntityState& state) const;
	virtual	float						GetWeaponAccuracy	() const;
			bool						IsZoomAimingMode	() const {return m_bZoomAimingMode;}

protected:
	//если актер целится в прицел
	bool								m_bZoomAimingMode;

	//настройки аккуратности стрельбы
	//базовая дисперсия (когда игрок стоит на месте)
	float								m_fDispBase;
	//коэффициенты на сколько процентов увеличится базовая дисперсия
	//учитывает скорость актера 
	float								m_fDispVelFactor;
	//если актер бежит
	float								m_fDispAccelFactor;
	//если актер сидит
	float								m_fDispCrouchFactor;

	//смещение firepoint относительно default firepoint для бросания болтов и гранат
	Fvector								m_vMissileOffset;
public:
	// Получение, и запись смещения для гранат
	Fvector								GetMissileOffset	() const;
	void								SetMissileOffset	(const Fvector &vNewOffset);

protected:
	//косточки используемые при стрельбе
	int									m_r_hand;
	int									m_l_finger1;
    int									m_r_finger2;



	//////////////////////////////////////////////////////////////////////////
	// Network
	//////////////////////////////////////////////////////////////////////////
public:
	virtual BOOL						net_Spawn			( LPVOID DC);
	virtual void						net_Export			( NET_Packet& P);				// export to server
	virtual void						net_Import			( NET_Packet& P);				// import from server
	virtual void						net_Destroy			();
	virtual void						net_ImportInput		( NET_Packet& P);				// import input from remote client
	virtual BOOL						net_Relevant		()	{ return getSVU() | getLocal(); };		// relevant for export to server
	virtual	void						net_Relcase			( CObject* O );					//
protected:
	xr_deque<net_update>	NET;
	Fvector					NET_SavedAccel;
	net_update				NET_Last;
	BOOL					NET_WasInterpolating;	// previous update was by interpolation or by extrapolation
	u32						NET_Time;				// server time of last update

	//---------------------------------------------
	void					net_Import_Base				( NET_Packet& P);
	void					net_Import_Physic			( NET_Packet& P);
	void					net_Import_Base_proceed		( );
	void					net_Import_Physic_proceed	( );
	//---------------------------------------------
	xr_deque<net_input>		NET_InputStack;
	void					NetInput_Save			( );
	void					NetInput_Send			( );
	void					NetInput_Apply			(net_input* pNI);
	void					NetInput_Update			( u32 Time );
	
	xr_deque<SMemoryPos>	SMemoryPosStack;
	Fvector					dDesyncVec;
	SMemoryPos*				FindMemoryPos (u32 Time);

	///////////////////////////////////////////////////////
	// апдайт с данными физики
	xr_deque<net_update_A>	NET_A;

	void					Set_Level_CrPr			( long dTime );
	//---------------------------------------------
	bool					m_bHasUpdate;	
	
	SPHNetState				RecalculatedState;
	SPHNetState				PredictedState;

//	Fvector					IStartPos;
//	Fvector					IEndPos;
	InterpData				IStart;
	InterpData				IEnd;
	
	bool					m_bInInterpolation;
	u32						m_dwLastUpdateTime;
	u32						m_dwIStartTime;
	u32						m_dwIEndTime;
	bool					m_bInterpolate;
	//---------------------------------------------
	virtual void			make_Interpolation ();

	
	//////////////////////////////////////////////////////////////////////////
	// Actor physics
	//////////////////////////////////////////////////////////////////////////
public:
			void			g_Physics		(Fvector& accel, float jump, float dt);
	virtual void			ForceTransform	(const Fmatrix &m);
			void			SetPhPosition	(const Fmatrix& pos);

	virtual void			PH_B_CrPr		(); // actions & operations before physic correction-prediction steps
	virtual void			PH_I_CrPr		(); // actions & operations after correction before prediction steps
	virtual void			PH_A_CrPr		(); // actions & operations after phisic correction-prediction steps
	virtual void			UpdatePosStack	( u32 Time0, u32 Time1 );

	virtual void			SpawnAmmoForWeapon		(CInventoryItem *pIItem);
	virtual void			RemoveAmmoForWeapon		(CInventoryItem *pIItem);

protected:
	int						skel_ddelay;

	Fvector					m_saved_dir;
	Fvector					m_saved_position;
	float					m_saved_impulse;
	s16						m_saved_element;


	/////////////////////////////////////////
	// DEBUG INFO
protected:
		CStatGraph				*pStatGraph;
#ifdef DEBUG
		friend class CLevelGraph;
#endif

};

extern float	g_fNumUpdates;
extern const float	s_fFallTime;
#endif // !defined(AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_)
