////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_animations.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Animations, Bone transformations and Sounds for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "ai_stalker_animations.h"
#include "../../../motion.h"
#include "../../weapon.h"
#include "../../missile.h"
#include "../../ai_script_actions.h"
#include "../../inventory.h"
#include "../../fooditem.h"

static const float y_spin_factor			= 0.25f;
static const float y_shoulder_factor		= 0.25f;
static const float y_head_factor			= 0.5f;
static const float p_spin_factor			= 0.2f;
static const float p_shoulder_factor		= 0.7f;
static const float p_head_factor			= 0.1f;

static const float y_spin_fire_factor		= 0.5f;
static const float y_shoulder_fire_factor	= 0.5f;
static const float y_head_fire_factor		= 0.0f;
static const float p_spin_fire_factor		= 0.2f;
static const float p_shoulder_fire_factor	= 0.7f;
static const float p_head_fire_factor		= 0.1f;

float faTurnAngles			[] = {
	0.f,
	M_PI,
	PI_DIV_6,
	0.f,
};

LPCSTR caStateNames			[] = {
	"cr_",
	"norm_",
	"norm_dmg_",
	0
};

LPCSTR caWeaponNames		[] = {
	"0_",
	"1_",
	"2_",
	"3_",
	"4_",
	"5_",
	"6_",
	0
};

LPCSTR caWeaponActionNames	[] = {
	"draw_",		// 0
	"attack_",		// 1
	"drop_",		// 2
	"holster_",		// 3
	"reload_",		// 4
	"pick_",		// 5
	"aim_",			// 6
	"walk_",		// 7
	"run_",			// 8
	"idle_",		// 9
	"escape_",		// 10
	"prepare",		// 11
	"playing",		// 12
	0
};

LPCSTR caMovementNames		[] = {
	"walk_",
	"run_",
	0
};

LPCSTR caMovementActionNames[] = {
	"fwd_",
	"back_",
	"ls_",
	"rs_",
	0
};

LPCSTR caInPlaceNames		[] = {
	"idle_1",
	"turn",
	"jump_begin",
	"jump_idle",
	"jump_end",
	"jump_end_1",
	0
};

LPCSTR caGlobalNames		[] = {
	"damage_",
	"escape_",
	"dead_stop_",
	0
};

