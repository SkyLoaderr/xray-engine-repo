#include "stdafx.h"
#include "searchlight.h"

#include "../LightAnimLibrary.h"

#include "ai_script_actions.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProjector::CProjector()
{
	light_render			= ::Render->light_create();
	light_render->set_type	(IRender_Light::SPOT);
	light_render->set_shadow(true);
	glow_render				= ::Render->glow_create();
	lanim					= 0;
	rot_bone_x				= BI_NONE;
	rot_bone_y				= BI_NONE;
}

CProjector::~CProjector()
{
	::Render->light_destroy	(light_render);
	::Render->glow_destroy	(glow_render);
}

void CProjector::Load(LPCSTR section)
{
	inherited::Load(section);

}


void __stdcall CProjector::BoneCallbackX(CBoneInstance *B)
{
	CProjector	*P = dynamic_cast<CProjector*> (static_cast<CObject*>(B->Callback_Param));

	float delta_pitch;

	delta_pitch = angle_difference(P->_start.pitch,P->_cur.pitch);
	if (angle_normalize_signed(P->_start.pitch - P->_cur.pitch) > 0) delta_pitch = -delta_pitch;

	Fvector c				= B->mTransform.c;
	Fmatrix					spin;

	spin.setXYZi			(delta_pitch, 0, 0);
	VERIFY					(_valid(spin));
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void __stdcall CProjector::BoneCallbackY(CBoneInstance *B)
{
	CProjector	*P = dynamic_cast<CProjector*> (static_cast<CObject*>(B->Callback_Param));

	float delta_pitch;

	delta_pitch = angle_difference(P->_start.pitch,P->_cur.pitch);
	if (angle_normalize_signed(P->_start.pitch - P->_cur.pitch) > 0) delta_pitch = -delta_pitch;

	Fvector c				= B->mTransform.c;
	Fmatrix					spin;

	spin.setXYZi			(delta_pitch, 0, 0);
	VERIFY					(_valid(spin));
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

BOOL CProjector::net_Spawn(LPVOID DC)
{
	CSE_Abstract				*e		= (CSE_Abstract*)(DC);
	CSE_ALifeObjectProjector	*slight	= dynamic_cast<CSE_ALifeObjectProjector*>(e);
	R_ASSERT				(slight);
	
	cNameVisual_set			(slight->get_visual());
	inherited::net_Spawn	(DC);
	
	R_ASSERT				(Visual() && PKinematics(Visual()));

	CKinematics* K			= PKinematics(Visual());
	CInifile* pUserData		= K->LL_UserData(); 
	R_ASSERT3				(pUserData,"Empty Torch user data!",slight->get_visual());
	lanim					= LALib.FindItem(pUserData->r_string("projector_definition","color_animator"));
	guid_bone				= K->LL_BoneID	(pUserData->r_string("projector_definition","guide_bone"));		VERIFY(guid_bone!=BI_NONE);
	rot_bone_x				= K->LL_BoneID	(pUserData->r_string("projector_definition","rotation_bone_x"));VERIFY(rot_bone_x!=BI_NONE);
	rot_bone_y				= K->LL_BoneID	(pUserData->r_string("projector_definition","rotation_bone_y"));VERIFY(rot_bone_y!=BI_NONE);
	Fcolor clr				= pUserData->r_fcolor				("projector_definition","color");
	fBrightness				= clr.intensity();
	light_render->set_color	(clr);
	light_render->set_range	(pUserData->r_float					("projector_definition","range"));
	light_render->set_cone	(deg2rad(pUserData->r_float			("projector_definition","spot_angle")));
	light_render->set_texture(pUserData->r_string				("projector_definition","spot_texture"));

	glow_render->set_texture(pUserData->r_string				("projector_definition","glow_texture"));
	glow_render->set_color	(clr);
	glow_render->set_radius	(pUserData->r_float					("projector_definition","glow_radius"));

	setVisible	(true);
	setEnabled	(true);

	TurnOn		();
	
	//////////////////////////////////////////////////////////////////////////
	CBoneInstance& b_x = PKinematics(Visual())->LL_GetBoneInstance(rot_bone_x);	
	b_x.set_callback(BoneCallbackX,this,TRUE);

	CBoneInstance& b_y = PKinematics(Visual())->LL_GetBoneInstance(rot_bone_y);	
	b_y.set_callback(BoneCallbackY,this,TRUE);
	
	Fvector dir = Direction();
	dir.invert().getHP(_start.yaw,_start.pitch);
	_cur.yaw	= _target.yaw	= _start.yaw;
	_cur.pitch	= _target.pitch	= _start.pitch;
	//////////////////////////////////////////////////////////////////////////

	return TRUE;
}

void CProjector::shedule_Update	(u32 dt)
{
	inherited::shedule_Update(dt);

}

void CProjector::TurnOn()
{
	if (light_render->get_active()) return;

	light_render->set_active(true);
	glow_render->set_active (true);
	PKinematics(Visual())->LL_SetBoneVisible(guid_bone, TRUE, TRUE);
}

void CProjector::TurnOff()
{
	if (!light_render->get_active()) return;

	light_render->set_active(false);
	glow_render->set_active (false);
	PKinematics(Visual())->LL_SetBoneVisible(guid_bone, FALSE, TRUE);
}

void CProjector::UpdateCL	()
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


void CProjector::renderable_Render()
{
	inherited::renderable_Render	();

	if(getVisible()) {
		::Render->set_Transform		(&XFORM());
		::Render->add_Visual		(Visual());
	}
}

BOOL CProjector::UsedAI_Locations()
{
	return					(FALSE);
}

bool CProjector::bfAssignWatch(CEntityAction *tpEntityAction)
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

bool CProjector::bfAssignObject(CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignObject(tpEntityAction))
		return	(false);

	CObjectAction	&l_tObjectAction = tpEntityAction->m_tObjectAction;

	if (l_tObjectAction.m_tGoalType == MonsterSpace::eObjectActionTurnOn)			TurnOn	();
	else if (l_tObjectAction.m_tGoalType == MonsterSpace::eObjectActionTurnOff)	TurnOff	();

	return	(true);
}

void CProjector::SetTarget(const Fvector &target_pos)
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

