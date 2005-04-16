#include "stdafx.h"
#include "helicopter.h"
#include "level.h"
#include "script_game_object.h"
#include "game_object_space.h"
#include "../skeletonanimated.h"
#include "../LightAnimLibrary.h"
#include "PhysicsShell.h"
#include "clsid_game.h"
#include "script_callback_ex.h"


bool CHelicopter::isObjectVisible			(CObject* O)
{
	Fvector dir_to_object;
	Fvector to_point;
	O->Center(to_point);
	Fvector from_point = XFORM().c;
	dir_to_object.sub(to_point,from_point).normalize_safe();
	float ray_length = from_point.distance_to(to_point);


	BOOL res = Level().ObjectSpace.RayTest(from_point, dir_to_object, ray_length, collide::rqtStatic);
	collide::rq_result rq;
	Level().ObjectSpace.RayPick(from_point, dir_to_object, ray_length, collide::rqtStatic,rq);
		
	return !res;
}

bool CHelicopter::isVisible			(CScriptGameObject* O)
{
	return isObjectVisible	(&O->object());
}


/*
float CHelicopter::GetfHealth() const 
{ 
	float f = CEntity::GetfHealth(); 
	return f;
}

float CHelicopter::SetfHealth(float value) {
	CEntity::SetfHealth(value);
	return value;
}
*/
void CHelicopter::TurnLighting(bool bOn)
{
	m_light_render->set_active						(bOn);
	m_light_started = bOn;

}
void  CHelicopter::TurnEngineSound(bool bOn)
{
	if(bOn)
		m_engineSound.set_volume(1.0f);
	else
		m_engineSound.set_volume(0.0f);

}

void CHelicopter::StartFlame ()
{
	if(m_pParticle)return;
	m_pParticle = xr_new<CParticlesObject>(*m_smoke_particle,FALSE);

	Fvector zero_vector;
	zero_vector.set(0.f,0.f,0.f);
	m_pParticle->UpdateParent(m_particleXFORM, zero_vector );
	m_pParticle->Play();
	m_flame_started = true;
}

void CHelicopter::UpdateHeliParticles	()
{
	CKinematics* K		= smart_cast<CKinematics*>(Visual());
	m_particleXFORM	= K->LL_GetTransform(m_smoke_bone);
	m_particleXFORM.mulA(XFORM());

	if (m_pParticle){
		
		Fvector vel;

		Fvector last_pos = PositionStack.back().vPosition;
		vel.sub(Position(), last_pos);
		vel.mul(5.0f);

		m_pParticle->UpdateParent(m_particleXFORM, vel );
	}
//lighting
	if(m_light_render->get_active()){
		Fmatrix xf;
		Fmatrix& M = K->LL_GetTransform(u16(m_light_bone));
		xf.mul		(XFORM(),M);
		VERIFY(!fis_zero(DET(xf)));

		m_light_render->set_rotation	(xf.k,xf.i);
		m_light_render->set_position	(xf.c);

		if (m_lanim)
		{
			int frame;
			u32 clr					= m_lanim->CalculateBGR(Device.fTimeGlobal,frame); // �������� � ������ BGR
			Fcolor					fclr;
			fclr.set				((float)color_get_B(clr),(float)color_get_G(clr),(float)color_get_R(clr),1.f);
			fclr.mul_rgb			(m_light_brightness/255.f);
			m_light_render->set_color	(fclr);
		}

	}
}
void CHelicopter::Explode ()
{
	if(m_pParticle){
		m_pParticle->Stop();
		m_pParticle->PSI_destroy();
		m_pParticle = NULL;
	}
	m_pParticle = xr_new<CParticlesObject>(*m_explode_particle,FALSE);

	Fvector zero_vector;
	zero_vector.set(0.f,0.f,0.f);
	m_pParticle->UpdateParent(m_particleXFORM, zero_vector );

	m_explodeSound.play_at_pos(0,XFORM().c);
	m_explodeSound.set_position(XFORM().c);

	m_pParticle->Play();
	m_exploded = true;
}

