//////////////////////////////////////////////////////////////////////
// HudItem.cpp: класс родитель для всех предметов имеющих
//				собственный HUD (CWeapon, CMissile etc)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HudItem.h"
#include "physic_item.h"
#include "WeaponHUD.h"
#include "actor.h"
#include "actoreffector.h"
#include "Missile.h"
#include "xrmessages.h"
#include "level.h"
#include "inventory.h"
#include "../CameraBase.h"

//////////////////////////////////////////////////////////////////////////

//для инерции HUD
//граничные значения
static float			m_fInertYawMin;
static float			m_fInertYawMax;
static float			m_fInertPitchMin;
static float			m_fInertPitchMax;
//скорости восстановления
static float			m_fInertYawRestoreSpeed;
static float			m_fInertPitchRestoreSpeed;



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CHudItem::CHudItem(void)
{
	m_pHUD = NULL;
	hud_mode = FALSE;
	m_dwStateTime = 0;
	m_bRenderHud = true;

	m_bInertionEnable  = true;
	m_bInertionOn = true;
}

CHudItem::~CHudItem(void)
{
	xr_delete			(m_pHUD);
}

DLL_Pure *CHudItem::_construct	()
{
	m_object			= smart_cast<CPhysicItem*>(this);
	VERIFY				(m_object);

	m_item				= smart_cast<CInventoryItem*>(this);
	VERIFY				(m_item);

	return				(m_object);
}

void CHudItem::Load(LPCSTR section)
{
	//загрузить hud, если он нужен
	if(pSettings->line_exist(section,"hud"))
		hud_sect = pSettings->r_string		(section,"hud");

	if(*hud_sect)
	{
		m_pHUD				= xr_new<CWeaponHUD> (this);
		m_pHUD->Load		(*hud_sect);
	}
	else
	{
		m_pHUD = NULL;
		//если hud не задан, но задан слот, то ошибка
		R_ASSERT2(item().GetSlot() == NO_ACTIVE_SLOT, "active slot is set, but hud for food item is not available");
	}

	m_animation_slot	= pSettings->r_u32(section,"animation_slot");
}

void CHudItem::net_Destroy()
{
	if(m_pHUD)
		m_pHUD->net_DestroyHud	();

	hud_mode = FALSE;
	m_dwStateTime = 0;
}


void CHudItem::PlaySound(HUD_SOUND& hud_snd,
						 const Fvector& position)
{
	HUD_SOUND::PlaySound(hud_snd, position, object().H_Root(), !!hud_mode);
}

BOOL  CHudItem::net_Spawn(CSE_Abstract* DC) 
{
	return TRUE;
}


void CHudItem::renderable_Render()
{
	UpdateXForm	();
	BOOL		_hud_render		= ::Render->get_HUD() && hud_mode;
	if(_hud_render && !m_pHUD->IsHidden() && !item().IsHidden())
	{ 
		// HUD render
		if(m_bRenderHud)
		{
			::Render->set_Transform		(&m_pHUD->Transform());
			::Render->add_Visual		(m_pHUD->Visual());
		}
	}
	else
		if(!object().H_Parent() || (!_hud_render && m_pHUD && !m_pHUD->IsHidden() && !item().IsHidden()))
		{
			on_renderable_Render();
		}
}

bool CHudItem::Action(s32 cmd, u32 flags) 
{
	return false;
}

void CHudItem::OnAnimationEnd()
{
}

void CHudItem::SwitchState(u32 S)
{
	NEXT_STATE		= S;	// Very-very important line of code!!! :)
	if (object().Local() && !object().getDestroy()/* && (S!=NEXT_STATE)*/)	
	{
		// !!! Just single entry for given state !!!
		NET_Packet		P;
		object().u_EventGen		(P,GE_WPN_STATE_CHANGE,object().ID());
		P.w_u8			(u8(S));
		object().u_EventSend		(P);
	}
}

void CHudItem::OnEvent		(NET_Packet& P, u16 type)
{
	switch (type)
	{
	case GE_WPN_STATE_CHANGE:
		{
			u8				S;
			P.r_u8			(S);
			OnStateSwitch	(u32(S));
		}
		break;
	}
}

void CHudItem::OnStateSwitch	(u32 S)
{
	m_dwStateTime = 0;
	STATE = S;
	if(object().Remote()) NEXT_STATE = S;
}


bool CHudItem::Activate() 
{
	if(m_pHUD) 
		m_pHUD->Init();

	Show();
	return true;
}

void CHudItem::Deactivate() 
{
	Hide();
}



void CHudItem::UpdateHudPosition	()
{
	if (m_pHUD && hud_mode)
	{
		if(item().IsHidden()) hud_mode = false;

		Fmatrix							trans;

		CActor* pActor = smart_cast<CActor*>(object().H_Parent());
		if(pActor)
		{
			pActor->EffectorManager().affected_Matrix	(trans);
			UpdateHudInertion							(trans, pActor->cam_FirstEye()->yaw, pActor->cam_FirstEye()->pitch);
			UpdateHudAdditonal							(trans);
			m_pHUD->UpdatePosition						(trans);
		}
	}
}

void CHudItem::UpdateHudAdditonal		(Fmatrix& hud_trans)
{
}


void CHudItem::StartHudInertion()
{
	m_bInertionEnable = true;
	m_bInertionOn = true;
}
void CHudItem::StopHudInertion()
{
	m_bInertionEnable = false;
	m_bInertionOn = false;
}



static float cur_yaw			= 0.f;
static float cur_pitch			= 0.f;

static float min_yaw_speed		= 0.0000006f;
static float max_yaw_speed		= 0.00456f;