void __stdcall CStalkerAnimations::HeadCallback(CBoneInstance *B)
{
	CAI_Stalker*			A = dynamic_cast<CAI_Stalker*> (static_cast<CObject*>(B->Callback_Param));
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	float					yaw_factor = 0, pitch_factor = 0;
	if (A->use_torso_look()) {
		yaw_factor			= y_head_fire_factor;
		pitch_factor		= p_head_fire_factor;
	}
	else {
		yaw_factor			= y_head_factor;
		pitch_factor		= p_head_factor;
	}
	float					yaw		= angle_normalize_signed(-yaw_factor * angle_normalize_signed(A->NET_Last.o_torso.yaw - A->NET_Last.o_model));
	float					pitch	= angle_normalize_signed(-pitch_factor * (A->NET_Last.o_torso.pitch));
	spin.setXYZ				(pitch, yaw, 0);
	VERIFY					(_valid(spin));
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void __stdcall CStalkerAnimations::ShoulderCallback(CBoneInstance *B)
{
	CAI_Stalker*			A = dynamic_cast<CAI_Stalker*> (static_cast<CObject*>(B->Callback_Param));
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	float					yaw_factor = 0, pitch_factor = 0;
	if (A->use_torso_look()) {
		yaw_factor			= y_shoulder_fire_factor;
		pitch_factor		= p_shoulder_fire_factor;
	}
	else {
		yaw_factor			= y_shoulder_factor;
		pitch_factor		= p_shoulder_factor;
	}
	float					yaw		= angle_normalize_signed(-yaw_factor * angle_normalize_signed(A->NET_Last.o_torso.yaw - A->NET_Last.o_model));
	float					pitch	= angle_normalize_signed(-pitch_factor * (A->NET_Last.o_torso.pitch));
	spin.setXYZ				(pitch, yaw, 0);
	VERIFY					(_valid(spin));
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void __stdcall CStalkerAnimations::SpinCallback(CBoneInstance *B)
{
	CAI_Stalker*			A = dynamic_cast<CAI_Stalker*> (static_cast<CObject*>(B->Callback_Param));
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	float					yaw_factor = 0, pitch_factor = 0;
	if (A->use_torso_look()) {
		yaw_factor			= y_spin_fire_factor;
		pitch_factor		= p_spin_fire_factor;
	}
	else {
		yaw_factor			= y_spin_factor;
		pitch_factor		= p_spin_factor;
	}
	float					yaw		= angle_normalize_signed(-yaw_factor * angle_normalize_signed(A->NET_Last.o_torso.yaw - A->NET_Last.o_model));
	float					pitch	= angle_normalize_signed(-pitch_factor * (A->NET_Last.o_torso.pitch));
	spin.setXYZ				(pitch, yaw, 0);
	VERIFY					(_valid(spin));
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void CStalkerAnimations::vfAssignGlobalAnimation(CMotionDef *&tpGlobalAnimation)
{
	CAI_Stalker				*stalker = dynamic_cast<CAI_Stalker*>(this);
	VERIFY					(stalker);
	if (!stalker->g_Alive())
		return;

	if ((eMentalStatePanic == stalker->mental_state()) && (stalker->speed() > EPS_L)) {
		tpGlobalAnimation	= m_tAnims.A[stalker->IsLimping() ? eBodyStateStandDamaged : eBodyStateStand].m_tGlobal.A[1].A[0];
		return;
	}

	CFoodItem				*food_item = dynamic_cast<CFoodItem*>(stalker->inventory().ActiveItem());
	if (food_item) {
		u32					dwCurrentAniSlot = u32(-1);
		switch (food_item->SUB_CLS_ID) {
			case CLSID_IITEM_BOTTLE : {
				dwCurrentAniSlot = 0;
				break;
			}
			default : NODEFAULT;
		}
		switch (food_item->STATE) {
			case FOOD_SHOWING: {
				tpGlobalAnimation = m_tAnims.A[eBodyStateStand].m_tGlobalItem.A[dwCurrentAniSlot].A[0].A[0];
				break;
			}
			case FOOD_HIDING : {
				tpGlobalAnimation = m_tAnims.A[eBodyStateStand].m_tGlobalItem.A[dwCurrentAniSlot].A[3].A[0];
				break;
			}
			case FOOD_IDLE	 : {
				tpGlobalAnimation = m_tAnims.A[eBodyStateStand].m_tGlobalItem.A[dwCurrentAniSlot].A[9].A[0];
				break;
			}
			case FOOD_PREPARE: {
				tpGlobalAnimation = m_tAnims.A[eBodyStateStand].m_tGlobalItem.A[dwCurrentAniSlot].A[11].A[0];
				break;
			}
			case FOOD_EATING : {
				tpGlobalAnimation = m_tAnims.A[eBodyStateStand].m_tGlobalItem.A[dwCurrentAniSlot].A[1].A[0];
				break;
			}
			case FOOD_PLAYING : {
				tpGlobalAnimation = m_tAnims.A[eBodyStateStand].m_tGlobalItem.A[dwCurrentAniSlot].A[12].A[0];
				break;
			}
			default			 : NODEFAULT;
		}
	}
}

void CStalkerAnimations::vfAssignTorsoAnimation(CMotionDef *&tpTorsoAnimation)
{
	CAI_Stalker				*stalker = dynamic_cast<CAI_Stalker*>(this);
	VERIFY					(stalker);
	if (!stalker->g_Alive())
		return;

	CWeapon					*tpWeapon	= dynamic_cast<CWeapon*>(stalker->inventory().ActiveItem());
	CMissile				*missile	= dynamic_cast<CMissile*>(stalker->inventory().ActiveItem());
	CLASS_ID				clsid = tpWeapon ? tpWeapon->SUB_CLS_ID : missile ? missile->SUB_CLS_ID : 0;
	u32						dwCurrentAniSlot = 0;
	if (tpWeapon || missile)
		switch (clsid) {
			case CLSID_OBJECT_W_VINTOREZ	:
			case CLSID_OBJECT_W_VAL			:
			case CLSID_OBJECT_W_GROZA		:
			case CLSID_OBJECT_W_M134		:
			case CLSID_OBJECT_W_FN2000		:
			case CLSID_OBJECT_W_AK74		:
			case CLSID_OBJECT_W_LR300		: 
			case CLSID_OBJECT_W_SVU			:
			case CLSID_OBJECT_W_SVD			: {
				dwCurrentAniSlot = 2;
				break;
			}
			case CLSID_OBJECT_W_WALTHER		:
			case CLSID_OBJECT_W_USP45		:
			case CLSID_OBJECT_W_HPSA		:
			case CLSID_OBJECT_W_PM			:
			case CLSID_OBJECT_W_FORT		: {
				dwCurrentAniSlot = 1;
				break;
			}
			case CLSID_OBJECT_W_BINOCULAR	: {
//				dwCurrentAniSlot = 5;
				dwCurrentAniSlot = 1;
				break;
			}
			case CLSID_OBJECT_W_SHOTGUN		: {
				dwCurrentAniSlot = 3;
				break;
			}
			case CLSID_OBJECT_W_RPG7		: {
				dwCurrentAniSlot = 2;
				break;
			}
			case CLSID_GRENADE_F1		:
			case CLSID_IITEM_BOLT		:
			case CLSID_GRENADE_RGD5		: {
				dwCurrentAniSlot = 6;
				break;
			}
			default : {
#pragma todo("Dima to Dima : Return nodefault")
				dwCurrentAniSlot = 1;
				//NODEFAULT;
			}
		}

	
//	Msg			("[%s] Current weapon slot   : %d",*cName(),dwCurrentAniSlot);
//	Msg			("[%s] Current movement type : %d",*cName(),stalker->movement_type());

	EBodyState l_tBodyState = (eBodyStateStand == stalker->body_state()) && stalker->IsLimping() ? eBodyStateStandDamaged : stalker->body_state();
	if (stalker->inventory().ActiveItem()) {
		if (tpWeapon) {
			switch (tpWeapon->STATE) {
				case CWeapon::eReload : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Reload");
					tpTorsoAnimation = m_tAnims.A[stalker->body_state()].m_tTorso.A[dwCurrentAniSlot].A[4].A[0];
					break;
				}
				case CWeapon::eShowing : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Showing");
					tpTorsoAnimation = m_tAnims.A[stalker->body_state()].m_tTorso.A[dwCurrentAniSlot].A[0].A[0];
					break;
				}
				case CWeapon::eHiding : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Hiding");
					tpTorsoAnimation = m_tAnims.A[stalker->body_state()].m_tTorso.A[dwCurrentAniSlot].A[3].A[0];
					break;
				}
				case CWeapon::eFire: {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Fire1");
					tpTorsoAnimation = m_tAnims.A[stalker->body_state()].m_tTorso.A[dwCurrentAniSlot].A[1].A[0];
					break;
				}
				case CWeapon::eFire2 : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Fire2");
					tpTorsoAnimation = m_tAnims.A[stalker->body_state()].m_tTorso.A[dwCurrentAniSlot].A[1].A[0];
					break;
				}
			}
		}
		
#pragma todo("Dima to Designers : make missile throw animations!")
		if (missile && (stalker->body_state() == eBodyStateStand)) {
			switch (missile->State()) {
				case MS_SHOWING	 : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Showing");
					tpTorsoAnimation = m_tAnims.A[stalker->body_state()].m_tTorso.A[dwCurrentAniSlot].A[0].A[0];
					break;
				}
				case MS_HIDING	 : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Hiding");
					tpTorsoAnimation = m_tAnims.A[stalker->body_state()].m_tTorso.A[dwCurrentAniSlot].A[3].A[0];
					break;
				}
				case MS_THREATEN : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Fire start");
					tpTorsoAnimation = m_tAnims.A[stalker->body_state()].m_tTorso.A[dwCurrentAniSlot].A[1].A[0];
					break;
				}
				case MS_READY	 : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Fire idle");
					tpTorsoAnimation = m_tAnims.A[stalker->body_state()].m_tTorso.A[dwCurrentAniSlot].A[1].A[1];
					break;
				}
				case MS_THROW	 :
				case MS_END		 : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Fire end");
					tpTorsoAnimation = m_tAnims.A[stalker->body_state()].m_tTorso.A[dwCurrentAniSlot].A[1].A[2];
					break;
				}
				case MS_PLAYING	 : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Fire end1");
					tpTorsoAnimation = m_tAnims.A[stalker->body_state()].m_tTorso.A[dwCurrentAniSlot].A[1].A[2];
					break;
				}
				case MS_IDLE	 :
				case MS_HIDDEN	 :
				case MS_EMPTY	 :
				default			 : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Aim");
					tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[stalker->IsLimping() ? 9 : 6].A[0];
					break;
				}
			}
		}
		else
			if ((stalker->body_state() == eBodyStateCrouch) && missile)
				dwCurrentAniSlot	= 0;

		if (!tpTorsoAnimation) {
#ifdef OLD_OBJECT_HANDLER
			switch (stalker->CObjectHandler::current_state_id()) {
				case eObjectActionAim1 :
				case eObjectActionAim2 :
				case eObjectActionFire1 :
				case eObjectActionFire2 :
#else
			switch (stalker->CObjectHandlerGOAP::current_action_state_id()) {
				case CObjectHandlerGOAP::eWorldOperatorAim1 :
				case CObjectHandlerGOAP::eWorldOperatorAim2 :
				case CObjectHandlerGOAP::eWorldOperatorAimingReady1 :
				case CObjectHandlerGOAP::eWorldOperatorAimingReady2 :
				case CObjectHandlerGOAP::eWorldOperatorFire1 :
				case CObjectHandlerGOAP::eWorldOperatorFire2 :
#endif
				{
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Aim");
					tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[stalker->IsLimping() ? 9 : 6].A[0];
					break;
				}
				default : {
//					Msg				("%6d : weapon state %s",Level().timeServer(),"Idle");
					if (eMentalStateFree == stalker->mental_state()) {
						tpTorsoAnimation = 0;
						R_ASSERT2(eBodyStateStand == stalker->body_state(),"Cannot run !free! animation when body state is not stand!");
						if ((eMovementTypeStand == stalker->movement_type()) || fis_zero(stalker->speed()))
							tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[9].A[0];
						else
							tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[stalker->IsLimping() ? 9 : (7 + stalker->movement_type())].A[stalker->IsLimping() ? 0 : 1];
						return;
					}
					else {
						if (fis_zero(stalker->speed())) {
							tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[stalker->IsLimping() ? 9 : 6].A[0];
							return;
						}
						switch (stalker->movement_type()) {
							case eMovementTypeStand : {
								tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[stalker->IsLimping() ? 9 : 6].A[0];
								break;
							}
							case eMovementTypeWalk : {
								tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[7].A[0];
								break;
							}
							case eMovementTypeRun : {
								tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[stalker->IsLimping() ? 7 : 8].A[0];
								break;
							}
							default : NODEFAULT;
						}
					}
				}
			}
		}
	}
	else
		tpTorsoAnimation = m_tAnims.A[stalker->body_state()].m_tTorso.A[0].A[6].A[0];
}