void CHelicopter::SetDestPosition (Fvector* pos)
{
	m_movement.SetDestPosition(pos);
}

float CHelicopter::GetDistanceToDestPosition()
{
	return m_movement.GetDistanceToDestPosition();
}

void CHelicopter::UnSetEnemy()
{
	m_enemy.type		= eEnemyNone;
}

void CHelicopter::SetEnemy(CScriptGameObject* e)
{
	m_enemy.type		= eEnemyEntity;
	m_enemy.destEnemyID = e->ID();
}

void CHelicopter::SetEnemy(Fvector* pos)
{
	m_enemy.type			= eEnemyPoint;
	m_enemy.destEnemyPos	= *pos;
}



float CHelicopter::GetCurrVelocity()
{
	return m_movement.curLinearSpeed;
}

void CHelicopter::SetMaxVelocity(float v)
{
	m_movement.maxLinearSpeed = v;
}
float CHelicopter::GetMaxVelocity()
{
	return m_movement.maxLinearSpeed;
}

void CHelicopter::SetSpeedInDestPoint(float sp)
{
	if(sp>=0.0f)
		m_movement.speedInDestPoint = sp;
}

float CHelicopter::GetSpeedInDestPoint(float sp)
{
	return m_movement.speedInDestPoint;
}
void CHelicopter::SetOnPointRangeDist(float d)
{
	m_movement.onPointRangeDist = d;
}

float CHelicopter::GetOnPointRangeDist()
{
	return m_movement.onPointRangeDist;
}

float CHelicopter::GetRealAltitude()
{
	collide::rq_result		cR;
	Fvector down_dir;

	down_dir.set(0.0f, -1.0f, 0.0f);


	Level().ObjectSpace.RayPick(XFORM().c, down_dir, 1000.0f, collide::rqtStatic, cR);
	
	return cR.range;
}

void CHelicopter::Hit(	float P, 
						Fvector &dir, 
						CObject* who, 
						s16 element, 
						Fvector position_in_bone_space, 
						float impulse,  
						ALife::EHitType hit_type)
{
	if(GetfHealth()<0.5f)
		return;

inherited::Hit(P,dir,who,element,position_in_bone_space,impulse,hit_type);
if(state() == CHelicopter::eDead ) return;

if(who==this)
	return;

	bonesIt It = m_hitBones.find(element);
	if(It != m_hitBones.end() && hit_type==ALife::eHitTypeFireWound) {
		float curHealth = GetfHealth();
		curHealth -= P*It->second*10.0f;
		SetfHealth(curHealth);
#ifdef DEBUG
		if (bDebug)	Log("----Helicopter::PilotHit(). health=",curHealth);
#endif
	}else {
		float hit_power		= P/100.f;
		hit_power			*= m_HitTypeK[hit_type];

		SetfHealth(GetfHealth()-hit_power);
#ifdef DEBUG
		float h= GetfHealth();
		if (bDebug)	Log("----Helicopter::Hit(). health=",h);
#endif
	};
	
	if (who){
		switch (who->CLS_ID){
			case CLSID_OBJECT_ACTOR:
			case CLSID_AI_STALKER:{
				callback(GameObject::eHelicopterOnHit)(P,impulse,hit_type,who->ID());
		}break;
		default:
			break;
		}
	}

}

void CHelicopter::PHHit(float P,Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	if(!g_Alive())inherited::PHHit(P,dir,who,element,p_in_object_space,impulse,hit_type);
}


