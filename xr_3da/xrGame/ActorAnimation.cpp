#include "stdafx.h"
#include "Actor.h"
#include "ActorAnimation.h"
#include "actor_anim_defs.h"

#include "hudmanager.h"
#include "UI.h"
#include "weapon.h"
#include "inventory.h"
#include "missile.h"
#include "level.h"
#include "Car.h"
#include "../skeletonanimated.h"
#include "clsid_game.h"
#include "ai_object_location.h"

static const float y_spin0_factor		= 0.0f;
static const float y_spin1_factor		= 0.4f;
static const float y_shoulder_factor	= 0.4f;
static const float y_head_factor		= 0.2f;
static const float p_spin0_factor		= 0.0f;
static const float p_spin1_factor		= 0.2f;
static const float p_shoulder_factor	= 0.7f;
static const float p_head_factor		= 0.1f;
static const float r_spin0_factor		= 0.3f;
static const float r_spin1_factor		= 0.3f;
static const float r_shoulder_factor	= 0.2f;
static const float r_head_factor		= 0.2f;


void __stdcall CActor::Spin0Callback(CBoneInstance* B)
{
	CActor*	A			= static_cast<CActor*>(B->Callback_Param);	VERIFY	(A);

	Fmatrix				spin;
	float				bone_yaw	= angle_normalize_signed(A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta)*y_spin0_factor;
	float				bone_pitch	= angle_normalize_signed(A->r_torso.pitch)*p_spin0_factor;
	float				bone_roll	= angle_normalize_signed(A->r_torso.roll)*r_spin0_factor;
	Fvector c			= B->mTransform.c;
	spin.setXYZ			(-bone_pitch,bone_yaw,bone_roll);
	B->mTransform.mulA_43(spin);
	B->mTransform.c		= c;
}
void __stdcall CActor::Spin1Callback(CBoneInstance* B)
{
	CActor*	A			= static_cast<CActor*>(B->Callback_Param);	VERIFY	(A);

	Fmatrix				spin;
	float				bone_yaw	= angle_normalize_signed(A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta)*y_spin1_factor;
	float				bone_pitch	= angle_normalize_signed(A->r_torso.pitch)*p_spin1_factor;
	float				bone_roll	= angle_normalize_signed(A->r_torso.roll)*r_spin1_factor;
	Fvector c			= B->mTransform.c;
	spin.setXYZ			(-bone_pitch,bone_yaw,bone_roll);
	B->mTransform.mulA_43(spin);
	B->mTransform.c		= c;
}
void __stdcall CActor::ShoulderCallback(CBoneInstance* B)
{
	CActor*	A			= static_cast<CActor*>(B->Callback_Param);	VERIFY	(A);
	Fmatrix				spin;
	float				bone_yaw	= angle_normalize_signed(A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta)*y_shoulder_factor;
	float				bone_pitch	= angle_normalize_signed(A->r_torso.pitch)*p_shoulder_factor;
	float				bone_roll	= angle_normalize_signed(A->r_torso.roll)*r_shoulder_factor;
	Fvector c			= B->mTransform.c;
	spin.setXYZ			(-bone_pitch,bone_yaw,bone_roll);
	B->mTransform.mulA_43(spin);
	B->mTransform.c		= c;
}
void __stdcall CActor::HeadCallback(CBoneInstance* B)
{
	CActor*	A			= static_cast<CActor*>(B->Callback_Param);	VERIFY	(A);
	Fmatrix				spin;
	float				bone_yaw	= angle_normalize_signed(A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta)*y_head_factor;
	float				bone_pitch	= angle_normalize_signed(A->r_torso.pitch)*p_head_factor;
	float				bone_roll	= angle_normalize_signed(A->r_torso.roll)*r_head_factor;
	Fvector c			= B->mTransform.c;
	spin.setXYZ			(-bone_pitch,bone_yaw,bone_roll);
	B->mTransform.mulA_43(spin);
	B->mTransform.c		= c;
}