void CStalkerAnimations::vfAssignLegsAnimation(CMotionDef *&tpLegsAnimation)
{
	CAI_Stalker				*stalker = dynamic_cast<CAI_Stalker*>(this);
	VERIFY					(stalker);
	if (!stalker->g_Alive())
		return;
	EBodyState l_tBodyState = (eBodyStateStand == stalker->body_state()) && stalker->IsLimping() ? eBodyStateStandDamaged : stalker->body_state();
	if ((stalker->speed() < EPS_L) || (eMovementTypeStand == stalker->movement_type())) {
		// standing
		if ((angle_difference(stalker->body_orientation().target.yaw,stalker->body_orientation().current.yaw) <= PI_DIV_4) && 
			(angle_difference(stalker->head_orientation().current.yaw,stalker->body_orientation().current.yaw) <= PI_DIV_4)
			) {
			tpLegsAnimation		= m_tAnims.A[stalker->body_state()].m_tInPlace.A[0];
		}
		else {
			tpLegsAnimation		= m_tAnims.A[stalker->body_state()].m_tInPlace.A[1];
		}
		return;
	}
	// moving
	float					yaw, pitch;
	stalker->GetDirectionAngles	(yaw,pitch);
	yaw						= angle_normalize_signed(-yaw);
//	float					fAngleDifference = _abs(angle_normalize_signed(stalker->body_orientation().current.yaw - stalker->head_orientation().current.yaw));
	float					fAnimationSwitchFactor = 1.f;//fAngleDifference < PI_DIV_2 ? 1.f : 1.f - (fAngleDifference - PI_DIV_2)/(PI - PI_DIV_2);
	if	(((eMovementDirectionForward == m_tDesirableDirection) && (eMovementDirectionBack == m_tMovementDirection))	||	((eMovementDirectionBack == m_tDesirableDirection) && (eMovementDirectionForward == m_tMovementDirection)))
		fAnimationSwitchFactor = .0f;
	if	(((eMovementDirectionRight == m_tDesirableDirection) && (eMovementDirectionLeft == m_tMovementDirection))	||	((eMovementDirectionLeft == m_tDesirableDirection) && (eMovementDirectionRight == m_tMovementDirection)))
		fAnimationSwitchFactor = .0f;

	if (eMentalStateDanger != stalker->mental_state())
		if (angle_difference(stalker->body_orientation().current.yaw,yaw) <= PI_DIV_6) {
			tpLegsAnimation = m_tAnims.A[l_tBodyState].m_tMoves.A[stalker->movement_type()].A[eMovementDirectionForward].A[stalker->mental_state()];
			return;
		}
		else {
			fAnimationSwitchFactor	= .0f;
//			stalker->m_body.speed	= PI_DIV_2;
//			stalker->m_head.speed	= 3*PI_DIV_2;
		}

	if (angle_difference(yaw,stalker->head_orientation().current.yaw) <= MAX_HEAD_TURN_ANGLE) {
		// moving forward
		if (eMovementDirectionForward == m_tMovementDirection)
			m_dwDirectionStartTime	= Level().timeServer();
		else
			if (eMovementDirectionForward != m_tDesirableDirection) {
				m_dwDirectionStartTime	= Level().timeServer();
				m_tDesirableDirection	= eMovementDirectionForward;
			}
			else
				if ((Level().timeServer() - m_dwDirectionStartTime) > m_dwAnimationSwitchInterval*fAnimationSwitchFactor) {
					m_dwDirectionStartTime	= Level().timeServer();
					m_tMovementDirection	= eMovementDirectionForward;
				}
	}
	else {
		bool	left = (angle_difference(yaw,angle_normalize_signed(stalker->head_orientation().current.yaw + PI_DIV_2)) <= PI_DIV_4);
		bool	back = left ? !(angle_difference(yaw,stalker->head_orientation().current.yaw) <= 2*PI_DIV_6) : !(angle_difference(yaw,stalker->head_orientation().current.yaw) <= 4*PI_DIV_6);
		if (!back)
			// moving left|right
			if (left) {
				// moving right, looking left
				if (eMovementDirectionRight == m_tMovementDirection)
					m_dwDirectionStartTime	= Level().timeServer();
				else
					if (eMovementDirectionRight != m_tDesirableDirection) {
						m_dwDirectionStartTime	= Level().timeServer();
						m_tDesirableDirection	= eMovementDirectionRight;
					}
					else
						if ((Level().timeServer() - m_dwDirectionStartTime) > m_dwAnimationSwitchInterval*fAnimationSwitchFactor) {
							m_dwDirectionStartTime	= Level().timeServer();
							m_tMovementDirection	= eMovementDirectionRight;
						}
			}
			else {
				// moving left, looking right
				if (eMovementDirectionLeft == m_tMovementDirection)
					m_dwDirectionStartTime	= Level().timeServer();
				else
					if (eMovementDirectionLeft != m_tDesirableDirection) {
						m_dwDirectionStartTime	= Level().timeServer();
						m_tDesirableDirection	= eMovementDirectionLeft;
					}
					else
						if ((Level().timeServer() - m_dwDirectionStartTime) > m_dwAnimationSwitchInterval*fAnimationSwitchFactor) {
							m_dwDirectionStartTime	= Level().timeServer();
							m_tMovementDirection	= eMovementDirectionLeft;
						}
			}
		else {
			// moving back
			if (eMovementDirectionBack == m_tMovementDirection)
				m_dwDirectionStartTime	= Level().timeServer();
			else
				if (eMovementDirectionBack != m_tDesirableDirection) {
					m_dwDirectionStartTime	= Level().timeServer();
					m_tDesirableDirection	= eMovementDirectionBack;
				}
				else
					if ((Level().timeServer() - m_dwDirectionStartTime) > m_dwAnimationSwitchInterval*fAnimationSwitchFactor) {
						m_dwDirectionStartTime	= Level().timeServer();
						m_tMovementDirection	= eMovementDirectionBack;
					}
		}
	}
//	Msg("----%d\n[W=%7.2f][TT=%7.2f][TC=%7.2f][T=%7.2f][C=%7.2f]",Level().timeServer(),yaw,stalker->body_orientation().target.yaw,stalker->body_orientation().current.yaw,stalker->head_orientation().target.yaw,stalker->head_orientation().current.yaw);
//	Msg("Trying %s\nMoving %s",caMovementActionNames[m_tDesirableDirection],caMovementActionNames[m_tMovementDirection]);
//	if (eMentalStateFree == stalker->mental_state())
//		m_tMovementDirection = eMovementDirectionForward;

	tpLegsAnimation					= m_tAnims.A[l_tBodyState].m_tMoves.A[stalker->movement_type()].A[m_tMovementDirection].A[0];
	MonsterSpace::SBoneRotation		body_orientation = stalker->body_orientation();
	body_orientation.target.yaw		= angle_normalize_signed(yaw + faTurnAngles[m_tMovementDirection]);
	stalker->set_body_orientation	(body_orientation);
//	Msg("[W=%7.2f][TT=%7.2f][TC=%7.2f][T=%7.2f][C=%7.2f]",yaw,stalker->body_orientation().target.yaw,stalker->body_orientation().current.yaw,stalker->head_orientation().target.yaw,stalker->head_orientation().current.yaw);
}

