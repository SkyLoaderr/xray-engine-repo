// Actor.h: interface for the CActor class.
//
//////////////////////////////////////////////////////////////////////
 
#if !defined(AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_)
#define AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_
#pragma once

#include "../feel_touch.h"
#include "../iinputreceiver.h"
#include "actor_flags.h"
#include "actor_defs.h"
#include "entity_alive.h"
#include "PHMovementControl.h"
#include "PhysicsShell.h"
#include "InventoryOwner.h"
#include "StatGraph.h"
#include "PhraseDialogManager.h"
#include "CharacterPhysicsSupport.h"
using namespace ACTOR_DEFS;

class CInfoPortion;
struct GAME_NEWS_DATA;
class CActorCondition;

class CKnownContactsRegistryWrapper;
class CEncyclopediaRegistryWrapper;
class CGameTaskRegistryWrapper;
class CGameNewsRegistryWrapper;
class CCharacterPhysicsSupport;
// refs
class ENGINE_API CCameraBase;
class ENGINE_API CBoneInstance;
class ENGINE_API CMotionDef;
class ENGINE_API CBlend;
class CWeaponList;
class CEffectorBobbing;
class CHolderCustom;
class CUsableScriptObject;

struct SShootingEffector;
struct SSleepEffector;
class  CSleepEffectorPP;

class  CActorEffector;

class	CHudItem;
class   CArtefact;

struct SActorMotions;
struct SActorVehicleAnims;
class  CActorCondition;
class CMaterialManager;

class	CActor: 
	public CEntityAlive, 
	public IInputReceiver,
	public Feel::Touch,
	public CInventoryOwner,
	public CPhraseDialogManager
#ifdef DEBUG
	,public pureRender
