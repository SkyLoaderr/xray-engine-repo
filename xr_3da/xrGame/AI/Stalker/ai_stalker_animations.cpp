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
#include "../../ai_script_actions.h"

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
	0
};

LPCSTR caWeaponActionNames	[] = {
	"draw_",
	"attack_",
	"drop_",
	"holster_",
	"reload_",
	"pick_",
	"aim_",
	"walk_",
	"run_",
	"idle_",
	"escape_",
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
	switch(A->look_type()) {
		case eLookTypeLookFireOver :
		case eLookTypeFirePoint : {
			yaw_factor		= y_head_fire_factor;
			pitch_factor	= p_head_fire_factor;
			break;
		}
		case eLookTypeLookOver :
		case eLookTypePathDirection :
		case eLookTypeDirection :
		case eLookTypeCurrentDirection :
		case eLookTypeSearch :
		case eLookTypeDanger :
		case eLookTypePoint : {
			yaw_factor		= y_head_factor;
			pitch_factor	= p_head_factor;
			break;
		}
		default : NODEFAULT;
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
	switch(A->look_type()) {
		case eLookTypeLookFireOver :
		case eLookTypeFirePoint : {
			yaw_factor		= y_shoulder_fire_factor;
			pitch_factor	= p_shoulder_fire_factor;
			break;
		}
		case eLookTypeLookOver :
		case eLookTypePathDirection :
		case eLookTypeDirection :
		case eLookTypeCurrentDirection :
		case eLookTypeSearch :
		case eLookTypeDanger :
		case eLookTypePoint : {
			yaw_factor		= y_shoulder_factor;
			pitch_factor	= p_shoulder_factor;
			break;
		}
		default : NODEFAULT;
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
	switch(A->look_type()) {
		case eLookTypeLookFireOver :
		case eLookTypeFirePoint : {
			yaw_factor		= y_spin_fire_factor;
			pitch_factor	= p_spin_fire_factor;
			break;
		}
		case eLookTypeLookOver :
		case eLookTypePathDirection :
		case eLookTypeDirection :
		case eLookTypeCurrentDirection :
		case eLookTypeSearch :
		case eLookTypeDanger :
		case eLookTypePoint : {
			yaw_factor		= y_spin_factor;
			pitch_factor	= p_spin_factor;
			break;
		}
		default : NODEFAULT;
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
	if (stalker->g_Alive()) {
		if ((eMentalStatePanic == stalker->mental_state()) && (stalker->speed() > EPS_L))
			tpGlobalAnimation = m_tAnims.A[stalker->IsLimping() ? eBodyStateStandDamaged : eBodyStateStand].m_tGlobal.A[1].A[0];
	}
}

void CStalkerAnimations::vfAssignTorsoAnimation(CMotionDef *&tpTorsoAnimation)
{
	CAI_Stalker				*stalker = dynamic_cast<CAI_Stalker*>(this);
	VERIFY					(stalker);
	if (!stalker->g_Alive())
		return;
	
	CWeapon					*tpWeapon = dynamic_cast<CWeapon*>(stalker->inventory().ActiveItem());
	u32						dwCurrentAniSlot = 0;
	if (tpWeapon)
		switch (tpWeapon->SUB_CLS_ID) {
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
			default : {
#pragma todo("Dima to Dima : Return nodefault")
				dwCurrentAniSlot = 1;
				//NODEFAULT;
			}
		}

	
//	Msg			("[%s] Current weapon slot   : %d",cName(),dwCurrentAniSlot);
//	Msg			("[%s] Current movement type : %d",cName(),stalker->movement_type());

	EBodyState l_tBodyState = (eBodyStateStand == stalker->body_state()) && stalker->IsLimping() ? eBodyStateStandDamaged : stalker->body_state();
	if (stalker->inventory().ActiveItem()) {
		CWeapon *tpWeapon = dynamic_cast<CWeapon*>(stalker->inventory().ActiveItem());
		if (tpWeapon) {
			switch (tpWeapon->STATE) {
				case CWeapon::eReload : {
					Msg				("%6d : weapon state %s",Level().timeServer(),"Reload");
					tpTorsoAnimation = m_tAnims.A[stalker->body_state()].m_tTorso.A[dwCurrentAniSlot].A[4].A[0];
					break;
				}
				case CWeapon::eShowing : {
					Msg				("%6d : weapon state %s",Level().timeServer(),"Showing");
					tpTorsoAnimation = m_tAnims.A[stalker->body_state()].m_tTorso.A[dwCurrentAniSlot].A[0].A[0];
					break;
				}
				case CWeapon::eHiding : {
					Msg				("%6d : weapon state %s",Level().timeServer(),"Hiding");
					tpTorsoAnimation = m_tAnims.A[stalker->body_state()].m_tTorso.A[dwCurrentAniSlot].A[3].A[0];
					break;
				}
				case CWeapon::eFire: {
					Msg				("%6d : weapon state %s",Level().timeServer(),"Fire1");
					tpTorsoAnimation = m_tAnims.A[stalker->body_state()].m_tTorso.A[dwCurrentAniSlot].A[1].A[0];
					break;
				}
				case CWeapon::eFire2 : {
					Msg				("%6d : weapon state %s",Level().timeServer(),"Fire2");
					tpTorsoAnimation = m_tAnims.A[stalker->body_state()].m_tTorso.A[dwCurrentAniSlot].A[1].A[0];
					break;
				}
				default : {
					switch (stalker->CObjectHandler::current_state_state_id()) {
						case eObjectActionAim1 :
						case eObjectActionAim2 :
						case eObjectActionFire1 :
						case eObjectActionFire2 :
						{
							Msg				("%6d : weapon state %s",Level().timeServer(),"Aim");
							tpTorsoAnimation = m_tAnims.A[l_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[stalker->IsLimping() ? 9 : 6].A[0];
							break;
						}
						default : {
							Msg				("%6d : weapon state %s",Level().timeServer(),"Idle");
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
		if (angle_difference(stalker->body_orientation().target.yaw,stalker->body_orientation().current.yaw) <= PI_DIV_6) {
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
		else
			fAnimationSwitchFactor = .0f;

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
		if (angle_difference(yaw,stalker->head_orientation().current.yaw) <= 4*PI_DIV_6)
			// moving left|right
			if (angle_difference(yaw,angle_normalize_signed(stalker->head_orientation().current.yaw + PI_DIV_2)) <= PI_DIV_4) {
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
	CMovementManager::SBoneRotation	body_orientation = stalker->body_orientation();
	body_orientation.target.yaw		= angle_normalize_signed(yaw + faTurnAngles[m_tMovementDirection]);
	stalker->set_body_orientation	(body_orientation);
//	Msg("[W=%7.2f][TT=%7.2f][TC=%7.2f][T=%7.2f][C=%7.2f]",yaw,stalker->body_orientation().target.yaw,stalker->body_orientation().current.yaw,stalker->head_orientation().target.yaw,stalker->head_orientation().current.yaw);
}

static void	TorsoPlayCallback(CBlend *B)
{
	CAI_Stalker							*stalker = (CAI_Stalker*)B->CallbackParam;
	VERIFY								(stalker);
//	stalker->m_tpCurrentTorsoAnimation	= 0;
}

static void	LegsPlayCallback(CBlend *B)
{
	CAI_Stalker							*stalker = (CAI_Stalker*)B->CallbackParam;
	VERIFY								(stalker);
//	stalker->m_tpCurrentLegsAnimation	= 0;
}

static void	GlobalPlayCallback(CBlend *B)
{
	CAI_Stalker							*stalker = (CAI_Stalker*)B->CallbackParam;
	VERIFY								(stalker);
//	stalker->m_tpCurrentGlobalAnimation	= 0;
}

void CAI_Stalker::SelectAnimation(const Fvector& /**_view/**/, const Fvector& /**_move/**/, float /**speed/**/)
{
	CAI_Stalker				*stalker = dynamic_cast<CAI_Stalker*>(this);
	VERIFY					(stalker);
	CSkeletonAnimated		&tVisualObject		=	*(PSkeletonAnimated(stalker->Visual()));

	if (m_tAnims.A.empty())
		return;

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

#ifdef DEBUG
//	if (tpGlobalAnimation)
//		Msg				("%6d Global animation : %s",Level().timeServer(),PSkeletonAnimated(Visual())->LL_MotionDefName_dbg(tpGlobalAnimation));
	
//	if (tpTorsoAnimation)
//		Msg				("%6d Torso animation : %s",Level().timeServer(),PSkeletonAnimated(Visual())->LL_MotionDefName_dbg(tpTorsoAnimation));
	
//	if (tpLegsAnimation)
//		Msg				("%6d Legs animation : %s",Level().timeServer(),PSkeletonAnimated(Visual())->LL_MotionDefName_dbg(tpLegsAnimation));
#endif
}