void __stdcall CActor::VehicleHeadCallback(CBoneInstance* B)
{
	CActor*	A			= static_cast<CActor*>(B->Callback_Param);	VERIFY	(A);
	Fmatrix				spin;
	float				bone_yaw	= angle_normalize_signed(A->r_torso.yaw)*0.75f;
	float				bone_pitch	= angle_normalize_signed(A->r_torso.pitch)*0.75f;
	float				bone_roll	= angle_normalize_signed(A->r_torso.roll)*r_head_factor;
	Fvector c			= B->mTransform.c;
	spin.setXYZi		(-bone_pitch,-bone_yaw,bone_roll);
	B->mTransform.mulA_43(spin);
	B->mTransform.c		= c;
}

void SActorMotions::SActorState::STorsoWpn::Create(CSkeletonAnimated* K, LPCSTR base0, LPCSTR base1)
{
	char			buf[128];
	aim				= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_aim_2"));
	aim_zoom		= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_aim_0"));
	holster			= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_holster_0"));
	draw			= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_draw_0"));
	reload			= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_reload_0"));
	drop			= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_drop_0"));
	attack			= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_attack_1"));
	attack_zoom		= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_attack_0"));
	fire_idle		= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_attack_1"));
	fire_end		= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_attack_2"));

	all_attack_0	= K->ID_Cycle_Safe(strconcat(buf,base0,"_all",base1,"_attack_0"));
	all_attack_1	= K->ID_Cycle_Safe(strconcat(buf,base0,"_all",base1,"_attack_1"));
	all_attack_2	= K->ID_Cycle_Safe(strconcat(buf,base0,"_all",base1,"_attack_2"));
}
void SActorMotions::SActorState::SAnimState::Create(CSkeletonAnimated* K, LPCSTR base0, LPCSTR base1)
{
	char			buf[128];
	legs_fwd		= K->ID_Cycle(strconcat(buf,base0,base1,"_fwd_0"));
	legs_back		= K->ID_Cycle(strconcat(buf,base0,base1,"_back_0"));
	legs_ls			= K->ID_Cycle(strconcat(buf,base0,base1,"_ls_0"));
	legs_rs			= K->ID_Cycle(strconcat(buf,base0,base1,"_rs_0"));
}

void SActorMotions::SActorState::SAnimState::CreateSprint(CSkeletonAnimated* K)
{
	legs_fwd		= K->ID_Cycle("norm_escape_0");
	legs_back		= K->ID_Cycle("norm_escape_0");
	legs_ls			= K->ID_Cycle("norm_escape_ls_0");
	legs_rs			= K->ID_Cycle("norm_escape_rs_0");
}

void SActorMotions::SActorState::CreateClimb(CSkeletonAnimated* K)
{
	string128		buf,buf1;
	string16		base;
	
	//climb anims
	strcpy(base,"cl");
	legs_idle		= K->ID_Cycle(strconcat(buf,base,"_idle_0"));
	m_torso_idle	= K->ID_Cycle(strconcat(buf,base,"_torso_0_aim_0"));
	m_walk.Create	(K,base,"_run");
	m_run.Create	(K,base,"_run");

	//norm anims
	strcpy(base,"norm");
	legs_turn		= K->ID_Cycle(strconcat(buf,base,"_turn"));
	death			= K->ID_Cycle(strconcat(buf,base,"_death_0"));
	m_torso[0].Create(K,base,"_1");
	m_torso[1].Create(K,base,"_2");
	m_torso[2].Create(K,base,"_3");
	m_torso[3].Create(K,base,"_4");
	m_torso[4].Create(K,base,"_5");
	m_torso[5].Create(K,base,"_6");
	m_torso[6].Create(K,base,"_7");
	m_torso[7].Create(K,base,"_8");


	m_head_idle		= NULL;///K->ID_Cycle("head_idle_0");
	jump_begin		= K->ID_Cycle(strconcat(buf,base,"_jump_begin"));
	jump_idle		= K->ID_Cycle(strconcat(buf,base,"_jump_idle"));
	landing[0]		= K->ID_Cycle(strconcat(buf,base,"_jump_end"));
	landing[1]		= K->ID_Cycle(strconcat(buf,base,"_jump_end_1"));

	for (int k=0; k<12; ++k)
		m_damage[k]	= K->ID_FX(strconcat(buf,base,"_damage_",itoa(k,buf1,10)));
}


