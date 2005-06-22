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
	m_pHUD				= NULL;
	hud_mode			= FALSE;
	m_dwStateTime		= 0;
	m_bRenderHud		= true;

	m_bInertionEnable	= true;
	m_bInertionAllow	= true;
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

	if(*hud_sect){
		m_pHUD			= xr_new<CWeaponHUD> (this);
		m_pHUD->Load	(*hud_sect);
		if(pSettings->line_exist(*hud_sect, "allow_inertion")) 
			m_bInertionAllow = !!pSettings->r_bool(*hud_sect, "allow_inertion");
	}else{
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

void CHudItem::PlaySound	(HUD_SOUND& hud_snd, const Fvector& position)
{
	HUD_SOUND::PlaySound	(hud_snd, position, object().H_Root(), !!hud_mode);
}

BOOL  CHudItem::net_Spawn	(CSE_Abstract* DC) 
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

	if (object().Local() && !object().getDestroy())	
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
		if(pActor){
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
}
void CHudItem::StopHudInertion()
{
	m_bInertionEnable = false;
}

static const float PITCH_OFFSET_R	= 0.017f;
static const float PITCH_OFFSET_N	= 0.012f;
static const float PITCH_OFFSET_D	= 0.02f;
static const float ORIGIN_OFFSET	= -0.05f;
static const float TENDTO_SPEED		= 5.f;

void CHudItem::UpdateHudInertion		(Fmatrix& hud_trans, float actor_yaw, float actor_pitch)
{
	if (m_pHUD && m_bInertionAllow && m_bInertionEnable){
		Fmatrix			xform,xform_orig; 
		Fvector& origin	= hud_trans.c; 
		Level().Cameras.affected_Matrix		(xform);
		Level().Cameras.unaffected_Matrix	(xform_orig);

		static Fvector	m_last_dir={0,0,0};

		// calc difference
		Fvector diff_dir;
		diff_dir.sub	(xform.k, m_last_dir);

		// clamp by PI_DIV_2
		Fvector last;	last.normalize_safe(m_last_dir);
		float dot		= last.dotproduct(xform.k);
		if (dot<EPS){
			Fvector v0;
			v0.crossproduct			(m_last_dir,xform.k);
			m_last_dir.crossproduct	(xform.k,v0);
			diff_dir.sub			(xform.k, m_last_dir);
		}

		// tend to forward
		m_last_dir.mad	(diff_dir,TENDTO_SPEED*Device.fTimeDelta);
		origin.mad		(diff_dir,ORIGIN_OFFSET);

		// pitch compensation
		float pitch		= angle_normalize_signed(xform_orig.k.getP());
		origin.mad		(xform_orig.k,	-pitch * PITCH_OFFSET_D);
		origin.mad		(xform_orig.i,	-pitch * PITCH_OFFSET_R);
		origin.mad		(xform_orig.j,	-pitch * PITCH_OFFSET_N);

		// calc moving inertion
	}
/*
	static u32 sl = 0;
	if (sl)	Sleep(sl);
*/
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
	UpdateXForm();
}

void CHudItem::OnH_A_Independent	()
{
}

/*
static float gDT	= 0.01f;

gDT					= 0.999f*gDT+0.001f*Device.fTimeDelta;

Fmatrix M;
Level().Cameras.affected_Matrix(M);

float h,p,b;
M.getHPB			(h,p,b);

inert.x				= angle_inertion_var_	(inert.x,h,0.1f,12.f,CLMP,gDT);
inert.y				= angle_inertion_var_	(inert.y,p,0.1f,12.f,CLMP,gDT);
//		Msg("-- %3.2f %3.2f - %3.2f",Device.fTimeDelta,inert.x,a);

xform.setHPB		(inert.x,inert.y,b);
xform.translate_over(c);
*/

/*
static float g_S	= 0.f;
static float g_H	= 0.f;
static float gDT	= 0.01f;

gDT					= 0.999f*gDT+0.001f*Device.fTimeDelta;

Fmatrix M;
Level().Cameras.affected_Matrix(M);

float h,p,b;
M.getHPB			(h,p,b);

float sp			= g_S;
g_S					= 0.01f*angle_difference_signed(h,g_H)/gDT+0.99f*g_S;
//.		clamp				(g_S,-.5f,.5f);
float sign_h		= fis_zero(g_S)?0.f:g_S/_abs(g_S);

float new_h			= h-5.f*g_S*gDT;//sign_h*pow(_abs(g_S),1.5f);
//		Log					("--",g_S);
//		float S				= g_S*10.f;
//		float new_h			= fis_zero(g_S)?h:h-(g_S/_abs(g_S))*_sqrt(_abs(g_S));

g_H					= h;
xform.setHPB		(new_h,p,b);
xform.translate_over(c);
*/

/*
static float g_S	= 0.f;
static float g_H	= 0.f;
static float gDT	= 0.01f;

gDT					= 0.999f*gDT+0.001f*Device.fTimeDelta;

Fmatrix M;
Level().Cameras.affected_Matrix(M);

float h,p,b;
M.getHPB			(h,p,b);

g_S					= 0.98f*g_S+0.02f*angle_difference_signed(h,g_H);

//		Log					("--",g_S);
//		clamp				(g_S,-.035f,.035f);
float sign_h		= fis_zero(g_S)?0.f:g_S/_abs(g_S);
float new_h			= h-sign_h*10.f*(pow(_abs(g_S),1.3f));
//		Log					("--",g_S);
//		float S				= g_S*10.f;
//		float new_h			= fis_zero(g_S)?h:h-(g_S/_abs(g_S))*_sqrt(_abs(g_S));

g_H					= h;
xform.setHPB		(new_h,p,b);
xform.translate_over(c);
*/

/*
static float g_S	= 0.f;
static float g_H	= 0.f;
static float gDT	= 0.01f;

gDT					= 0.999f*gDT+0.001f*Device.fTimeDelta;

Fmatrix M;
Level().Cameras.affected_Matrix(M);

float h,p,b;
M.getHPB			(h,p,b);

g_S					= 0.95f*g_S+0.05f*angle_difference_signed(h,g_H);

float sign_h		= fis_zero(g_S)?0.f:g_S/_abs(g_S);

//		static float s		= 0.f;
//		s					= 0.9f*s + 0.1f*g_S*gDT;//sign_h*10.f*(pow(_abs(g_S),1.3f));

static u32 xxx		= 0;
if (xxx!=0)			Sleep(xxx);
//		Log					("--",g_S);
//		clamp				(g_S,-.035f,.035f);
float new_h			= h-g_S;
//		Log					("--",g_S);
//		float S				= g_S*10.f;
//		float new_h			= fis_zero(g_S)?h:h-(g_S/_abs(g_S))*_sqrt(_abs(g_S));

g_H					= h;
xform.setHPB		(new_h,p,b);
xform.translate_over(c);
*/

/*
float t_xx (float V0, float V1, float a0, float a1, float d, float fSign)
{
return (V1+_sqrt(V1*V1 - (a1/(a1-a0))*(V1*V1-V0*V0-2*a0*d) )*fSign )/a1;
}

float t_1(float t10, float t11)
{
if(t10<0)
return t11;
else
if(t11<0)
return t10;
else
return _min(t10,t11);

}

float t_0(float V0, float V1, float a0, float a1, float t1)
{
return (V1-V0-a1*t1)/a0;
}

float getA(float t0, float a1, float a0)
{
float eps = 0.001f;
return (t0<eps)?a1:a0;
}

float GetCurrAcc(float Vcur, float Vtgt, float dist, float a0, float a1)
{
float t10,t11,t0,t1;

t10 = t_xx      (Vcur, Vtgt, a0, a1, dist, 1.0f);
t11 = t_xx      (Vcur, Vtgt, a0, a1, dist, -1.0f);
t1      = t_1   (t10, t11); 
t0      = t_0   (Vcur, Vtgt, a0, a1, t1);
return getA     (t0, a1, a0);
}

Fmatrix M;
Level().Cameras.affected_Matrix(M);

float h,p,b;
M.getHPB			(h,p,b);

static float g_Vp	= 0.f;
static float g_Sp	= 0.f;

float dist			= angle_difference_signed(h,g_Sp);
float dist_a		= _abs(dist);
float sign			= fis_zero(dist)?0.f:dist/dist_a;


float t10,t11,t0,t1;
float Vcur=g_Vp, Vtgt=0.f, d=dist_a, a0=PI, a1=-PI;

t10 = t_xx      (Vcur, Vtgt, a0, a1, d, 1.0f);
t11 = t_xx      (Vcur, Vtgt, a0, a1, d, -1.0f);
t1      = t_1   (t10, t11); 
t0      = t_0   (Vcur, Vtgt, a0, a1, t1);

float new_h;
if (t0>=Device.fTimeDelta){
new_h		= g_Sp+sign*(g_Vp*Device.fTimeDelta+a0*Device.fTimeDelta*Device.fTimeDelta*0.5f);
g_Vp		+= a0*Device.fTimeDelta;
}else{
float t1	= Device.fTimeDelta-t0;
new_h		= g_Sp+sign*(g_Vp*t0+a0*t0*t0*0.5f);
g_Vp		+= a0*t0;
new_h		+= sign*(g_Vp*t1+a1*t1*t1*0.5f);
g_Vp		+= a1*t1;
}

//		float a			= getA(t0, a1, a0);



//.		float a				= GetCurrAcc(g_Vp,0.f,dist_a,PI_MUL_4,-PI_MUL_4);

//		float new_h			= g_Sp+sign*(g_Vp*Device.fTimeDelta+a*Device.fTimeDelta*Device.fTimeDelta*0.5f);

//		g_Vp				+= a*Device.fTimeDelta;

g_Sp				= angle_normalize_signed(new_h);

xform.setHPB		(new_h,p,b);
xform.translate_over(c);

static float g_S	= 0.f;
static float g_H	= 0.f;
static float gDT	= 0.01f;

gDT					= 0.999f*gDT+0.001f*Device.fTimeDelta;

Fmatrix M;
Level().Cameras.affected_Matrix(M);

float h,p,b;
M.getHPB			(h,p,b);

g_S					= 0.95f*g_S+0.05f*angle_difference_signed(h,g_H);

float sign_h		= fis_zero(g_S)?0.f:g_S/_abs(g_S);

static float s		= 0.f;
s					= 0.9f*s + 0.1f*g_S*gDT;//sign_h*10.f*(pow(_abs(g_S),1.3f));

static u32 xxx		= 0;
if (xxx!=0)			Sleep(xxx);
float new_h			= h-s;

g_H					= new_h;
xform.setHPB		(new_h,p,b);
xform.translate_over(c);
/*
static float pH		= 0.f;

Fmatrix M;
Level().Cameras.affected_Matrix(M);

float h,p,b;
M.getHPB			(h,p,b);

static float k		= 2000.f;
static float m		= 10.f;

float dist			= angle_difference_signed(h,pH);
float dist_a		= _abs(dist);
float sign			= fis_zero(dist)?0.f:dist/dist_a;
float new_h			= pH + dist*k/(2*m)*_sqr(dist)*_sqr(Device.fTimeDelta);

pH					= new_h;

xform.setHPB		(new_h,p,b);
xform.translate_over(c);

/*
Fmatrix M;
//		Level().Cameras.smooth_Matrix(M);
Level().Cameras.affected_Matrix(M);

static float dH		= 0.f;
static float pH		= 0.f;
static float sV		= 0.f;

float h,p,b;
M.getHPB			(h,p,b);

sV					= angle_difference_signed(h,pH)/Device.fTimeDelta;

dH					+= sV*Device.fTimeDelta;

//.		dH					= angle_inertion_var_	(dH,0.f,0.1f,10.f,CLMP,Device.fTimeDelta);

pH					= angle_difference_signed(h,dH);

xform.setHPB		(pH,p,b);
xform.translate_over(c);

static u32 xxx		= 0;
//		xxx					= Random.randI	(2,4);
if (xxx)			Sleep			(xxx);
/*
gDT					= 



static float g_Hd	= 0.f;
static float g_Hp	= 0.f;

g_Hd				+= angle_difference_signed(h,g_Hp);
g_Hd				*= 0.1f;

float new_h			= h-g_Hd;

xform.setHPB		(new_h,p,b);
xform.translate_over(c);


/*
inert.x				= angle_inertion_var_	(inert.x,h,0.0001f,2.f,CLMP,0.01f);
//.		inert.y				= angle_inertion_var_	(inert.y,p,0.0001f,2.f,CLMP,0.01f);

xform.setHPB		(inert.x,p,b);
xform.translate_over(c);
*/
/*
static Fvector g_dir={0,0,0};

g_dir.inertion		(xform.k,0.98f);
xform.k.set			(g_dir);

Fvector::generate_orthonormal_basis_normalized(xform.k,xform.j,xform.i);
g_dir.set			(xform.k);
/*
static float g_S	= 0.f;
static float g_H	= 0.f;
//		static float gDT	= 0.01f;

//		gDT					= 0.999f*gDT+0.001f*Device.fTimeDelta;

Fmatrix M;
Level().Cameras.affected_Matrix(M);

float h,p,b;
M.getHPB			(h,p,b);

float sp			= g_S;
g_S					= 0.02f*angle_difference_signed(h,g_H)+0.98f*g_S;
clamp				(g_S,-.5f,.5f);
float sign_h		= fis_zero(g_S)?0.f:g_S/_abs(g_S);
float new_h			= h-sign_h*pow(_abs(g_S),1.5f);
//		Log					("--",g_S);
//		float S				= g_S*10.f;
//		float new_h			= fis_zero(g_S)?h:h-(g_S/_abs(g_S))*_sqrt(_abs(g_S));

g_H					= h;
xform.setHPB		(new_h,p,b);
xform.translate_over(c);
/*

xform.k.inertion	(g_dir,0.1f);

Fvector::generate_orthonormal_basis_normalized(xform.k,xform.j,xform.i);

g_dir.set			(xform.k);
/*
float h,p,b;
xform.getHPB		(h,p,b);

float diff			= angle_difference_signed(g_h_curr,h);
g_h_accum			+= diff;

float h_diff		= g_h_accum*0.1f;

g_h_accum			-= h_diff;





g_h_curr			= h;


inert.x				+= h_diff;
inert.y				= p;

//		inert.x			= angle_inertion_var_	(inert.x,h,0.1f,12.f,CLMP,Device.fTimeDelta);
//		inert.y			= angle_inertion_var_	(inert.y,p,0.1f,12.f,CLMP,Device.fTimeDelta);
//		Msg("-- %3.2f %3.2f - %3.2f",Device.fTimeDelta,inert.x,a);

xform.setHPB		(-inert.x,inert.y,b);
xform.translate_over(c);
*/
/*
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

//	float restore_yaw_accel = -(cur_yaw-actor_yaw)*0.7f;
//добавить затухание трения
//restore_yaw_accel += 0.73f*(restore_yaw_speed>0?-1.f:1.f)*_abs(restore_yaw_speed);
//restore_yaw_speed += restore_yaw_accel*Device.fTimeDelta;
//cur_yaw += restore_yaw_speed*Device.fTimeDelta;
//
//if(yaw_w_mean > 0)
//{
//Msg("-- yaw w %f",yaw_w_mean);
//Msg("restore yaw accel %f", restore_yaw_accel);
//Msg("restore yaw speed %f", restore_yaw_speed);
//Msg("cur yaw%f", cur_yaw);
//}
//	float k = 0.93f;
//	if(_abs(cur_yaw-actor_yaw)>0.01f)
//	cur_yaw = k*cur_yaw + (1.f-k)*actor_yaw;

//float delta_cur_yaw		= cur_yaw - old_cur_yaw;
//	float delta_actor_yaw	= _abs(actor_yaw - old_actor_yaw);

//	if(yaw_w_mean<0.0001f)
	{	
		actor_yaw = angle_normalize(actor_yaw);
		float old_cur_yaw = cur_yaw;
		angle_lerp(cur_yaw, actor_yaw, PI_DIV_2, Device.fTimeDelta);
	}
//	if(_abs(delta_cur_yaw - delta_actor_yaw)<0.01f && _abs(delta_cur_yaw)>0.001f
//	&& _abs(delta_actor_yaw)>0.001f)
//	{
//	//		Msg("cur yaw delta   %f", cur_yaw - old_cur_yaw);
//	//		Msg("actor yaw delta %f", actor_yaw - old_actor_yaw);
//
//	cur_yaw = old_cur_yaw +delta_actor_yaw;
//	}
	old_actor_yaw = actor_yaw;

	Fmatrix inertion; 
	inertion.identity();
	inertion.rotateY(cur_yaw-actor_yaw);
	//inertion.rotateY(0.2f);
	hud_trans.mulB(inertion);
#endif
*/

/*
static Fvector m_vecLastFacing={0,0,1};

Fvector	forward		= xform.k;
Fvector& origin		= xform.c;

Fvector vDifference;
vDifference.sub		(forward, m_vecLastFacing);

if (_abs( vDifference.x ) > MAX_VIEWMODEL_LAG ||
_abs( vDifference.y ) > MAX_VIEWMODEL_LAG ||
_abs( vDifference.z ) > MAX_VIEWMODEL_LAG )
{
m_vecLastFacing	= forward;
}

// FIXME:  Needs to be predictable?
m_vecLastFacing.mad	(m_vecLastFacing, vDifference, 5.f * Device.fTimeDelta);
// Make sure it doesn't grow out of control!!!
m_vecLastFacing.normalize();
origin.mad			(vDifference,-0.05f);

Fvector right, up;
right.set(xform.i);
up.set(xform.j);
forward.set(xform.k);

float pitch = h*180.f/PI;
if ( pitch > 180.0f )
pitch -= 360.0f;
else if ( pitch < -180.0f )
pitch += 360.0f;

//FIXME: These are the old settings that caused too many exposed polys on some models
origin.mad(forward,pitch * 0.0003f);
origin.mad(right,pitch * 0.0003f);
origin.mad(up,pitch * 0.0003f);




/*
static CEnvelope* env[6]={0,0,0,0,0,0};
if (0==env[0]){	
env[0]			= xr_new<CEnvelope>();
env[1]			= xr_new<CEnvelope>();
env[2]			= xr_new<CEnvelope>();
env[3]			= xr_new<CEnvelope>();
env[4]			= xr_new<CEnvelope>();
env[5]			= xr_new<CEnvelope>();
env[0]->InsertKey	(Device.fTimeGlobal-1.f,h);
env[1]->InsertKey	(Device.fTimeGlobal-1.f,p);
env[2]->InsertKey	(Device.fTimeGlobal-1.f,b);
env[3]->InsertKey	(Device.fTimeGlobal-1.f,m_cam.c.x);
env[4]->InsertKey	(Device.fTimeGlobal-1.f,m_cam.c.y);
env[5]->InsertKey	(Device.fTimeGlobal-1.f,m_cam.c.z);
}

env[0]->InsertKey	(Device.fTimeGlobal,h);
env[1]->InsertKey	(Device.fTimeGlobal,p);
env[2]->InsertKey	(Device.fTimeGlobal,b);
env[3]->InsertKey	(Device.fTimeGlobal,m_cam.c.x);
env[4]->InsertKey	(Device.fTimeGlobal,m_cam.c.y);
env[5]->InsertKey	(Device.fTimeGlobal,m_cam.c.z);

float n_h = env[0]->Evaluate(Device.fTimeGlobal-0.05f); 
Log("-",angle_difference_signed(n_h,h));
h = n_h;
//		p = env[1]->Evaluate(Device.fTimeGlobal-0.05f);
//		b = env[2]->Evaluate(Device.fTimeGlobal-0.05f);

xform.setHPB		(h,p,b);
xform.c.set			(c);

for (u32 k=0; k<6; k++){
float diff_t	= env[k]->keys.back()->time-env[k]->keys.front()->time;
if ((diff_t>2.f)&&(env[k]->keys.size()>4)){
xr_delete	(env[k]->keys.front());
env[k]->keys.erase(env[k]->keys.begin());
}
}

/*
Fvector pt0,pt1;
pt0.set				(xform.c);
pt1.mad				(xform.c,xform.k,-1.f);

//
Fmatrix				m_previous, m_current;
{
static Fmatrix  m_saved_view=Fidentity;

// (new-camera) -> (world) -> (old_view)
Fmatrix m_cam;
Level().Cameras.affected_Matrix(m_cam);
//			float h,p,b;
//			M.getHPB			(h,p,b);

//			Fmatrix m_invview;  m_invview.invert        (Device.mView);
m_previous.mul		(m_saved_view,m_cam);
m_current.set       (Fidentity);
m_saved_view.invert	(m_cam);
}

Fvector c_pt0,c_pt1;
m_current.transform_tiny(c_pt0,pt0);
m_current.transform_tiny(c_pt1,pt1);

Fvector p_pt0,p_pt1;
m_previous.transform_tiny(p_pt0,pt0);
m_previous.transform_tiny(p_pt1,pt1);

Fvector d0,d1;
static float k = 0.99f;
d0.sub(p_pt0,c_pt0).mul(k);
d1.sub(p_pt1,c_pt1).mul(k);

pt0.sub(d0);
pt1.sub(d1);

xform.k.sub				(pt0,pt1);
xform.k.normalize		();
xform.j.normalize		();
xform.i.crossproduct	(xform.j,xform.k);
xform.j.crossproduct	(xform.k,xform.i);

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

#define MEASURING_NUM 10
static float yaw_w[MEASURING_NUM] = {0.f, 0.f, 0.f};
static int cur_measure = 0;

static float restore_yaw_speed = 0;

*/