#endif
{
	friend class CActorCondition;
private:
	typedef CEntityAlive	inherited;
	//////////////////////////////////////////////////////////////////////////
	// General fucntions
	//////////////////////////////////////////////////////////////////////////
public:
										CActor				();
	virtual								~CActor				();

public:
	virtual CInventoryOwner*			cast_inventory_owner		()						{return this;}
	virtual CActor*						cast_actor					()						{return this;}
	virtual CGameObject*				cast_game_object			()						{return this;}
	virtual IInputReceiver*				cast_input_receiver			()						{return this;}
	virtual	CCharacterPhysicsSupport*	character_physics_support	()						{return m_pPhysics_support;}
	virtual	CCharacterPhysicsSupport*	character_physics_support	() const				{return m_pPhysics_support;}
	virtual CPHDestroyable*				ph_destroyable				()						;
			CHolderCustom*				Holder						()						{return m_holder;}
public:

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

public:
	//information receive & dialogs
	virtual bool OnReceiveInfo		(INFO_INDEX info_index) const;
	virtual void OnDisableInfo		(INFO_INDEX info_index) const;
	virtual void ReceivePdaMessage	(u16 who, EPdaMsg msg, INFO_INDEX info_index);

	virtual void	 NewPdaContact		(CInventoryOwner*);
	virtual void	 LostPdaContact		(CInventoryOwner*);

protected:
//	virtual void AddMapLocationsFromInfo (const CInfoPortion* info_portion) const;
	virtual void AddEncyclopediaArticle	 (const CInfoPortion* info_portion) const;
	virtual void AddGameTask			 (const CInfoPortion* info_portion) const;

protected:
struct SDefNewsMsg{
		GAME_NEWS_DATA*	news_data;
		u32				time;
		bool operator < (const SDefNewsMsg& other){return time>other.time;}
	};
	xr_vector<SDefNewsMsg> m_defferedMessages;
	void UpdateDefferedMessages();	
public:	
	void			AddGameNews_deffered	 (GAME_NEWS_DATA& news_data, u32 delay);
	virtual void	AddGameNews				 (GAME_NEWS_DATA& news_data);
	
public:
	virtual void StartTalk			(CInventoryOwner* talk_partner);
	virtual	void UpdateContact		(u16 contact_id);
	virtual	void RunTalkDialog		(CInventoryOwner* talk_partner);
	
	CKnownContactsRegistryWrapper	*contacts_registry;
	CEncyclopediaRegistryWrapper	*encyclopedia_registry;
	CGameTaskRegistryWrapper		*game_task_registry;
	CGameNewsRegistryWrapper		*game_news_registry;
	CCharacterPhysicsSupport		*m_pPhysics_support;
	//игровое имя 
	virtual LPCSTR	Name        () const {return CInventoryOwner::Name();}

public:
	//PhraseDialogManager
	virtual void ReceivePhrase			(DIALOG_SHARED_PTR& phrase_dialog);
	virtual void UpdateAvailableDialogs	(CPhraseDialogManager* partner);
	virtual void TryToTalk				();

	virtual void reinit			();
	virtual void reload			(LPCSTR section);
	virtual bool use_bolts		() const;

	virtual void OnItemTake		(CInventoryItem *inventory_item);
	
	virtual void OnItemRuck		(CInventoryItem *inventory_item, EItemPlace previous_place);
	virtual void OnItemBelt		(CInventoryItem *inventory_item, EItemPlace previous_place);
	
	virtual void OnItemDrop		(CInventoryItem *inventory_item);
	virtual void OnItemDropUpdate ();

	/////////////////////////////////////////////////////////////////
	// condition and hits
	virtual void						Die				(CObject* who);
	virtual	void						Hit				(float P, Fvector &dir,			CObject* who, s16 element, Fvector position_in_bone_space, float impulse,  ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual	void						PHHit			(float P,Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type /* = ALife::eHitTypeWound */);
	virtual void						HitSignal		(float P, Fvector &vLocalDir,	CObject* who, s16 element);
			bool						ActivateBox		(DWORD id);
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
	//сон
			void		UpdateSleep			();

	//свойства артефактов
	virtual void		UpdateArtefactsOnBelt	();
	virtual void		MoveArtefactBelt		(const CArtefact* artefact, bool on_belt);
	virtual float		HitArtefactsOnBelt		(float hit_power, ALife::EHitType hit_type);
protected:
	//звук тяжелого дыхания
	ref_sound			m_HeavyBreathSnd;
	bool				m_bHeavyBreathSndPlaying;

	xr_vector<const CArtefact*> m_ArtefactsOnBelt;

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
	// Death
	float					hit_slowmo;


	// media
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
	//random seed для Weapon Effector Shot
	s32						m_ShotRndSeed;

	//разрешения на удаление трупа актера 
	//после того как контролирующий его игрок зареспавнился заново. 
	//устанавливается в game
public:
	bool					m_bAllowDeathRemove;
//	u32						m_u32RespawnTime;

	////////////////////////////////////////////////////////
	void					SetZoomRndSeed			(s32 Seed = 0);
	s32						GetZoomRndSeed			()	{ return m_ZoomRndSeed;	};
	////////////////////////////////////////////////////////
	void					SetShotRndSeed			(s32 Seed = 0);
	s32						GetShotRndSeed			()	{ return m_ShotRndSeed;	};
	/////////////////////////////////////////////////////////
	// car usage
	/////////////////////////////////////////////////////////
public:
	void					detach_Vehicle			();
	void					steer_Vehicle			(float angle);
	void					attach_Vehicle			(CHolderCustom* vehicle);
protected:
	CHolderCustom*			m_holder;
	u16						m_holderID;
	bool					use_Vehicle				(CPhysicsShellHolder* object);
	bool					use_MountedWeapon		(CPhysicsShellHolder* object);
	void					ActorUse				();


	/////////////////////////////////////////////////////////
	// actor model & animations
	/////////////////////////////////////////////////////////
protected:
	BOOL					m_bAnimTorsoPlayed;
	static void				AnimTorsoPlayCallBack(CBlend* B);

	// Rotation
	SRotation				r_torso;
	float					r_torso_tgt_roll;
	//положение торса без воздействия эффекта отдачи оружия
	SRotation				unaffected_r_torso;

	//ориентация модели
	float					r_model_yaw_dest;
	float					r_model_yaw;			// orientation of model
	float					r_model_yaw_delta;		// effect on multiple "strafe"+"something"


public:
	SActorMotions*			m_anims;
	SActorVehicleAnims*		m_vehicle_anims;

	CBlend*					m_current_legs_blend;
	CBlend*					m_current_torso_blend;
	CBlend*					m_current_jump_blend;
	CMotionDef*				m_current_legs;
	CMotionDef*				m_current_torso;
	CMotionDef*				m_current_head;

	// callback на анимации модели актера
	void					SetCallbacks		();
	void					ResetCallbacks		();
	static void	__stdcall	Spin0Callback		(CBoneInstance*);
	static void	__stdcall	Spin1Callback		(CBoneInstance*);
	static void	__stdcall	ShoulderCallback	(CBoneInstance*);
	static void	__stdcall	HeadCallback		(CBoneInstance*);
	static void __stdcall	VehicleHeadCallback	(CBoneInstance*);

	virtual const SRotation	Orientation			()	const	{ return r_torso; };
	SRotation				&Orientation		()			 { return r_torso; };

	void					g_SetAnimation		(u32 mstate_rl);
	void					g_SetSprintAnimation(u32 mstate_rl,CMotionDef* &head,CMotionDef* &toroso,CMotionDef* &legs);
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
	CActorEffector&			EffectorManager		() 	{VERIFY(m_pActorEffector); return *m_pActorEffector;}
	IC CCameraBase*			cam_Active			()	{return cameras[cam_active];}
	IC CCameraBase*			cam_FirstEye		()	{return cameras[eacFirstEye];}

protected:
	void					cam_Set					(EActorCameras style);
	void					cam_Update				(float dt, float fFOV);
	void					cam_SetLadder			();
	void					cam_UnsetLadder			();

	// Cameras
	CCameraBase*			cameras[eacMaxCam];
	EActorCameras			cam_active;
	float					fPrevCamPos;
	CEffectorBobbing*		pCamBobbing;

	void					LoadShootingEffector	(LPCSTR section);
	SShootingEffector*		m_pShootingEffector;

	void					LoadSleepEffector		(LPCSTR section);
	SSleepEffector*			m_pSleepEffector;
	CSleepEffectorPP*		m_pSleepEffectorPP;

	//менеджер эффекторов, есть у каждого актрера
	CActorEffector*			m_pActorEffector;
	static float			f_Ladder_cam_limit;
	////////////////////////////////////////////
	// для взаимодействия с другими персонажами 
	// или предметами
	///////////////////////////////////////////
public:
	virtual void			feel_touch_new				(CObject* O);
	virtual void			feel_touch_delete			(CObject* O);
	virtual BOOL			feel_touch_contact			(CObject* O);
	virtual BOOL			feel_touch_on_contact		(CObject* O);

	CGameObject*			ObjectWeLookingAt			() {return m_pObjectWeLookingAt;}
	CInventoryOwner*		PersonWeLookingAt			() {return m_pPersonWeLookingAt;}
	LPCSTR					GetDefaultActionForObject	() {return *m_sDefaultObjAction;}
protected:

	CUsableScriptObject*	m_pUsableObject;
	// Person we're looking at
	CInventoryOwner*		m_pPersonWeLookingAt;
	// Vehicle or lorry we're looking at
	CHolderCustom*			m_pVehicleWeLookingAt;
	CGameObject*			m_pObjectWeLookingAt;
	// Tip for action for object we're looking at
	shared_str				m_sDefaultObjAction;

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
	bool					g_LadderOrient			() ;
	static bool				isAccelerated			(u32 mstate);
	void					UpdateMotionIcon		(u32 mstate_rl);

	bool					CanAccelerate			();
	bool					CanJump					();
	bool					CanMove					();
	float					CameraHeight			();
	bool					CanSprint				();
protected:
	u32						mstate_wishful;
	u32						mstate_old;
	u32						mstate_real;

	BOOL					m_bJumpKeyPressed;

	float					m_fWalkAccel;
	float					m_fJumpSpeed;
	float					m_fRunFactor;
	float					m_fRunBackFactor;
	float					m_fWalkBackFactor;
	float					m_fCrouchFactor;
	float					m_fClimbFactor;
	float					m_fSprintFactor;
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
	int									m_head;



	//////////////////////////////////////////////////////////////////////////
	// Network
	//////////////////////////////////////////////////////////////////////////
public:
	virtual BOOL						net_Spawn			( CSE_Abstract* DC);
	virtual void						net_Export			( NET_Packet& P);				// export to server
	virtual void						net_Import			( NET_Packet& P);				// import from server
	virtual void						net_Destroy			();
	virtual void						net_ImportInput		( NET_Packet& P);				// import input from remote client
	virtual BOOL						net_Relevant		();//	{ return getSVU() | getLocal(); };		// relevant for export to server
	virtual	void						net_Relcase			( CObject* O );					//
	virtual void						on_reguested_spawn  (CObject *object);
	//object serialization
	virtual void						save				(NET_Packet &output_packet);
	virtual void						load				(IReader &input_packet);
	virtual void						net_Save			(NET_Packet& P)																	;
	virtual	BOOL						net_SaveRelevant	()																				;
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
	
	///////////////////////////////////////////////////////
	// апдайт с данными физики
	xr_deque<net_update_A>	NET_A;
	
	//---------------------------------------------
//	bool					m_bHasUpdate;	
	/// spline coeff /////////////////////
	float			SCoeff[3][4];			//коэффициэнты для сплайна Бизье
	float			HCoeff[3][4];			//коэффициэнты для сплайна Эрмита
	Fvector			IPosS, IPosH, IPosL;	//положение актера после интерполяции Бизье, Эрмита, линейной

#ifdef DEBUG
	DEF_DEQUE		(VIS_POSITION, Fvector);

	VIS_POSITION	LastPosS;
	VIS_POSITION	LastPosH;
	VIS_POSITION	LastPosL;
#endif

	
	SPHNetState				LastState;
	SPHNetState				RecalculatedState;
	SPHNetState				PredictedState;
	
	InterpData				IStart;
	InterpData				IRec;
	InterpData				IEnd;
	
	bool					m_bInInterpolation;
	bool					m_bInterpolate;
	u32						m_dwIStartTime;
	u32						m_dwIEndTime;
	u32						m_dwILastUpdateTime;

	//---------------------------------------------
	DEF_DEQUE		(PH_STATES, SPHNetState);
	PH_STATES				m_States;
	u16						m_u16NumBones;
	void					net_ExportDeadBody		(NET_Packet &P);
	//---------------------------------------------
	void					CalculateInterpolationParams();
	//---------------------------------------------
	virtual void			make_Interpolation ();
#ifdef DEBUG
	//---------------------------------------------
	virtual void			OnRender_Network();
	//---------------------------------------------
#endif

	ref_geom 				hFriendlyIndicator;
	//////////////////////////////////////////////////////////////////////////
	// Actor physics
	//////////////////////////////////////////////////////////////////////////
public:
			void			g_Physics		(Fvector& accel, float jump, float dt);
	virtual void			ForceTransform	(const Fmatrix &m);
			void			SetPhPosition	(const Fmatrix& pos);
	virtual CPHMovementControl*	movement_control(){return inherited::movement_control();}
	virtual void			PH_B_CrPr		(); // actions & operations before physic correction-prediction steps
	virtual void			PH_I_CrPr		(); // actions & operations after correction before prediction steps
	virtual void			PH_A_CrPr		(); // actions & operations after phisic correction-prediction steps
//	virtual void			UpdatePosStack	( u32 Time0, u32 Time1 );

	virtual void			SpawnAmmoForWeapon		(CInventoryItem *pIItem);
	virtual void			RemoveAmmoForWeapon		(CInventoryItem *pIItem);
	virtual	void			spawn_supplies			();
	virtual bool			human_being				() const
	{
		return				(true);
	}

	virtual	shared_str			GetDefaultVisualOutfit	() const	{return m_DefaultVisualOutfit;};
	virtual	void			SetDefaultVisualOutfit	(shared_str DefaultOutfit) {m_DefaultVisualOutfit = DefaultOutfit;};
	virtual void			UpdateAnimation			() 	{ g_SetAnimation(mstate_real); };

	virtual void			ChangeVisual			( shared_str NewVisual );
	virtual void			OnChangeVisual			();

	virtual void			RenderIndicator			(Fvector dpos, float r1, float r2, ref_shader IndShader);

	//////////////////////////////////////////////////////////////////////////
	// Controlled Routines
	//////////////////////////////////////////////////////////////////////////
public:	
			bool			IsControlled			() const {return m_controlled;}
			void			SetControlled			(bool b_controlled = true) {m_controlled = b_controlled; if (!m_controlled) m_controlled_mouse_scale_factor = 1.0f; else mstate_wishful = 0;}
			void			SetMouseScaleFactor		(float value) {m_controlled_mouse_scale_factor = ((value > 0)? value : 1.0f); }

private:
			bool			m_controlled;
			float			m_controlled_mouse_scale_factor;

	/////////////////////////////////////////
	// DEBUG INFO
protected:
		CStatGraph				*pStatGraph;
		u32						m_dwStartKickTime;

		shared_str				m_DefaultVisualOutfit;

		LPCSTR					invincibility_fire_shield_3rd;
		LPCSTR					invincibility_fire_shield_1st;
		u32						last_hit_frame;

		LPCSTR					m_spawn_effect;
#ifdef DEBUG
		friend class CLevelGraph;
#endif
		u32								m_iCurWeaponHideState;
		void							Check_Weapon_ShowHideState	();

		Fvector							m_AutoPickUp_AABB;
		Fvector							m_AutoPickUp_AABB_Offset;

		void							Check_for_AutoPickUp	();
		void							SelectBestWeapon	();
public:
		virtual void					HideCurrentWeapon		(u32 Msg);//, bool only2handed);
		virtual	void					RestoreHidedWeapon		(u32 Msg);

private:
	CActorCondition				*m_entity_condition;
	CMaterialManager			*m_material_manager;

protected:
	virtual	CEntityCondition	*create_entity_condition	();

public:
	IC		CActorCondition		&conditions					() const;
	IC		CMaterialManager	&material					() const;
	virtual DLL_Pure			*_construct					();
	virtual bool				natural_weapon				() const {return false;}
	virtual bool				natural_detector			() const {return false;}
	virtual bool				use_center_to_aim			() const {return true;}
};

IC	CActorCondition	&CActor::conditions	() const
{
	VERIFY			(m_entity_condition);
	return			(*m_entity_condition);
}

IC	CMaterialManager	&CActor::material	() const
{
	VERIFY			(m_material_manager);
	return			(*m_material_manager);
}

//extern float	g_fNumUpdates;
extern const float	s_fFallTime;
#endif // !defined(AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_)
