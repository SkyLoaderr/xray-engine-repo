#pragma once
#include "entity.h"
#include "PHDynamicData.h"
#include "PhysicsShell.h"
#include "script_entity.h"
#include "CarLights.h"
#include "phobject.h"
#include "holder_custom.h"
#include "PHSkeleton.h"
#include "DamagableItem.h"
#include "PHDestroyable.h"
#include "phcollisiondamagereceiver.h"
#include "CarDamageParticles.h"
#include "xrserver_objects_alife.h"
#include "CarDamageParticles.h"
#include "hit_immunity.h"
#include "Explosive.h"
// refs
class ENGINE_API			CBoneInstance;
class						CActor;
class						CInventory;
class						CSE_PHSkeleton;
// defs

class CScriptEntityAction;

class CCar : 
	public CEntity, 
	public CScriptEntity,
	public CPHUpdateObject,
	public CHolderCustom,
	public CPHSkeleton,
	public CDamagableItem,
	public CPHDestroyable,
	public CPHCollisionDamageReceiver,
	public CHitImmunity,
	public CExplosive
{
	static BONE_P_MAP bone_map; //interface for PhysicsShell
	virtual void						PhDataUpdate				(dReal step)			;
	virtual void						PhTune						(dReal step)			;
/////////////////////////////////////////////////////////////////////////
	virtual void						ApplyDamage					(u16 level)				;
	virtual	float						Health						()						{return fEntityHealth;}
	virtual CPhysicsShellHolder*		PPhysicsShellHolder			()						{return static_cast<CPhysicsShellHolder*>(this);}
	virtual CPHCollisionDamageReceiver	*PHCollisionDamageReceiver(){return static_cast<CPHCollisionDamageReceiver*>(this);}
////////////////////////////////////////////////////////////////////////
	CCarDamageParticles					m_damage_particles;
///////////////////////////////////////////////////////////////////////
protected:
	enum ECarCamType{
		ectFirst	= 0,
		ectChase,
		ectFree
	};
public:


	bool rsp,lsp,fwp,bkp,brp;
	Fmatrix m_root_transform;
	Fvector m_exit_position;

	enum eStateDrive
	{
		drive,
		neutral
	};

	eStateDrive e_state_drive;

	enum eStateSteer
	{
		right,
		idle,
		left
	};

	eStateSteer e_state_steer;

	bool b_wheels_limited;
	bool b_engine_on;
	bool b_clutch;
	bool b_starting;
	bool b_stalling;
	bool b_breaks;

	u32	 m_dwStartTime;
	float m_fuel;
	float m_fuel_tank;
	float m_fuel_consumption;
	u16	  m_driver_anim_type;

	float   m_break_start;
	float	m_break_time;
	float	m_breaks_to_back_rate;
	u32		m_death_time;
	u32		m_time_to_explode;

	struct SWheel: 
	public CDamagableHealthItem
	{
		typedef		CDamagableHealthItem inherited;
		u16 bone_id;
		bool inited;
		float radius;
		CPhysicsJoint* joint;
		CCar*	car;
		void Init();//asumptions: bone_map is 1. ini parsed 2. filled in 3. bone_id is set 
		void RestoreNetState(const CSE_ALifeCar::SWheelState& a_state);
		void SaveNetState(NET_Packet& P);
		void ApplyDriveAxisVel(float vel);
		void ApplyDriveAxisTorque(float torque);
		void ApplyDriveAxisVelTorque(float vel,float torque);
		void ApplySteerAxisVel(float vel);
		void ApplySteerAxisTorque(float torque);
		void ApplySteerAxisVelTorque(float vel,float torque);
		void SetSteerLoLimit		(float lo);
		void SetSteerHiLimit		(float hi);
		void SetSteerLimits			(float hi,float lo);

virtual void ApplyDamage			(u16 level);
		SWheel(CCar* acar)
		{
			bone_id=BI_NONE;
			car=acar;
			joint=NULL;
			inited=false;
		}
	};
	struct SWheelDrive  
	{
		SWheel* pwheel;
		float	pos_fvd;
		float gear_factor;
		void  Init();
		void  Drive();
		void  Neutral();
		void  UpdatePower();
		float ASpeed();
	};
	struct SWheelSteer 
	{
		SWheel* pwheel;
		float pos_right;
		float lo_limit;
		float hi_limit;
		float steering_velocity;
		float steering_torque;
		bool  limited;			//zero limited for idle steering drive
		float GetSteerAngle()
		{
			return -pos_right*dJointGetHinge2Angle1 (pwheel->joint->GetDJoint());
		}
		void Init();
		void SteerRight();
		void SteerLeft();
		void SteerIdle();
		void Limit();
	};
	struct SWheelBreak 
	{
		SWheel* pwheel;
		float break_torque;
		float hand_break_torque;
		void Init();
		void Break(float k);
		void HandBreak();
		void Neutral();
	};

	struct SExhaust
	{
		u16					bone_id;
		Fmatrix				transform;
		//Fvector				velocity;
		CParticlesObject*	p_pgobject;
		CPhysicsElement*	pelement;
		CCar*				pcar;
		void Init();
		void Play();
		void Stop();
		void Update();
		void Clear ();
		SExhaust(CCar* acar)
		{
			bone_id=BI_NONE;
			pcar=acar;
			p_pgobject=NULL;
			pelement=NULL;
		}
		~SExhaust();
	};

	struct SDoor;
	struct SDoor :
	public CDamagableHealthItem
	{
		typedef CDamagableHealthItem inherited;
		u16 bone_id;
		CCar* pcar;
		bool  update;
		CPhysicsJoint*  joint;
		float			torque;
		float			a_vel;
		float			pos_open;
		float			opened_angle;
		float			closed_angle;
		u32				open_time;
		struct SDoorway
		{
		Fvector2		door_plane_ext;
		_vector2<int>	door_plane_axes;
			SDoor			*door;
				 SDoorway	();
			void SPass		();
			void Init		(SDoor	*adoor);
			void Trace		(const Fvector &point,const Fvector &dir);
		};
		Fvector2		door_plane_ext;
		_vector2<int>	door_plane_axes;
		Fvector			door_dir_in_door;
		Fmatrix			closed_door_form_in_object;
		void Use();
		void Switch();
		void Init();
		void Open();
		void Close();
		void Break();
virtual void ApplyDamage(u16 level);
		void Update();
		float GetAngle();
		bool CanEnter(const Fvector& pos,const Fvector& dir,const Fvector& foot_pos);
		bool IsInArea(const Fvector& pos,const Fvector& dir);
		bool IsFront (const Fvector& pos,const Fvector& dir);
		bool CanExit(const Fvector& pos,const Fvector& dir);
		bool TestPass(const Fvector& pos,const Fvector& dir);
		//bool TestPass1(const Fvector& pos,const Fvector& dir);
		void GetExitPosition(Fvector& pos);
		void ApplyOpenTorque();
		void ApplyTorque(float atorque,float aa_vel);
		void ApplyCloseTorque();
		void NeutralTorque(float atorque);
		void ClosingToClosed();
		void ClosedToOpening();
		void PlaceInUpdate();
		void RemoveFromUpdate();
		void SaveNetState(NET_Packet& P);
		void RestoreNetState(const CSE_ALifeCar::SDoorState& a_state);
		void SetDefaultNetState();
		enum eState
		{
			opening,
			closing,
			opened,
			closed,
			broken
		};
		eState state;
		SDoor(CCar* acar)
		{
			bone_id=BI_NONE;
			pcar=acar;
			joint=NULL;
			state=closed;
			torque=500.f;
			a_vel=M_PI;
		}
	};

	struct SCarSound
	{
		ref_sound					snd_engine;
		ref_sound					snd_transmission;
		enum 
		{
			sndOff,
			sndStalling,
			sndStoping,
			sndStarting,
			sndDrive
		} eCarSound;
		void Update();
		void SwitchOff();
		void SwitchOn();
		void Init();
		void Destroy();
		void Start();
		void Stop();
		void Stall();
		void Drive();
		void TransmissionSwitch();

		SCarSound(CCar* car);
		~SCarSound();
		u32		time_state_start;
		Fvector relative_pos;
		float	 volume;
		CCar*	pcar;
	} *m_car_sound;

private:
	typedef CEntity			inherited;
private:
	float					m_steer_angle;
	bool					m_repairing;
	u16						m_bone_steer;
	CCameraBase*			camera[3];
	CCameraBase*			active_camera;

	Fvector					m_camera_position;

	////////////////////////////////////////////////////
	friend struct SWheel;
	friend struct SDoor;

	xr_map   <u16,SWheel>	m_wheels_map;
	xr_vector <SWheelDrive> m_driving_wheels;
	xr_vector <SWheelSteer> m_steering_wheels;
	xr_vector <SWheelBreak> m_breaking_wheels;
	xr_vector <SExhaust>	m_exhausts;
	shared_str					m_exhaust_particles;
	xr_map	  <u16,SDoor>	m_doors;
	xr_vector <SDoor*>		m_doors_update;
	xr_vector <Fvector>		m_gear_ratious;
	xr_vector <Fmatrix>		m_sits_transforms;// m_sits_transforms[0] - driver_place
	float					m_current_gear_ratio;

	/////////////////////////////////////////////////////////////
	bool					b_auto_switch_transmission;

	/////////////////////////////////////////////////////////////
	float					m_doors_torque_factor;
	/////////////////////////////////////////////////////////////

	float					m_max_power;//best rpm
	float					m_power_increment_factor;
	float					m_rpm_increment_factor;
	/////////////////////porabola
	float m_a,m_b,m_c;

	float					m_current_engine_power;
	float					m_current_rpm;

	float					m_axle_friction;

	float					m_fSaveMaxRPM;
	float					m_max_rpm;
	float					m_min_rpm;
	float					m_power_rpm;//max power
	float					m_torque_rpm;//max torque


	float					m_steering_speed;
	float					m_ref_radius;
	float					m_break_torque;
	float					m_hand_break_torque;
	size_t					m_current_transmission_num;
	///////////////////////////////////////////////////
	CCarLights				m_lights;
	////////////////////////////////////////////////////
	/////////////////////////////////////////////////
	void  InitParabola();
	float Parabola(float rpm);
	//float GetSteerAngle();
	void LimitWheels();
	void Drive();
	
	void Starter();

	void StartEngine();
	void StopEngine();
	void Stall();
	void Clutch();
	void Unclutch();
	void SwitchEngine();
	void NeutralDrive();
	void UpdatePower();
	void ReleasePedals();
	void ResetKeys();

	////////////////////////////////////////////////////////////////////////
	float RefWheelMaxSpeed()
	{
		return m_max_rpm/m_current_gear_ratio;
	}
	float EngineCurTorque()
	{
		return m_current_engine_power/m_current_rpm;
	}
	float RefWheelCurTorque()
	{
		return EngineCurTorque()*((m_current_gear_ratio<0.f) ? -m_current_gear_ratio : m_current_gear_ratio);
	}
	float EnginePower();
	float EngineDriveSpeed();
	float DriveWheelsMeanAngleRate();
	/////////////////////////////////////////////////////////////////////////	
	void SteerRight();
	void SteerLeft();
	void SteerIdle();
	void Transmission(size_t num);
	void CircleSwitchTransmission();
	void TransmissionUp();
	void TransmissionDown();
IC	size_t CurrentTransmission(){return m_current_transmission_num;}
	void PressRight();
	void PressLeft();
	void PressForward();
	void PressBack();
	void PressBreaks();

	void ReleaseRight();
	void ReleaseLeft();
	void ReleaseForward();
	void ReleaseBack();
	void ReleaseBreaks();

	void Revert							();

	void StartBreaking					();
	void StopBreaking					();
	void UpdateBack						();

	void HandBreak						();
	void ReleaseHandBreak				();
	void DriveForward					();
	void DriveBack						();
	void ParseDefinitions				();
	void CreateSkeleton					();//creates m_pPhysicsShell
	void Init							();

	void PlayExhausts					();
	void StopExhausts					();
	void UpdateExhausts					();
	void ClearExhausts					();
	void UpdateFuel						(float time_delta);
	float AddFuel						(float ammount); //ammount - fuel to load, ret - fuel loaded
	void CarExplode						();
	////////////////////////////////////////////////////

	void					OnCameraChange		(int type);





	bool					HUDview				( ) { return IsFocused(); }

	static void __stdcall	cb_Steer			(CBoneInstance* B);
	virtual void Hit							(float P,Fvector &dir,CObject *who,s16 element,Fvector p_in_object_space, float impulse,  ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual void PHHit							(float P,Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type/* =ALife::eHitTypeWound */);
			bool WheelHit						(float P,s16 element,ALife::EHitType hit_type);
			bool DoorHit						(float P,s16 element,ALife::EHitType hit_type);
public:
	virtual Fvector			ExitPosition		(){return m_exit_position;}
	void					GetVelocity			(Fvector& vel)	{m_pPhysicsShell->get_LinearVel(vel);}
	void					cam_Update			(float dt);
	void					detach_Actor		();
	bool					attach_Actor		(CActor* actor);
	bool					is_Door				(u16 id,xr_map<u16,SDoor>::iterator& i);
	bool					is_Door				(u16 id);
	bool					DoorOpen			(u16 id);
	bool					DoorClose			(u16 id);
	bool					DoorUse				(u16 id);
	bool					DoorSwitch			(u16 id);
	bool					Enter				(const Fvector& pos,const Fvector& dir,const Fvector& foot_pos);
	bool					Exit				(const Fvector& pos,const Fvector& dir);
	bool					Use					(const Fvector& pos,const Fvector& dir,const Fvector& foot_pos);
	u16						DriverAnimationType ();
	// Core events
	virtual DLL_Pure		*_construct			();
	virtual void			Load				( LPCSTR section );
	virtual BOOL			net_Spawn			( CSE_Abstract* DC );
	virtual void			net_Destroy			();
	virtual void			UpdateCL			( ); 
	virtual void			shedule_Update		(u32 dt);
	virtual void			renderable_Render	( ); 
	virtual	bool			bfAssignMovement	(CScriptEntityAction *tpEntityAction);
	virtual	bool			bfAssignObject		(CScriptEntityAction *tpEntityAction);

	// Network
	virtual void			net_Export			(NET_Packet& P);				// export to server
	virtual void			net_Import			(NET_Packet& P);				// import from server
	virtual BOOL			net_Relevant		()	{ return getLocal(); };		// relevant for export to server
	virtual BOOL			UsedAI_Locations	();

	// Input
	virtual void			OnMouseMove			(int x, int y);
	virtual void			OnKeyboardPress		(int dik);
	virtual void			OnKeyboardRelease	(int dik);
	virtual void			OnKeyboardHold		(int dik);
	virtual void			vfProcessInputKey	(int iCommand, bool bPressed);
	virtual void			OnEvent				( NET_Packet& P, u16 type);
	virtual void			OnAfterExplosion	();
	virtual void			OnBeforeExplosion	();
	virtual void			ResetScriptData		(void *P=0);

	// Hits
	virtual void			HitSignal			(float /**HitAmount/**/,	Fvector& /**local_dir/**/, CObject* /**who/**/, s16 /**element/**/)	{};
	virtual void			HitImpulse			(float /**amount/**/,		Fvector& /**vWorldDir/**/, Fvector& /**vLocalDir/**/)			{};
	virtual void			g_fireParams		(const CHudItem* /**pHudItem/**/, Fvector& /**P/**/, Fvector& /**D/**/)											{};

	// HUD
	virtual void			OnHUDDraw			(CCustomHUD* hud);

	CCameraBase*			Camera				(){return active_camera;}

	// Inventory for the car	
	CInventory*						GetInventory						(){return inventory;}
		  void						VisualUpdate						();
protected:
	virtual void					SpawnInitPhysics					(CSE_Abstract	*D)																;
	virtual void					net_Save							(NET_Packet& P)																	;
	virtual	BOOL					net_SaveRelevant					()																				;
			void					SaveNetState						(NET_Packet& P)																	;
	virtual	void					RestoreNetState						(CSE_PHSkeleton* po)															;
			void					SetDefaultNetState					(CSE_PHSkeleton* po)															;

	
public:
	CCar(void);
	virtual ~CCar(void);
public:
	virtual CEntity*					cast_entity				()						{return this;}
private:
	template <class T> IC void fill_wheel_vector(LPCSTR S,xr_vector<T>& type_wheels);
	IC void fill_exhaust_vector(LPCSTR S,xr_vector<SExhaust>& exhausts);
	IC void fill_doors_map(LPCSTR S,xr_map<u16,SDoor>& doors);

	//Inventory for the car
	CInventory	*inventory;
	
	virtual	void reinit			();
	virtual	void reload			(LPCSTR section);
	virtual CGameObject			*cast_game_object			()	{return this;}
	virtual CPhysicsShellHolder	*cast_physics_shell_holder	()	{return this;}
	virtual CParticlesPlayer	*cast_particles_player		()	{return this;}
	virtual CScriptEntity		*cast_script_entity			()	{return this;}
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CCar)
#undef script_type_list
#define script_type_list save_type_list(CCar)