void CHelicopter::DieHelicopter()
{
	if ( state() == CHelicopter::eDead )
		return;
	CKinematics* K		= smart_cast<CKinematics*>(Visual());
	if(true /*!PPhysicsShell()*/){
		string256						I;
		LPCSTR bone;
		
		u16 bone_id;
		for (u32 i=0, n=_GetItemCount(*m_death_bones_to_hide); i<n; ++i){
			bone = _GetItem(*m_death_bones_to_hide,i,I);
			bone_id		= K->LL_BoneID	(bone);
			K->LL_SetBoneVisible(bone_id,FALSE,TRUE);
		}

		///PPhysicsShell()=P_build_Shell	(this,false);
		PPhysicsShell()->EnabledCallbacks(TRUE);
		PPhysicsShell()->set_ObjectContactCallback(NULL);
	}
	Fvector lin_vel;

	Fvector prev_pos = PositionStack.front().vPosition;
	lin_vel.sub(XFORM().c,prev_pos);
	lin_vel.div((Device.dwTimeGlobal-PositionStack.front().dwTime)/1000.0f);

	lin_vel.mul(m_death_lin_vel_k);
	PPhysicsShell()->set_LinearVel(lin_vel);
	PPhysicsShell()->set_AngularVel(m_death_ang_vel);
	PPhysicsShell()->Enable();
	K->CalculateBones_Invalidate();
	K->CalculateBones();
	setState(CHelicopter::eDead);
	m_engineSound.stop();
	processing_deactivate();
	m_dead = true;
}
void SHeliEnemy::reinit()
{
	type			=eEnemyNone;
	destEnemyPos.set(0.0f,0.0f,0.0f);
	destEnemyID		=u32(-1);

}
void SHeliEnemy::Update()
{
	switch(type){
		case eEnemyNone:
		case eEnemyPoint:
			break;
		case eEnemyEntity:{
				CObject* O = Level().Objects.net_Find(destEnemyID);
				if(O)	O->Center( destEnemyPos );
				else	type = eEnemyNone;
			}break;
		default:
			NODEFAULT;
	};
}

void SHeliEnemy::save(NET_Packet &output_packet)
{
	output_packet.w_s16		((s16)type);
	output_packet.w_vec3	(destEnemyPos);
	output_packet.w_u32		(destEnemyID);
}

void SHeliEnemy::load(IReader &input_packet)
{
	type			=	(EHeliHuntState)input_packet.r_s16();
	input_packet.r_fvector3	(destEnemyPos);
	destEnemyID		=	input_packet.r_u32();
}


void SHeliBodyState::reinit()
{
	type = eBodyByPath;
	b_looking_at_point = false;
	looking_point.set(0.0f,0.0f,0.0f);
	parent->XFORM().getHPB(currBodyH, currBodyP, currBodyB);

}

void SHeliBodyState::LookAtPoint			(Fvector point, bool do_it)
{
	b_looking_at_point		= do_it;
	looking_point			= point;
	type = (do_it)?eBodyToPoint:eBodyByPath;
}

void SHeliBodyState::save(NET_Packet &output_packet)
{
	output_packet.w_s16((s16)type);
	output_packet.w_u8(b_looking_at_point ? 1 : 0);    
	output_packet.w_float(currBodyH);
	output_packet.w_float(currBodyP);
	output_packet.w_float(currBodyB);
}

void SHeliBodyState::load(IReader &input_packet)
{
	type				= (EHeliBodyState)input_packet.r_s16();
	b_looking_at_point	= !!input_packet.r_u8();
	currBodyH			= input_packet.r_float();
	currBodyP			= input_packet.r_float();
	currBodyB			= input_packet.r_float();
}



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

float GetCurrAcc(float V0, float V1, float dist, float a0, float a1)
{
	float t10,t11,t0,t1;
	
	t10 = t_xx	(V0, V1, a0, a1, dist, 1.0f);
	t11 = t_xx	(V0, V1, a0, a1, dist, -1.0f);
	t1	= t_1	(t10, t11); 
	t0	= t_0	(V0, V1, a0, a1, t1);
	return getA	(t0, a1, a0);
}