void SActorMotions::SActorState::Create(CSkeletonAnimated* K, LPCSTR base)
{
	string128		buf,buf1;
	legs_turn		= K->ID_Cycle(strconcat(buf,base,"_turn"));
	legs_idle		= K->ID_Cycle(strconcat(buf,base,"_idle_1"));
	death			= K->ID_Cycle(strconcat(buf,base,"_death_0"));
	
	m_walk.Create	(K,base,"_walk");
	m_run.Create	(K,base,"_run");
	m_sprint.CreateSprint(K);
	m_torso[0].Create(K,base,"_1");
	m_torso[1].Create(K,base,"_2");
	m_torso[2].Create(K,base,"_3");
	m_torso[3].Create(K,base,"_4");
	m_torso[4].Create(K,base,"_5");
	m_torso[5].Create(K,base,"_6");
	m_torso[6].Create(K,base,"_7");
	m_torso[7].Create(K,base,"_8");
	
	m_torso_idle	= K->ID_Cycle(strconcat(buf,base,"_torso_0_aim_0"));
	m_head_idle		= K->ID_Cycle("head_idle_0");
	jump_begin		= K->ID_Cycle(strconcat(buf,base,"_jump_begin"));
	jump_idle		= K->ID_Cycle(strconcat(buf,base,"_jump_idle"));
	landing[0]		= K->ID_Cycle(strconcat(buf,base,"_jump_end"));
	landing[1]		= K->ID_Cycle(strconcat(buf,base,"_jump_end_1"));

	for (int k=0; k<12; ++k)
		m_damage[k]	= K->ID_FX(strconcat(buf,base,"_damage_",itoa(k,buf1,10)));
}

void SActorMotions::Create(CSkeletonAnimated* V)
{
	//m_steering_torso_left	= V->ID_Cycle_Safe("steering_torso_ls");
	//m_steering_torso_right	= V->ID_Cycle_Safe("steering_torso_rs");
	//m_steering_torso_idle	= V->ID_Cycle_Safe("steering_torso_idle");
	//m_steering_legs_idle	= V->ID_Cycle_Safe("steering_legs_idle");
	m_dead_stop				= V->ID_Cycle("norm_dead_stop_0");

	m_normal.Create	(V,"norm");
	m_crouch.Create	(V,"cr");
	//m_climb.Create	(V,"cr");
	m_climb.CreateClimb(V);
}

SActorVehicleAnims::SActorVehicleAnims()
{
	
}
void SActorVehicleAnims::Create(CSkeletonAnimated* V)
{
	for(u16 i=0;TYPES_NUMBER>i;++i) m_vehicles_type_collections[i].Create(V,i);
}

SActorVehicleAnims::SOneTypeCollection::SOneTypeCollection()
{
	for(u16 i=0;MAX_IDLES>i;++i) idles[i]			=	0		;
	idles_num										=	0		;
	steer_left										= NULL		;
	steer_right										= NULL		;
}

void SActorVehicleAnims::SOneTypeCollection::Create(CSkeletonAnimated* V,u16 num)
{
	string128 buff,buff1,buff2;
	strconcat(buff1,itoa(num,buff,10),"_");
	steer_left=	V->ID_Cycle(strconcat(buff,"steering_idle_",buff1,"ls"));
	steer_right=V->ID_Cycle(strconcat(buff,"steering_idle_",buff1,"rs"));

	for(int i=0;MAX_IDLES>i;++i)
	{
			idles[i]=V->ID_Cycle_Safe(strconcat(buff,"steering_idle_",buff1,itoa(i,buff2,10)));
			if(idles[i]) idles_num++;
			else break;
	}
}