static void	TorsoPlayCallback(CBlend *B)
{
#ifdef DEBUG
	CAI_Stalker							*stalker = (CAI_Stalker*)B->CallbackParam;
	VERIFY								(stalker);
	//	stalker->m_tpCurrentTorsoAnimation	= 0;
#endif
}

static void	LegsPlayCallback(CBlend *B)
{
#ifdef DEBUG
	CAI_Stalker							*stalker = (CAI_Stalker*)B->CallbackParam;
	VERIFY								(stalker);
	//	stalker->m_tpCurrentLegsAnimation	= 0;
#endif
}

static void	GlobalPlayCallback(CBlend *B)
{
#ifdef DEBUG
	CAI_Stalker							*stalker = (CAI_Stalker*)B->CallbackParam;
	VERIFY								(stalker);
	//	stalker->m_tpCurrentGlobalAnimation	= 0;
#endif
}

static void	ScriptPlayCallback(CBlend *B)
{
#ifdef DEBUG
	CAI_Stalker							*stalker = (CAI_Stalker*)B->CallbackParam;
	VERIFY								(stalker);
#endif
	VERIFY								(!stalker->m_current_script_animation || stalker->m_current_script_animation == stalker->m_script_animations.front().m_motion);
	stalker->m_script_animations.pop_front();
	stalker->m_current_script_animation	= 0;
}

