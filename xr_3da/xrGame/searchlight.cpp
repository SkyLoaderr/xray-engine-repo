#include "stdafx.h"
#include "searchlight.h"

#include "../LightAnimLibrary.h"

#include "ai_script_actions.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSearchlight::CSearchlight()
{
	light_render			= ::Render->light_create();
	light_render->set_type	(IRender_Light::SPOT);
	light_render->set_shadow(true);
	glow_render				= ::Render->glow_create();
	lanim					= 0;

}

CSearchlight::~CSearchlight()
{
	::Render->light_destroy	(light_render);
	::Render->glow_destroy	(glow_render);
}

void CSearchlight::Load(LPCSTR section)
{
	inherited::Load(section);

}


void __stdcall CSearchlight::BoneCallback(CBoneInstance *B)
{
	CSearchlight	*this_class = dynamic_cast<CSearchlight*> (static_cast<CObject*>(B->Callback_Param));
	
	this_class->UpdateBones();
}


BOOL CSearchlight::net_Spawn(LPVOID DC)
{
	CSE_Abstract				*e		= (CSE_Abstract*)(DC);
	CSE_ALifeObjectSearchlight	*slight	= dynamic_cast<CSE_ALifeObjectSearchlight*>(e);
	R_ASSERT				(slight);
	
	cNameVisual_set			(slight->get_visual());
	inherited::net_Spawn	(DC);
	
	R_ASSERT				(Visual() && PKinematics(Visual()));
	
	Fcolor					clr;
	clr.set					(slight->color);			clr.a = 1.f;
	clr.mul_rgb				(slight->spot_brightness);
	fBrightness				= slight->spot_brightness;
	light_render->set_range	(slight->spot_range);
	light_render->set_color	(clr);
	light_render->set_cone	(slight->spot_cone_angle);
	light_render->set_texture(slight->spot_texture[0]?slight->spot_texture:0);

	glow_render->set_texture(slight->glow_texture[0]?slight->glow_texture:0);
	glow_render->set_color	(clr);
	glow_render->set_radius	(slight->glow_radius);

	lanim					= LALib.FindItem(slight->animator);

	guid_bone				= slight->guid_bone;		VERIFY(guid_bone!=BI_NONE);
	rot_bone				= slight->rotation_bone;	VERIFY(rot_bone!=BI_NONE);
	cone_bone				= PKinematics(Visual())->LL_BoneID("bone_cone");

	setVisible	(true);
	setEnabled	(true);

	TurnOn		();
	
	//////////////////////////////////////////////////////////////////////////
	CBoneInstance& BI = PKinematics(Visual())->LL_GetBoneInstance(rot_bone);	
	BI.set_callback(BoneCallback,this);
	
	Fvector dir = Direction();
	dir.invert().getHP(_start.yaw,_start.pitch);
	_cur.yaw	= _target.yaw	= _start.yaw;
	_cur.pitch	= _target.pitch	= _start.pitch;
	//////////////////////////////////////////////////////////////////////////

	return TRUE;
}

void CSearchlight::shedule_Update	(u32 dt)
{
	inherited::shedule_Update(dt);

}

void CSearchlight::TurnOn()
{
	if (light_render->get_active()) return;

	light_render->set_active(true);
	glow_render->set_active (true);
	PKinematics(Visual())->LL_SetBoneVisible(cone_bone, TRUE, TRUE);
}

void CSearchlight::TurnOff()
{
	if (!light_render->get_active()) return;

	light_render->set_active(false);
	glow_render->set_active (false);
	PKinematics(Visual())->LL_SetBoneVisible(cone_bone, FALSE, TRUE);
}