void CActor::steer_Vehicle(float angle)	
{
	if(!m_holder)		return;
	CCar*	car			= smart_cast<CCar*>(m_holder);
	u16 anim_type       = car->DriverAnimationType();
	SActorVehicleAnims::SOneTypeCollection& anims=m_vehicle_anims->m_vehicles_type_collections[anim_type];
	if(angle==0.f) 		smart_cast<CSkeletonAnimated*>	(Visual())->PlayCycle(anims.idles[0]);
	else if(angle>0.f)	smart_cast<CSkeletonAnimated*>	(Visual())->PlayCycle(anims.steer_right);
	else				smart_cast<CSkeletonAnimated*>	(Visual())->PlayCycle(anims.steer_left);
}

void legs_play_callback		(CBlend *blend)
{
	CActor					*object = (CActor*)blend->CallbackParam;
	VERIFY					(object);
	object->m_current_legs	= 0;
}

BOOL	g_ShowAnimationInfo = TRUE;
void CActor::g_SetAnimation( u32 mstate_rl )
{
	if (g_Alive())
	{
		SActorMotions::SActorState*				ST = 0;
		SActorMotions::SActorState::SAnimState*	AS = 0;
		
		if		(mstate_rl&mcCrouch)	ST = &m_anims->m_crouch;
		else if	(mstate_rl&mcClimb)		ST = &m_anims->m_climb;
		else							ST = &m_anims->m_normal;

		if (isAccelerated(mstate_rl))	AS = &ST->m_run;
		else							AS = &ST->m_walk;
		if	(mstate_rl&mcSprint)		
			AS  = &ST->m_sprint;
		// ��������
		CMotionDef* M_legs	= 0;
		CMotionDef* M_torso	= 0;
		CMotionDef* M_head	= 0;

		//���� �� ������ ����� �� �����
		bool is_standing = false;

		// Legs
		if		(mstate_rl&mcLanding)	M_legs	= ST->landing[0];
		else if (mstate_rl&mcLanding2)	M_legs	= ST->landing[1];
		else if ((mstate_rl&mcTurn)&&
				!(mstate_rl&mcClimb))	M_legs	= ST->legs_turn;
		else if (mstate_rl&mcFall)		M_legs	= ST->jump_idle;
		else if (mstate_rl&mcJump)		M_legs	= ST->jump_begin;
		else if (mstate_rl&mcFwd)		M_legs	= AS->legs_fwd;
		else if (mstate_rl&mcBack)		M_legs	= AS->legs_back;
		else if (mstate_rl&mcLStrafe)	M_legs	= AS->legs_ls;
		else if (mstate_rl&mcRStrafe)	M_legs	= AS->legs_rs;
		else is_standing = true;
	
		if	(mstate_rl&mcSprint)
		{
			M_torso	= M_legs;
			M_head	= M_legs;
		}
		// Torso
		if(mstate_rl&mcClimb)
		{
			if		(mstate_rl&mcFwd)		M_torso	= AS->legs_fwd;
			else if (mstate_rl&mcBack)		M_torso	= AS->legs_back;
			else if (mstate_rl&mcLStrafe)	M_torso	= AS->legs_ls;
			else if (mstate_rl&mcRStrafe)	M_torso	= AS->legs_rs;
		}
		
		if(!M_torso)
		{
			CHudItem	*H = smart_cast<CHudItem*>(inventory().ActiveItem());
			CWeapon		*W = smart_cast<CWeapon*>(inventory().ActiveItem());
			CMissile	*M = smart_cast<CMissile*>(inventory().ActiveItem());
						
			if (H) {
				SActorMotions::SActorState::STorsoWpn* TW	= &ST->m_torso[H->animation_slot() - 1];
				if (!b_DropActivated&&!fis_zero(f_DropPower)){
					M_torso					= TW->drop;
					m_bAnimTorsoPlayed		= TRUE;
				}else{
					if (!m_bAnimTorsoPlayed) {
						if (W) {
							bool K =inventory().ActiveItem()->object().CLS_ID == CLSID_OBJECT_W_KNIFE;
							
							if(K)
							{
								switch (W->STATE){
								case CWeapon::eIdle:	M_torso	= TW->aim;		break;
								
								case CWeapon::eFire:	
									if(is_standing)
										M_torso = M_legs = M_head = TW->all_attack_0;
									else
										M_torso	= TW->attack_zoom;
									break;
								case CWeapon::eFire2:
									if(is_standing)
										M_torso = M_legs = M_head = TW->all_attack_1;
									else
										M_torso	= TW->fire_idle;
									break;
								case CWeapon::eReload:	M_torso	= TW->reload;	break;
								case CWeapon::eShowing:	M_torso	= TW->draw;		break;
								case CWeapon::eHiding:	M_torso	= TW->holster;	break;
								default				 :  M_torso	= TW->aim;		break;
								}
							}
							else
							{
								switch (W->STATE){
								case CWeapon::eIdle:	M_torso	= W->IsZoomed()?TW->aim_zoom:TW->aim;		break;
								case CWeapon::eFire:	M_torso	= W->IsZoomed()?TW->attack_zoom:TW->attack;	break;
								case CWeapon::eFire2:	M_torso	= W->IsZoomed()?TW->attack_zoom:TW->attack; break;
								case CWeapon::eReload:	M_torso	= TW->reload;	break;
								case CWeapon::eShowing:	M_torso	= TW->draw;		break;
								case CWeapon::eHiding:	M_torso	= TW->holster;	break;
								default				 :  M_torso	= TW->aim;		break;
								}
							}
						}
						else if (M) {
							if(is_standing)
							{
								switch (M->State()){
								case MS_SHOWING	 :		M_torso	= TW->draw;			break;
								case MS_HIDING	 :		M_torso	= TW->holster;		break;
								case MS_IDLE	 :		M_torso	= TW->aim_zoom;		break;
								case MS_EMPTY	 :		M_torso	= TW->aim_zoom;		break;
								case MS_THREATEN :		M_torso = M_legs = M_head = TW->all_attack_0;	break;
								case MS_READY	 :		M_torso = M_legs = M_head = TW->all_attack_1;	break;
								case MS_THROW	 :		M_torso = M_legs = M_head = TW->all_attack_2;	break;
								case MS_END		 :		M_torso = M_legs = M_head = TW->all_attack_2;	break;
								default			 :		M_torso	= TW->draw;			break; 
								}
							}
							else
							{
								switch (M->State()){
								case MS_SHOWING	 :		M_torso	= TW->draw;			break;
								case MS_HIDING	 :		M_torso	= TW->holster;		break;
								case MS_IDLE	 :		M_torso	= TW->aim_zoom;		break;
								case MS_EMPTY	 :		M_torso	= TW->aim_zoom;		break;
								case MS_THREATEN :		M_torso	= TW->attack_zoom;	break;
								case MS_READY	 :		M_torso	= TW->fire_idle;	break;
								case MS_THROW	 :		M_torso	= TW->fire_end;		break;
								case MS_END		 :		M_torso	= TW->fire_end;		break;
								default			 :		M_torso	= TW->draw;			break; 
								}
							}
						}
					}
				}
			}
		}

		if (!M_legs)
		{
			if((mstate_rl&mcCrouch)&&isAccelerated(mstate_rl))//!(mstate_rl&mcAccel))
			{
				M_legs=smart_cast<CSkeletonAnimated*>(Visual())->ID_Cycle("cr_idle_0");
			}
			else 
				M_legs	= ST->legs_idle;
		}
		if (!M_head)					M_head	= ST->m_head_idle;
		if (!M_torso){				
			if (m_bAnimTorsoPlayed)		M_torso	= m_current_torso;
			else						M_torso = ST->m_torso_idle;
		}
		
		// ���� �������� ��� ����� - �������� / ����� �������� �������� �� ������
		if (m_current_torso!=M_torso){
			if (m_bAnimTorsoPlayed)		smart_cast<CSkeletonAnimated*>	(Visual())->PlayCycle(M_torso,TRUE,AnimTorsoPlayCallBack,this);
			else						smart_cast<CSkeletonAnimated*>	(Visual())->PlayCycle(M_torso);
			m_current_torso=M_torso;
		}
		if(m_current_head!=M_head)
		{
			if(M_head)smart_cast<CSkeletonAnimated*>(Visual())->PlayCycle(M_head);
			m_current_head=M_head;
		}
		if (m_current_legs!=M_legs){
			m_current_legs_blend = smart_cast<CSkeletonAnimated*>(Visual())->PlayCycle(M_legs,TRUE,legs_play_callback,this);
			m_current_legs=M_legs;
		}


	}else{
		if (m_current_legs||m_current_torso){
			SActorMotions::SActorState* ST = 0;
			if (mstate_rl&mcCrouch)	ST = &m_anims->m_crouch;
			else					ST = &m_anims->m_normal;
			mstate_real			= 0;
			m_current_legs		= 0;
			m_current_torso		= 0;

			smart_cast<CSkeletonAnimated*>(Visual())->PlayCycle(m_anims->m_dead_stop);
		}
	}
//#ifdef DEBUG
//	if ((Level().CurrentControlEntity() == this) && g_ShowAnimationInfo) 
//	{
//
//		HUD().Font().pFontSmall->SetColor	(0xffffffff);
//		HUD().Font().pFontSmall->OutSet	(170,450);
//		HUD().Font().pFontSmall->OutNext	("LUMINOCITY: [%f]",ROS()->get_luminocity());
//	};
//#endif

#ifdef _DEBUG
	if ((Level().CurrentControlEntity() == this) && g_ShowAnimationInfo) {
		string128 buf;
		strcpy(buf,"");
		if (isAccelerated(mstate_rl))		strcat(buf,"Accel ");
		if (mstate_rl&mcCrouch)		strcat(buf,"Crouch ");
		if (mstate_rl&mcFwd)		strcat(buf,"Fwd ");
		if (mstate_rl&mcBack)		strcat(buf,"Back ");
		if (mstate_rl&mcLStrafe)	strcat(buf,"LStrafe ");
		if (mstate_rl&mcRStrafe)	strcat(buf,"RStrafe ");
		if (mstate_rl&mcJump)		strcat(buf,"Jump ");
		if (mstate_rl&mcFall)		strcat(buf,"Fall ");
		if (mstate_rl&mcTurn)		strcat(buf,"Turn ");
		if (mstate_rl&mcLanding)	strcat(buf,"Landing ");
		if (mstate_rl&mcLLookout)	strcat(buf,"LLookout ");
		if (mstate_rl&mcRLookout)	strcat(buf,"RLookout ");
		if (m_bJumpKeyPressed)		strcat(buf,"+Jumping ");
		HUD().Font().pFontSmall->OutNext	("MSTATE:     [%s]",buf);
		//	if (buf[0]) 
		//		Msg("%s",buf);
		switch (m_PhysicMovementControl->Environment())
		{
		case CPHMovementControl::peOnGround:	strcpy(buf,"ground");			break;
		case CPHMovementControl::peInAir:		strcpy(buf,"air");				break;
		case CPHMovementControl::peAtWall:		strcpy(buf,"wall");				break;
		}
		HUD().Font().pFontSmall->OutNext	(buf);
		HUD().Font().pFontSmall->OutNext	("Accel     [%3.2f, %3.2f, %3.2f]",VPUSH(NET_SavedAccel));
		HUD().Font().pFontSmall->OutNext	("V         [%3.2f, %3.2f, %3.2f]",VPUSH(m_PhysicMovementControl->GetVelocity()));
		HUD().Font().pFontSmall->OutNext	("vertex ID   %d",ai_location().level_vertex_id());
		
		strcpy(buf,"");
		if (m_iCurWeaponHideState & (1<<0)) strcat(buf, "INVENTORY_MENU_OPEN ");
		if (m_iCurWeaponHideState & (1<<1)) strcat(buf, "BUY_MENU_OPEN ");
		if (m_iCurWeaponHideState & (1<<2)) strcat(buf, "DEACTIVATE_CURRENT_SLOT ");
		if (m_iCurWeaponHideState & (1<<3)) strcat(buf, "SPRINT ");
		HUD().Font().pFontSmall->OutNext	("WeaponHideState:     [%s]",buf);
	};
#endif
}

void CActor::g_SetSprintAnimation( u32 mstate_rl )
{

}