void CAI_Stalker::SelectAnimation(const Fvector& /**_view/**/, const Fvector& /**_move/**/, float /**speed/**/)
{
	CAI_Stalker				*stalker = dynamic_cast<CAI_Stalker*>(this);
	VERIFY					(stalker);
	CSkeletonAnimated		&tVisualObject		=	*(PSkeletonAnimated(stalker->Visual()));

	if (!m_script_animations.empty()) {
		bool				_continue = false;
		if (m_current_script_animation) {
			VERIFY			(m_current_script_animation == m_script_animations.front().m_motion);
			return;
		}
		tVisualObject.PlayCycle	(m_current_script_animation = m_script_animations.front().m_motion,TRUE,ScriptPlayCallback,this);
		if (!_continue)
			return;
	}

	if (m_tAnims.A.empty())	return;

	m_current_script_animation	= 0;
	CMotionDef				*tpGlobalAnimation	=	0;
	CMotionDef				*tpTorsoAnimation	=	0;
	CMotionDef				*tpLegsAnimation	=	0;

	vfAssignGlobalAnimation	(tpGlobalAnimation);

	if (tpGlobalAnimation) {
		m_tpCurrentTorsoAnimation	= 0;
		m_tpCurrentLegsAnimation	= 0;
		if (m_tpCurrentGlobalAnimation != tpGlobalAnimation)
			m_tpCurrentGlobalBlend = tVisualObject.PlayCycle(m_tpCurrentGlobalAnimation = tpGlobalAnimation,TRUE,GlobalPlayCallback,this);
	}
	else
		if (stalker->g_Alive()) {
			m_tpCurrentGlobalBlend	= 0;
			m_tpCurrentGlobalAnimation = 0;
			vfAssignTorsoAnimation	(tpTorsoAnimation);
			vfAssignLegsAnimation	(tpLegsAnimation);

			if (tpTorsoAnimation && (m_tpCurrentTorsoAnimation != tpTorsoAnimation))
				m_tpCurrentTorsoBlend	= tVisualObject.PlayCycle(m_tpCurrentTorsoAnimation = tpTorsoAnimation,TRUE,TorsoPlayCallback,this);
			if (tpLegsAnimation && (m_tpCurrentLegsAnimation != tpLegsAnimation))
				m_tpCurrentLegsBlend	= tVisualObject.PlayCycle(m_tpCurrentLegsAnimation = tpLegsAnimation,TRUE,LegsPlayCallback,this);
			
			if (tpTorsoAnimation && tpLegsAnimation){
				if ((tpTorsoAnimation->flags & esmSyncPart) && (tpLegsAnimation->flags & esmSyncPart))
					if (m_tpCurrentTorsoBlend && m_tpCurrentLegsBlend)
						m_tpCurrentTorsoBlend->timeCurrent = m_tpCurrentLegsBlend->timeCurrent;
			}
		}

#ifdef _DEBUG
//	if (tpGlobalAnimation)
//		Msg				("%6d Global animation : %s",Level().timeServer(),PSkeletonAnimated(Visual())->LL_MotionDefName_dbg(tpGlobalAnimation));
//	
//	if (tpTorsoAnimation)
//		Msg				("%6d Torso animation : %s",Level().timeServer(),PSkeletonAnimated(Visual())->LL_MotionDefName_dbg(tpTorsoAnimation));
//	
//	if (tpLegsAnimation)
//		Msg				("%6d Legs animation : %s",Level().timeServer(),PSkeletonAnimated(Visual())->LL_MotionDefName_dbg(tpLegsAnimation));
#endif
}

void CStalkerAnimations::add_animation		(LPCSTR animation, bool hand_usage)
{
	CMotionDef						*motion = PSkeletonAnimated(m_visual)->ID_Cycle_Safe(animation);
	if (!motion) {
		CObject						*object = dynamic_cast<CObject*>(this);
		VERIFY						(object);
		ai().script_engine().script_log(eLuaMessageTypeError,"There is no animation %s (object %s)!",animation,*object->cName());
		return;
	}
	m_script_animations.push_back	(CScriptAnimation(hand_usage,motion));
}

void CStalkerAnimations::clear_animations	()
{
	m_script_animations.clear		();
}