void CSearchlight::UpdateCL	()
{
	inherited::UpdateCL();

	// update light source
	if (light_render->get_active()){
		// calc color animator
		if (lanim){
			int frame;
			// возвращает в формате BGR
			u32 clr			= lanim->Calculate(Device.fTimeGlobal,frame); 

			Fcolor			fclr;
			fclr.set		((float)color_get_B(clr),(float)color_get_G(clr),(float)color_get_R(clr),1.f);
			fclr.mul_rgb	(fBrightness/255.f);
			light_render->set_color(fclr);
			glow_render->set_color(fclr);
		}

		CBoneInstance& BI = PKinematics(Visual())->LL_GetBoneInstance(guid_bone);
		Fmatrix M;

		M.mul(XFORM(),BI.mTransform);

		light_render->set_direction	(M.k);
		light_render->set_position	(M.c);
		glow_render->set_position	(M.c);
		glow_render->set_direction	(M.k);

	}

	// Update searchlight 
	angle_lerp(_cur.yaw,	_target.yaw,	bone_vel_x,	Device.fTimeDelta);
	angle_lerp(_cur.pitch,	_target.pitch,	bone_vel_y,	Device.fTimeDelta);
	
}


void CSearchlight::renderable_Render()
{
	inherited::renderable_Render	();

	if(getVisible()) {
		::Render->set_Transform		(&XFORM());
		::Render->add_Visual		(Visual());
	}
}

BOOL CSearchlight::UsedAI_Locations()
{
	return					(FALSE);
}

bool CSearchlight::bfAssignWatch(CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignWatch(tpEntityAction))
		return		(false);
	
	CWatchAction	&l_tWatchAction = tpEntityAction->m_tWatchAction;
	
	(!l_tWatchAction.m_tpObjectToWatch) ?	SetTarget(l_tWatchAction.m_tTargetPoint) : 
											SetTarget(l_tWatchAction.m_tpObjectToWatch->Position());

	bone_vel_x	= l_tWatchAction.vel_bone_x;
	
	float time = angle_difference(_target.yaw, _cur.yaw) / bone_vel_x;
	bone_vel_y	= (fis_zero(time)? l_tWatchAction.vel_bone_y : angle_difference(_target.pitch, _cur.pitch) / time);
	
	if ((angle_difference(_cur.yaw,_target.yaw) < EPS_L) && (angle_difference(_cur.pitch,_target.pitch) < EPS_L)) {
		l_tWatchAction.m_bCompleted = true;
	} else {
		l_tWatchAction.m_bCompleted = false;
	}
	
	return !l_tWatchAction.m_bCompleted;
}

bool CSearchlight::bfAssignObject(CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignObject(tpEntityAction))
		return	(false);

	CObjectAction	&l_tObjectAction = tpEntityAction->m_tObjectAction;

	if (l_tObjectAction.m_tGoalType == MonsterSpace::eObjectActionTurnOn)			TurnOn	();
	else if (l_tObjectAction.m_tGoalType == MonsterSpace::eObjectActionTurnOff)	TurnOff	();

	return	(true);
}

void CSearchlight::UpdateBones()
{
	float delta_yaw;
	float delta_pitch;
	
	delta_yaw = angle_difference(_start.yaw,_cur.yaw);
	if (angle_normalize_signed(_start.yaw - _cur.yaw) > 0) delta_yaw = -delta_yaw;

	delta_pitch = angle_difference(_start.pitch,_cur.pitch);
	if (angle_normalize_signed(_start.pitch - _cur.pitch) > 0) delta_pitch = -delta_pitch;

	CBoneInstance& BI = PKinematics(Visual())->LL_GetBoneInstance(rot_bone);
	
	Fvector c					= BI.mTransform.c;
	Fmatrix						spin;

	spin.setXYZi				(delta_pitch, delta_yaw, 0);
	VERIFY						(_valid(spin));
	BI.mTransform.mulA_43	(spin);
	BI.mTransform.c			= c;
}


void CSearchlight::SetTarget(const Fvector &target_pos)
{
	float  th,tp;
	Fvector().sub(target_pos, Position()).getHP(th,tp);

	float delta_h;
	delta_h = angle_difference(th,_start.yaw);

	if (angle_normalize_signed(th - _start.yaw) > 0) delta_h = -delta_h;
	clamp(delta_h, -PI_DIV_2, PI_DIV_2);

	_target.yaw = angle_normalize(_start.yaw + delta_h);

	clamp(tp, -PI_DIV_2, PI_DIV_2);
	_target.pitch = tp;
}