static float min_pitch_speed	= 0.01f;

static float max_yaw_delta		= 0.15f;
static float max_pitch_delta	= PI_DIV_4;

static float delta_yaw			= 0;
static float old_actor_yaw			= 0;

/*
delta_yaw = cur_yaw - actor_yaw;

bool do_reverse = true;

if(delta_yaw > max_yaw_delta)
{
cur_yaw = actor_yaw + max_yaw_delta;
delta_yaw = max_yaw_delta;
do_reverse = false;
} else if(delta_yaw < -max_yaw_delta)
{
cur_yaw = actor_yaw - max_yaw_delta;
delta_yaw = - max_yaw_delta;
do_reverse = false;
}

float yaw_speed = min_yaw_speed + (max_yaw_speed - min_yaw_speed)*_abs(delta_yaw/max_yaw_delta)*_abs(delta_yaw/max_yaw_delta)*_abs(delta_yaw/max_yaw_delta);

if(do_reverse && fis_zero(delta_yaw, min_yaw_speed))
{
delta_yaw = 0;
}
else if(!fis_zero(delta_yaw+yaw_speed, max_yaw_speed))
{
if(delta_yaw>0)
cur_yaw -= yaw_speed;
else
cur_yaw += yaw_speed;
}
else
{
Msg("%f", delta_yaw+yaw_speed);
cur_yaw = actor_yaw - delta_yaw;
}

//float delta_pitch = cur_pitch - actor_pitch;
*/


#define MEASURING_NUM 10
static float yaw_w[MEASURING_NUM] = {0.f, 0.f, 0.f};
static int cur_measure = 0;

static float restore_yaw_speed = 0;

void CHudItem::UpdateHudInertion		(Fmatrix& hud_trans, float actor_yaw, float actor_pitch)
{
#if 1
	return;
#else

	if(!m_bInertionEnable) 
	{
		cur_yaw = actor_yaw;
		cur_pitch = actor_pitch;
		return;
	}

	float delta_actor_yaw = actor_yaw - old_actor_yaw;
	old_actor_yaw = actor_yaw;

	//вычслить мгновенную скорость поворота
	cur_measure = cur_measure<MEASURING_NUM-1?cur_measure+1:0;
	yaw_w[cur_measure]= delta_actor_yaw/Device.fTimeDelta;
	
	//вычислить усредненное значение скорости
	float yaw_w_mean = 0.f;
	for(int i=0; i<MEASURING_NUM; i++)
		yaw_w_mean += yaw_w[i];

	yaw_w_mean /= MEASURING_NUM;
	
/*	float restore_yaw_accel = -(cur_yaw-actor_yaw)*0.7f;
	//добавить затухание трения
	restore_yaw_accel += 0.73f*(restore_yaw_speed>0?-1.f:1.f)*_abs(restore_yaw_speed);
	restore_yaw_speed += restore_yaw_accel*Device.fTimeDelta;
	cur_yaw += restore_yaw_speed*Device.fTimeDelta;

	if(yaw_w_mean > 0)
	{
		Msg("-- yaw w %f",yaw_w_mean);
		Msg("restore yaw accel %f", restore_yaw_accel);
		Msg("restore yaw speed %f", restore_yaw_speed);
		Msg("cur yaw%f", cur_yaw);
	}
//	float k = 0.93f;
//	if(_abs(cur_yaw-actor_yaw)>0.01f)
//	cur_yaw = k*cur_yaw + (1.f-k)*actor_yaw;

	//float delta_cur_yaw		= cur_yaw - old_cur_yaw;
//	float delta_actor_yaw	= _abs(actor_yaw - old_actor_yaw);
*/

//	if(yaw_w_mean<0.0001f)
	{	
		actor_yaw = angle_normalize(actor_yaw);
		float old_cur_yaw = cur_yaw;
		angle_lerp(cur_yaw, actor_yaw, PI_DIV_2, Device.fTimeDelta);
	}
/*	
	if(_abs(delta_cur_yaw - delta_actor_yaw)<0.01f && _abs(delta_cur_yaw)>0.001f
		&& _abs(delta_actor_yaw)>0.001f)
	{
//		Msg("cur yaw delta   %f", cur_yaw - old_cur_yaw);
//		Msg("actor yaw delta %f", actor_yaw - old_actor_yaw);

		cur_yaw = old_cur_yaw +delta_actor_yaw;
	}
*/	
	old_actor_yaw = actor_yaw;

	Fmatrix inertion; 
	inertion.identity();
	inertion.rotateY(cur_yaw-actor_yaw);
	//inertion.rotateY(0.2f);
	hud_trans.mulB(inertion);
#endif
}

void CHudItem::UpdateCL()
{
	m_dwStateTime += Device.dwTimeDelta;

	if(m_pHUD) m_pHUD->UpdateHud();
	UpdateHudPosition	();
}

void CHudItem::OnH_A_Chield		()
{
	hud_mode = FALSE;
	
	if (m_pHUD) {
		if(Level().CurrentEntity() == object().H_Parent() && smart_cast<CActor*>(object().H_Parent()))
			m_pHUD->SetCurrentEntityHud(true);
		else
			m_pHUD->SetCurrentEntityHud(false);
	}
}

void CHudItem::OnH_B_Chield		()
{
	if (item().m_pInventory && item().m_pInventory->ActiveItem() == smart_cast<PIItem>(this))
		OnActiveItem ();
	else
		OnHiddenItem ();
}

void CHudItem::OnH_B_Independent	()
{
	hud_mode = FALSE;
	if (m_pHUD)
		m_pHUD->SetCurrentEntityHud(false);
	
	StopHUDSounds();
}
