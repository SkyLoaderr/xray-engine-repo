#pragma once
#include "entity.h"
#include "PHDynamicData.h"
#include "PhysicsShell.h"


// refs
class ENGINE_API			CBoneInstance;
class						CActor;

// defs


class CCar : public CEntity,
	public CPHObject
{
	static BONE_P_MAP bone_map; //interface for PhysicsShell

	xr_list<CPHObject*>::iterator m_ident;
	virtual void PhDataUpdate(dReal step){};
	virtual void PhTune(dReal step);
	virtual void InitContact(dContact* c){};
	virtual void StepFrameUpdate(dReal step){};
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
	u32	 m_dwStartTime;
	float m_fuel;
	float m_fuel_tank;
	float m_fuel_consumption;
	struct SWheel 
	{
		int bone_id;
		bool inited;
		float radius;
		dJointID joint;
		CCar*	car;
		void Init();//asumptions: bone_map is 1. ini parsed 2. filled in 3. bone_id is set 
		SWheel(CCar* acar)
		{
			bone_id=-1;
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
		float ASpeed();
		void UpdatePower();
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
			return -pos_right*dJointGetHinge2Angle1 (pwheel->joint);
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
		void Break();
		void HandBreak();
		void Neutral();
	};

	struct SExhaust
	{
		int					bone_id;
		Fmatrix				transform;
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
			bone_id=-1;
			pcar=acar;
			p_pgobject=NULL;
			pelement=NULL;
		}
		~SExhaust();
	};

	struct SDoor;
	struct SDoor 
	{
		int bone_id;
		CCar* pcar;
		bool  update;
		CPhysicsJoint*  joint;
		float			torque;
		float			a_vel;
		float			pos_open;
		float			opened_angle;
		float			closed_angle;
		u32				open_time;
		Fvector2		door_plane_ext;
		_vector2<int>	door_plane_axes;
		Fvector			door_dir_in_door;
		Fmatrix			closed_door_form_in_object;
		void Use();
		void Init();
		void Open();
		void Close();
		void Update();
		float GetAngle();
		bool CanEnter(const Fvector& pos,const Fvector& dir,const Fvector& foot_pos);
		bool IsInArea(const Fvector& pos);
		bool CanExit(const Fvector& pos,const Fvector& dir);
		bool TestPass(const Fvector& pos,const Fvector& dir);
		void GetExitPosition(Fvector& pos);
		void ApplyOpenTorque();
		void ApplyTorque(float atorque,float aa_vel);
		void ApplyCloseTorque();
		void NeutralTorque(float atorque);
		void ClosingToClosed();
		void ClosedToOpening();
		void PlaceInUpdate();
		void RemoveFromUpdate();
		enum eState
		{
			opening,
			closing,
			opened,
			closed
		};
		eState state;
		SDoor(CCar* acar)
		{
			bone_id=-1;
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
		SCarSound(CCar* car);
		~SCarSound();
		u32 time_state_start;
		float volume;
		CCar* pcar;
	} *m_car_sound;

private:
	typedef CEntity			inherited;
private:
	float					m_steer_angle;
	bool					m_repairing;
	CCameraBase*			camera[3];
	CCameraBase*			active_camera;

	Fvector					m_camera_position;

	CActor*					m_owner;
	////////////////////////////////////////////////////
	friend struct SWheel;
	friend struct SDoor;

	xr_map   <int,SWheel>	m_wheels_map;
	xr_vector <SWheelDrive> m_driving_wheels;
	xr_vector <SWheelSteer> m_steering_wheels;
	xr_vector <SWheelBreak> m_breaking_wheels;
	xr_vector <SExhaust>	m_exhausts;
	xr_map	  <int,SDoor>	m_doors;
	xr_vector <SDoor*>		m_doors_update;
	//xr_list   <SDoor*>		m_doors_opened;
	xr_vector <float>		m_gear_ratious;
	xr_vector <Fmatrix>		m_sits_transforms;// m_sits_transforms[0] - driver_place
	float					m_current_gear_ratio;

	/////////////////////////////////////////////////////////////
	bool					b_auto_switch_transmission;
	Fvector2				m_auto_switch_rpm;//lo,hi
	/////////////////////////////////////////////////////////////


	float					m_max_power;//best rpm

	/////////////////////porabola
	float m_a,m_b,m_c;

	float					m_current_engine_power;
	float					m_current_rpm;
	float					m_axle_friction;

	float					m_max_rpm;
	float					m_min_rpm;
	float					m_power_rpm;//max power
	float					m_torque_rpm;//max torque


	float					m_steering_speed;
	float					m_ref_radius;
	float					m_break_torque;
	float					m_hand_break_torque;
	size_t					m_current_transmission_num;

	////////////////////////////////////////////////////
	/////////////////////////////////////////////////
	void  InitParabola();
	float Parabola(float rpm);
	float GetSteerAngle();
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
	/////////////////////////////////////////////////////////////////////////	
	void SteerRight();
	void SteerLeft();
	void SteerIdle();
	void Transmision(size_t num);
	void CircleSwitchTransmission();
	void TransmisionUp();
	void TransmisionDown();
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

	void Revert();
	void Break();
	void Unbreak();
	void ParseDefinitions				();
	void CreateSkeleton					();//creates m_pPhysicsShell
	void Init							();

	void PlayExhausts					();
	void StopExhausts					();
	void UpdateExhausts					();
	void ClearExhausts					();
	void UpdateFuel						(float time_delta);
	float AddFuel						(float ammount); //ammount - fuel to load, ret - fuel loaded
	////////////////////////////////////////////////////

	void					OnCameraChange		(int type);





	bool					HUDview				( ) { return IsFocused(); }

	static void __stdcall	cb_Steer			(CBoneInstance* B);
	virtual void Hit(float P,Fvector &dir,CObject *who,s16 element,Fvector p_in_object_space, float impulse,  ALife::EHitType hit_type = eHitTypeWound);
public:
	Fvector&				ExitPosition		(){return m_exit_position;}
	void					GetVelocity			(Fvector& vel)	{m_pPhysicsShell->get_LinearVel(vel);}
	void					cam_Update			(float dt);
	void					detach_Actor		();
	bool					attach_Actor		(CActor* actor);
	bool					is_Door				(int id,xr_map<int,SDoor>::iterator& i);
	bool					Enter				(const Fvector& pos,const Fvector& dir,const Fvector& foot_pos);
	bool					Exit				(const Fvector& pos,const Fvector& dir);
	bool					Use					(int id,const Fvector& pos,const Fvector& dir,const Fvector& foot_pos);
	// Core events
	virtual void			Load				( LPCSTR section );
	virtual BOOL			net_Spawn			( LPVOID DC );
	virtual void			net_Destroy			();
	virtual void			UpdateCL			( ); 
#ifdef DEBUG
	virtual void			shedule_Update		(u32 dt);
#endif

	virtual void			renderable_Render			( ); 

	// Network
	virtual void			net_Export			(NET_Packet& P);				// export to server
	virtual void			net_Import			(NET_Packet& P);				// import from server
	virtual BOOL			net_Relevant		()	{ return getLocal(); };		// relevant for export to server

	// Input
	virtual void			IR_OnMouseMove			(int x, int y);
	virtual void			IR_OnKeyboardPress		(int dik);
	virtual void			IR_OnKeyboardRelease	(int dik);
	virtual void			IR_OnKeyboardHold		(int dik);

	// Hits
	virtual void			HitSignal			(float HitAmount,	Fvector& local_dir, CObject* who, s16 element)	{};
	virtual void			HitImpulse			(float amount,		Fvector& vWorldDir, Fvector& vLocalDir)			{};
	virtual void			Die					()																	{};
	virtual void			g_fireParams		(Fvector& P, Fvector& D)											{};

	// HUD
	virtual void			OnHUDDraw			(CCustomHUD* hud);

	CCameraBase*			Camera				(){return active_camera;}

public:
	CCar(void);
	virtual ~CCar(void);
private:
	template <class T> IC void fill_wheel_vector(LPCSTR S,xr_vector<T>& type_wheels)
	{
		CKinematics* pKinematics	=PKinematics(Visual());
		string64					S1;
		int count =					_GetItemCount(S);
		for (int i=0 ;i<count; i++) 
		{
			_GetItem					(S,i,S1);

			int bone_id	=				pKinematics->LL_BoneID(S1);

			type_wheels.push_back		(T());
			T& twheel				= type_wheels.back();


			BONE_P_PAIR_IT J		= bone_map.find(bone_id);
			if (J == bone_map.end()) 
			{
				bone_map.insert(mk_pair(bone_id,physicsBone()));


				SWheel& wheel			=	(m_wheels_map.insert(mk_pair(bone_id,SWheel(this)))).first->second;
				wheel.bone_id			=	bone_id;
				twheel.pwheel			=	&wheel;
			}
			else
			{
				twheel.pwheel			=	&(m_wheels_map.find(bone_id))->second;
			}
		}
	}
	IC void fill_exhaust_vector(LPCSTR S,xr_vector<SExhaust>& exhausts)
	{
		CKinematics* pKinematics	=PKinematics(Visual());
		string64					S1;
		int count =					_GetItemCount(S);
		for (int i=0 ;i<count; i++) 
		{
			_GetItem					(S,i,S1);

			int bone_id	=				pKinematics->LL_BoneID(S1);

			exhausts.push_back		(SExhaust(this));
			SExhaust& exhaust				= exhausts.back();
			exhaust.bone_id						= bone_id;

			BONE_P_PAIR_IT J		= bone_map.find(bone_id);
			if (J == bone_map.end()) 
			{
				bone_map.insert(mk_pair(bone_id,physicsBone()));
			}

		}
	}
	IC void fill_doors_map(LPCSTR S,xr_map<int,SDoor>& doors)
	{
		CKinematics* pKinematics	=PKinematics(Visual());
		string64					S1;
		int count =					_GetItemCount(S);
		for (int i=0 ;i<count; i++) 
		{
			_GetItem					(S,i,S1);

			int bone_id	=				pKinematics->LL_BoneID(S1);
			SDoor						door(this);
			door.bone_id=				bone_id;
			doors.insert				(mk_pair(bone_id,door));
			BONE_P_PAIR_IT J		= bone_map.find(bone_id);
			if (J == bone_map.end()) 
			{
				bone_map.insert(mk_pair(bone_id,physicsBone()));
			}

		}
	}
};



